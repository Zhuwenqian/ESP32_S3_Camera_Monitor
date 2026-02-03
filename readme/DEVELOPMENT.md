# Development Documentation

## License Information

This project is licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)

### Important Restrictions

- ❌ **Commercial Use Prohibited**: This project cannot be used for any commercial purposes 
- ✅ **Non-Commercial Use Allowed **: Personal learning, research, education, and other non-commercial purposes
- ✅ **Modification and Distribution Allowed**: But must use the same license
- ✅ **Attribution Required**: Must retain the original author's attribution

For details, please visit: https://creativecommons.org/licenses/by-nc-sa/3.0/

## Code Sources

This project is modified from the following open-source projects:
- Freenove ESP32-S3 Camera Example (CC BY-NC-SA 3.0)
- Espressif Camera Library Example (Apache License 2.0) - from Arduino ESP32 Board Package

---

## Project Overview

### 1.1 Project Background
This project is based on the ESP32-S3-CAM (Guoyun version) development board, implementing a basic network video monitoring system. The system supports real-time video streaming and photo capture, suitable for home monitoring, security monitoring, and other scenarios.

### 1.2 Project Goals
- Implement real-time video streaming display on the Web (Manual Control)
- Implement photo capture control on the Web
- Implement opening video streaming in a new tab
- Implement basic HTTP server

### 1.3 Technology Stack
- **Development Framework**: Arduino Framework
- **Camera Driver**: ESP32 Camera Library (Official)
- **Web Server**: ESP-IDF httpd component
- **Video Encoding**: MJPEG

---

## II. System Architecture Design

### 2.1 System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                        ESP32-S3-CAM                          │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐                                           │
│  │  Camera Module │                                         │
│  │  OV5640      │                                           │
│  └──────────────┘                                           │
│         │                                                   │
│  ┌──────┴─────────────────────────────────────────────┐   │
│  │              Core Control Module                   │   │
│  │  - Video capture and encoding                      │   │
│  │  - Video streaming                                 │   │
│  └─────────────────────────────────────────────────┘   │
│         │                                                 │
│  ┌──────┴─────────────────────────────────────────────┐   │
│  │              Web Server Module                     │   │
│  │  - Video streaming service                         │   │
│  │  - Photo capture API                               │   │
│  │  - Web interface                                   │   │
│  └───────────────────────────────────────────────────┘   │
│         │                                                 │
│  ┌──────┴─────────────────────────────────────────────┐   │
│  │              WiFi Communication Module              │   │
│  └───────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ WiFi
                            │
         ┌──────────────────┴──────────────────┐
         │                                      │
    ┌─────┴─────┐                        ┌─────┴─────┐
    │  Web Browser │                       │  Mobile Device │
    └───────────┘                        └───────────┘
```

### 2.2 Module Division

#### 2.2.1 Camera Module
- Responsible for OV5640 camera initialization and configuration
- Responsible for video frame capture
- Responsible for JPEG encoding
- Supports multiple resolutions

#### 2.2.2 Web Server Module
- Provides video streaming interface
- Provides photo capture API
- Provides web interface

#### 2.2.3 WiFi Communication Module
- Responsible for WiFi connection
- Provides network communication support

---

## III. Functional Module Detailed Design

### 3.1 Web Video Streaming Display Function

#### 3.1.1 Function Description
Display video stream captured by ESP32-S3-CAM in real-time on the web, supporting basic video display functions.

#### 3.1.2 Technical Implementation
- Use MJPEG format for video streaming
- Use multipart/x-mixed-replace protocol
- Use ESP-IDF httpd component to implement HTTP server
- Support video stream retry mechanism

#### 3.1.3 API Interface
```
GET /stream
```

**Response Format:**
```
Content-Type: multipart/x-mixed-replace;boundary=123456789000000000000987654321

--123456789000000000000987654321
Content-Type: image/jpeg
Content-Length: 12345
X-Timestamp: 1234567890.123456

[JPEG data]
--123456789000000000000987654321
...
```

#### 3.1.4 Key Parameters
- **Resolution**: According to camera configuration
- **JPEG Quality**: According to camera configuration

#### 3.1.5 Code Structure
```cpp
// Video stream handler function
static esp_err_t stream_handler(httpd_req_t *req) {
    // Set response type
    httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    
    // Loop to send video frames
    while (true) {
        // Get camera frame
        camera_fb_t *fb = esp_camera_fb_get();
        
        // Convert to JPEG format
        // ...
        
        // Send frame data
        httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        
        // Release frame buffer
        esp_camera_fb_return(fb);
    }
}
```

---

### 3.2 Web Photo Capture Function

#### 3.2.1 Function Description
Control camera to capture photos on the web, obtaining current camera image.

#### 3.2.2 Technical Implementation
- Use ESP32 Camera Library to capture single frame image
- Use JPEG format for encoding
- Return image data through HTTP response

#### 3.2.3 API Interface
```
GET /capture
```

**Response Format:**
- Content-Type: image/jpeg
- Response body: JPEG image data

#### 3.2.4 Key Parameters
- **Image Format**: JPEG
- **Resolution**: According to camera configuration
- **JPEG Quality**: According to camera configuration

#### 3.2.5 Code Structure
```cpp
// Photo capture handler function
static esp_err_t capture_handler(httpd_req_t *req) {
    // Get camera frame
    camera_fb_t *fb = esp_camera_fb_get();
    
    // Set response type
    httpd_resp_set_type(req, "image/jpeg");
    
    // Send image data
    httpd_resp_send(req, (const char *)fb->buf, fb->len);
    
    // Release frame buffer
    esp_camera_fb_return(fb);
    
    return ESP_OK;
}
```

---

### 3.3 Open Video Stream in New Tab Function

#### 3.3.1 Function Description
Open video stream in a new tab, providing clearer video viewing experience.

#### 3.3.2 Technical Implementation
- Use JavaScript's window.open() function
- Build complete video stream URL
- Load video stream in new tab

#### 3.3.3 Implementation Code
```javascript
function openStreamInNewTab() {
    const streamUrl = 'http://' + window.location.hostname + ':81/stream';
    window.open(streamUrl, '_blank');
}
```

---

## IV. Hardware Configuration

### 4.1 Camera Configuration

#### 4.1.1 OV5640 Camera Parameters
```cpp
#define CAMERA_MODEL_ESP32S3_EYE

