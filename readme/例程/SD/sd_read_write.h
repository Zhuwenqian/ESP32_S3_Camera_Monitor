/**********************************************************************
  文件名称    : sd_read_write.h
  文件用途    : SD卡读写功能头文件
               声明了SD卡操作相关的函数原型和宏定义
  作者        : www.freenove.com
  修改日期    : 2022/11/02
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

/**
 * @brief SD_MMC存储卡初始化函数
 * @details 初始化SD_MMC接口，挂载文件系统，检测SD卡信息
 * @note 必须在使用其他SD卡函数之前调用
 */
void sdmmcInit(void); 

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
 * @brief 清理无效视频文件函数
 * @details 功能说明：
 *          1. 遍历videos目录中的所有文件
 *          2. 检查每个文件的大小是否为0KB
 *          3. 删除所有大小为0KB的视频文件
 *          4. 返回删除的文件数量
 * @note 用于清理启动时可能产生的无效视频文件
 */
int cleanInvalidVideoFiles(void);

#endif
