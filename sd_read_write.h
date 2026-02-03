/**********************************************************************
  文件名称 / Filename : sd_read_write.h
  文件用途 / File Purpose : SD_MMC存储卡读写功能头文件 / SD_MMC Storage Card Read/Write Header File
               声明了SD卡操作相关的函数原型和宏定义
               Declares function prototypes and macro definitions related to SD card operations
  作者 / Author : www.freenove.com
  修改日期 / Modification Date : 2026-02-03
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : Arduino.h - Arduino核心库 / Arduino Core Library
               FS.h - 文件系统基础库 / File System Base Library
               SD_MMC.h - SD_MMC驱动库 / SD_MMC Driver Library
  引脚说明 / Pin Description : SD_MMC_CMD (GPIO38) - SD卡命令线（固定）/ SD card command line (fixed)
               SD_MMC_CLK (GPIO39) - SD卡时钟线（固定）/ SD card clock line (fixed)
               SD_MMC_D0  (GPIO40) - SD卡数据线（固定）/ SD card data line (fixed)
  使用说明 / Usage Instructions : 1. 包含本头文件 #include "sd_read_write.h" / Include this header file
               2. 调用sdmmcInit()初始化SD卡 / Call sdmmcInit() to initialize SD card
               3. 使用提供的函数进行文件操作 / Use provided functions for file operations
  注意事项 / Important Notes : 这些引脚是ESP32S3-EYE开发板固定的SDIO接口 / These pins are the fixed SDIO interface of ESP32S3-EYE development board
               请勿修改，否则可能导致SD卡无法正常工作 / Do not modify, otherwise SD card may not work properly
**********************************************************************/

#ifndef __SD_READ_WRITE_H
#define __SD_READ_WRITE_H

#include "Arduino.h"
#include "FS.h"
#include "SD_MMC.h"

// SD_MMC引脚定义 - 这些引脚是ESP32S3-EYE开发板固定的SDIO接口 / SD_MMC pin definitions - These pins are fixed SDIO interface of ESP32S3-EYE development board
// 请勿修改这些值，否则可能导致SD卡无法正常工作
#define SD_MMC_CMD  38  // SD卡命令线（Command），GPIO38
#define SD_MMC_CLK  39  // SD卡时钟线（Clock），GPIO39
#define SD_MMC_D0   40  // SD卡数据线0（Data 0），GPIO40

// SD卡挂载点路径 / SD card mount point path
#define SD_MOUNT_POINT "/sdcard"

// 摄像头根目录 / Camera root directory
#define CAMERA_DIR "/camera"

// 照片保存目录 / Photo save directory
#define PHOTO_DIR "/camera/photos"

// 视频保存目录 / Video save directory
#define VIDEO_DIR "/camera/videos"

// AVI文件相关定义 / AVI file related definitions
#define AVI_FOURCC "RIFF"
#define AVI_AVI "AVI "
#define AVI_LIST "LIST"
#define AVI_HDRL "hdrl"
#define AVI_STRL "strl"
#define AVI_VIDS "vids"
#define AVI_MJPG "MJPG"
#define AVI_STRF "strf"
#define AVI_MOVI "movi"
#define AVI_IDX1 "idx1"
#define AVI_00DC "00dc"
#define AVI_00DB "00db"

// AVI文件头结构体 / AVI file header structure
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t fileSize;      // 文件总大小（不包括前8字节）/ Total file size (excluding first 8 bytes)
    char avi[4];            // "AVI "
    char list[4];           // "LIST"
    uint32_t listSize;      // hdrl列表大小 / hdrl list size
    char hdrl[4];           // "hdrl"
    char avih[4];           // "avih"
    uint32_t avihSize;      // avih块大小（56字节）/ avih chunk size (56 bytes)
    uint32_t microSecPerFrame; // 每帧微秒数 / Microseconds per frame
    uint32_t maxBytesPerSec;   // 最大每秒字节数 / Maximum bytes per second
    uint32_t paddingGranularity; // 填充粒度 / Padding granularity
    uint32_t flags;         // 标志 / Flags / Flags
    uint32_t totalFrames;   // 总帧数 / Total frames
    uint32_t initialFrames; // 初始帧 / Initial frames数 / Initial frames
    uint32_t streams;       // 流数量 / Number of streams
    uint32_t suggestedBufferSize; // 建议缓冲区大小 / Suggested buffer size
    uint32_t width;         // 宽度 / Width
    uint32_t height;        // 高度 / Height
    uint32_t reserved[4];   // 保留 / Reserved
} AVI_MAIN_HEADER;