// Camera pin configuration
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM      4
#define SIOC_GPIO_NUM      5

#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM        8
#define Y3_GPIO_NUM        9
#define Y2_GPIO_NUM       11

#define VSYNC_GPIO_NUM     6
#define HREF_GPIO_NUM      7
#define PCLK_GPIO_NUM     13
```

#### 4.1.2 Camera Configuration Parameters
```cpp
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
config.xclk_freq_hz = 22000000;  // 22MHz clock
config.frame_size = FRAMESIZE_UXGA;
config.pixel_format = PIXFORMAT_JPEG;
config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
config.fb_location = CAMERA_FB_IN_PSRAM;
config.jpeg_quality = 12;
config.fb_count = 1;

// If PSRAM is available, use higher resolution and quality
if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
} else {
    // Limit frame size when no PSRAM
    config.frame_size = FRAMESIZE_XGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
}
```

#### 4.1.3 exFAT File System
- ESP32-S3-CAM's SD card library supports exFAT file system by default
- No additional configuration needed, use SD library directly
- Supports large files (>4GB)

---

## V. Web Interface Design

### 5.1 Interface Layout

```
┌─────────────────────────────────────────────────────────────┐
│              ESP32-S3 Monitoring Console                    │
├─────────────────────────────────────────────────────────────┤
│  Language: [English ▼]  Theme: [Light ▼]                 │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐  │
│  │                                                     │  │
│  │              Video Display Area                      │  │
│  │                                                     │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐  │
│  │  [Capture Photo]  [Getstream]          │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 Language and Theme Selection

#### 5.2.1 Language Selection Function
- **Supported Languages**: English, Chinese
- **Default Language**: English
- **Location**: Top-right corner of the page
- **Persistence**: Use localStorage to save user selection
- **Auto Restore**: Automatically restore last selected language on page load

**Technical Implementation:**
- Use JavaScript's translations object to store translations for all UI elements
- Identify elements that need translation through data-lang attribute
- Use changeLanguage() function to switch language
- Use localStorage.setItem('preferredLanguage', currentLanguage) to save preference
- Use localStorage.getItem('preferredLanguage') to restore preference

**Translation Object Structure:**
```javascript
const translations = {
    en: {
        title: "ESP32-S3 Monitoring Console",
        language: "Language",
        theme: "Theme",
        lightTheme: "Light",
        darkTheme: "Dark",
        liveVideo: "Live Video",
        loadingStream: "Loading video stream...",
        capturePhoto: "Capture Photo",
        openStreamNewTab: "Open Stream in New Tab",
        sdCardInfo: "SD Card Storage Info",
        totalSpace: "Total Space",
        usedSpace: "Used Space",
        freeSpace: "Free Space",
        systemUptime: "System Uptime",
        uptimeUnit: "Uptime",
        cameraSettings: "Camera Settings",
        resolution: "Resolution",
        imageQuality: "Image Quality",
        lowQuality: "Low Quality (63)",
        mediumQuality: "Medium Quality (31)",
        highQuality: "High Quality (10)",
        veryHighQuality: "Very High Quality (5)",
        brightness: "Brightness",
        contrast: "Contrast",
        saturation: "Saturation"
    },
    zh: {
        title: "ESP32-S3监控控制台",
        language: "语言",
        theme: "主题",
        lightTheme: "浅色",
        darkTheme: "深色",
        liveVideo: "实时视频",
        loadingStream: "正在加载视频流...",
        capturePhoto: "拍照",
        openStreamNewTab: "在新标签页中打开视频流",
        sdCardInfo: "SD卡存储信息",
        totalSpace: "总空间",
        usedSpace: "已用空间",
        freeSpace: "剩余空间",
        systemUptime: "系统运行时长",
        uptimeUnit: "运行时间",
        cameraSettings: "摄像头设置",
        resolution: "分辨率",
        imageQuality: "图像质量",
        lowQuality: "低质量 (63)",
        mediumQuality: "中等质量 (31)",
        highQuality: "高质量 (10)",
        veryHighQuality: "极高质量 (5)",
        brightness: "亮度",
        contrast: "对比度",
        saturation: "饱和度"
    }
};
```

#### 5.2.2 Theme Selection Function
- **Supported Themes**: Light, Dark
- **Default Theme**: Light
- **Location**: Top-right corner of the page (next to language selector)
- **Persistence**: Use localStorage to save user selection
- **Auto Restore**: Automatically restore last selected theme on page load
- **Transition Animation**: 0.3 seconds smooth transition

**Technical Implementation:**
- Use CSS variables to define theme colors
- Switch theme through data-theme attribute
- Use changeTheme() function to switch theme
- Use localStorage.setItem('preferredTheme', currentTheme) to save preference
- Use localStorage.getItem('preferredTheme') to restore preference

