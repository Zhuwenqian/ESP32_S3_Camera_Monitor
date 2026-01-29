/**********************************************************************
  文件名称    : sd_read_write.cpp
  文件用途    : SD_MMC存储卡读写功能实现
               本文件实现了SD卡的初始化、文件读写、目录操作等功能
               主要功能包括：
               1. SD_MMC卡初始化和检测
               2. 目录创建、删除、列表显示
               3. 文件创建、读取、写入、删除、重命名
               4. JPEG图像文件写入
               5. 目录文件计数
  作者        : www.freenove.com
  修改日期    : 2022/11/02
  硬件平台    : ESP32S3-EYE开发板
  依赖库      : FS.h - 文件系统基础库
               SD_MMC.h - SD_MMC驱动库
  引脚说明    : SD_MMC_CLK (GPIO39) - SD卡时钟线（不可修改）
               SD_MMC_CMD (GPIO38) - SD卡命令线（不可修改）
               SD_MMC_D0  (GPIO40) - SD卡数据线0（不可修改）
  使用说明    : 1. 调用sdmmcInit()初始化SD卡
               2. 使用文件操作函数进行读写
               3. SD卡使用SDIO模式，4位并行传输，速度快
  参数调整    : SDMMC_FREQ_DEFAULT - SD卡通信频率
               调整建议：使用Class 10以上SD卡以获得最佳性能
**********************************************************************/
#include "sd_read_write.h"

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
  if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
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