// 视频流头结构体 / Video stream header structure
typedef struct {
    char list[4];           // "LIST"
    uint32_t listSize;      // strl列表大小
    char strl[4];           // "strl"
    char strh[4];           // "strh"
    uint32_t strhSize;      // strh块大小（56字节）
    char fccType[4];        // "vids"
    char fccHandler[4];     // "MJPG"
    uint32_t flags;         // 标志 / Flags / Flags
    uint16_t priority;      // 优先级 / Priority
    uint16_t language;      // 语言 / Language
    uint32_t initialFrames; // 初始帧 / Initial frames
    uint32_t scale;         // 时间刻度 / Time scale
    uint32_t rate;          // 速率 / Rate
    uint32_t start;         // 开始时间 / Start time
    uint32_t length;        // 长度 / Length
    uint32_t suggestedBufferSize; // 建议缓冲区大小 / Suggested buffer size
    uint32_t quality;       // 质量
    uint32_t sampleSize;    // 样本大小
    uint16_t left;          // 左边距 / Left margin
    uint16_t top;           // 上边距 / Top margin
    uint16_t right;         // 右边距 / Right margin
    uint16_t bottom;        // 下边距 / Bottom margin
} AVI_STREAM_HEADER;

// 位图信息头结构体 / Bitmap info header structure
typedef struct {
    char strf[4];           // "strf"
    uint32_t strfSize;      // strf块大小（40字节）
    uint32_t size;          // 结构体大小（40字节）/ Structure size (40 bytes)
    int32_t width;          // 宽度 / Width
    int32_t height;         // 高度 / Height
    uint16_t planes;        // 平面数 / Number of planes
    uint16_t bitCount;      // 每像素位数 / Bits per pixel
    uint32_t compression;   // 压缩方式 / Compression method
    uint32_t sizeImage;      // 图像大小 / Image size
    int32_t xPelsPerMeter;  // 水平分辨率 / Horizontal resolution
    int32_t yPelsPerMeter;  // 垂直分辨率 / Vertical resolution
    uint32_t clrUsed;       // 使用的颜色数 / Number of colors used
    uint32_t clrImportant;  // 重要颜色数 / Number of important colors
} AVI_BITMAP_INFO;

// 索引条目结构体 / Index entry structure
typedef struct {
    char id[4];             // "00dc"或"00db" / "00dc" or "00db"
    uint32_t flags;         // 标志 / Flags / Flags
    uint32_t offset;        // 偏移量 / Offset
    uint32_t size;          // 大小 / Size
} AVI_INDEX_ENTRY;

// SD卡空间管理配置 / SD card space management configuration
#define SD_SPACE_RESERVE_GB 5           // 保留空间阈值（GB），当剩余空间小于此值时触发清理 / Reserved space threshold (GB), triggers cleanup when free space is less than this value
#define SD_CLEAN_TARGET_GB 2            // 清理目标空间（GB），每次清理释放约2GB空间 / Cleanup target space (GB), releases approximately 2GB space per cleanup
#define SD_SPACE_CHECK_INTERVAL_MS 5000 // 空间检测间隔（毫秒），默认5秒检测一次 / Space check interval (ms), default 5 seconds

// 清理优先级配置 / Cleanup priority configuration
// 0 = 只删除视频文件 / Only delete video files
// 1 = 优先删除视频，再删除照片 / Prioritize videos, then photos
// 2 = 只删除照片文件 / Only delete photo files
#define SD_CLEANUP_PRIORITY 1           // 清理优先级，默认1（优先删除视频，再删除照片）/ Cleanup priority, default 1 (prioritize videos, then photos)

// 文件信息结构体 / File information structure
typedef struct {
    char path[128];          // 文件路径 / File path
    char name[64];           // 文件名 / File name
    uint64_t size;           // 文件大小（字节）/ File size (bytes)
    time_t mtime;            // 修改时间（时间戳）/ Modification time (timestamp)
} FileInfo;

/**
 * @brief SD_MMC存储卡初始化函数 / SD_MMC storage card initialization function
 * @details 初始化SD_MMC接口，挂载文件系统，检测SD卡信息 / Initialize SD_MMC interface, mount file system, detect SD card information
 * @note 必须在使用其他SD卡函数之前调用 / Must be called before using other SD card functions
 */
void sdmmcInit(void); 