**CSS Variable Definition:**
```css
:root {
    /* Light theme variables */
    --bg-color: #f5f5f5;
    --container-bg: #ffffff;
    --text-color: #333333;
    --section-bg: #f9f9f9;
    --section-title-color: #555555;
    --border-color: #dddddd;
    --btn-primary-bg: #007bff;
    --btn-primary-hover: #0056b3;
    --btn-success-bg: #28a745;
    --btn-success-hover: #218838;
    --btn-danger-bg: #dc3545;
    --btn-danger-hover: #c82333;
    --btn-warning-bg: #ffc107;
    --btn-warning-hover: #e0a800;
    --btn-warning-text: #212529;
    --info-label-color: #666666;
    --info-value-color: #333333;
    --sd-info-value-color: #333333;
    --sd-info-unit-color: #999999;
    --setting-label-color: #666666;
    --setting-border-color: #dddddd;
    --setting-border-hover: #007bff;
    --uptime-value-color: #333333;
    --uptime-unit-color: #666666;
    --language-label-color: #666666;
    --loading-text-color: #666666;
    --shadow-color: rgba(0, 0, 0, 0.1);
}

[data-theme="dark"] {
    /* Dark theme variables */
    --bg-color: #1a1a1a;
    --container-bg: #2d2d2d;
    --text-color: #e0e0e0;
    --section-bg: #383838;
    --section-title-color: #b0b0b0;
    --border-color: #555555;
    --btn-primary-bg: #007bff;
    --btn-primary-hover: #0056b3;
    --btn-success-bg: #28a745;
    --btn-success-hover: #218838;
    --btn-danger-bg: #dc3545;
    --btn-danger-hover: #c82333;
    --btn-warning-bg: #ffc107;
    --btn-warning-hover: #e0a800;
    --btn-warning-text: #212529;
    --info-label-color: #b0b0b0;
    --info-value-color: #e0e0e0;
    --sd-info-value-color: #e0e0e0;
    --sd-info-unit-color: #888888;
    --setting-label-color: #b0b0b0;
    --setting-border-color: #555555;
    --setting-border-hover: #007bff;
    --uptime-value-color: #e0e0e0;
    --uptime-unit-color: #b0b0b0;
    --language-label-color: #b0b0b0;
    --loading-text-color: #b0b0b0;
    --shadow-color: rgba(0, 0, 0, 0.3);
}

/* Global transition effect */
* {
    transition: background-color 0.3s, color 0.3s, border-color 0.3s;
}
```

**Theme Switching Function:**
```javascript
function changeTheme() {
    const themeSelect = document.getElementById('theme-select');
    currentTheme = themeSelect.value;
    
    // Set data-theme attribute
    if (currentTheme === 'dark') {
        document.documentElement.setAttribute('data-theme', 'dark');
    } else {
        document.documentElement.removeAttribute('data-theme');
    }
    
    // Save theme preference to localStorage
    localStorage.setItem('preferredTheme', currentTheme);
}
```

### 5.3 Function Buttons

#### 5.3.1 Photo Capture Button
- **Capture Photo**: Take a photo and open it in a new window

#### 5.3.2 Video Stream Control Button
- **Open Stream in New Tab**: Open video stream in a new tab

### 5.4 Status Display
- Video stream status
- Photo capture status

### 5.5 User Preference Persistence

#### 5.5.1 LocalStorage Usage
- **preferredLanguage**: Save user selected language ('en' or 'zh')
- **preferredTheme**: Save user selected theme ('light' or 'dark')

#### 5.5.2 Restore Preferences on Page Load
```javascript
// Restore language and theme preferences on page load
window.onload = function() {
    // Restore language preference
    const savedLanguage = localStorage.getItem('preferredLanguage');
    if (savedLanguage) {
        document.getElementById('language-select').value = savedLanguage;
        currentLanguage = savedLanguage;
        changeLanguage();
    }
    
    // Restore theme preference
    const savedTheme = localStorage.getItem('preferredTheme');
    if (savedTheme) {
        document.getElementById('theme-select').value = savedTheme;
        currentTheme = savedTheme;
        changeTheme();
    }
};
```

---

## VI. API Interface Documentation

### 6.1 Video Stream Interface

#### 6.1.1 Get Video Stream
```
GET /stream
```

**Response:**
```
Content-Type: multipart/x-mixed-replace;boundary=123456789000000000000987654321

--123456789000000000000987654321
Content-Type: image/jpeg
Content-Length: 12345
X-Timestamp: 1234567890.123456

[JPEG data]
--123456789000000000000987654321
...
```

### 6.2 Photo Capture Interface

#### 6.2.1 Capture Photo
```
GET /capture
```

**Response:**
- Content-Type: image/jpeg
- Response body: JPEG image data

### 6.3 System Status Interface

#### 6.3.1 Get System Status
```
GET /status
```

**Response:**
```json
{
  "xclk": 22,
  "pixformat": 3,
  "framesize": 7,
  "quality": 10,
  "brightness": 1,
  "contrast": 0,
  "saturation": 0,
  "sharpness": 0,
  "special_effect": 0,
  "wb_mode": 0,
  "awb": 1,
  "awb_gain": 1,
  "aec": 1,
  "aec2": 1,
  "ae_level": 0,
  "aec_value": 300,
  "agc": 1,
  "agc_gain": 0,
  "gainceiling": 0,
  "bpc": 0,
  "wpc": 1,
  "raw_gma": 1,
  "lenc": 1,
  "hmirror": 0,
  "dcw": 1,
  "colorbar": 0,
  "led_intensity": -1
}
```

---

## VII. File Structure Design

### 7.1 Project File Structure

