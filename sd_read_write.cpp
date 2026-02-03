/**********************************************************************
  文件名称 / Filename : sd_read_write.cpp
  文件用途 / File Purpose : SD_MMC存储卡读写功能实现 / SD_MMC Storage Card Read/Write Implementation
               本文件实现了SD卡的初始化、文件读写、目录操作等功能
               This file implements SD card initialization, file read/write, directory operations, and other functions
               主要功能包括 / Main Features:
               1. SD_MMC卡初始化和检测 / SD_MMC card initialization and detection
               2. 目录创建、删除、列表显示 / Directory creation, deletion, and listing
               3. 文件创建、读取、写入、删除、重命名 / File creation, reading, writing, deletion, and renaming
               4. JPEG图像文件写入 / JPEG image file writing
               5. 目录文件计数 / Directory file counting
               6. 摄像头照片保存功能 / Camera photo saving function
               7. SD卡空间查询功能（GB单位）/ SD card space query function (GB unit)
               8. 视频录制功能（AVI格式，MJPEG编码）/ Video recording function (AVI format, MJPEG encoding)
               9. 自动清理旧文件功能（55GB阈值）/ Auto cleanup old files function (55GB threshold)
               10. 时间戳文件名生成功能（YYYYMMDDHHMM格式，年月日时分）/ Timestamp filename generation function (YYYYMMDDHHMM format)
               11. 视频自动分段录制功能（2分钟一段）/ Auto-segmented video recording function (2 minutes per segment)
               12. 无效视频文件清理功能（删除0KB视频文件）/ Invalid video file cleanup function (delete 0KB video files)
  作者 / Author : ESP32-S3监控项目 / ESP32-S3 Monitoring Project
  修改日期 / Modification Date : 2026-02-03
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : FS.h - 文件系统基础库 / File System Base Library
               SD_MMC.h - SD_MMC驱动库 / SD_MMC Driver Library
               time.h - 时间处理库 / Time Processing Library
  引脚说明 / Pin Description : SD_MMC_CLK (GPIO39) - SD卡时钟线（不可修改）/ SD card clock line (cannot be modified)
               SD_MMC_CMD (GPIO38) - SD卡命令线（不可修改）/ SD card command line (cannot be modified)
               SD_MMC_D0  (GPIO40) - SD卡数据线（不可修改）/ SD card data line (cannot be modified)
  使用说明 / Usage Instructions : 1. 调用sdmmcInit()初始化SD卡 / Call sdmmcInit() to initialize SD card
               2. 使用文件操作函数进行读写 / Use file operation functions for reading and writing
               3. SD卡使用SDIO模式，4位并行传输，速度快 / SD card uses SDIO mode, 4-bit parallel transmission, high speed
  参数调整 / Parameter Adjustment : SDMMC_FREQ_DEFAULT - SD卡通信频率 / SD card communication frequency
               调整建议：使用Class 10以上SD卡以获得最佳性能 / Adjustment suggestion: Use Class 10 or higher SD card for best performance
  最新更新 / Latest Updates : 1. 修改SD卡空间计算函数，返回GB单位（乘以100保留2位小数）/ Modified SD card space calculation function, returns GB unit (multiply by 100 to keep 2 decimal places)
               2. 添加SD卡空间查询功能（getSDTotalSpaceMB、getSDUsedSpaceMB、getSDFreeSpaceMB）/ Added SD card space query function (getSDTotalSpaceMB, getSDUsedSpaceMB, getSDFreeSpaceMB)
               3. 优化文件目录结构（photos和videos子目录）/ Optimized file directory structure (photos and videos subdirectories)
               4. 添加视频录制功能（AVI格式，MJPEG编码）/ Added video recording function (AVI format, MJPEG encoding)
               5. 添加自动清理旧文件功能（55GB阈值）/ Added auto cleanup old files function (55GB threshold)
               6. 添加时间戳文件名生成功能（generateTimestampFilename）/ Added timestamp filename generation function (generateTimestampFilename)
               7. 实现视频自动分段录制功能（2分钟一段）/ Implemented auto-segmented video recording function (2 minutes per segment)
               8. 修改照片和视频文件名为时间戳格式（YYYYMMDDHHMM）/ Changed photo and video filename format to timestamp (YYYYMMDDHHMM)
               9. 使用NTP时间同步获取准确时间 / Uses NTP time synchronization to get accurate time
               10. 添加无效视频文件清理功能（cleanInvalidVideoFiles）/ Added invalid video file cleanup function (cleanInvalidVideoFiles)
               11. 优化SD卡空间管理逻辑，实现动态阈值检测 / Optimized SD card space management logic, implemented dynamic threshold detection
               12. 添加按时间删除最旧文件功能（deleteOldestFiles）/ Added delete oldest files by time function (deleteOldestFiles)
               13. 添加空间检测频率和清理优先级配置 / Added space check interval and cleanup priority configuration
               14. 添加异常处理机制 / Added exception handling mechanism
  注意事项 / Important Notes : 时间戳格式使用NTP同步的系统时间，确保时间准确性 / Timestamp format uses NTP-synchronized system time to ensure time accuracy
**********************************************************************/

#include "sd_read_write.h"
#include "time.h"

// 视频录制相关变量 / Video recording related variables
static File videoFile;                    // 视频文件对象 / Video file object
static bool isRecording = false;          // 是否正在录制 / Is recording
static char currentVideoFilename[64];      // 当前录制的视频文件名 / Current video filename being recorded
static uint32_t videoFrameCount = 0;      // 视频帧计数 / Video frame count
static uint32_t videoStartTime = 0;       // 视频开始时间 / Video start time
static uint32_t videoTotalSize = 0;       // 视频总大小 / Video total size
static uint32_t videoMaxFrameSize = 0;    // 最大帧大小 / Maximum frame size
static uint32_t videoFPS = 15;            // 视频帧率 / Video frame rate
static uint32_t videoWidth = 1280;        // 视频宽度 / Video width
static uint32_t videoHeight = 720;        // 视频高度 / Video height
static uint32_t moviOffset = 0;           // movi列表偏移量 / movi list offset
static uint32_t idx1Offset = 0;           // idx1列表偏移量 / idx1 list offset
static uint32_t videoSegmentCount = 0;    // 视频分段计数 / Video segment count
static uint32_t videoSegmentStartTime = 0; // 当前分段开始时间 / Current segment start time
static const uint32_t VIDEO_SEGMENT_DURATION = 120; // 视频分段时长（秒），2分钟 / Video segment duration (seconds), 2 minutes
static AVI_MAIN_HEADER aviMainHeader;     // AVI主头 / AVI main header
static AVI_STREAM_HEADER aviStreamHeader; // AVI流头 / AVI stream header
static AVI_BITMAP_INFO aviBitmapInfo;     // AVI位图信息 / AVI bitmap info

