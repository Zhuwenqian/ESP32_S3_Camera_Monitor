/**********************************************************************
  文件名称 / Filename : ota_server.h
  文件用途 / File Purpose : OTA服务器头文件 / OTA Server Header File
               声明了OTA升级服务相关的函数原型和宏定义
               Declares function prototypes and macro definitions related to OTA upgrade service
  作者 / Author : ESP32-S3监控项目 / ESP32-S3 Monitoring Project
  修改日期 / Modification Date : 2026-02-04
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : Arduino.h - Arduino核心库 / Arduino Core Library
               esp_http_server.h - HTTP服务器库 / HTTP Server Library
               esp_ota_ops.h - OTA操作库 / OTA Operations Library
               Update.h - ESP32 Update库 / ESP32 Update Library
  使用说明 / Usage Instructions : 1. 包含本头文件 #include "ota_server.h" / Include this header file
               2. 调用ota_init()初始化OTA服务器 / Call ota_init() to initialize OTA server
               3. 通过Web界面上传固件进行升级 / Upload firmware via web interface for upgrade
  参数调整 / Parameter Adjustment : OTA_PORT - OTA服务端口（默认8080）/ OTA server port (default 8080)
  最新更新 / Latest Updates : 1. 实现基础OTA升级功能 / Implemented basic OTA upgrade function
               2. 添加固件验证机制 / Added firmware verification mechanism
               3. 添加HTTP Basic Authentication认证 / Added HTTP Basic Authentication
**********************************************************************/

#ifndef __OTA_SERVER_H
#define __OTA_SERVER_H

#include "Arduino.h"
#include "esp_http_server.h"
#include "esp_ota_ops.h"
#include "Update.h"

// OTA服务端口 / OTA server port
#define OTA_PORT 8080

// OTA状态枚举 / OTA status enumeration
typedef enum {
    OTA_IDLE,           // OTA空闲 / OTA idle
    OTA_UPLOADING,       // OTA上传中 / OTA uploading
    OTA_SUCCESS,         // OTA成功 / OTA success
    OTA_FAILED           // OTA失败 / OTA failed
} OTAStatus;

/**
 * @brief OTA服务器初始化函数 / OTA server initialization function
 * @details 功能说明 / Function Description:
 *          1. 注册OTA升级处理程序 / Register OTA upgrade handler
 *          2. 启动OTA HTTP服务器 / Start OTA HTTP server
 *          3. 添加认证保护 / Add authentication protection
 * @note 必须在WiFi连接后调用 / Must be called after WiFi connection
 */
void ota_init(void);

/**
 * @brief 获取OTA状态 / Get OTA status
 * @return OTAStatus 返回当前OTA状态 / Returns current OTA status
 * @note 用于查询OTA升级进度 / Used to query OTA upgrade progress
 */
OTAStatus ota_get_status(void);

/**
 * @brief 获取OTA进度 / Get OTA progress
 * @return int 返回OTA进度百分比（0-100）/ Returns OTA progress percentage (0-100)
 * @note 用于显示OTA升级进度 / Used to display OTA upgrade progress
 */
int ota_get_progress(void);

/**
 * @brief 设置OTA状态 / Set OTA status
 * @param status OTA状态 / OTA status
 * @note 用于更新OTA状态 / Used to update OTA status
 */
void ota_set_status(OTAStatus status);

/**
 * @brief 设置OTA进度 / Set OTA progress
 * @param progress OTA进度百分比（0-100）/ OTA progress percentage (0-100)
 * @note 用于更新OTA进度 / Used to update OTA progress
 */
void ota_set_progress(int progress);

#endif // __OTA_SERVER_H