```
ESP32-S3_Camera_Monitor/
├── ESP32_S3_Camera_Monitor.ino      # Main program file
├── camera_pins.h                    # Camera pin configuration
├── camera_index.h                   # Web interface HTML
├── app_httpd.cpp                    # HTTP server implementation
├── partitions.csv                   # Partition table file
├── readme/
│   ├── PROJECT_REQUIREMENTS.md
│   ├── TECHNOLOGY_STACK.md
│   ├── DEVELOPMENT.md
│   ├── IMPLEMENTATION.md
│   ├── FEATURE_UPDATES.md
│   └── examples/
│       ├── Sketch_07.1_CameraWebServer.ino
│       ├── app_httpd.cpp
│       ├── camera_index.h
│       ├── camera_pins.h
│       └── partitions.csv
```

---

## VIII. Configuration Parameters

### 8.1 WiFi Configuration

```cpp
// WiFi configuration
const char* ssid = "your-ssid";         // WiFi SSID
const char* password = "your-password";  // WiFi password
```

### 8.2 Camera Configuration

```cpp
// Camera configuration
#define CAMERA_MODEL_ESP32S3_EYE       // Camera model
#define CAMERA_XCLK_FREQ 20000000     // Camera clock frequency: 22MHz
#define CAMERA_FRAME_SIZE FRAMESIZE_UXGA  // Resolution: UXGA
#define CAMERA_JPEG_QUALITY 12        // JPEG quality: 12 (1-31, lower is better)
#define CAMERA_FB_COUNT 1             // Frame buffer count: 1

// If PSRAM is available, use higher resolution and quality
if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
} else {
    // Limit frame size when no PSRAM
    config.frame_size = FRAMESIZE_XGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
}
```

---

## IX. Error Handling Mechanism

### 9.1 Camera Error Handling

```cpp
// Camera initialization failure
if (esp_camera_init(&config) != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
}

// Frame capture failure
camera_fb_t *fb = esp_camera_fb_get();
if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
}
```

### 9.2 WiFi Error Handling

```cpp
// WiFi connection
WiFi.begin(ssid, password);
WiFi.setSleep(false);

while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
```

---

## X. Performance Optimization

### 10.1 Memory Optimization

- Use PSRAM to store camera frame buffers
- Reasonably set frame buffer count (2)
- Release unused memory in time
- Avoid memory leaks

### 10.2 Performance Optimization

- Optimize JPEG encoding quality to balance image quality and performance
- Reasonably allocate CPU resources

### 10.3 Network Optimization

- Use WiFi keep-alive to avoid frequent reconnections
- Support video stream retry mechanism

---

## XI. Test Plan

### 11.1 Functional Testing

#### 11.1.1 Video Stream Testing
- Test if video stream displays normally
- Test video stream retry mechanism
- Test opening video stream in new tab function

#### 11.1.2 Photo Capture Testing
- Test if photo capture works normally
- Test photo quality
- Test photo capture response speed

### 11.2 Stability Testing

#### 11.2.1 Long-term Operation Testing
- Test video stream long-term stability
- Test WiFi connection stability
- Test camera continuous operation stability

---

## XII. Deployment Instructions

### 12.1 Development Environment Setup

#### 12.1.1 Install Arduino IDE
1. Download Arduino IDE 2.x
2. Install ESP32 board support

#### 12.1.2 Install Required Libraries
```cpp
// Install the following libraries in Arduino IDE:
ArduinoJson
```

### 12.2 Compilation and Upload

#### 12.2.1 Configure Board
1. Select board: ESP32S3 Dev Module
2. Configure partition scheme: Use provided partitions.csv
3. Configure PSRAM: OPI PSRAM
4. Configure Flash mode: QIO
5. Configure Flash size: 8MB (according to actual hardware)

#### 12.2.2 Compile and Upload
1. Connect ESP32-S3-CAM to computer
2. Select correct serial port
3. Click upload button
4. Wait for compilation and upload to complete

### 12.3 First Boot

1. Power on ESP32-S3-CAM
2. Wait for WiFi connection
3. Check serial output to get IP address
4. Access IP address in browser
5. Test video stream display function
6. Test photo capture function
7. Test opening video stream in new tab function

---

## XIII. Maintenance Instructions

### 13.1 Daily Maintenance

- Check system logs
- Monitor device operation status
- Monitor SD card storage space
- System automatically cleans up invalid video files (0KB) on startup
- Regularly check video recording quality and file integrity
- SD card space automatic management:
  - Dynamic threshold detection: triggers cleanup when free space < 5GB
  - Delete oldest files by time: prioritizes deleting the oldest files
  - Cleanup target: releases approximately 2GB space per cleanup
  - Cleanup priority: configurable (videos only, videos then photos, photos only)
  - Space check interval: checks every 5 seconds by default

### 13.2 Troubleshooting

#### 13.2.1 Video Stream Not Displaying
- Check WiFi connection
- Check camera initialization
- Check browser compatibility
- Check network bandwidth

#### 13.2.2 Photo Capture Failure
- Check if camera is working normally
- Check network connection

### 13.3 Upgrade Instructions

1. Download new firmware version
2. Upload new firmware to ESP32-S3-CAM
3. Test all functions

---

## XIV. Security Recommendations

### 14.1 Network Security

- Use strong WiFi password
- Change WiFi password regularly
- Consider using WPA3 encryption
- Avoid using in public networks

### 14.2 Data Security

- Avoid exposing private information in video

### 14.3 Physical Security

- Ensure device is installed in a secure location
- Avoid exposing device to harsh environments
- Check device status regularly
- Ensure stable power supply

---

## XV. Future Extensions

### 15.1 Feature Extensions

- Add motion detection function
- Add OTA Update

### 15.2 Performance Extensions

- Support higher resolution
- Support higher frame rate
- Optimize video streaming performance

### 15.3 Actual Use

- Create an assembly tutorial and upload it to Bilibili


------------------------------------------------------------------------------------

# ESP32-S3监控项目开发文档

## 许可证信息

本项目采用 Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)

### 重要限制