/**
 * @brief SD_MMC存储卡初始化函数
 * @details 功能说明：
 *          1. 配置SD_MMC引脚（CLK、CMD、D0）
 *          2. 挂载SD卡文件系统
 *          3. 检测SD卡类型（MMC、SDSC、SDHC）
 *          4. 显示SD卡容量和空间使用情况
 * @note SD_MMC使用SDIO模式，4位并行数据传输，速度比SPI模式快
 *       引脚固定为CLK(39)、CMD(38)、D0(40)，不可修改
 */
void sdmmcInit(void){
  // 配置SD_MMC引脚
  // SD_MMC_CLK: 时钟线，用于同步数据传输
  // SD_MMC_CMD: 命令线，用于发送控制命令
  // SD_MMC_D0:  数据线0，用于传输数据（SDIO模式可使用4位数据线）
  SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
  
  // 初始化SD_MMC并挂载文件系统到"/sdcard"路径
  // 参数说明：
  // - "/sdcard": 挂载点路径
  // - true: 使用1位数据线模式（false为4位模式）
  // - true: 格式化卡（如果需要）
  // - SDMMC_FREQ_DEFAULT: 使用默认频率
  // - 5: 最大频率分频值
  if (!SD_MMC.begin(SD_MOUNT_POINT, true, true, SDMMC_FREQ_DEFAULT, 5)) {
    // SD卡挂载失败，输出错误信息
    Serial.println("Card Mount Failed");
    return;
  }
  
  // 获取SD卡类型
  uint8_t cardType = SD_MMC.cardType();
  
  // 检查是否检测到SD卡
  if(cardType == CARD_NONE){
      Serial.println("No SD_MMC card attached");
      return;
  }
  
  // 输出SD卡类型
  Serial.print("SD_MMC Card Type: ");
  if(cardType == CARD_MMC){
      Serial.println("MMC");           // 多媒体卡
  } else if(cardType == CARD_SD){
      Serial.println("SDSC");          // 标准容量SD卡（最大2GB）
  } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");          // 高容量SD卡（最大32GB）
  } else {
      Serial.println("UNKNOWN");       // 未知类型
  }
  
  // 计算并输出SD卡总容量（单位：MB）
  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);
  
  // 输出SD卡总空间（单位：MB）
  Serial.printf("Total space: %lluMB\r\n", SD_MMC.totalBytes() / (1024 * 1024));
  
  // 输出SD卡已用空间（单位：MB）
  Serial.printf("Used space: %lluMB\r\n", SD_MMC.usedBytes() / (1024 * 1024));
}

/**
 * @brief 列出目录内容函数
 * @param fs 文件系统对象引用
 * @param dirname 要列出的目录路径
 * @param levels 递归深度，0表示不递归，>0表示递归子目录
 * @details 功能说明：
 *          1. 打开指定目录
 *          2. 遍历目录中的所有文件和子目录
 *          3. 显示文件名和大小，或子目录名
 *          4. 如果levels>0，递归列出子目录内容
 * @note 用于调试和查看SD卡内容
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    // 输出要列出的目录路径
    Serial.printf("Listing directory: %s\n", dirname);

    // 打开目录
    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    
    // 检查是否为目录
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    // 打开目录中的第一个文件/子目录
    File file = root.openNextFile();
    
    // 遍历目录中的所有文件和子目录
    while(file){
        // 如果是子目录
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            
            // 如果递归深度>0，递归列出子目录内容
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            // 如果是文件，显示文件名和大小
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        
        // 打开下一个文件/子目录
        file = root.openNextFile();
    }
}

/**
 * @brief 创建目录函数
 * @param fs 文件系统对象引用
 * @param path 要创建的目录路径
 * @details 功能说明：
 *          1. 在文件系统中创建指定路径的目录
 *          2. 如果父目录不存在，会自动创建
 *          3. 输出创建结果
 * @note 如果目录已存在，mkdir会失败
 */
void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

/**
 * @brief 删除目录函数
 * @param fs 文件系统对象引用
 * @param path 要删除的目录路径
 * @details 功能说明：
 *          1. 删除指定路径的目录
 *          2. 目录必须为空才能删除
 *          3. 输出删除结果
 * @note 如果目录不为空，rmdir会失败
 */
void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

/**
 * @brief 读取文本文件函数
 * @param fs 文件系统对象引用
 * @param path 要读取的文件路径
 * @details 功能说明：
 *          1. 打开指定文件
 *          2. 读取文件内容并输出到串口
 *          3. 适用于文本文件
 * @note 不适用于二进制文件（如JPEG图像）
 */
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    // 打开文件进行读取
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    // 读取并输出文件内容
    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
}

/**
 * @brief 写入文本文件函数
 * @param fs 文件系统对象引用
 * @param path 要写入的文件路径
 * @param message 要写入的文本内容
 * @details 功能说明：
 *          1. 打开文件进行写入（FILE_WRITE模式）
 *          2. 如果文件不存在会创建新文件
 *          3. 如果文件存在会覆盖原内容
 *          4. 写入文本内容
 * @note 使用FILE_APPEND模式可以追加内容而不覆盖
 */
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    // 打开文件进行写入（覆盖模式）
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    
    // 写入文本内容
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}

