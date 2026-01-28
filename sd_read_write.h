/**********************************************************************
  文件名称    : sd_read_write.h
  文件用途    : SD_MMC存储卡读写功能头文件
               声明了SD卡操作相关的函数原型和宏定义
  作者        : ESP32-S3监控项目
  修改日期    : 2026-01-27
  硬件平台    : ESP32S3-EYE开发板
  依赖库      : Arduino.h - Arduino核心库
               FS.h - 文件系统基础库
               SD_MMC.h - SD_MMC驱动库
  引脚说明    : SD_MMC_CMD (GPIO38) - SD卡命令线（固定）
               SD_MMC_CLK (GPIO39) - SD卡时钟线（固定）
               SD_MMC_D0  (GPIO40) - SD卡数据线（固定）
  使用说明    : 1. 包含本头文件 #include "sd_read_write.h"
               2. 调用sdmmcInit()初始化SD卡
               3. 使用提供的函数进行文件操作
  注意事项    : 这些引脚是ESP32S3-EYE开发板固定的SD_MMC接口
               请勿修改，否则可能导致SD卡无法正常工作
**********************************************************************/
#ifndef __SD_READ_WRITE_H
#define __SD_READ_WRITE_H

#include "Arduino.h"
#include "FS.h"
#include "SD_MMC.h"

// SD_MMC引脚定义 - 这些引脚是ESP32S3-EYE开发板固定的SDIO接口
// 请勿修改这些值，否则可能导致SD卡无法正常工作
#define SD_MMC_CMD  38  // SD卡命令线（Command），GPIO38
#define SD_MMC_CLK  39  // SD卡时钟线（Clock），GPIO39
#define SD_MMC_D0   40  // SD卡数据线0（Data 0），GPIO40

// SD卡挂载点路径
#define SD_MOUNT_POINT "/sdcard"

// 摄像头根目录
#define CAMERA_DIR "/camera"

// 照片保存目录
#define PHOTO_DIR "/camera/photos"

// 视频保存目录
#define VIDEO_DIR "/camera/videos"

// AVI文件相关定义
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

// AVI文件头结构体
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t fileSize;      // 文件总大小（不包括前8字节）
    char avi[4];            // "AVI "
    char list[4];           // "LIST"
    uint32_t listSize;      // hdrl列表大小
    char hdrl[4];           // "hdrl"
    char avih[4];           // "avih"
    uint32_t avihSize;      // avih块大小（56字节）
    uint32_t microSecPerFrame; // 每帧微秒数
    uint32_t maxBytesPerSec;   // 最大每秒字节数
    uint32_t paddingGranularity; // 填充粒度
    uint32_t flags;         // 标志
    uint32_t totalFrames;   // 总帧数
    uint32_t initialFrames; // 初始帧数
    uint32_t streams;       // 流数量
    uint32_t suggestedBufferSize; // 建议缓冲区大小
    uint32_t width;         // 宽度
    uint32_t height;        // 高度
    uint32_t reserved[4];   // 保留
} AVI_MAIN_HEADER;

// 视频流头结构体
typedef struct {
    char list[4];           // "LIST"
    uint32_t listSize;      // strl列表大小
    char strl[4];           // "strl"
    char strh[4];           // "strh"
    uint32_t strhSize;      // strh块大小（56字节）
    char fccType[4];        // "vids"
    char fccHandler[4];     // "MJPG"
    uint32_t flags;         // 标志
    uint16_t priority;      // 优先级
    uint16_t language;      // 语言
    uint32_t initialFrames; // 初始帧
    uint32_t scale;         // 时间刻度
    uint32_t rate;          // 速率
    uint32_t start;         // 开始时间
    uint32_t length;        // 长度
    uint32_t suggestedBufferSize; // 建议缓冲区大小
    uint32_t quality;       // 质量
    uint32_t sampleSize;    // 样本大小
    uint16_t left;          // 左边距
    uint16_t top;           // 上边距
    uint16_t right;         // 右边距
    uint16_t bottom;        // 下边距
} AVI_STREAM_HEADER;

