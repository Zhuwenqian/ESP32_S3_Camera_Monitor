/**********************************************************************
  Filename    : Camera Web Server / 摄像头Web服务器
  Description : The camera images captured by ESP32S3 are displayed on web page.
                ESP32S3采集的摄像头图像在网页上显示。
                支持功能 / Supported Features:
                1. Web端实时视频流显示 / Real-time video streaming on Web
                2. Web端控制拍照并保存到SD卡 / Web-controlled photo capture and save to SD card
                3. SD卡文件系统支持 / SD card file system support
                4. 系统运行时长显示 / System uptime display
                5. 视频录制功能（AVI格式，MJPEG编码）/ Video recording (AVI format, MJPEG encoding)
                6. 视频自动分段录制（2分钟一段）/ Auto-segmented video recording (2 minutes per segment)
                7. 启动时自动开始录制 / Auto-start recording on boot
                8. 时间戳文件名（YYYYMMDDHHMM格式，年月日时分）/ Timestamp filename (YYYYMMDDHHMM format)
                9. SD卡空间自动清理（动态阈值，每次清理释放约2GB空间）/ Auto SD card cleanup (dynamic threshold, frees ~2GB per cleanup)
                10. NTP时间同步功能 / NTP time synchronization
                11. HTTP Basic Authentication认证 / HTTP Basic Authentication
                12. OTA升级功能 / OTA upgrade function
                13. WS2812B LED状态指示 / WS2812B LED status indication
  Auther      : ESP32-S3监控项目 / ESP32-S3 Monitoring Project
  Modification: 2026-02-04
  
  代码来源 / Code Source:
  - 基于Freenove ESP32-S3 Camera Example修改 / Modified from Freenove ESP32-S3 Camera Example
  - 集成Espressif CameraWebServer (Apache License 2.0) / Integrated Espressif CameraWebServer
  
  许可证 / License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)
  详情 / Details: https://creativecommons.org/licenses/by-nc-sa/3.0/
  
  重要限制 / Important Restrictions:
  - 禁止商业用途 / Commercial use prohibited
  - 必须保留原作者署名 / Must retain original author attribution
  - 衍生作品必须使用相同许可证 / Derivative works must use same license
**********************************************************************/
#include "esp_camera.h"
#include <WiFi.h>
#include <time.h>
#include "sd_read_write.h"
#include "auth.h"
#include "servo_control.h"
#include "ota_server.h"
#include "led_control.h"

// =================== / ===================
// Select camera model / 选择摄像头型号 / 选择摄像头型号
// =================== / ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM / 有PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM / 有PSRAM
#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM / 有PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM / 有PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM / M5Camera版本B有PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM / 有PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM / 无PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM / 无PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM / 有PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM / 无PSRAM
// ** Espressif Internal Boards / Espressif内部开发板 **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD

#include "camera_pins.h"

// =================== / ===========================
// Enter your WiFi credentials / 输入WiFi凭证 / 输入WiFi凭证
// =================== / ===========================
const char* ssid     = "zhuline";
const char* password = "zhu8437547";

// 运行时长统计 / Uptime counter / Uptime counter
unsigned long startTime = 0;

void startCameraServer();

// 视频录制任务 / Video recording task / Video recording task / Video recording task
void videoRecordTask(void *pvParameters);