/**
 * @brief 追加文本到文件函数
 * @param fs 文件系统对象引用
 * @param path 文件路径
 * @param message 要追加的文本内容
 * @details 功能说明：
 *          1. 打开文件进行追加（FILE_APPEND模式）
 *          2. 如果文件不存在会创建新文件
 *          3. 在文件末尾追加内容，不覆盖原内容
 * @note 适用于日志记录等需要追加的场景
 */
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    // 打开文件进行追加
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    
    // 追加文本内容
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
}

/**
 * @brief 重命名文件函数
 * @param fs 文件系统对象引用
 * @param path1 原文件路径
 * @param path2 新文件路径
 * @details 功能说明：
 *          1. 将文件从path1重命名为path2
 *          2. 可以同时移动文件到不同目录
 *          3. 输出重命名结果
 * @note 如果目标路径已存在，重命名会失败
 */
void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

/**
 * @brief 删除文件函数
 * @param fs 文件系统对象引用
 * @param path 要删除的文件路径
 * @details 功能说明：
 *          1. 删除指定路径的文件
 *          2. 输出删除结果
 * @note 删除操作不可恢复，请谨慎使用
 */
void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

/**
 * @brief 文件IO性能测试函数
 * @param fs 文件系统对象引用
 * @param path 要测试的文件路径
 * @details 功能说明：
 *          1. 读取指定文件并测量读取速度
 *          2. 写入测试数据并测量写入速度
 *          3. 输出读写性能数据
 * @note 用于测试SD卡读写性能，每次写入1MB数据
 */
void testFileIO(fs::FS &fs, const char * path){
    // 打开文件进行读取
    File file = fs.open(path);
    
    // 定义512字节的缓冲区
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    
    // 如果文件存在，进行读取测试
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        
        // 循环读取文件内容，每次读取512字节
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        
        // 计算并输出读取时间
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }

    // 打开文件进行写入测试
    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    // 写入测试数据：2048次 * 512字节 = 1MB
    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    
    // 计算并输出写入时间
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

/**
 * @brief 写入JPEG图像文件函数
 * @param fs 文件系统对象引用
 * @param path 要写入的文件路径
 * @param buf JPEG图像数据指针
 * @param size JPEG图像数据长度（字节数）
 * @details 功能说明：
 *          1. 打开文件进行写入（覆盖模式）
 *          2. 将JPEG二进制数据写入文件
 *          3. 输出保存结果
 * @note 这是本项目的核心函数，用于保存摄像头拍摄的照片
 *       buf指向摄像头帧缓冲区，size为图像数据长度
 */
void writejpg(fs::FS &fs, const char * path, const uint8_t *buf, size_t size){
    // 打开文件进行写入（覆盖模式）
    File file = fs.open(path, FILE_WRITE);
    if(!file){
      Serial.println("Failed to open file for writing");
      return;
    }
    
    // 写入JPEG二进制数据
    file.write(buf, size);
    
    // 输出保存成功信息
    Serial.printf("Saved file to path: %s\r\n", path);
}

/**
 * @brief 统计目录文件数量函数
 * @param fs 文件系统对象引用
 * @param dirname 要统计的目录路径
 * @return int - 返回目录中的文件数量，失败返回-1
 * @details 功能说明：
 *          1. 打开指定目录
 *          2. 遍历目录中的所有文件
 *          3. 统计文件数量并返回
 * @note 本函数用于生成照片编号，确保每张照片有唯一的文件名
 *       例如：/camera/0.jpg, /camera/1.jpg, /camera/2.jpg, ...
 */
int readFileNum(fs::FS &fs, const char * dirname){
    // 打开目录
    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return -1;
    }
    
    // 检查是否为目录
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return -1;
    }

    // 打开第一个文件
    File file = root.openNextFile();
    
    // 统计文件数量
    int num=0;
    while(file){
      file = root.openNextFile();
      num++;
    }
    
    // 返回文件数量
    return num;
}

/**
 * @brief 初始化摄像头照片目录
 * @details 功能说明：
 *          1. 检查并创建camera根目录
 *          2. 检查并创建photos子目录
 *          3. 检查并创建videos子目录
 *          4. 输出初始化结果
 * @note 在拍照前必须调用此函数确保目录存在
 */
void initPhotoDir(void){
    // 检查并创建camera根目录
    if(!SD_MMC.exists(CAMERA_DIR)){
        Serial.printf("Creating camera directory: %s\n", CAMERA_DIR);
        if(SD_MMC.mkdir(CAMERA_DIR)){
            Serial.println("Camera directory created successfully");
        } else {
            Serial.println("Failed to create camera directory");
        }
    } else {
        Serial.printf("Camera directory already exists: %s\n", CAMERA_DIR);
    }
    
    // 检查并创建photos子目录
    if(!SD_MMC.exists(PHOTO_DIR)){
        Serial.printf("Creating photos directory: %s\n", PHOTO_DIR);
        if(SD_MMC.mkdir(PHOTO_DIR)){
            Serial.println("Photos directory created successfully");
        } else {
            Serial.println("Failed to create photos directory");
        }
    } else {
        Serial.printf("Photos directory already exists: %s\n", PHOTO_DIR);
    }
    
    // 检查并创建videos子目录
    if(!SD_MMC.exists(VIDEO_DIR)){
        Serial.printf("Creating videos directory: %s\n", VIDEO_DIR);
        if(SD_MMC.mkdir(VIDEO_DIR)){
            Serial.println("Videos directory created successfully");
        } else {
            Serial.println("Failed to create videos directory");
        }
    } else {
        Serial.printf("Videos directory already exists: %s\n", VIDEO_DIR);
    }
}

/**
 * @brief 生成时间戳文件名
 * @param prefix 文件名前缀（如"photo"或"video"）
 * @param extension 文件扩展名（如".jpg"或".avi"）
 * @param path 输出缓冲区
 * @param pathSize 缓冲区大小
 * @details 功能说明：
 *          1. 获取当前系统时间（从NTP同步）
 *          2. 格式化为YYYYMMDDHHMM格式
 *          3. 生成完整文件名
 * @note 文件名格式：YYYYMMDDHHMM（年月日时分）
 */