// 位图信息头结构体
typedef struct {
    char strf[4];           // "strf"
    uint32_t strfSize;      // strf块大小（40字节）
    uint32_t size;          // 结构体大小（40字节）
    int32_t width;          // 宽度
    int32_t height;         // 高度
    uint16_t planes;        // 平面数
    uint16_t bitCount;      // 每像素位数
    uint32_t compression;   // 压缩方式
    uint32_t sizeImage;      // 图像大小
    int32_t xPelsPerMeter;  // 水平分辨率
    int32_t yPelsPerMeter;  // 垂直分辨率
    uint32_t clrUsed;       // 使用的颜色数
    uint32_t clrImportant;  // 重要颜色数
} AVI_BITMAP_INFO;

// 索引条目结构体
typedef struct {
    char id[4];             // "00dc"或"00db"
    uint32_t flags;         // 标志
    uint32_t offset;        // 偏移量
    uint32_t size;          // 大小
} AVI_INDEX_ENTRY;

// SD卡空间阈值（GB）
#define SD_SPACE_THRESHOLD_GB 55

/**
 * @brief SD_MMC存储卡初始化函数
 * @details 初始化SD_MMC接口，挂载文件系统，检测SD卡信息
 * @note 必须在使用其他SD卡函数之前调用
 */
void sdmmcInit(void); 

/**
 * @brief 生成时间戳文件名
 * @param prefix 文件名前缀（如"photo"或"video"）
 * @param extension 文件扩展名（如".jpg"或".avi"）
 * @param path 输出缓冲区
 * @param pathSize 缓冲区大小
 * @details 功能说明：
 *          1. 获取当前时间（从millis()计算）
 *          2. 格式化为ESP32-xxxx.xx.xx.xx格式
 *          3. 生成完整文件名
 * @note 文件名格式：ESP32-xxxx.xx.xx.xx（xxxx为启动后的秒数，xx.xx.xx为时分秒）
 */
void generateTimestampFilename(const char *prefix, const char *extension, char *path, size_t pathSize);

/**
 * @brief 列出目录内容函数
 * @param fs 文件系统对象引用
 * @param dirname 要列出的目录路径
 * @param levels 递归深度，0=不递归，>0=递归子目录
 * @note 递归深度为0时只列出指定目录的文件
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);

/**
 * @brief 创建目录函数
 * @param fs 文件系统对象引用
 * @param path 要创建的目录路径
 * @note 如果目录已存在会失败
 */
void createDir(fs::FS &fs, const char * path);

/**
 * @brief 删除目录函数
 * @param fs 文件系统对象引用
 * @param path 要删除的目录路径
 * @note 目录必须为空才能删除成功
 */
void removeDir(fs::FS &fs, const char * path);

/**
 * @brief 读取文本文件函数
 * @param fs 文件系统对象引用
 * @param path 要读取的文件路径
 * @note 不适用于二进制文件（如JPEG图像）
 */
void readFile(fs::FS &fs, const char * path);

/**
 * @brief 写入文本文件函数
 * @param fs 文件系统对象引用
 * @param path 要写入的文件路径
 * @param message 要写入的文本内容
 * @note 文件不存在会创建，存在会覆盖
 */
void writeFile(fs::FS &fs, const char * path, const char * message);

/**
 * @brief 追加文本到文件函数
 * @param fs 文件系统对象引用
 * @param path 文件路径
 * @param message 要追加的文本内容
 * @note 在文件末尾追加，不覆盖原内容
 */
void appendFile(fs::FS &fs, const char * path, const char * message);

/**
 * @brief 重命名文件函数
 * @param fs 文件系统对象引用
 * @param path1 原文件路径
 * @param path2 新文件路径
 * @note 可以用于移动文件到不同目录
 */
void renameFile(fs::FS &fs, const char * path1, const char * path2);

/**
 * @brief 删除文件函数
 * @param fs 文件系统对象引用
 * @param path 要删除的文件路径
 * @note 删除操作不可恢复
 */
void deleteFile(fs::FS &fs, const char * path);

/**
 * @brief 文件IO性能测试函数
 * @param fs 文件系统对象引用
 * @param path 要测试的文件路径
 * @note 测试读写速度，输出耗时统计
 */
void testFileIO(fs::FS &fs, const char * path);

/**
 * @brief 写入JPEG图像文件函数
 * @param fs 文件系统对象引用
 * @param path 要写入的文件路径
 * @param buf JPEG图像数据指针
 * @param size JPEG图像数据长度（字节数）
 * @note 这是保存摄像头照片的核心函数
 */
void writejpg(fs::FS &fs, const char * path, const uint8_t *buf, size_t size);