- ❌ **禁止商业用途**：本项目不能用于任何商业目的
- ✅ **允许非商业用途**：个人学习、研究、教育等非商业用途
- ✅ **允许修改和分发**：但必须使用相同的许可证
- ✅ **必须署名**：必须保留原作者的署名

详情请查看：https://creativecommons.org/licenses/by-nc-sa/3.0/

## 代码来源

本项目基于以下开源项目修改而来：
- Freenove ESP32-S3 Camera Example (CC BY-NC-SA 3.0)
- Espressif Camera Library Example (Apache License 2.0) - 来自 Arduino ESP32 Board Package

---

## 项目概述

### 1.1 项目背景
本项目基于ESP32-S3-CAM(果云版)开发板，实现一个基础的网络视频监控系统。系统支持实时视频流传输和拍照功能，适用于家庭监控、安防监控等场景。

### 1.2 项目目标
- 实现Web端实时视频流显示
- 实现Web端控制拍照功能
- 实现在新标签页中打开视频流功能
- 实现基础HTTP服务器

### 1.3 技术栈
- **开发框架**：Arduino框架
- **摄像头驱动**：ESP32 Camera库（官方）
- **Web服务器**：ESP-IDF的httpd组件
- **视频编码**：MJPEG

---

## 二、系统架构设计

### 2.1 系统架构图

```
┌─────────────────────────────────────────────────────────────┐
│                        ESP32-S3-CAM                          │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐                                           │
│  │  摄像头模块   │                                           │
│  │  OV5640      │                                           │
│  └──────────────┘                                           │
│         │                                                   │
│  ┌──────┴─────────────────────────────────────────────┐   │
│  │              核心控制模块                          │   │
│  │  - 视频采集与编码                                │   │
│  │  - 视频流传输                                    │   │
│  └─────────────────────────────────────────────────┘   │
│         │                                                 │
│  ┌──────┴─────────────────────────────────────────────┐   │
│  │              Web服务器模块                          │   │
│  │  - 视频流服务                                       │   │
│  │  - 拍照控制API                                     │   │
│  │  - Web界面                                         │   │
│  └───────────────────────────────────────────────────┘   │
│         │                                                 │
│  ┌──────┴─────────────────────────────────────────────┐   │
│  │              WiFi通信模块                           │   │
│  └───────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ WiFi
                            │
         ┌──────────────────┴──────────────────┐
         │                                      │
    ┌─────┴─────┐                        ┌─────┴─────┐
    │  Web浏览器 │                        │  移动设备  │
    └───────────┘                        └───────────┘
```

### 2.2 模块划分

#### 2.2.1 摄像头模块
- 负责OV5640摄像头的初始化和配置
- 负责视频帧的采集
- 负责JPEG编码
- 支持多种分辨率

#### 2.2.2 Web服务器模块
- 提供视频流接口
- 提供拍照控制API
- 提供Web界面

#### 2.2.3 WiFi通信模块
- 负责WiFi连接
- 提供网络通信支持

---

## 三、功能模块详细设计

### 3.1 Web端视频流显示功能

#### 3.1.1 功能描述
在Web端实时显示ESP32-S3-CAM采集的视频流，支持基本的视频显示功能。

#### 3.1.2 技术实现
- 使用MJPEG格式进行视频流传输
- 使用multipart/x-mixed-replace协议
- 使用ESP-IDF的httpd组件实现HTTP服务器
- 支持视频流重试机制

#### 3.1.3 API接口
```
GET /stream
```

**响应格式：**
```
Content-Type: multipart/x-mixed-replace;boundary=123456789000000000000987654321

--123456789000000000000987654321
Content-Type: image/jpeg
Content-Length: 12345
X-Timestamp: 1234567890.123456

[JPEG数据]
--123456789000000000000987654321
...
```

#### 3.1.4 关键参数
- **分辨率**：根据摄像头配置
- **JPEG质量**：根据摄像头配置

#### 3.1.5 代码结构
```cpp
// 视频流处理函数
static esp_err_t stream_handler(httpd_req_t *req) {
    // 设置响应类型
    httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    
    // 循环发送视频帧
    while (true) {
        // 获取摄像头帧
        camera_fb_t *fb = esp_camera_fb_get();
        
        // 转换为JPEG格式
        // ...
        
        // 发送帧数据
        httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        
        // 释放帧缓冲区
        esp_camera_fb_return(fb);
    }
}
```

---

### 3.2 Web端拍照功能

#### 3.2.1 功能描述
在Web端控制摄像头拍照，获取当前摄像头画面。

#### 3.2.2 技术实现
- 使用ESP32 Camera库捕获单帧图像
- 使用JPEG格式编码
- 通过HTTP响应返回图像数据

#### 3.2.3 API接口
```
GET /capture
```

**响应格式：**
- Content-Type: image/jpeg
- 响应体：JPEG图像数据

#### 3.2.4 关键参数
- **图片格式**：JPEG
- **分辨率**：根据摄像头配置
- **JPEG质量**：根据摄像头配置

#### 3.2.5 代码结构
```cpp
// 拍照处理函数
static esp_err_t capture_handler(httpd_req_t *req) {
    // 获取摄像头帧
    camera_fb_t *fb = esp_camera_fb_get();
    
    // 设置响应类型
    httpd_resp_set_type(req, "image/jpeg");
    
    // 发送图像数据
    httpd_resp_send(req, (const char *)fb->buf, fb->len);
    
    // 释放帧缓冲区
    esp_camera_fb_return(fb);
    
    return ESP_OK;
}
```

---

### 3.3 在新标签页中打开视频流功能

#### 3.3.1 功能描述
在新标签页中打开视频流，提供更清晰的视频观看体验。

#### 3.3.2 技术实现
- 使用JavaScript的window.open()函数
- 构建完整的视频流URL
- 在新标签页中加载视频流