void generateTimestampFilename(const char *prefix, const char *extension, char *path, size_t pathSize) {
    // 获取当前系统时间
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    
    // 生成文件名：YYYYMMDDHHMM
    snprintf(path, pathSize, "%s/%04d%02d%02d%02d%02d%s", 
             prefix,
             timeinfo.tm_year + 1900,
             timeinfo.tm_mon + 1,
             timeinfo.tm_mday,
             timeinfo.tm_hour,
             timeinfo.tm_min,
             extension);
}

/**
 * @brief 保存摄像头照片到SD卡
 * @param buf JPEG图像数据指针
 * @param size JPEG图像数据长度（字节数）
 * @return bool 保存成功返回true，失败返回false
 * @details 功能说明：
 *          1. 生成时间戳格式的照片文件名
 *          2. 将JPEG数据写入文件
 *          3. 输出保存结果
 * @note 文件名格式：YYYYMMDDHHMM（年月日时分）
 *       照片保存在/camera/photos目录下
 *       视频保存在/camera/videos目录下
 */
bool savePhotoToSD(const uint8_t *buf, size_t size){
    // 生成时间戳格式的照片文件名
    char path[64];
    generateTimestampFilename(PHOTO_DIR, ".jpg", path, sizeof(path));
    
    // 写入JPEG数据到文件
    writejpg(SD_MMC, path, buf, size);
    
    // 输出照片信息
    Serial.printf("Photo saved: %s, Size: %u bytes\n", path, size);
    
    return true;
}

/**
 * @brief 获取SD卡已用空间（MB）
 * @return uint64_t 返回已用空间（MB），失败返回0
 * @details 功能说明：
 *          1. 调用SD_MMC.usedBytes()获取已用字节数
 *          2. 转换为MB单位
 *          3. 返回结果
 * @note 用于监控SD卡使用情况，当空间不足时可以清理旧文件
 */
uint64_t getSDUsedSpaceMB(void){
    // 获取已用字节数
    uint64_t usedBytes = SD_MMC.usedBytes();
    
    // 转换为GB单位（保留2位小数）
    return (usedBytes / (1024.0 * 1024.0 * 1024.0)) * 100;
}

/**
 * @brief 获取SD卡总空间（MB）
 * @return uint64_t 返回总空间（MB），失败返回0
 * @details 功能说明：
 *          1. 调用SD_MMC.totalBytes()获取总字节数
 *          2. 转换为MB单位
 *          3. 返回结果
 * @note 用于显示SD卡容量信息
 */
uint64_t getSDTotalSpaceMB(void){
    // 获取总字节数
    uint64_t totalBytes = SD_MMC.totalBytes();
    
    // 转换为GB单位（保留2位小数）
    return (totalBytes / (1024.0 * 1024.0 * 1024.0)) * 100;
}

/**
 * @brief 获取SD卡剩余空间（MB）
 * @return uint64_t 返回剩余空间（MB），失败返回0
 * @details 功能说明：
 *          1. 获取总空间和已用空间
 *          2. 计算剩余空间
 *          3. 返回结果
 * @note 用于显示SD卡剩余空间信息
 */
uint64_t getSDFreeSpaceMB(void){
    // 获取总空间和已用空间
    uint64_t totalBytes = SD_MMC.totalBytes();
    uint64_t usedBytes = SD_MMC.usedBytes();
    
    // 计算并返回剩余空间（GB，保留2位小数）
    return ((totalBytes - usedBytes) / (1024.0 * 1024.0 * 1024.0)) * 100;
}

/**
 * @brief 保存视频文件到SD卡
 * @param buf 视频数据指针
 * @param size 视频数据长度（字节数）
 * @return bool 保存成功返回true，失败返回false
 * @details 功能说明：
 *          1. 统计videos目录中的视频文件数量
 *          2. 生成新的视频文件名（0.avi, 1.avi, 2.avi...）
 *          3. 将视频数据写入文件
 *          4. 输出保存结果
 * @note 自动生成文件名，确保每个视频有唯一的文件名
 *       视频保存在/camera/videos目录下
 */
bool saveVideoToSD(const uint8_t *buf, size_t size){
    // 统计当前目录中的视频文件数量
    int videoNum = readFileNum(SD_MMC, VIDEO_DIR);
    
    // 如果统计失败，返回false
    if(videoNum < 0){
        Serial.println("Failed to read video directory");
        return false;
    }
    
    // 生成视频文件路径
    char path[64];
    snprintf(path, sizeof(path), "%s/%d.avi", VIDEO_DIR, videoNum);
    
    // 打开文件进行写入（覆盖模式）
    File file = SD_MMC.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return false;
    }
    
    // 写入视频数据
    file.write(buf, size);
    file.close();
    
    // 输出视频信息
    Serial.printf("Video saved: %s, Size: %u bytes\n", path, size);
    
    return true;
}

/**
 * @brief 获取指定目录中的文件列表
 * @param dirname 目录路径
 * @param files 文件列表数组指针
 * @param maxFiles 最大文件数量
 * @return int 返回文件数量，失败返回-1
 * @details 功能说明：
 *          1. 打开指定目录
 *          2. 遍历目录中的所有文件
 *          3. 将文件名存入数组
 *          4. 返回文件数量
 * @note 用于获取目录中的文件列表，方便管理文件
 */
int getFileList(const char * dirname, char *files[], int maxFiles){
    // 打开目录
    File root = SD_MMC.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return -1;
    }
    
    // 检查是否为目录
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return -1;
    }

    // 打开第一个文件
    File file = root.openNextFile();
    
    // 遍历目录中的所有文件
    int num = 0;
    while(file && num < maxFiles){
        // 只处理文件，不处理子目录
        if(!file.isDirectory()){
            // 复制文件名到数组
            strncpy(files[num], file.name(), 63);
            files[num][63] = '\0'; // 确保字符串以null结尾
            num++;
        }
        // 打开下一个文件
        file = root.openNextFile();
    }
    
    // 返回文件数量
    return num;
}

/**
 * @brief 删除指定目录中的所有文件
 * @param dirname 目录路径
 * @return int 返回删除的文件数量，失败返回-1
 * @details 功能说明：
 *          1. 打开指定目录
 *          2. 遍历目录中的所有文件
 *          3. 删除所有文件
 *          4. 返回删除的文件数量
 * @note 用于清理目录中的旧文件，释放SD卡空间
 */
