/**********************************************************************
  Filename    : Camera Web Server
  Description : The camera images captured by the ESP32S3 are displayed on the web page.
                支持功能：
                1. Web端实时视频流显示
                2. Web端控制拍照并保存到SD卡
                3. SD卡文件系统支持
                4. 系统运行时长显示
                5. 视频录制功能（AVI格式，MJPEG编码）
                6. 视频自动分段录制（2分钟一段）
                7. 启动时自动开始录制
                8. 时间戳文件名（YYYYMMDDHHMM格式，年月日时分）
                9. SD卡空间自动清理（55GB阈值）
                10. NTP时间同步功能
  Original Author      : www.freenove.com
  Modifier             ：Zhuwenqian
  Modification: 2026-01-28
**********************************************************************/
#include "esp_camera.h"
#include <WiFi.h>
#include <time.h>
#include "sd_read_write.h"
#include "auth.h"

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD

#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid     = "zhuline";
const char* password = "zhu8437547";

// 运行时长统计
unsigned long startTime = 0;

void startCameraServer();

// 视频录制任务
void videoRecordTask(void *pvParameters);

// 获取运行时长（秒）
unsigned long getUptimeSeconds() {
  return (millis() - startTime) / 1000;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // 初始化认证模块
  if(!auth_init()) {
    Serial.println("Failed to initialize auth module");
  } else {
    Serial.println("Auth module initialized");
  }

  // 记录启动时间
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
  config.frame_size = FRAMESIZE_SVGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
  
  // if PSRAM IC present, init with SVGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_QVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, 1); // flip it back
  s->set_brightness(s, 1); // up the brightness just a bit
  s->set_saturation(s, 0); // lower the saturation
  
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // 配置NTP时间同步
  Serial.println("Configuring NTP time...");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
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

  // 初始化SD卡
  Serial.println("Initializing SD card...");
  sdmmcInit();
  
  // 初始化照片保存目录
  initPhotoDir();

  // 启动视频录制（启动时自动开始录制）
  Serial.println("Starting video recording...");
  if(startVideoRecording(15, 1280, 720)){
    Serial.println("Video recording started successfully");
    
    // 创建视频录制任务
    int *fpsParam = (int*)malloc(sizeof(int));
    *fpsParam = 15;
    xTaskCreate(videoRecordTask, "video_record", 4096, fpsParam, 5, NULL);
  } else {
    Serial.println("Failed to start video recording");
  }

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

// 视频录制任务
void videoRecordTask(void *pvParameters) {
  camera_fb_t *fb = NULL;
  int fps = *((int*)pvParameters);
  int delayMs = 1000 / fps;
  
  Serial.printf("Video recording task started, FPS: %d\n", fps);
  
  while(isRecordingVideo()) {
    // 获取摄像头帧
    fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed during recording");
      vTaskDelay(pdMS_TO_TICKS(delayMs));
      continue;
    }
    
    // 写入视频帧
    if(!writeVideoFrame(fb->buf, fb->len)) {
      Serial.println("Failed to write video frame");
    }
    
    // 释放帧缓冲区
    esp_camera_fb_return(fb);
    
    // 延迟以控制帧率
    vTaskDelay(pdMS_TO_TICKS(delayMs));
  }
  
  Serial.println("Video recording task stopped");
  free(pvParameters);
  vTaskDelete(NULL);
}

void loop() {
  // Do nothing. Everything is done in another task by the web server
  delay(10000);
  
  // 每小时清理一次过期会话（3600秒 = 1小时）
  static unsigned long last_cleanup = 0;
  unsigned long current_time = millis();
  if(current_time - last_cleanup >= 3600000) { // 3600000毫秒 = 1小时
    uint32_t cleaned = auth_cleanup_expired_sessions();
    if(cleaned > 0) {
      Serial.printf("Cleaned up %u expired sessions\n", cleaned);
    }
    last_cleanup = current_time;
  }
}