/**
 * @brief 统计目录文件数量函数
 * @param fs 文件系统对象引用
 * @param dirname 要统计的目录路径
 * @return int 返回目录中的文件数量，失败返回-1
 * @note 用于生成照片文件名编号
 */
int readFileNum(fs::FS &fs, const char * dirname);

/**
 * @brief 初始化摄像头照片目录
 * @details 检查并创建照片保存目录
 * @note 在拍照前必须调用此函数确保目录存在
 */
void initPhotoDir(void);

/**
 * @brief 保存摄像头照片到SD卡
 * @param buf JPEG图像数据指针
 * @param size JPEG图像数据长度（字节数）
 * @return bool 保存成功返回true，失败返回false
 * @note 自动生成文件名（0.jpg, 1.jpg, 2.jpg...）
 */
bool savePhotoToSD(const uint8_t *buf, size_t size);

/**
 * @brief 获取SD卡已用空间（MB）
 * @return uint64_t 返回已用空间（MB），失败返回0
 * @note 用于监控SD卡使用情况，当空间不足时可以清理旧文件
 */
uint64_t getSDUsedSpaceMB(void);

/**
 * @brief 获取SD卡总空间（MB）
 * @return uint64_t 返回总空间（MB），失败返回0
 * @note 用于显示SD卡容量信息
 */
uint64_t getSDTotalSpaceMB(void);

/**
 * @brief 获取SD卡剩余空间（MB）
 * @return uint64_t 返回剩余空间（MB），失败返回0
 * @note 用于显示SD卡剩余空间信息
 */
uint64_t getSDFreeSpaceMB(void);

/**
 * @brief 保存视频文件到SD卡
 * @param buf 视频数据指针
 * @param size 视频数据长度（字节数）
 * @return bool 保存成功返回true，失败返回false
 * @note 自动生成文件名，确保每个视频有唯一的文件名
 */
bool saveVideoToSD(const uint8_t *buf, size_t size);

/**
 * @brief 获取指定目录中的文件列表
 * @param dirname 目录路径
 * @param files 文件列表数组指针
 * @param maxFiles 最大文件数量
 * @return int 返回文件数量，失败返回-1
 * @note 用于获取目录中的文件列表，方便管理文件
 */
int getFileList(const char * dirname, char *files[], int maxFiles);

/**
 * @brief 删除指定目录中的所有文件
 * @param dirname 目录路径
 * @return int 返回删除的文件数量，失败返回-1
 * @note 用于清理目录中的旧文件，释放SD卡空间
 */
int deleteAllFiles(const char * dirname);

/**
 * @brief 检查SD卡空间是否达到阈值
 * @return bool 达到阈值返回true，否则返回false
 * @note 阈值为55GB，当已用空间达到55GB时返回true
 */
bool checkSDSpaceThreshold(void);

/**
 * @brief 自动清理旧文件以释放空间
 * @return int 返回删除的文件数量，失败返回-1
 * @note 当SD卡空间达到55GB时，自动删除最旧的文件
 *       优先删除videos目录中的文件，然后删除photos目录中的文件
 */
int autoCleanOldFiles(void);

/**
 * @brief 开始视频录制
 * @param fps 帧率（每秒帧数）
 * @param width 视频宽度
 * @param height 视频高度
 * @return bool 成功返回true，失败返回false
 * @note 创建AVI文件并写入文件头
 */
bool startVideoRecording(int fps, int width, int height);

/**
 * @brief 写入视频帧
 * @param buf JPEG图像数据指针
 * @param size JPEG图像数据长度（字节数）
 * @return bool 成功返回true，失败返回false
 * @note 将JPEG帧写入AVI文件
 */
bool writeVideoFrame(const uint8_t *buf, size_t size);

/**
 * @brief 停止视频录制
 * @param keepRecordingState 是否保持录制状态（true=保持，false=停止）
 * @return bool 成功返回true，失败返回false
 * @note 完成AVI文件，更新文件头和索引
 *       keepRecordingState参数用于自动分段时保持录制状态
 */
bool stopVideoRecording(bool keepRecordingState = false);

/**
 * @brief 检查是否正在录制视频
 * @return bool 正在录制返回true，否则返回false
 */
bool isRecordingVideo(void);

/**
 * @brief 获取当前录制的视频文件名
 * @return const char* 返回文件名指针，未录制时返回NULL
 */
const char* getCurrentVideoFilename(void);

#endif