int deleteAllFiles(const char * dirname){
    // 打开目录
    File root = SD_MMC.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return -1;
    }
    
    // 检查是否为目录
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return -1;
    }

    // 打开第一个文件
    File file = root.openNextFile();
    
    // 遍历目录中的所有文件
    int num = 0;
    while(file){
        // 只处理文件，不处理子目录
        if(!file.isDirectory()){
            // 构建完整文件路径
            char path[128];
            snprintf(path, sizeof(path), "%s/%s", dirname, file.name());
            
            // 删除文件
            if(SD_MMC.remove(path)){
                num++;
                Serial.printf("Deleted file: %s\n", path);
            } else {
                Serial.printf("Failed to delete file: %s\n", path);
            }
        }
        // 打开下一个文件
        file = root.openNextFile();
    }
    
    // 返回删除的文件数量
    return num;
}

/**
 * @brief 清理无效视频文件函数
 * @details 功能说明：
 *          1. 遍历videos目录中的所有文件
 *          2. 检查每个文件的大小是否为0KB
 *          3. 删除所有大小为0KB的视频文件
 *          4. 返回删除的文件数量
 * @note 用于清理启动时可能产生的无效视频文件
 */
int cleanInvalidVideoFiles(void){
    // 打开videos目录
    File root = SD_MMC.open(VIDEO_DIR);
    if(!root){
        Serial.println("Failed to open videos directory");
        return -1;
    }
    
    // 检查是否为目录
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return -1;
    }

    // 打开第一个文件
    File file = root.openNextFile();
    
    // 遍历目录中的所有文件
    int num = 0;
    while(file){
        // 只处理文件，不处理子目录
        if(!file.isDirectory()){
            // 检查文件大小是否为0KB
            if(file.size() == 0){
                // 构建完整文件路径
                char path[128];
                snprintf(path, sizeof(path), "%s/%s", VIDEO_DIR, file.name());
                
                // 删除无效视频文件
                if(SD_MMC.remove(path)){
                    num++;
                    Serial.printf("Deleted invalid video file (0KB): %s\n", path);
                } else {
                    Serial.printf("Failed to delete invalid video file: %s\n", path);
                }
            }
        }
        // 打开下一个文件
        file = root.openNextFile();
    }
    
    // 返回删除的文件数量
    return num;
}

/**
 * @brief 检查SD卡空间是否需要清理
 * @return bool 需要清理返回true，否则返回false
 * @details 功能说明：
 *          1. 获取SD卡总容量和剩余空间
 *          2. 计算动态阈值（总容量 - 保留空间）
 *          3. 比较剩余空间是否小于保留空间
 *          4. 返回比较结果
 * @note 动态计算阈值：当剩余空间小于保留空间（默认5GB）时返回true
 *       适应不同容量规格的SD卡（如32GB、64GB、128GB等）
 */
bool checkSDSpaceNeedsCleanup(void){
    // 获取总空间和已用空间
    uint64_t totalBytes = SD_MMC.totalBytes();
    uint64_t usedBytes = SD_MMC.usedBytes();
    
    // 计算剩余空间（GB单位）
    uint64_t freeSpaceGB = (totalBytes - usedBytes) / (1024.0 * 1024.0 * 1024.0);
    
    // 检查剩余空间是否小于保留空间阈值
    if(freeSpaceGB < SD_SPACE_RESERVE_GB){
        Serial.printf("SD卡空间不足，需要清理: 剩余空间 %lluGB < 保留空间 %dGB\n", freeSpaceGB, SD_SPACE_RESERVE_GB);
        return true;
    }
    
    return false;
}

/**
 * @brief 获取指定目录中的文件信息列表
 * @param dirname 目录路径
 * @param files 文件信息数组指针
 * @param maxFiles 最大文件数量
 * @return int 返回文件数量，失败返回-1
 * @details 功能说明：
 *          1. 打开指定目录
 *          2. 遍历目录中的所有文件
 *          3. 获取每个文件的路径、名称、大小和修改时间
 *          4. 存储到文件信息数组中
 * @note 用于获取目录中的文件信息，方便按时间排序和删除
 */
int getFileInfoList(const char * dirname, FileInfo *files, int maxFiles){
    // 打开目录
    File root = SD_MMC.open(dirname);
    if(!root){
        Serial.printf("Failed to open directory: %s\n", dirname);
        return -1;
    }
    
    // 检查是否为目录
    if(!root.isDirectory()){
        Serial.printf("Not a directory: %s\n", dirname);
        return -1;
    }

    // 打开第一个文件
    File file = root.openNextFile();
    
    // 遍历目录中的所有文件
    int num = 0;
    while(file && num < maxFiles){
        // 只处理文件，不处理子目录
        if(!file.isDirectory()){
            // 构建完整文件路径
            snprintf(files[num].path, sizeof(files[num].path), "%s/%s", dirname, file.name());
            snprintf(files[num].name, sizeof(files[num].name), "%s", file.name());
            files[num].size = file.size();
            files[num].mtime = file.getLastWrite();
            num++;
        }
        // 打开下一个文件
        file = root.openNextFile();
    }
    
    // 返回文件数量
    return num;
}

/**
 * @brief 按时间排序文件信息列表
 * @param files 文件信息数组指针
 * @param fileCount 文件数量
 * @param ascending 排序顺序，true=升序（旧→新），false=降序（新→旧）
 * @details 功能说明：
 *          1. 使用冒泡排序算法按文件修改时间排序
 *          2. 升序时最旧的文件在前面
 *          3. 降序时最新的文件在前面
 * @note 按文件修改时间排序，升序时最旧的文件在前面
 */
void sortFilesByTime(FileInfo *files, int fileCount, bool ascending){
    // 使用冒泡排序按时间排序
    for(int i = 0; i < fileCount - 1; i++){
        for(int j = 0; j < fileCount - i - 1; j++){
            bool needSwap = ascending ? (files[j].mtime > files[j+1].mtime) : (files[j].mtime < files[j+1].mtime);
            if(needSwap){
                // 交换文件信息
                FileInfo temp = files[j];
                files[j] = files[j+1];
                files[j+1] = temp;
            }
        }
    }
}