/**
 * @brief 生成时间戳文件名
 * @param prefix 文件名前缀（如"photo"或"video"）
 * @param extension 文件扩展名（如".jpg"或".avi"）
 * @param path 输出缓冲区
 * @param pathSize 缓冲区大小
 * @details 功能说明 / Function description:
 *          1. 获取当前时间（从millis()计算）/ 1. Get current time (calculated from millis())
 *          2. 格式化为ESP32-xxxx.xx.xx.xx格式 / 2. Format as ESP32-xxxx.xx.xx.xx
 *          3. 生成完整文件名 / 3. Generate complete filename
 * @note 文件名格式：ESP32-xxxx.xx.xx.xx（xxxx为启动后的秒数，xx.xx.xx为时分秒）/ Filename format: ESP32-xxxx.xx.xx.xx (xxxx is seconds after boot, xx.xx.xx is hours:minutes:seconds)
 */
void generateTimestampFilename(const char *prefix, const char *extension, char *path, size_t pathSize);

/**
 * @brief 列出目录内容函数 / List directory contents function
 * @param fs 文件系统对象引用
 * @param dirname 要列出的目录路径 / Directory path to list
 * @param levels 递归深度，0=不递归，>0=递归子目录 / Recursion depth, 0=no recursion, >0=recursively list subdirectories
 * @note 递归深度为0时只列出指定目录的文件 / When recursion depth is 0, only lists files in the specified directory
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

/**
 * @brief 创建目录函数 / Create directory function
 * @param fs 文件系统对象引用
 * @param path 要创建的目录路径 / Directory path to create
 * @note 如果目录已存在会失败 / Fails if directory already exists
 */
void createDir(fs::FS &fs, const char * path);

/**
 * @brief 删除目录函数 / Delete directory function
 * @param fs 文件系统对象引用
 * @param path 要删除的目录路径
 * @note 目录必须为空才能删除成功 / Directory must be empty to delete successfully
 */
void removeDir(fs::FS &fs, const char * path);

/**
 * @brief 读取文本文件函数 / Read text file function
 * @param fs 文件系统对象引用
 * @param path 要读取的文件路径
 * @note 不适用于二进制文件（如JPEG图像）/ Not suitable for binary files (e.g., JPEG images)
 */
void readFile(fs::FS &fs, const char * path);

/**
 * @brief 写入文本文件函数 / Write text file function
 * @param fs 文件系统对象引用
 * @param path 要写入的文件路径
 * @param message 要写入的文本内容 / Text content to write
 * @note 文件不存在会创建，存在会覆盖 / Creates file if not exists, overwrites if exists
 */
void writeFile(fs::FS &fs, const char * path, const char * message);

/**
 * @brief 追加文本到文件函数 / Append text to file function
 * @param fs 文件系统对象引用
 * @param path 文件路径
 * @param message 要追加的文本内容
 * @note 在文件末尾追加，不覆盖原内容 / Appends to end of file, does not overwrite original content
 */
void appendFile(fs::FS &fs, const char * path, const char * message);

/**
 * @brief 重命名文件函数 / Rename file function
 * @param fs 文件系统对象引用
 * @param path1 原文件路径 / Original file path
 * @param path2 新文件路径 / New file path
 * @note 可以用于移动文件到不同目录 / Can be used to move file to different directory
 */
void renameFile(fs::FS &fs, const char * path1, const char * path2);

/**
 * @brief 删除文件函数 / Delete file function
 * @param fs 文件系统对象引用
 * @param path 要删除的文件路径
 * @note 删除操作不可恢复 / Deletion operation cannot be undone
 */
void deleteFile(fs::FS &fs, const char * path);

/**
 * @brief 文件IO性能测试函数 / File IO performance test function
 * @param fs 文件系统对象引用
 * @param path 要测试的文件路径
 * @note 测试读写速度，输出耗时统计 / Tests read/write speed, outputs timing statistics
 */
void testFileIO(fs::FS &fs, const char * path);

/**
 * @brief 写入JPEG图像文件函数 / Write JPEG image file function
 * @param fs 文件系统对象引用
 * @param path 要写入的文件路径
 * @param buf JPEG图像数据指针 / JPEG image data pointer
 * @param size JPEG图像数据长度（字节数）/ JPEG image data length (bytes)
 * @note 这是保存摄像头照片的核心函数 / This is the core function for saving camera photos
 */
void writejpg(fs::FS &fs, const char * path, const uint8_t *buf, size_t size);

/**
 * @brief 统计目录文件数量函数 / Count directory files function
 * @param fs 文件系统对象引用
 * @param dirname 要统计的目录路径
 * @return int 返回目录中的文件数量，失败返回-1 / Returns number of files in directory, -1 on failure
 * @note 用于生成照片文件名编号
 */
