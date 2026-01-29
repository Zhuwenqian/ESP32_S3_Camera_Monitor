// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/**********************************************************************
  文件名称 / Filename : app_httpd.cpp
  文件用途 / File Purpose : HTTP服务器实现 / HTTP Server Implementation
               本文件实现了Web服务器功能，提供视频流、拍照等接口
               This file implements web server functionality, providing video streaming, photo capture, and other interfaces
               主要功能包括 / Main Features:
               1. 视频流接口（/stream）/ Video streaming interface (/stream)
               2. 拍照接口（/capture）/ Photo capture interface (/capture)
               3. 系统状态接口（/status）/ System status interface (/status)
               4. 摄像头参数控制接口（分辨率、图像质量、亮度、对比度、饱和度）/ Camera parameter control interface (resolution, quality, brightness, contrast, saturation)
               5. 系统运行时长统计 / System uptime statistics
               6. HTTP Basic Authentication认证 / HTTP Basic Authentication
               7. 会话管理（14天过期）/ Session management (14 days expiration)
  作者 / Author : ESP32-S3监控项目 / ESP32-S3 Monitoring Project
  修改日期 / Modification Date : 2026-01-28
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : esp_http_server.h - HTTP服务器库 / HTTP Server Library
               esp_camera.h - 摄像头库 / Camera Library
               sd_read_write.h - SD卡读写库 / SD Card Read/Write Library
               auth.h - 认证模块 / Authentication Module
  使用说明 / Usage Instructions : 1. 调用startCameraServer()启动HTTP服务器 / Call startCameraServer() to start HTTP server
               2. 通过Web界面或API访问各项功能 / Access features via web interface or API
               3. 首次访问需要输入用户名和密码 / First access requires username and password
               4. 登录后14天内无需重复验证 / No need to re-authenticate within 14 days after login
  参数调整 / Parameter Adjustment : JSON缓冲区大小 - 2048字节，用于存储系统状态信息 / JSON buffer size - 2048 bytes, used to store system status information
               调整建议：如需添加更多状态信息，可适当增大缓冲区 / Adjustment suggestion: Increase buffer size if more status information is needed
  最新更新 / Latest Updates : 1. 添加SD卡空间信息到status接口（GB单位）/ Added SD card space information to status interface (GB unit)
               2. 支持动态调整摄像头分辨率、图像质量、亮度、对比度、饱和度 / Support dynamic adjustment of camera resolution, quality, brightness, contrast, saturation
               3. 删除视频录制控制接口（/record/start、/record/stop、/record/status）/ Removed video recording control interfaces (/record/start, /record/stop, /record/status)
               4. 删除视频录制任务（videoRecordTask）/ Removed video recording task (videoRecordTask)
               5. 添加HTTP Basic Authentication认证 / Added HTTP Basic Authentication
               6. 添加会话管理功能（14天过期）/ Added session management (14 days expiration)
               7. 为主页、视频流、拍照接口、摄像头控制接口添加认证保护 / Added authentication protection to main page, video stream, photo capture, and camera control interfaces
  注意事项 / Important Notes : 视频流和拍照功能都需要摄像头正常工作 / Video streaming and photo capture require camera to work properly
               时钟频率需要在启动时设置，不支持运行时动态调整 / Clock frequency must be set at startup, runtime adjustment not supported
               视频录制功能在系统启动时自动开始，无需手动操作 / Video recording starts automatically on boot, no manual operation needed
               Web前端已移除视频录制控制界面，录制完全自动化 / Web frontend has removed video recording control interface, recording is fully automated
               所有敏感接口都需要认证，用户名和密码在auth.h中定义 / All sensitive interfaces require authentication, username and password are defined in auth.h
               会话14天后自动过期，需要重新登录 / Sessions expire automatically after 14 days, need to re-login
**********************************************************************/

#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "driver/ledc.h"
#include "sdkconfig.h"
#include "camera_index.h"
#include "sd_read_write.h"
#include "auth.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "camera_httpd";
#endif

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
int led_duty = 0;
bool isStreaming = false;
#ifdef CONFIG_LED_LEDC_LOW_SPEED_MODE
#define CONFIG_LED_LEDC_SPEED_MODE LEDC_LOW_SPEED_MODE
#else
#define CONFIG_LED_LEDC_SPEED_MODE LEDC_HIGH_SPEED_MODE
#endif
#endif