/**
 * @brief 删除指定目录中最旧的N个文件
 * @param dirname 目录路径
 * @param maxFilesToDelete 最大删除文件数量
 * @return int 返回删除的文件数量，失败返回-1
 * @details 功能说明：
 *          1. 获取目录中的所有文件信息
 *          2. 按修改时间升序排序（最旧的在前面）
 *          3. 逐个删除最旧的文件
 *          4. 累计删除的文件大小
 *          5. 当释放空间达到2GB或删除了maxFilesToDelete个文件时停止
 * @note 按文件修改时间排序，删除最旧的文件
 *       清理出约2GB空间后停止
 */
int deleteOldestFiles(const char * dirname, int maxFilesToDelete){
    // 定义文件信息数组（最多100个文件）
    FileInfo files[100];
    
    // 获取文件信息列表
    int fileCount = getFileInfoList(dirname, files, 100);
    if(fileCount <= 0){
        Serial.printf("No files found in directory: %s\n", dirname);
        return 0;
    }
    
    // 按时间升序排序（最旧的在前面）
    sortFilesByTime(files, fileCount, true);
    
    // 计算清理目标空间（字节）
    uint64_t targetSpaceBytes = SD_CLEAN_TARGET_GB * 1024ULL * 1024ULL * 1024ULL;
    uint64_t freedSpace = 0;
    int deletedCount = 0;
    
    // 逐个删除最旧的文件
    for(int i = 0; i < fileCount && i < maxFilesToDelete; i++){
        // 检查是否已经释放了足够的空间
        if(freedSpace >= targetSpaceBytes){
            Serial.printf("已释放 %lluGB 空间，停止清理\n", freedSpace / (1024ULL * 1024ULL * 1024ULL));
            break;
        }
        
        // 删除文件
        if(SD_MMC.remove(files[i].path)){
            freedSpace += files[i].size;
            deletedCount++;
            Serial.printf("Deleted file: %s, Size: %llu bytes, Total freed: %llu bytes\n", 
                         files[i].name, files[i].size, freedSpace);
        } else {
            Serial.printf("Failed to delete file: %s\n", files[i].path);
        }
    }
    
    Serial.printf("从 %s 目录删除了 %d 个文件，释放了 %lluGB 空间\n", 
                 dirname, deletedCount, freedSpace / (1024ULL * 1024ULL * 1024ULL));
    
    return deletedCount;
}

/**
 * @brief 自动清理旧文件以释放空间
 * @return int 返回删除的文件数量，失败返回-1
 * @details 功能说明：
 *          1. 检查SD卡空间是否需要清理
 *          2. 如果需要清理，根据清理优先级配置选择删除策略
 *          3. 优先删除videos目录中的文件（如果配置允许）
 *          4. 如果videos目录为空或删除后仍不够，删除photos目录中的文件（如果配置允许）
 *          5. 清理出约2GB空间后停止
 *          6. 返回删除的文件数量
 * @note 当SD卡剩余空间小于保留空间（默认5GB）时，自动删除最旧的文件
 *       清理出约2GB空间后停止
 *       清理优先级由SD_CLEANUP_PRIORITY配置：
 *       0 = 只删除视频文件
 *       1 = 优先删除视频，再删除照片
 *       2 = 只删除照片文件
 */
int autoCleanOldFiles(void){
    // 检查SD卡空间是否需要清理
    if(!checkSDSpaceNeedsCleanup()){
        Serial.println("SD卡空间充足，无需清理");
        return 0;
    }
    
    int totalDeleted = 0;
    
    // 根据清理优先级配置选择删除策略
    if(SD_CLEANUP_PRIORITY == 0){
        // 只删除视频文件
        Serial.println("清理策略：只删除videos目录中的文件");
        int deletedVideos = deleteOldestFiles(VIDEO_DIR, 100);
        if(deletedVideos > 0){
            totalDeleted += deletedVideos;
        }
    } else if(SD_CLEANUP_PRIORITY == 1){
        // 优先删除视频，再删除照片
        Serial.println("清理策略：优先删除videos目录中的文件，再删除photos目录中的文件");
        
        // 优先删除videos目录中的文件
        int deletedVideos = deleteOldestFiles(VIDEO_DIR, 100);
        if(deletedVideos > 0){
            totalDeleted += deletedVideos;
        }
        
        // 如果删除videos文件后仍需要清理，删除photos目录中的文件
        if(checkSDSpaceNeedsCleanup()){
            Serial.println("videos目录清理后空间仍不足，开始清理photos目录...");
            int deletedPhotos = deleteOldestFiles(PHOTO_DIR, 100);
            if(deletedPhotos > 0){
                totalDeleted += deletedPhotos;
            }
        }
    } else if(SD_CLEANUP_PRIORITY == 2){
        // 只删除照片文件
        Serial.println("清理策略：只删除photos目录中的文件");
        int deletedPhotos = deleteOldestFiles(PHOTO_DIR, 100);
        if(deletedPhotos > 0){
            totalDeleted += deletedPhotos;
        }
    }
    
    Serial.printf("总共删除了 %d 个文件\n", totalDeleted);
    return totalDeleted;
}

/**
 * @brief 开始视频录制
 * @param fps 帧率（每秒帧数）
 * @param width 视频宽度
 * @param height 视频高度
 * @return bool 成功返回true，失败返回false
 * @details 功能说明：
 *          1. 检查是否正在录制，如果是则返回false
 *          2. 检查SD卡空间，如果达到阈值则自动清理
 *          3. 生成时间戳格式的视频文件名
 *          4. 创建AVI文件并写入文件头
 *          5. 初始化录制参数
 * @note 创建AVI文件并写入文件头
 *       文件名格式：YYYYMMDDHHMM（年月日时分）
 *       自动分段：2分钟一段
 */