int readFileNum(fs::FS &fs, const char * dirname);

/**
 * @brief 初始化摄像头照片目录 / Initialize camera photo directory
 * @details 检查并创建照片保存目录 / Check and create photo save directory
 * @note 在拍照前必须调用此函数确保目录存在 / Must call this function before taking photos to ensure directory exists
 */
void initPhotoDir(void);

/**
 * @brief 保存摄像头照片到SD卡 / Save camera photo to SD card
 * @param buf JPEG图像数据指针 / JPEG image data pointer
 * @param size JPEG图像数据长度（字节数）/ JPEG image data length (bytes)
 * @return bool 保存成功返回true，失败返回false
 * @note 自动生成文件名（0.jpg, 1.jpg, 2.jpg...）/ Automatically generates filename (0.jpg, 1.jpg, 2.jpg...)
 */
bool savePhotoToSD(const uint8_t *buf, size_t size);

/**
 * @brief 获取SD卡已用空间（MB）/ Get SD card used space (MB)
 * @return uint64_t 返回已用空间（MB），失败返回0 / Returns used space (MB), 0 on failure
 * @note 用于监控SD卡使用情况，当空间不足时可以清理旧文件 / Used to monitor SD card usage, can clean up old files when space is low
 */
uint64_t getSDUsedSpaceMB(void);

/**
 * @brief 获取SD卡总空间（MB）/ Get SD card total space (MB)
 * @return uint64_t 返回总空间（MB），失败返回0
 * @note 用于显示SD卡容量信息 / Used to display SD card capacity information
 */
uint64_t getSDTotalSpaceMB(void);

/**
 * @brief 获取SD卡剩余空间（MB）/ Get SD card free space (MB)
 * @return uint64_t 返回剩余空间（MB），失败返回0
 * @note 用于显示SD卡剩余空间信息 / Used to display SD card free space information
 */
uint64_t getSDFreeSpaceMB(void);

/**
 * @brief 保存视频文件到SD卡 / Save video file to SD card
 * @param buf 视频数据指针
 * @param size 视频数据长度（字节数）
 * @return bool 保存成功返回true，失败返回false
 * @note 自动生成文件名，确保每个视频有唯一的文件名 / Automatically generates filename, ensuring each video has a unique filename
 */
bool saveVideoToSD(const uint8_t *buf, size_t size);

/**
 * @brief 获取指定目录中的文件列表 / Get file list in specified directory
 * @param dirname 目录路径
 * @param files 文件列表数组指针 / File list array pointer
 * @param maxFiles 最大文件数量 / Maximum number of files
 * @return int 返回文件数量，失败返回-1 / Returns number of files, -1 on failure
 * @note 用于获取目录中的文件列表，方便管理文件 / Used to get file list in directory, convenient for file management
 */
int getFileList(const char * dirname, char *files[], int maxFiles);

/**
 * @brief 删除指定目录中的所有文件 / Delete all files in specified directory
 * @param dirname 目录路径
 * @return int 返回删除的文件数量，失败返回-1 / Returns number of files deleted, -1 on failure / Returns number of files deleted, -1 on failure
 * @note 用于清理目录中的旧文件，释放SD卡空间 / Used to clean up old files in directory, free up SD card space
 */
int deleteAllFiles(const char * dirname);

/**
 * @brief 检查SD卡空间是否需要清理 / Check if SD card space needs cleanup
 * @return bool 需要清理返回true，否则返回false / Returns true if cleanup is needed, false otherwise
 * @note 动态计算阈值：当剩余空间小于保留空间（默认5GB）时返回true / Dynamically calculates threshold: returns true when free space is less than reserved space (default 5GB)
 *       适应不同容量规格的SD卡 / Adapts to SD cards of different capacities
 */
bool checkSDSpaceNeedsCleanup(void);

/**
 * @brief 获取指定目录中的文件信息列表 / Get file information list in specified directory
 * @param dirname 目录路径 / Directory path
 * @param files 文件信息数组指针 / File information array pointer
 * @param maxFiles 最大文件数量 / Maximum number of files
 * @return int 返回文件数量，失败返回-1 / Returns number of files, -1 on failure
 * @note 获取文件路径、名称、大小和修改时间 / Gets file path, name, size, and modification time
 */
int getFileInfoList(const char * dirname, FileInfo *files, int maxFiles);