typedef struct
{
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

typedef struct
{
    size_t size;  //number of values used for filtering
    size_t index; //current value index
    size_t count; //value count
    int sum;
    int *values; //array to be filled with values
} ra_filter_t;

static ra_filter_t ra_filter;

static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size)
{
    memset(filter, 0, sizeof(ra_filter_t));

    filter->values = (int *)malloc(sample_size * sizeof(int));
    if (!filter->values)
    {
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));

    filter->size = sample_size;
    return filter;
}



#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
void enable_led(bool en)
{ // Turn LED On or Off
    int duty = en ? led_duty : 0;
    if (en && isStreaming && (led_duty > CONFIG_LED_MAX_INTENSITY))
    {
        duty = CONFIG_LED_MAX_INTENSITY;
    }
    ledc_set_duty(CONFIG_LED_LEDC_SPEED_MODE, CONFIG_LED_LEDC_CHANNEL, duty);
    ledc_update_duty(CONFIG_LED_LEDC_SPEED_MODE, CONFIG_LED_LEDC_CHANNEL);
    ESP_LOGI(TAG, "Set LED intensity to %d", duty);
}
#endif

static esp_err_t bmp_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
    uint64_t fr_start = esp_timer_get_time();
#endif
    fb = esp_camera_fb_get();
    if (!fb)
    {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/x-windows-bmp");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.bmp");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    char ts[32];
    snprintf(ts, 32, "%ld.%06ld", fb->timestamp.tv_sec, fb->timestamp.tv_usec);
    httpd_resp_set_hdr(req, "X-Timestamp", (const char *)ts);


    uint8_t * buf = NULL;
    size_t buf_len = 0;
    bool converted = frame2bmp(fb, &buf, &buf_len);
    esp_camera_fb_return(fb);
    if(!converted){
        ESP_LOGE(TAG, "BMP Conversion failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    res = httpd_resp_send(req, (const char *)buf, buf_len);
    free(buf);
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
    uint64_t fr_end = esp_timer_get_time();
#endif
    ESP_LOGI(TAG, "BMP: %llums, %uB", (uint64_t)((fr_end - fr_start) / 1000), buf_len);
    return res;
}

static size_t jpg_encode_stream(void *arg, size_t index, const void *data, size_t len)
{
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if (!index)
    {
        j->len = 0;
    }
    if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK)
    {
        return 0;
    }
    j->len += len;
    return len;
}

static esp_err_t capture_handler(httpd_req_t *req)
{
    // 验证认证
    auth_result_t auth_result = auth_verify(req);
    if(auth_result != AUTH_SUCCESS) {
        ESP_LOGW(TAG, "Capture handler: authentication failed (%d)", auth_result);
        return auth_send_401(req);
    }

    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
    int64_t fr_start = esp_timer_get_time();
    size_t fb_len = 0;
#endif

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    enable_led(true);
    vTaskDelay(150 / portTICK_PERIOD_MS); // The LED needs to be turned on ~150ms before the call to esp_camera_fb_get()
    fb = esp_camera_fb_get();             // or it won't be visible in the frame. A better way to do this is needed.
    enable_led(false);
#else
    fb = esp_camera_fb_get();
#endif

    if (!fb)
    {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // 保存照片到SD卡
    bool saveSuccess = savePhotoToSD(fb->buf, fb->len);
    if(saveSuccess){
        ESP_LOGI(TAG, "Photo saved to SD card successfully");
    } else {
        ESP_LOGE(TAG, "Failed to save photo to SD card");
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    char ts[32];
    snprintf(ts, 32, "%ld.%06ld", fb->timestamp.tv_sec, fb->timestamp.tv_usec);
    httpd_resp_set_hdr(req, "X-Timestamp", (const char *)ts);

    if (fb->format == PIXFORMAT_JPEG)
    {
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
        fb_len = fb->len;
#endif
        res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    }
    else
    {
        jpg_chunking_t jchunk = {req, 0};
        res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk) ? ESP_OK : ESP_FAIL;
        httpd_resp_send_chunk(req, NULL, 0);
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
        fb_len = jchunk.len;
#endif
    }
    esp_camera_fb_return(fb);
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
    int64_t fr_end = esp_timer_get_time();
#endif
    ESP_LOGI(TAG, "JPG: %uB %ums", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start) / 1000));
    return res;
}

