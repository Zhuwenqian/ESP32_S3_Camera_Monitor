/**********************************************************************
  文件名称 / Filename : ota_server.cpp
  文件用途 / File Purpose : OTA服务器实现文件 / OTA Server Implementation File
               本文件实现了OTA升级服务功能
               This file implements OTA upgrade service functionality
               主要功能包括 / Main Features:
               1. OTA升级处理程序 / OTA upgrade handler
               2. 固件上传和验证 / Firmware upload and verification
               3. 升级进度跟踪 / Upgrade progress tracking
               4. HTTP Basic Authentication认证 / HTTP Basic Authentication
  作者 / Author : ESP32-S3监控项目 / ESP32-S3 Monitoring Project
  修改日期 / Modification Date : 2026-02-04
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : esp_http_server.h - HTTP服务器库 / HTTP Server Library
               esp_ota_ops.h - OTA操作库 / OTA Operations Library
               Update.h - ESP32 Update库 / ESP32 Update Library
  使用说明 / Usage Instructions : 1. 调用ota_init()初始化OTA服务器 / Call ota_init() to initialize OTA server
               2. 通过Web界面上传固件文件（.bin格式）/ Upload firmware file (.bin format) via web interface
               3. 系统自动验证并安装固件 / System automatically verifies and installs firmware
               4. 升级完成后自动重启 / Auto restart after upgrade completion
  参数调整 / Parameter Adjustment : OTA_PORT - OTA服务端口（默认8080）/ OTA server port (default 8080)
  最新更新 / Latest Updates : 1. 实现基础OTA升级功能 / Implemented basic OTA upgrade function
               2. 添加固件验证机制 / Added firmware verification mechanism
               3. 添加HTTP Basic Authentication认证 / Added HTTP Basic Authentication
               4. 添加进度跟踪和状态报告 / Added progress tracking and status reporting
**********************************************************************/

#include "ota_server.h"
#include "auth.h"
#include <WiFi.h>

// OTA服务器句柄 / OTA server handle
httpd_handle_t ota_httpd = NULL;

// OTA状态变量 / OTA status variables
static OTAStatus currentOTAStatus = OTA_IDLE;
static int currentProgress = 0;

/**
 * @brief OTA升级处理程序 / OTA upgrade handler
 * @param req HTTP请求对象 / HTTP request object
 * @return esp_err_t 返回ESP_OK表示成功 / Returns ESP_OK on success
 * @details 功能说明 / Function Description:
 *          1. 检查请求方法是否为POST / Check if request method is POST
 *          2. 验证用户认证 / Verify user authentication
 *          3. 读取上传的固件文件 / Read uploaded firmware file
 *          4. 验证固件文件 / Verify firmware file
 *          5. 写入固件到Flash / Write firmware to Flash
 *          6. 重启设备 / Restart device
 * @note 固件文件必须是.bin格式 / Firmware file must be in .bin format
 *       升级过程中设备会自动重启 / Device will automatically restart during upgrade
 */