// 获取运行时长（秒）/ Get uptime in seconds/ Get uptime in seconds
unsigned long getUptimeSeconds() {
  return (millis() - startTime) / 1000;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // 初始化LED / Initialize LED / Initialize LED
  Serial.println("Initializing LED... / 初始化LED...");
  led_init();
  
  // 设置LED为初始化慢闪状态 / Set LED to initialization slow flash status
  led_set_status(LED_INIT_SLOW_FLASH);

  // 初始化认证模块 / Initialize authentication module / Initialize authentication module
  if(!auth_init()) {
    Serial.println("Failed to initialize auth module / 认证模块初始化失败");
  } else {
    Serial.println("Auth module initialized / 认证模块已初始化");
  }

  // 初始化云台舵机 / Initialize pan-tilt servos / Initialize pan-tilt servos
  Serial.println("Initializing pan-tilt servos... / 初始化云台舵机...");
  if(servo_init()) {
    Serial.println("Pan-tilt servos initialized / 云台舵机初始化完成");
  } else {
    Serial.println("Failed to initialize pan-tilt servos / 云台舵机初始化失败");
  }

  // 记录启动时间 / Record start time / Record start time
  startTime = millis();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_XGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming / 用于流媒体 / 用于流媒体
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // if PSRAM IC present, init with SVGA resolution and higher JPEG quality / 如果有PSRAM，使用SVGA分辨率和更高的JPEG质量
  // for larger pre-allocated frame buffer. / 以获得更大的预分配帧缓冲区。
  // 如果有PSRAM，使用SVGA分辨率和更高的JPEG质量
  // 以获得更大的预分配帧缓冲区。
  if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available / 没有PSRAM时限制帧大小
    // 没有PSRAM时限制帧大小
    config.frame_size = FRAMESIZE_QVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // camera init / 摄像头初始化 / 摄像头初始化
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x / 摄像头初始化失败，错误代码 0x%x", err);
    // 设置LED为摄像头错误状态 / Set LED to camera error status
    led_set_status(LED_CAMERA_ERROR);
    return;
  }

  // 摄像头初始化成功，设置LED为就绪状态 / Camera init successful, set LED to ready status
  led_set_status(LED_CAMERA_READY);

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated / 初始传感器垂直翻转，颜色有点饱和
  // 初始传感器垂直翻转，颜色有点饱和
  s->set_vflip(s, 1); // flip it back / 翻转回来 / 翻转回来
  s->set_brightness(s, 1); // up the brightness just a bit / 稍微提高亮度 / 稍微提高亮度
  s->set_saturation(s, 0); // lower the saturation / 降低饱和度 / 降低饱和度
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected / WiFi已连接");

  // 配置NTP时间同步 / Configure NTP time synchronization / Configure NTP time synchronization
  Serial.println("Configuring NTP time... / 配置NTP时间...");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: / 等待NTP时间同步: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  Serial.printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n",
                timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  // 初始化SD卡 / Initialize SD card / Initialize SD card
  Serial.println("Initializing SD card... / 初始化SD卡...");
  if(!sdmmcInit()){
    Serial.println("SD card initialization failed / SD卡初始化失败");
    // 设置LED为SD卡错误状态 / Set LED to SD card error status
    led_set_status(LED_SD_ERROR);
  } else {
    Serial.println("SD card initialized successfully / SD卡初始化成功");
  }
  
  // 初始化照片保存目录 / Initialize photo save directory / Initialize photo save directory
  initPhotoDir();

  // 清理无效视频文件（大小为0KB的视频）/ Clean up invalid video files (0KB video files) / Clean up invalid video files (0KB video files)
  Serial.println("Cleaning up invalid video files... / 清理无效视频文件...");
  int cleanedFiles = cleanInvalidVideoFiles();
  if(cleanedFiles > 0){
    Serial.printf("Cleaned up %d invalid video file(s)\n", cleanedFiles);
    Serial.printf("清理了 %d 个无效视频文件\n", cleanedFiles);
  } else if(cleanedFiles == 0){
    Serial.println("No invalid video files found / 未发现无效视频文件");
  } else {
    Serial.println("Failed to clean up invalid video files / 清理无效视频文件失败");
  }

  // 启动视频录制（启动时自动开始录制）/ Start video recording (auto-start on boot)/ Start video recording (auto-start on boot)
  Serial.println("Starting video recording... / 启动视频录制...");
  if(startVideoRecording(20, 1024, 768)){
    Serial.println("Video recording started successfully / 视频录制启动成功");
    
    // 创建视频录制任务 / Create video recording task / Create video recording task
    int *fpsParam = (int*)malloc(sizeof(int));
    *fpsParam = 20;
    xTaskCreate(videoRecordTask, "video_record", 4096, fpsParam, 5, NULL);
  } else {
    Serial.println("Failed to start video recording / 视频录制启动失败");
  }

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect / 摄像头就绪！使用 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' 连接");
}

// 视频录制任务 / Video recording task / Video recording task / Video recording task
void videoRecordTask(void *pvParameters) {
  camera_fb_t *fb = NULL;
  int fps = *((int*)pvParameters);
  int delayMs = 1000 / fps;
  
  Serial.printf("Video recording task started, FPS: %d / 视频录制任务已启动，帧率: %d\n", fps);
  
  while(isRecordingVideo()) {
    // 获取摄像头帧 / Get camera frame / Get camera frame
    fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed during recording / 录制过程中摄像头捕获失败");
      vTaskDelay(pdMS_TO_TICKS(delayMs));
      continue;
    }
    
    // 写入视频帧 / Write video frame / Write video frame
    if(!writeVideoFrame(fb->buf, fb->len)) {
      Serial.println("Failed to write video frame / 写入视频帧失败");
    }
    
    // 释放帧缓冲区 / Release frame buffer / Release frame buffer
    esp_camera_fb_return(fb);
    
    // 延迟以控制帧率 / Delay to control frame rate / Delay to control frame rate
    vTaskDelay(pdMS_TO_TICKS(delayMs));
  }
  
  Serial.println("Video recording task stopped / 视频录制任务已停止");
  free(pvParameters);
  vTaskDelete(NULL);
}

void loop() {
  // Do nothing. Everything is done in another task by the web server / 什么都不做。所有工作都由Web服务器在另一个任务中完成
  // 什么都不做。所有工作都由Web服务器在另一个任务中完成
  delay(10000);
  
  // 每小时清理一次过期会话（3600秒 = 1小时）/ Clean up expired sessions every hour (3600s = 1 hour)/ Clean up expired sessions every hour (3600s = 1 hour)
  static unsigned long last_cleanup = 0;
  unsigned long current_time = millis();
  if(current_time - last_cleanup >= 3600000) { // 3600000毫秒 = 1小时 / 3600000ms = 1 hour / 3600000ms = 1 hour
    uint32_t cleaned = auth_cleanup_expired_sessions();
    if(cleaned > 0) {
      Serial.printf("Cleaned up %u expired sessions / 清理了 %u 个过期会话\n", cleaned);
    }
    last_cleanup = current_time;
  }
}