#### 3.3.3 实现代码
```javascript
function openStreamInNewTab() {
    const streamUrl = 'http://' + window.location.hostname + ':81/stream';
    window.open(streamUrl, '_blank');
}
```

---

## 四、硬件配置

### 4.1 摄像头配置

#### 4.1.1 OV5640摄像头参数
```cpp
#define CAMERA_MODEL_ESP32S3_EYE

// 摄像头引脚配置
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM      4
#define SIOC_GPIO_NUM      5

#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM        8
#define Y3_GPIO_NUM        9
#define Y2_GPIO_NUM       11

#define VSYNC_GPIO_NUM     6
#define HREF_GPIO_NUM      7
#define PCLK_GPIO_NUM     13
```

#### 4.1.2 摄像头配置参数
```cpp
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
config.xclk_freq_hz = 22000000;  // 22MHz时钟
config.frame_size = FRAMESIZE_UXGA;
config.pixel_format = PIXFORMAT_JPEG;
config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
config.fb_location = CAMERA_FB_IN_PSRAM;
config.jpeg_quality = 12;
config.fb_count = 1;

// 如果有PSRAM，使用更高的分辨率和质量
if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
} else {
    // 没有PSRAM时限制帧大小
    config.frame_size = FRAMESIZE_XGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
}
```

#### 4.3.3 exFAT文件系统
- ESP32-S3-CAM的SD卡库默认支持exFAT文件系统
- 无需额外配置，直接使用SD库即可
- 支持大文件（>4GB）

---

## 五、Web界面设计

### 5.1 界面布局

```
┌─────────────────────────────────────────────────────────────┐
│                    ESP32-S3监控控制台                        │
├─────────────────────────────────────────────────────────────┤
│  语言: [English ▼]  主题: [Light ▼]                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐  │
│  │                                                     │  │
│  │              视频显示区域                            │  │
│  │                                                     │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐  │
│  │  [拍照]  [在新标签页中打开视频流]                  │  │
│  └─────────────────────────────────────────────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 语言和主题选择

#### 5.2.1 语言选择功能
- **支持语言**：English（英语）、中文
- **默认语言**：English
- **位置**：页面右上角
- **持久化**：使用localStorage保存用户选择
- **自动恢复**：页面加载时自动恢复上次选择的语言

**技术实现：**
- 使用JavaScript的translations对象存储所有UI元素的翻译
- 通过data-lang属性标识需要翻译的元素
- 使用changeLanguage()函数切换语言
- 使用localStorage.setItem('preferredLanguage', currentLanguage)保存偏好
- 使用localStorage.getItem('preferredLanguage')恢复偏好

**翻译对象结构：**
```javascript
const translations = {
    en: {
        title: "ESP32-S3 Monitoring Console",
        language: "Language",
        theme: "Theme",
        lightTheme: "Light",
        darkTheme: "Dark",
        liveVideo: "Live Video",
        loadingStream: "Loading video stream...",
        capturePhoto: "Capture Photo",
        openStreamNewTab: "Open Stream in New Tab",
        sdCardInfo: "SD Card Storage Info",
        totalSpace: "Total Space",
        usedSpace: "Used Space",
        freeSpace: "Free Space",
        systemUptime: "System Uptime",
        uptimeUnit: "Uptime",
        cameraSettings: "Camera Settings",
        resolution: "Resolution",
        imageQuality: "Image Quality",
        lowQuality: "Low Quality (63)",
        mediumQuality: "Medium Quality (31)",
        highQuality: "High Quality (10)",
        veryHighQuality: "Very High Quality (5)",
        brightness: "Brightness",
        contrast: "Contrast",
        saturation: "Saturation"
    },
    zh: {
        title: "ESP32-S3监控控制台",
        language: "语言",
        theme: "主题",
        lightTheme: "浅色",
        darkTheme: "深色",
        liveVideo: "实时视频",
        loadingStream: "正在加载视频流...",
        capturePhoto: "拍照",
        openStreamNewTab: "在新标签页中打开视频流",
        sdCardInfo: "SD卡存储信息",
        totalSpace: "总空间",
        usedSpace: "已用空间",
        freeSpace: "剩余空间",
        systemUptime: "系统运行时长",
        uptimeUnit: "运行时间",
        cameraSettings: "摄像头设置",
        resolution: "分辨率",
        imageQuality: "图像质量",
        lowQuality: "低质量 (63)",
        mediumQuality: "中等质量 (31)",
        highQuality: "高质量 (10)",
        veryHighQuality: "极高质量 (5)",
        brightness: "亮度",
        contrast: "对比度",
        saturation: "饱和度"
    }
};
```

#### 5.2.2 主题选择功能
- **支持主题**：Light（浅色）、Dark（深色）
- **默认主题**：Light
- **位置**：页面右上角（语言选择器旁边）
- **持久化**：使用localStorage保存用户选择
- **自动恢复**：页面加载时自动恢复上次选择的主题
- **过渡动画**：0.3秒平滑过渡

**技术实现：**
- 使用CSS变量定义主题颜色
- 通过data-theme属性切换主题
- 使用changeTheme()函数切换主题
- 使用localStorage.setItem('preferredTheme', currentTheme)保存偏好
- 使用localStorage.getItem('preferredTheme')恢复偏好

**CSS变量定义：**
```css
:root {
    /* 浅色主题变量 / Light theme variables */
    --bg-color: #f5f5f5;
    --container-bg: #ffffff;
    --text-color: #333333;
    --section-bg: #f9f9f9;
    --section-title-color: #555555;
    --border-color: #dddddd;
    --btn-primary-bg: #007bff;
    --btn-primary-hover: #0056b3;
    --btn-success-bg: #28a745;
    --btn-success-hover: #218838;
    --btn-danger-bg: #dc3545;
    --btn-danger-hover: #c82333;
    --btn-warning-bg: #ffc107;
    --btn-warning-hover: #e0a800;
    --btn-warning-text: #212529;
    --info-label-color: #666666;
    --info-value-color: #333333;
    --sd-info-value-color: #333333;
    --sd-info-unit-color: #999999;
    --setting-label-color: #666666;
    --setting-border-color: #dddddd;
    --setting-border-hover: #007bff;
    --uptime-value-color: #333333;
    --uptime-unit-color: #666666;
    --language-label-color: #666666;
    --loading-text-color: #666666;
    --shadow-color: rgba(0, 0, 0, 0.1);
}