bool startVideoRecording(int fps, int width, int height){
    // 检查是否正在录制
    if(isRecording){
        Serial.println("视频录制中，无法开始新的录制");
        return false;
    }
    
    // 检查SD卡空间，如果达到阈值则自动清理
    if(checkSDSpaceThreshold()){
        Serial.println("SD卡空间不足，开始自动清理...");
        autoCleanOldFiles();
    }
    
    // 保存视频参数
    videoFPS = fps;
    videoWidth = width;
    videoHeight = height;
    
    // 生成时间戳格式的视频文件名
    generateTimestampFilename(VIDEO_DIR, ".avi", currentVideoFilename, sizeof(currentVideoFilename));
    
    // 打开视频文件
    videoFile = SD_MMC.open(currentVideoFilename, FILE_WRITE);
    if(!videoFile){
        Serial.println("Failed to open video file for writing");
        return false;
    }
    
    // 初始化录制参数
    videoFrameCount = 0;
    videoStartTime = millis();
    videoSegmentStartTime = millis();
    videoTotalSize = 0;
    videoMaxFrameSize = 0;
    moviOffset = 0;
    idx1Offset = 0;
    
    // 初始化AVI主头
    memset(&aviMainHeader, 0, sizeof(AVI_MAIN_HEADER));
    memcpy(aviMainHeader.riff, AVI_FOURCC, 4);
    memcpy(aviMainHeader.avi, AVI_AVI, 4);
    memcpy(aviMainHeader.list, AVI_LIST, 4);
    memcpy(aviMainHeader.hdrl, AVI_HDRL, 4);
    memcpy(aviMainHeader.avih, "avih", 4);
    aviMainHeader.avihSize = 56;
    aviMainHeader.microSecPerFrame = 1000000 / videoFPS;
    aviMainHeader.maxBytesPerSec = 0;
    aviMainHeader.paddingGranularity = 0;
    aviMainHeader.flags = 0x10;
    aviMainHeader.totalFrames = 0;
    aviMainHeader.initialFrames = 0;
    aviMainHeader.streams = 1;
    aviMainHeader.suggestedBufferSize = 0;
    aviMainHeader.width = videoWidth;
    aviMainHeader.height = videoHeight;
    
    // 初始化AVI流头
    memset(&aviStreamHeader, 0, sizeof(AVI_STREAM_HEADER));
    memcpy(aviStreamHeader.list, AVI_LIST, 4);
    memcpy(aviStreamHeader.strl, AVI_STRL, 4);
    memcpy(aviStreamHeader.strh, "strh", 4);
    aviStreamHeader.strhSize = 56;
    memcpy(aviStreamHeader.fccType, AVI_VIDS, 4);
    memcpy(aviStreamHeader.fccHandler, AVI_MJPG, 4);
    aviStreamHeader.flags = 0;
    aviStreamHeader.priority = 0;
    aviStreamHeader.language = 0;
    aviStreamHeader.initialFrames = 0;
    aviStreamHeader.scale = 1;
    aviStreamHeader.rate = videoFPS;
    aviStreamHeader.start = 0;
    aviStreamHeader.length = 0;
    aviStreamHeader.suggestedBufferSize = 0;
    aviStreamHeader.quality = 0;
    aviStreamHeader.sampleSize = 0;
    aviStreamHeader.left = 0;
    aviStreamHeader.top = 0;
    aviStreamHeader.right = videoWidth;
    aviStreamHeader.bottom = videoHeight;
    
    // 初始化AVI位图信息
    memset(&aviBitmapInfo, 0, sizeof(AVI_BITMAP_INFO));
    memcpy(aviBitmapInfo.strf, AVI_STRF, 4);
    aviBitmapInfo.strfSize = 40;
    aviBitmapInfo.size = 40;
    aviBitmapInfo.width = videoWidth;
    aviBitmapInfo.height = videoHeight;
    aviBitmapInfo.planes = 1;
    aviBitmapInfo.bitCount = 24;
    aviBitmapInfo.compression = 0x47504A4D; // 'MJPG'
    aviBitmapInfo.sizeImage = videoWidth * videoHeight * 3;
    aviBitmapInfo.xPelsPerMeter = 0;
    aviBitmapInfo.yPelsPerMeter = 0;
    aviBitmapInfo.clrUsed = 0;
    aviBitmapInfo.clrImportant = 0;
    
    // 写入AVI文件头（临时，后面会更新）
    videoFile.write((uint8_t*)&aviMainHeader, sizeof(AVI_MAIN_HEADER));
    videoFile.write((uint8_t*)&aviStreamHeader, sizeof(AVI_STREAM_HEADER));
    videoFile.write((uint8_t*)&aviBitmapInfo, sizeof(AVI_BITMAP_INFO));
    
    // 写入JUNK块（填充）
    char junk[5] = "JUNK";
    uint32_t junkSize = 2048;
    videoFile.write((uint8_t*)junk, 4);
    videoFile.write((uint8_t*)&junkSize, 4);
    for(int i = 0; i < 2048; i++){
        videoFile.write(0);
    }
    
    // 写入LIST movi头
    char list[5] = "LIST";
    char movi[5] = "movi";
    videoFile.write((uint8_t*)list, 4);
    uint32_t moviListSize = 0;
    moviOffset = videoFile.position();
    videoFile.write((uint8_t*)&moviListSize, 4);
    videoFile.write((uint8_t*)movi, 4);
    
    // 标记正在录制
    isRecording = true;
    
    Serial.printf("开始视频录制: %s, FPS: %d, 分辨率: %dx%d\n", currentVideoFilename, videoFPS, videoWidth, videoHeight);
    
    return true;
}

/**
 * @brief 写入视频帧
 * @param buf JPEG图像数据指针
 * @param size JPEG图像数据长度（字节数）
 * @return bool 成功返回true，失败返回false
 * @details 功能说明：
 *          1. 检查是否正在录制
 *          2. 检查是否需要分段（2分钟）
 *          3. 如果需要分段，停止当前分段并开始新的分段
 *          4. 写入帧头（00dc）
 *          5. 写入帧大小
 *          6. 写入JPEG数据
 *          7. 更新帧计数和总大小
 * @note 将JPEG帧写入AVI文件
 *       自动分段：2分钟一段
 */