static esp_err_t stream_handler(httpd_req_t *req)
{
    // 验证认证
    auth_result_t auth_result = auth_verify(req);
    if(auth_result != AUTH_SUCCESS) {
        ESP_LOGW(TAG, "Stream handler: authentication failed (%d)", auth_result);
        // 视频流认证失败时，返回401并关闭连接
        httpd_resp_set_status(req, "401 Unauthorized");
        httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"ESP32 Camera\"");
        httpd_resp_set_hdr(req, "Content-Length", "0");
        httpd_resp_set_hdr(req, "Connection", "close");
        return httpd_resp_send(req, NULL, 0);
    }

    camera_fb_t *fb = NULL;
    struct timeval _timestamp;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[128];

    static int64_t last_frame = 0;
    if (!last_frame)
    {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    enable_led(true);
    isStreaming = true;
#endif

    while (true)
    {
        fb = esp_camera_fb_get();
        if (!fb)
        {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        }
        else
        {
            _timestamp.tv_sec = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;

            if (fb->format != PIXFORMAT_JPEG)
            {
                bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                esp_camera_fb_return(fb);
                fb = NULL;
                if (!jpeg_converted)
                {
                    ESP_LOGE(TAG, "JPEG compression failed");
                    res = ESP_FAIL;
                }
            }
            else
            {
                _jpg_buf_len = fb->len;
                _jpg_buf = fb->buf;
            }
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK)
        {
            ESP_LOGE(TAG, "send frame failed failed");
            break;
        }
        int64_t fr_end = esp_timer_get_time();

        int64_t frame_time = fr_end - last_frame;
        frame_time /= 1000;
        ESP_LOGI(TAG, "MJPG: %uB %ums (%.1ffps)",
                 (uint32_t)(_jpg_buf_len),
                 (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
    }

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    isStreaming = false;
    enable_led(false);
#endif

    return res;
}

static esp_err_t parse_get(httpd_req_t *req, char **obuf)
{
    char *buf = NULL;
    size_t buf_len = 0;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char *)malloc(buf_len);
        if (!buf) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

static esp_err_t cmd_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char variable[32];
    char value[32];

    if (parse_get(req, &buf) != ESP_OK) {
        return ESP_FAIL;
    }
    if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) != ESP_OK ||
        httpd_query_key_value(buf, "val", value, sizeof(value)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int val = atoi(value);
    ESP_LOGI(TAG, "%s = %d", variable, val);
    sensor_t *s = esp_camera_sensor_get();
    int res = 0;

    if (!strcmp(variable, "framesize")) {
        if (s->pixformat == PIXFORMAT_JPEG) {
            res = s->set_framesize(s, (framesize_t)val);
        }
    }
    else if (!strcmp(variable, "quality"))
        res = s->set_quality(s, val);
    else if (!strcmp(variable, "contrast"))
        res = s->set_contrast(s, val);
    else if (!strcmp(variable, "brightness"))
        res = s->set_brightness(s, val);
    else if (!strcmp(variable, "saturation"))
        res = s->set_saturation(s, val);
    else if (!strcmp(variable, "gainceiling"))
        res = s->set_gainceiling(s, (gainceiling_t)val);
    else if (!strcmp(variable, "colorbar"))
        res = s->set_colorbar(s, val);
    else if (!strcmp(variable, "awb"))
        res = s->set_whitebal(s, val);
    else if (!strcmp(variable, "agc"))
        res = s->set_gain_ctrl(s, val);
    else if (!strcmp(variable, "aec"))
        res = s->set_exposure_ctrl(s, val);
    else if (!strcmp(variable, "hmirror"))
        res = s->set_hmirror(s, val);
    else if (!strcmp(variable, "vflip"))
        res = s->set_vflip(s, val);
    else if (!strcmp(variable, "awb_gain"))
        res = s->set_awb_gain(s, val);
    else if (!strcmp(variable, "agc_gain"))
        res = s->set_agc_gain(s, val);
    else if (!strcmp(variable, "aec_value"))
        res = s->set_aec_value(s, val);
    else if (!strcmp(variable, "aec2"))
        res = s->set_aec2(s, val);
    else if (!strcmp(variable, "dcw"))
        res = s->set_dcw(s, val);
    else if (!strcmp(variable, "bpc"))
        res = s->set_bpc(s, val);
    else if (!strcmp(variable, "wpc"))
        res = s->set_wpc(s, val);
    else if (!strcmp(variable, "raw_gma"))
        res = s->set_raw_gma(s, val);
    else if (!strcmp(variable, "lenc"))
        res = s->set_lenc(s, val);
    else if (!strcmp(variable, "special_effect"))
        res = s->set_special_effect(s, val);
    else if (!strcmp(variable, "wb_mode"))
        res = s->set_wb_mode(s, val);
    else if (!strcmp(variable, "ae_level"))
        res = s->set_ae_level(s, val);
#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    else if (!strcmp(variable, "led_intensity")) {
        led_duty = val;
        if (isStreaming)
            enable_led(true);
    }
#endif


    else {
        ESP_LOGI(TAG, "Unknown command: %s", variable);
        res = -1;
    }

    if (res < 0) {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static int print_reg(char * p, sensor_t * s, uint16_t reg, uint32_t mask){
    return sprintf(p, "\"0x%x\":%u,", reg, s->get_reg(s, reg, mask));
}

static esp_err_t status_handler(httpd_req_t *req)
{
    // 验证认证
    auth_result_t auth_result = auth_verify(req);
    if(auth_result != AUTH_SUCCESS) {
        ESP_LOGW(TAG, "Status handler: authentication failed (%d)", auth_result);
        return auth_send_401(req);
    }

    static char json_response[2048];  // 增大缓冲区以容纳SD卡信息

    sensor_t *s = esp_camera_sensor_get();
    char *p = json_response;
    *p++ = '{';

    // 添加运行时长信息
    extern unsigned long getUptimeSeconds();
    unsigned long uptime = getUptimeSeconds();
    unsigned long hours = uptime / 3600;
    unsigned long minutes = (uptime % 3600) / 60;
    unsigned long seconds = uptime % 60;
    p += sprintf(p, "\"uptime\":\"%02lu:%02lu:%02lu\",", hours, minutes, seconds);

    if(s->id.PID == OV5640_PID || s->id.PID == OV3660_PID){
        for(int reg = 0x3400; reg < 0x3406; reg+=2){
            p+=print_reg(p, s, reg, 0xFFF);//12 bit
        }
        p+=print_reg(p, s, 0x3406, 0xFF);

        p+=print_reg(p, s, 0x3500, 0xFFFF0);//16 bit
        p+=print_reg(p, s, 0x3503, 0xFF);
        p+=print_reg(p, s, 0x350a, 0x3FF);//10 bit
        p+=print_reg(p, s, 0x350c, 0xFFFF);//16 bit

        for(int reg = 0x5480; reg <= 0x5490; reg++){
            p+=print_reg(p, s, reg, 0xFF);
        }

        for(int reg = 0x5380; reg <= 0x538b; reg++){
            p+=print_reg(p, s, reg, 0xFF);
        }

        for(int reg = 0x5580; reg < 0x558a; reg++){
            p+=print_reg(p, s, reg, 0xFF);
        }
        p+=print_reg(p, s, 0x558a, 0x1FF);//9 bit
    } else if(s->id.PID == OV2640_PID){
        p+=print_reg(p, s, 0xd3, 0xFF);
        p+=print_reg(p, s, 0x111, 0xFF);
        p+=print_reg(p, s, 0x132, 0xFF);
    }

    p += sprintf(p, "\"xclk\":%u,", s->xclk_freq_hz / 1000000);
    p += sprintf(p, "\"pixformat\":%u,", s->pixformat);
    p += sprintf(p, "\"framesize\":%u,", s->status.framesize);
    p += sprintf(p, "\"quality\":%u,", s->status.quality);
    p += sprintf(p, "\"brightness\":%d,", s->status.brightness);
    p += sprintf(p, "\"contrast\":%d,", s->status.contrast);
    p += sprintf(p, "\"saturation\":%d,", s->status.saturation);
    p += sprintf(p, "\"sharpness\":%d,", s->status.sharpness);
    p += sprintf(p, "\"special_effect\":%u,", s->status.special_effect);
    p += sprintf(p, "\"wb_mode\":%u,", s->status.wb_mode);
    p += sprintf(p, "\"awb\":%u,", s->status.awb);
    p += sprintf(p, "\"awb_gain\":%u,", s->status.awb_gain);
    p += sprintf(p, "\"aec\":%u,", s->status.aec);
    p += sprintf(p, "\"aec2\":%u,", s->status.aec2);
    p += sprintf(p, "\"ae_level\":%d,", s->status.ae_level);
    p += sprintf(p, "\"aec_value\":%u,", s->status.aec_value);
    p += sprintf(p, "\"agc\":%u,", s->status.agc);
    p += sprintf(p, "\"agc_gain\":%u,", s->status.agc_gain);
    p += sprintf(p, "\"gainceiling\":%u,", s->status.gainceiling);
    p += sprintf(p, "\"bpc\":%u,", s->status.bpc);
    p += sprintf(p, "\"wpc\":%u,", s->status.wpc);
    p += sprintf(p, "\"raw_gma\":%u,", s->status.raw_gma);
    p += sprintf(p, "\"lenc\":%u,", s->status.lenc);
    p += sprintf(p, "\"hmirror\":%u,", s->status.hmirror);
    p += sprintf(p, "\"dcw\":%u,", s->status.dcw);
    p += sprintf(p, "\"colorbar\":%u", s->status.colorbar);
#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    p += sprintf(p, ",\"led_intensity\":%u", led_duty);
#else
    p += sprintf(p, ",\"led_intensity\":%d", -1);
#endif

    // 添加SD卡空间信息（单位：GB，保留2位小数）
    uint64_t totalSpace = getSDTotalSpaceMB();
    uint64_t usedSpace = getSDUsedSpaceMB();
    uint64_t freeSpace = getSDFreeSpaceMB();
    p += sprintf(p, ",\"sd_total\":%.2f", totalSpace / 100.0);
    p += sprintf(p, ",\"sd_used\":%.2f", usedSpace / 100.0);
    p += sprintf(p, ",\"sd_free\":%.2f", freeSpace / 100.0);

    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t xclk_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char _xclk[32];

    if (parse_get(req, &buf) != ESP_OK) {
        return ESP_FAIL;
    }
    if (httpd_query_key_value(buf, "xclk", _xclk, sizeof(_xclk)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int xclk = atoi(_xclk);
    ESP_LOGI(TAG, "Set XCLK: %d MHz", xclk);

    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_xclk(s, LEDC_TIMER_0, xclk);
    if (res) {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}


static esp_err_t reg_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char _reg[32];
    char _mask[32];
    char _val[32];

    if (parse_get(req, &buf) != ESP_OK) {
        return ESP_FAIL;
    }
    if (httpd_query_key_value(buf, "reg", _reg, sizeof(_reg)) != ESP_OK ||
        httpd_query_key_value(buf, "mask", _mask, sizeof(_mask)) != ESP_OK ||
        httpd_query_key_value(buf, "val", _val, sizeof(_val)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int reg = atoi(_reg);
    int mask = atoi(_mask);
    int val = atoi(_val);
    ESP_LOGI(TAG, "Set Register: reg: 0x%02x, mask: 0x%02x, value: 0x%02x", reg, mask, val);

    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_reg(s, reg, mask, val);
    if (res) {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t greg_handler(httpd_req_t *req)
{
    char *buf = NULL;
    char _reg[32];
    char _mask[32];

    if (parse_get(req, &buf) != ESP_OK) {
        return ESP_FAIL;
    }
    if (httpd_query_key_value(buf, "reg", _reg, sizeof(_reg)) != ESP_OK ||
        httpd_query_key_value(buf, "mask", _mask, sizeof(_mask)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int reg = atoi(_reg);
    int mask = atoi(_mask);
    sensor_t *s = esp_camera_sensor_get();
    int res = s->get_reg(s, reg, mask);
    if (res < 0) {
        return httpd_resp_send_500(req);
    }
    ESP_LOGI(TAG, "Get Register: reg: 0x%02x, mask: 0x%02x, value: 0x%02x", reg, mask, res);

    char buffer[20];
    const char * val = itoa(res, buffer, 10);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, val, strlen(val));
}

static int parse_get_var(char *buf, const char * key, int def)
{
    char _int[16];
    if(httpd_query_key_value(buf, key, _int, sizeof(_int)) != ESP_OK){
        return def;
    }
    return atoi(_int);
}

static esp_err_t pll_handler(httpd_req_t *req)
{
    char *buf = NULL;

    if (parse_get(req, &buf) != ESP_OK) {
        return ESP_FAIL;
    }

    int bypass = parse_get_var(buf, "bypass", 0);
    int mul = parse_get_var(buf, "mul", 0);
    int sys = parse_get_var(buf, "sys", 0);
    int root = parse_get_var(buf, "root", 0);
    int pre = parse_get_var(buf, "pre", 0);
    int seld5 = parse_get_var(buf, "seld5", 0);
    int pclken = parse_get_var(buf, "pclken", 0);
    int pclk = parse_get_var(buf, "pclk", 0);
    free(buf);

    ESP_LOGI(TAG, "Set Pll: bypass: %d, mul: %d, sys: %d, root: %d, pre: %d, seld5: %d, pclken: %d, pclk: %d", bypass, mul, sys, root, pre, seld5, pclken, pclk);
    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_pll(s, bypass, mul, sys, root, pre, seld5, pclken, pclk);
    if (res) {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t win_handler(httpd_req_t *req)
{
    char *buf = NULL;

    if (parse_get(req, &buf) != ESP_OK) {
        return ESP_FAIL;
    }

    int startX = parse_get_var(buf, "sx", 0);
    int startY = parse_get_var(buf, "sy", 0);
    int endX = parse_get_var(buf, "ex", 0);
    int endY = parse_get_var(buf, "ey", 0);
    int offsetX = parse_get_var(buf, "offx", 0);
    int offsetY = parse_get_var(buf, "offy", 0);
    int totalX = parse_get_var(buf, "tx", 0);
    int totalY = parse_get_var(buf, "ty", 0);
    int outputX = parse_get_var(buf, "ox", 0);
    int outputY = parse_get_var(buf, "oy", 0);
    bool scale = parse_get_var(buf, "scale", 0) == 1;
    bool binning = parse_get_var(buf, "binning", 0) == 1;
    free(buf);

    ESP_LOGI(TAG, "Set Window: Start: %d %d, End: %d %d, Offset: %d %d, Total: %d %d, Output: %d %d, Scale: %u, Binning: %u", startX, startY, endX, endY, offsetX, offsetY, totalX, totalY, outputX, outputY, scale, binning);
    sensor_t *s = esp_camera_sensor_get();
    int res = s->set_res_raw(s, startX, startY, endX, endY, offsetX, offsetY, totalX, totalY, outputX, outputY, scale, binning);
    if (res) {
        return httpd_resp_send_500(req);
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

static esp_err_t index_handler(httpd_req_t *req){
    // 验证认证
    auth_result_t auth_result = auth_verify(req);
    if(auth_result != AUTH_SUCCESS) {
        ESP_LOGW(TAG, "Index handler: authentication failed (%d)", auth_result);
        return auth_send_401(req);
    }

    httpd_resp_set_type(req, "text/html");
    // 使用我们的未压缩HTML文件
    return httpd_resp_send(req, (const char *)index_ov2640_html, strlen(index_ov2640_html));
}

void startCameraServer()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL
    };

    httpd_uri_t status_uri = {
        .uri = "/status",
        .method = HTTP_GET,
        .handler = status_handler,
        .user_ctx = NULL
    };

    httpd_uri_t cmd_uri = {
        .uri = "/control",
        .method = HTTP_GET,
        .handler = cmd_handler,
        .user_ctx = NULL
    };

    httpd_uri_t capture_uri = {
        .uri = "/capture",
        .method = HTTP_GET,
        .handler = capture_handler,
        .user_ctx = NULL
    };

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL
    };

    httpd_uri_t bmp_uri = {
        .uri = "/bmp",
        .method = HTTP_GET,
        .handler = bmp_handler,
        .user_ctx = NULL
    };

    httpd_uri_t xclk_uri = {
        .uri = "/xclk",
        .method = HTTP_GET,
        .handler = xclk_handler,
        .user_ctx = NULL
    };

    httpd_uri_t reg_uri = {
        .uri = "/reg",
        .method = HTTP_GET,
        .handler = reg_handler,
        .user_ctx = NULL
    };

    httpd_uri_t greg_uri = {
        .uri = "/greg",
        .method = HTTP_GET,
        .handler = greg_handler,
        .user_ctx = NULL
    };

    httpd_uri_t pll_uri = {
        .uri = "/pll",
        .method = HTTP_GET,
        .handler = pll_handler,
        .user_ctx = NULL
    };

    httpd_uri_t win_uri = {
        .uri = "/resolution",
        .method = HTTP_GET,
        .handler = win_handler,
        .user_ctx = NULL
    };

    ra_filter_init(&ra_filter, 20);


    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
        httpd_register_uri_handler(camera_httpd, &status_uri);
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        httpd_register_uri_handler(camera_httpd, &bmp_uri);

        httpd_register_uri_handler(camera_httpd, &xclk_uri);
        httpd_register_uri_handler(camera_httpd, &reg_uri);
        httpd_register_uri_handler(camera_httpd, &greg_uri);
        httpd_register_uri_handler(camera_httpd, &pll_uri);
        httpd_register_uri_handler(camera_httpd, &win_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    ESP_LOGI(TAG, "Starting stream server on port: '%d'", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}