[data-theme="dark"] {
    /* 深色主题变量 / Dark theme variables */
    --bg-color: #1a1a1a;
    --container-bg: #2d2d2d;
    --text-color: #e0e0e0;
    --section-bg: #383838;
    --section-title-color: #b0b0b0;
    --border-color: #555555;
    --btn-primary-bg: #007bff;
    --btn-primary-hover: #0056b3;
    --btn-success-bg: #28a745;
    --btn-success-hover: #218838;
    --btn-danger-bg: #dc3545;
    --btn-danger-hover: #c82333;
    --btn-warning-bg: #ffc107;
    --btn-warning-hover: #e0a800;
    --btn-warning-text: #212529;
    --info-label-color: #b0b0b0;
    --info-value-color: #e0e0e0;
    --sd-info-value-color: #e0e0e0;
    --sd-info-unit-color: #888888;
    --setting-label-color: #b0b0b0;
    --setting-border-color: #555555;
    --setting-border-hover: #007bff;
    --uptime-value-color: #e0e0e0;
    --uptime-unit-color: #b0b0b0;
    --language-label-color: #b0b0b0;
    --loading-text-color: #b0b0b0;
    --shadow-color: rgba(0, 0, 0, 0.3);
}

/* 全局过渡效果 / Global transition effect */
* {
    transition: background-color 0.3s, color 0.3s, border-color 0.3s;
}
```

**主题切换函数：**
```javascript
function changeTheme() {
    const themeSelect = document.getElementById('theme-select');
    currentTheme = themeSelect.value;
    
    // 设置data-theme属性 / Set data-theme attribute
    if (currentTheme === 'dark') {
        document.documentElement.setAttribute('data-theme', 'dark');
    } else {
        document.documentElement.removeAttribute('data-theme');
    }
    
    // 保存主题偏好到localStorage / Save theme preference to localStorage
    localStorage.setItem('preferredTheme', currentTheme);
}
```

### 5.3 功能按钮

#### 5.3.1 拍照控制按钮
- **拍照**：拍摄一张照片，在新窗口打开

#### 5.3.2 视频流控制按钮
- **在新标签页中打开视频流**：在新标签页中打开视频流

### 5.4 状态显示
- 视频流状态
- 拍照状态

### 5.5 用户偏好持久化

#### 5.5.1 LocalStorage使用
- **preferredLanguage**：保存用户选择的语言（'en'或'zh'）
- **preferredTheme**：保存用户选择的主题（'light'或'dark'）

#### 5.5.2 页面加载时恢复偏好
```javascript
// 页面加载时恢复语言和主题偏好 / Restore language and theme preferences on page load
window.onload = function() {
    // 恢复语言偏好 / Restore language preference
    const savedLanguage = localStorage.getItem('preferredLanguage');
    if (savedLanguage) {
        document.getElementById('language-select').value = savedLanguage;
        currentLanguage = savedLanguage;
        changeLanguage();
    }
    
    // 恢复主题偏好 / Restore theme preference
    const savedTheme = localStorage.getItem('preferredTheme');
    if (savedTheme) {
        document.getElementById('theme-select').value = savedTheme;
        currentTheme = savedTheme;
        changeTheme();
    }
};
```

---

## 六、API接口文档

### 6.1 视频流接口

#### 6.1.1 获取视频流
```
GET /stream
```

**响应：**
```
Content-Type: multipart/x-mixed-replace;boundary=123456789000000000000987654321

--123456789000000000000987654321
Content-Type: image/jpeg
Content-Length: 12345
X-Timestamp: 1234567890.123456

[JPEG数据]
--123456789000000000000987654321
...
```

### 6.2 拍照接口

#### 6.2.1 拍照
```
GET /capture
```

**响应：**
- Content-Type: image/jpeg
- 响应体：JPEG图像数据

### 6.3 系统状态接口

#### 6.3.1 获取系统状态
```
GET /status
```

**响应：**
```json
{
  "xclk": 22,
  "pixformat": 3,
  "framesize": 7,
  "quality": 10,
  "brightness": 1,
  "contrast": 0,
  "saturation": 0,
  "sharpness": 0,
  "special_effect": 0,
  "wb_mode": 0,
  "awb": 1,
  "awb_gain": 1,
  "aec": 1,
  "aec2": 1,
  "ae_level": 0,
  "aec_value": 300,
  "agc": 1,
  "agc_gain": 0,
  "gainceiling": 0,
  "bpc": 0,
  "wpc": 1,
  "raw_gma": 1,
  "lenc": 1,
  "hmirror": 0,
  "dcw": 1,
  "colorbar": 0,
  "led_intensity": -1
}
```

---

## 七、文件结构设计

### 7.1 项目文件结构

```
ESP32-S3监控/
├── ESP32_S3_Camera_Monitor.ino      # 主程序文件
├── camera_pins.h                    # 摄像头引脚配置
├── camera_index.h                   # Web界面HTML
├── app_httpd.cpp                    # HTTP服务器实现
├── partitions.csv                   # 分区表文件
├── readme/
│   ├── 项目需求.md
│   ├── 技术栈.md
│   ├── 开发文档.md
│   ├── 实施文档.md
│   ├── 功能更新.md
│   └── 例程/
│       ├── Sketch_07.1_CameraWebServer.ino
│       ├── app_httpd.cpp
│       ├── camera_index.h
│       ├── camera_pins.h
│       └── partitions.csv
```

---

## 八、配置参数说明

### 8.1 WiFi配置

```cpp
// WiFi配置
const char* ssid = "your-ssid";         // WiFi名称
const char* password = "your-password";  // WiFi密码
```

### 8.2 摄像头配置

```cpp
// 摄像头配置
#define CAMERA_MODEL_ESP32S3_EYE       // 摄像头型号
#define CAMERA_XCLK_FREQ 20000000     // 摄像头时钟频率：22MHz
#define CAMERA_FRAME_SIZE FRAMESIZE_UXGA  // 分辨率：UXGA
#define CAMERA_JPEG_QUALITY 12        // JPEG质量：12（1-31，越小越好）
#define CAMERA_FB_COUNT 1             // 帧缓冲区数量：1