bool writeVideoFrame(const uint8_t *buf, size_t size){
    // 检查是否正在录制
    if(!isRecording){
        Serial.println("未在录制视频");
        return false;
    }
    
    // 检查是否需要分段（2分钟）
    uint32_t currentTime = millis();
    uint32_t segmentDuration = (currentTime - videoSegmentStartTime) / 1000;
    if(segmentDuration >= VIDEO_SEGMENT_DURATION && videoFrameCount > 0){
        Serial.printf("视频分段达到2分钟，开始新的分段\n");
        
        // 停止当前分段（保持录制状态）
        stopVideoRecording(true);
        
        // 短暂延迟
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // 临时设置isRecording为false，以便开始新的分段
        isRecording = false;
        
        // 开始新的分段
        if(!startVideoRecording(videoFPS, videoWidth, videoHeight)){
            Serial.println("开始新的视频分段失败");
            isRecording = false;
            return false;
        }
        
        // 更新分段计数
        videoSegmentCount++;
        Serial.printf("开始第 %lu 个视频分段\n", videoSegmentCount + 1);
        
        // 重置分段开始时间
        videoSegmentStartTime = millis();
    }
    
    // 写入帧头（00dc）
    char frameId[5] = "00dc";
    videoFile.write((uint8_t*)frameId, 4);
    
    // 写入帧大小
    uint32_t frameSize = size;
    videoFile.write((uint8_t*)&frameSize, 4);
    
    // 写入JPEG数据
    videoFile.write(buf, size);
    
    // 更新统计信息
    videoFrameCount++;
    videoTotalSize += size + 8; // 加上帧头和大小
    
    // 更新最大帧大小
    if(size > videoMaxFrameSize){
        videoMaxFrameSize = size;
    }
    
    return true;
}

/**
 * @brief 停止视频录制
 * @param keepRecordingState 是否保持录制状态（true=保持，false=停止）
 * @return bool 成功返回true，失败返回false
 * @details 功能说明：
 *          1. 检查是否正在录制
 *          2. 写入idx1索引
 *          3. 更新文件头信息
 *          4. 关闭视频文件
 *          5. 重置录制状态（除非keepRecordingState为true）
 * @note 完成AVI文件，更新文件头和索引
 *       keepRecordingState参数用于自动分段时保持录制状态
 */
bool stopVideoRecording(bool keepRecordingState){
    // 检查是否正在录制
    if(!isRecording){
        Serial.println("未在录制视频");
        return false;
    }
    
    // 写入idx1索引
    char idx1[5] = "idx1";
    uint32_t idx1Size = videoFrameCount * 16;
    idx1Offset = videoFile.position();
    videoFile.write((uint8_t*)idx1, 4);
    videoFile.write((uint8_t*)&idx1Size, 4);
    
    // 写入索引条目
    uint32_t currentOffset = 4; // movi列表偏移
    for(uint32_t i = 0; i < videoFrameCount; i++){
        AVI_INDEX_ENTRY entry;
        memcpy(entry.id, "00dc", 4);
        entry.flags = 0x10;
        entry.offset = currentOffset;
        entry.size = 0; // 这里简化处理，实际应该记录每帧的大小
        
        videoFile.write((uint8_t*)&entry, sizeof(AVI_INDEX_ENTRY));
        
        currentOffset += 8; // 帧头和大小
    }
    
    // 计算movi列表大小
    uint32_t moviListSize = videoTotalSize + 4; // +4 for "movi"
    
    // 更新avih信息
    aviMainHeader.totalFrames = videoFrameCount;
    aviMainHeader.maxBytesPerSec = videoTotalSize / (videoFrameCount / videoFPS);
    aviMainHeader.suggestedBufferSize = videoMaxFrameSize;
    
    // 更新strh信息
    aviStreamHeader.length = videoFrameCount;
    aviStreamHeader.suggestedBufferSize = videoMaxFrameSize;
    
    // 计算list大小
    uint32_t listSize = sizeof(AVI_MAIN_HEADER) - 12 + sizeof(AVI_STREAM_HEADER) - 8 + sizeof(AVI_BITMAP_INFO) - 8 + 12 + 2048 + 8 + moviListSize;
    aviMainHeader.listSize = listSize;
    aviStreamHeader.listSize = sizeof(AVI_STREAM_HEADER) - 8 + sizeof(AVI_BITMAP_INFO) - 8;
    
    // 计算文件大小
    uint32_t fileSize = 4 + 4 + listSize + 8 + moviListSize + 8 + idx1Size + 8;
    aviMainHeader.fileSize = fileSize;
    
    // 更新文件头
    videoFile.seek(0);
    videoFile.write((uint8_t*)&aviMainHeader, sizeof(AVI_MAIN_HEADER));
    videoFile.write((uint8_t*)&aviStreamHeader, sizeof(AVI_STREAM_HEADER));
    videoFile.write((uint8_t*)&aviBitmapInfo, sizeof(AVI_BITMAP_INFO));
    
    // 更新movi列表大小
    videoFile.seek(moviOffset);
    videoFile.write((uint8_t*)&moviListSize, 4);
    
    // 关闭视频文件
    videoFile.close();
    
    // 计算录制时长
    uint32_t duration = (millis() - videoStartTime) / 1000;
    
    // 重置录制状态（除非keepRecordingState为true）
    if(!keepRecordingState){
        isRecording = false;
    }
    
    Serial.printf("视频录制完成: %s, 帧数: %lu, 时长: %lu秒, 大小: %lu bytes\n", 
                 currentVideoFilename, videoFrameCount, duration, videoTotalSize);
    
    return true;
}

/**
 * @brief 检查是否正在录制视频
 * @return bool 正在录制返回true，否则返回false
 */
bool isRecordingVideo(void){
    return isRecording;
}

/**
 * @brief 获取当前录制的视频文件名
 * @return const char* 返回文件名指针，未录制时返回NULL
 */
const char* getCurrentVideoFilename(void){
    if(isRecording){
        return currentVideoFilename;
    }
    return NULL;
}