/**
 * @brief 按时间排序文件信息列表 / Sort file information list by time
 * @param files 文件信息数组指针 / File information array pointer
 * @param fileCount 文件数量 / Number of files
 * @param ascending 排序顺序，true=升序（旧→新），false=降序（新→旧）/ Sort order, true=ascending (old→new), false=descending (new→old)
 * @note 按文件修改时间排序，升序时最旧的文件在前面 / Sorts by file modification time, oldest files are first when ascending
 */
void sortFilesByTime(FileInfo *files, int fileCount, bool ascending);

/**
 * @brief 删除指定目录中最旧的N个文件 / Delete N oldest files in specified directory
 * @param dirname 目录路径 / Directory path
 * @param maxFilesToDelete 最大删除文件数量 / Maximum number of files to delete
 * @return int 返回删除的文件数量，失败返回-1 / Returns number of files deleted, -1 on failure
 * @note 按文件修改时间排序，删除最旧的文件 / Sorts by file modification time, deletes oldest files
 *       清理出约2GB空间后停止 / Stops after freeing approximately 2GB space
 */
int deleteOldestFiles(const char * dirname, int maxFilesToDelete);

/**
 * @brief 自动清理旧文件以释放空间 / Automatically clean up old files to free space
 * @return int 返回删除的文件数量，失败返回-1 / Returns number of files deleted, -1 on failure / Returns number of files deleted, -1 on failure
 * @note 当SD卡剩余空间小于保留空间（默认5GB）时，自动删除最旧的文件 / When SD card free space is less than reserved space (default 5GB), automatically deletes oldest files
 *       清理出约2GB空间后停止 / Stops after freeing approximately 2GB space
 *       优先删除videos目录中的文件，然后删除photos目录中的文件 / Prioritizes deleting files in videos directory, then photos directory
 *       清理优先级由SD_CLEANUP_PRIORITY配置 / Cleanup priority is configured by SD_CLEANUP_PRIORITY
 */
int autoCleanOldFiles(void);

/**
 * @brief 开始视频录制 / Start video recording
 * @param fps 帧率（每秒帧数）/ Frame rate (frames per second)
 * @param width 视频宽度 / Video width
 * @param height 视频高度 / Video height
 * @return bool 成功返回true，失败返回false
 * @note 创建AVI文件并写入文件头 / Creates AVI file and writes file header
 */
bool startVideoRecording(int fps, int width, int height);

/**
 * @brief 写入视频帧 / Write video frame
 * @param buf JPEG图像数据指针 / JPEG image data pointer
 * @param size JPEG图像数据长度（字节数）/ JPEG image data length (bytes)
 * @return bool 成功返回true，失败返回false
 * @note 将JPEG帧写入AVI文件 / Writes JPEG frame to AVI file
 */
bool writeVideoFrame(const uint8_t *buf, size_t size);

/**
 * @brief 停止视频录制 / Stop video recording
 * @param keepRecordingState 是否保持录制状态（true=保持，false=停止）/ Whether to keep recording state (true=keep, false=stop)
 * @return bool 成功返回true，失败返回false
 * @note 完成AVI文件，更新文件头和索引 / Completes AVI file, updates file header and index
 *       keepRecordingState参数用于自动分段时保持录制状态 / keepRecordingState parameter is used to keep recording state during auto-segmentation
 */
bool stopVideoRecording(bool keepRecordingState = false);

/**
 * @brief 检查是否正在录制视频 / Check if video is being recorded
 * @return bool 正在录制返回true，否则返回false / Returns true if recording, false otherwise
 */
bool isRecordingVideo(void);

/**
 * @brief 获取当前录制的视频文件名 / Get currently recorded video filename
 * @return const char* 返回文件名指针，未录制时返回NULL / Returns filename pointer, NULL if not recording
 */
const char* getCurrentVideoFilename(void);

/**
 * @brief 清理无效视频文件函数 / Clean up invalid video files function
 * @details 功能说明 / Function description:
 *          1. 遍历videos目录中的所有文件 / 1. Iterate through all files in videos directory
 *          2. 检查每个文件的大小是否为0KB / 2. Check if each file size is 0KB
 *          3. 删除所有大小为0KB的视频文件 / 3. Delete all video files with 0KB size
 *          4. 返回删除的文件数量 / 4. Return the number of deleted files
 * @note 用于清理启动时可能产生的无效视频文件 / Used to clean up invalid video files that may be generated during startup
 * @return int 返回删除的文件数量，失败返回-1 / Returns number of deleted files, -1 on failure
 */
int cleanInvalidVideoFiles(void);

#endif