// 如果有PSRAM，使用更高的分辨率和质量
if(psramFound()){
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
} else {
    // 没有PSRAM时限制帧大小
    config.frame_size = FRAMESIZE_XGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
}
```

---

## 九、错误处理机制

### 9.1 摄像头错误处理

```cpp
// 摄像头初始化失败
if (esp_camera_init(&config) != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
}

// 获取帧失败
camera_fb_t *fb = esp_camera_fb_get();
if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
}
```

### 9.2 WiFi错误处理

```cpp
// WiFi连接
WiFi.begin(ssid, password);
WiFi.setSleep(false);

while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
```

---

## 十、性能优化

### 10.1 内存优化

- 使用PSRAM存储摄像头帧缓冲区
- 合理设置帧缓冲区数量（2个）
- 及时释放不再使用的内存
- 避免内存泄漏

### 10.2 性能优化

- 优化JPEG编码质量平衡画质和性能
- 合理分配CPU资源

### 10.3 网络优化

- 使用WiFi保持连接，避免频繁重连
- 支持视频流重试机制

---

## 十一、测试计划

### 11.1 功能测试

#### 11.1.1 视频流测试
- 测试视频流是否正常显示
- 测试视频流重试机制
- 测试在新标签页中打开视频流功能

#### 11.1.2 拍照功能测试
- 测试拍照是否正常
- 测试照片质量
- 测试拍照响应速度

### 11.2 稳定性测试

#### 11.2.1 长时间运行测试
- 测试视频流长时间稳定性
- 测试WiFi连接稳定性
- 测试摄像头持续运行稳定性

---

## 十二、部署说明

### 12.1 开发环境搭建

#### 12.1.1 安装Arduino IDE
1. 下载Arduino IDE 2.x
2. 安装ESP32开发板支持

#### 12.1.2 安装必要的库
```cpp
// 在Arduino IDE中安装以下库：
ArduinoJson

```

### 12.2 编译上传

#### 12.2.1 配置开发板
1. 选择开发板：ESP32S3 Dev Module
2. 配置分区方案：使用提供的partitions.csv
3. 配置PSRAM：OPI PSRAM
4. 配置Flash模式：QIO
5. 配置Flash大小：8MB（根据实际硬件）

#### 12.2.2 编译上传
1. 连接ESP32-S3-CAM到电脑
2. 选择正确的串口
3. 点击上传按钮
4. 等待编译和上传完成

### 12.3 首次启动

1. 给ESP32-S3-CAM上电
2. 等待WiFi连接
3. 查看串口输出获取IP地址
4. 在浏览器中访问IP地址
5. 测试视频流显示功能
6. 测试拍照功能
7. 测试在新标签页中打开视频流功能

---

## 十三、维护说明

### 13.1 日常维护

- 检查系统日志
- 监控设备运行状态
- 监控SD卡存储空间
- 系统启动时自动清理无效视频文件（大小为0KB）
- 定期检查视频录制质量和文件完整性
- SD卡空间自动管理：
  - 动态阈值检测：当剩余空间 < 5GB时触发清理
  - 按时间删除最旧文件：优先删除最旧的文件
  - 清理目标：每次清理释放约2GB空间
  - 清理优先级：可配置（只删除视频、优先删除视频再删除照片、只删除照片）
  - 空间检测频率：默认5秒检测一次

### 13.2 故障排查

#### 13.2.1 视频流无法显示
- 检查WiFi连接
- 检查摄像头初始化
- 检查浏览器兼容性
- 检查网络带宽

#### 13.2.2 拍照失败
- 检查摄像头是否正常工作
- 检查网络连接

### 13.3 升级说明

1. 下载新版本固件
2. 上传新固件到ESP32-S3-CAM
3. 测试各项功能

---

## 十四、安全建议

### 14.1 网络安全

- 使用强WiFi密码
- 定期更换WiFi密码
- 考虑使用WPA3加密
- 避免在公共网络中使用

### 14.2 数据安全

- 避免在视频中暴露隐私信息

### 14.3 物理安全

- 确保设备安装在安全位置
- 避免设备暴露在恶劣环境中
- 定期检查设备状态
- 确保电源供应稳定

---

## 十五、未来扩展

### 15.1 功能扩展

- 添加移动侦测功能
- 添加OTA更新功能

### 15.2 性能扩展

- 支持更高分辨率
- 支持更高帧率
- 优化视频流性能

### 15.3 实际使用

- 制作组装教程发布到B站

**文档版本：** 1.3
**创建日期：** 2026-01-26
**最后更新：** 2026-02.3