static esp_err_t ota_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/plain");
    
    // 检查请求方法 / Check request method
    if(req->method != HTTP_POST) {
        httpd_resp_send_err(req, HTTPD_405_METHOD_NOT_ALLOWED, "Method not allowed");
        return ESP_FAIL;
    }
    
    // 验证用户认证 / Verify user authentication
    auth_result_t auth_result = auth_verify(req);
    if(auth_result != AUTH_SUCCESS) {
        httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"OTA Upgrade\"");
        httpd_resp_send_err(req, HTTPD_401_UNAUTHORIZED, "Unauthorized");
        return ESP_FAIL;
    }
    
    // 设置OTA状态为上传中 / Set OTA status to uploading
    ota_set_status(OTA_UPLOADING);
    ota_set_progress(0);
    
    Serial.println("OTA upgrade started / OTA升级开始");
    
    // 获取内容长度 / Get content length
    int content_len = req->content_len;
    if(content_len <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid content length");
        ota_set_status(OTA_FAILED);
        return ESP_FAIL;
    }
    
    Serial.printf("Firmware size: %d bytes / 固件大小: %d 字节\n", content_len);
    
    // 初始化Update类 / Initialize Update class
    if(!Update.begin(content_len)) {
        Serial.println("Not enough space to begin OTA / 没有足够的空间开始OTA");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Not enough space");
        ota_set_status(OTA_FAILED);
        return ESP_FAIL;
    }
    
    // 读取和写入固件 / Read and write firmware
    int remaining = content_len;
    int received = 0;
    uint8_t buffer[1024];
    
    while(remaining > 0) {
        int read_len = httpd_req_recv(req, (char*)buffer, sizeof(buffer));
        if(read_len <= 0) {
            break;
        }
        
        if(Update.write(buffer, read_len) != read_len) {
            Serial.println("Write failed / 写入失败");
            Update.abort();
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Write failed");
            ota_set_status(OTA_FAILED);
            return ESP_FAIL;
        }
        
        remaining -= read_len;
        received += read_len;
        
        // 更新进度 / Update progress
        int progress = (received * 100) / content_len;
        ota_set_progress(progress);
        
        if(progress % 10 == 0) {
            Serial.printf("OTA progress: %d%% / OTA进度: %d%%\n", progress);
        }
    }
    
    // 检查是否接收完整 / Check if fully received
    if(remaining > 0) {
        Serial.println("Incomplete upload / 上传不完整");
        Update.abort();
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Incomplete upload");
        ota_set_status(OTA_FAILED);
        return ESP_FAIL;
    }
    
    // 完成更新 / Complete update
    if(!Update.end(true)) {
        Serial.println("Update failed / 更新失败");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Update failed");
        ota_set_status(OTA_FAILED);
        return ESP_FAIL;
    }
    
    // 设置OTA状态为成功 / Set OTA status to success
    ota_set_status(OTA_SUCCESS);
    ota_set_progress(100);
    
    Serial.println("OTA upgrade successful / OTA升级成功");
    httpd_resp_send(req, "OTA upgrade successful. Device will restart...", 0);
    
    // 延迟以发送响应 / Delay to send response
    delay(1000);
    
    // 重启设备 / Restart device
    ESP.restart();
    
    return ESP_OK;
}

/**
 * @brief OTA服务器初始化函数 / OTA server initialization function
 * @details 功能说明 / Function Description:
 *          1. 配置OTA HTTP服务器 / Configure OTA HTTP server
 *          2. 注册OTA升级处理程序 / Register OTA upgrade handler
 *          3. 启动OTA服务器 / Start OTA server
 * @note 必须在WiFi连接后调用 / Must be called after WiFi connection
 */
void ota_init(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = OTA_PORT;
    config.max_open_sockets = 5;
    config.max_uri_handlers = 10;
    config.stack_size = 8192;
    
    // 启动OTA HTTP服务器 / Start OTA HTTP server
    if(httpd_start(&ota_httpd, &config) == ESP_OK) {
        Serial.println("OTA server started / OTA服务器已启动");
        Serial.printf("OTA server port: %d / OTA服务器端口: %d\n", OTA_PORT);
        
        // 注册OTA升级处理程序 / Register OTA upgrade handler
        httpd_uri_t ota_uri = {
            .uri      = "/update",
            .method   = HTTP_POST,
            .handler  = ota_handler,
            .user_ctx = NULL
        };
        
        httpd_register_uri_handler(ota_httpd, &ota_uri);
        
        Serial.println("OTA handler registered / OTA处理程序已注册");
        Serial.print("OTA URL: http://");
        Serial.print(WiFi.localIP());
        Serial.print(":");
        Serial.print(OTA_PORT);
        Serial.println("/update");
    } else {
        Serial.println("Failed to start OTA server / OTA服务器启动失败");
    }
}

/**
 * @brief 获取OTA状态 / Get OTA status
 * @return OTAStatus 返回当前OTA状态 / Returns current OTA status
 */
OTAStatus ota_get_status(void) {
    return currentOTAStatus;
}

/**
 * @brief 获取OTA进度 / Get OTA progress
 * @return int 返回OTA进度百分比（0-100）/ Returns OTA progress percentage (0-100)
 */
int ota_get_progress(void) {
    return currentProgress;
}

/**
 * @brief 设置OTA状态 / Set OTA status
 * @param status OTA状态 / OTA status
 */
void ota_set_status(OTAStatus status) {
    currentOTAStatus = status;
}

/**
 * @brief 设置OTA进度 / Set OTA progress
 * @param progress OTA进度百分比（0-100）/ OTA progress percentage (0-100)
 */
void ota_set_progress(int progress) {
    if(progress >= 0 && progress <= 100) {
        currentProgress = progress;
    }
}
