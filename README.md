# ESP32-S3 Camera Monitor

A network video monitoring system based on ESP32-S3-CAM, supporting real-time video streaming, photo capture, video recording, and more.

## ⚠️ License Declaration

**This project is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)**

### Important Restrictions

- ❌ **No Commercial Use**: This project cannot be used for any commercial purposes
- ✅ **Non-Commercial Use Allowed**: Personal learning, research, education, and other non-commercial purposes
- ✅ **Modification and Distribution Allowed**: But must use the same license
- ✅ **Attribution Required**: Must preserve the original author's attribution

For details, please visit: https://creativecommons.org/licenses/by-nc-sa/3.0/

## Code Sources

This project is modified from following open-source projects:

### 1. Freenove ESP32-S3 Camera Example
- **Copyright**: Freenove
- **Website**: www.freenove.com
- **License**: CC BY-NC-SA 3.0
- **Note**: The entire project is constrained by this license

### 2. Espressif CameraWebServer
- **Copyright**: Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
- **License**: Apache License 2.0
- **Note**: Although this code allows commercial use, the entire project is restricted by CC BY-NC-SA 3.0

## Features

- Real-time video streaming on web (MJPEG format)
- Web-based photo capture and save to SD card
- Video recording (AVI format, MJPEG encoding)
- Automatic video segmentation (2 minutes per segment)
- Automatic SD card space cleanup (55GB threshold)
- HTTP Basic Authentication
- NTP time synchronization
- System uptime display
- Camera parameter adjustment (resolution, quality, brightness, etc.)

## Hardware Requirements

- ESP32-S3-CAM(GOOUUU version) development board or ESP32-S3 WROVER Board (Freenove)
- OV5640 camera or other cameras supporting ESP32-S3
- SD card (64GB or larger recommended)
- SG-90 servo (optional, for pan-tilt control)

## Deployment Guide

### Step 1: Hardware Preparation

**For Users in China:**
- Use ESP32-S3-CAM (GOOUUU version / 果云版)
- Ensure the camera module is properly connected

**For International Users:**
- Use ESP32-S3 WROVER Board
- Ensure the camera module is properly connected

### Step 2: Software Setup

1. **Install Arduino IDE**
   - Download and install the latest Arduino IDE from https://www.arduino.cc/en/software

2. **Install ESP32 Board Package**
   - Open Arduino IDE
   - Go to Tools > Board > Boards Manager
   - Search for "esp32" and install "esp32 by Espressif Systems"

3. **Select Board**
   - Go to Tools > Board > esp32
   - Select "ESP32S3 Dev Module"

4. **Configure Board Settings**
   - **PSRAM**: "OPI PSRAM" (for ESP32-S3-CAM with PSRAM)
   - **Flash Mode**: "QIO"
   - **Flash Size**: "8MB (app 3MB OTA 1.9MB)"
   - **Partition Scheme**: "Default 8MB with spiffs (3MB APP/1.5MB SPIFFS)"
   - **CPU Frequency**: "240MHz (WiFi)"
   - **Upload Speed**: "921600"

5. **Modify Configuration**
    - Open `ESP32_S3_Camera_Monitor.ino`
    - Find and modify the following lines:
      ```cpp
      const char* ssid     = "your_ssid";
      const char* password = "your_password";
      ```
    - Open `auth.h`
    - Find and modify the following lines:
      ```cpp
      #define AUTH_USERNAME "admin"
      #define AUTH_PASSWORD "password123"
      ```
    - **IMPORTANT**: Change the default password to a strong password for security
    - For ESP32-S3-CAM (GOOUUU version) or ESP32-S3 WROVER Board (Freenove), ensure:
      ```cpp
      #define CAMERA_MODEL_ESP32S3_EYE
      ```
    - Note: Both GOOUUU version and WROVER Board use the same camera model definition

6. **Upload Code**
   - Connect the board to your computer via USB
   - Select the correct COM port in Tools > Port
   - Click the Upload button (→) in Arduino IDE

### Step 3: Access the Web Interface

1. After successful upload, open the Serial Monitor (Tools > Serial Monitor)
2. Set baud rate to 115200
3. Wait for the WiFi connection message
4. Note the IP address displayed (e.g., "192.168.1.100")
5. Open a web browser and navigate to: `http://192.168.1.100`
6. Login with the username and password configured in Step 5

### Step 4: Using the System

- **View Live Video**: Click the "Open the video stream in a new tab" button to load the video stream (Note: Video stream does not auto-load, this is a known issue)
- **Capture Photo**: Click the "Capture Photo" button to save a photo to the SD card
- **Adjust Settings**: Modify camera parameters (resolution, quality, brightness, etc.) in the settings panel
- **View SD Card Info**: Monitor SD card usage and system uptime on the dashboard

### Troubleshooting

- **Camera not working**: 1.Check camera connection and ensure the correct camera model is selected;2.Do not close the tab, restart the board, when the board is restarted, open the video stream again.(The second one is a known issue)
- **WiFi not connecting**: Verify WiFi credentials and ensure the board is within range
- **SD card not detected**: Format the SD card as FAT32 and ensure it's properly inserted
- **Video stream not loading**: Click the "Start Stream" button manually (Note: Auto-load is a known issue, manual start is required)

## Documentation

For detailed documentation, please see the [readme](/readme/) directory:
- [Tech Stack](/readme/TECHNOLOGY_STACK.md) (English / Chinese)
- [Development Documentation](/readme/DEVELOPMENT.md) (English / Chinese)
- [Feature Updates](/readme/CHANGELOG.md) (English / Chinese)

## Disclaimer

**AI-Generated Code Notice**

This project's source code was generated by GLM-4.7 (an AI language model). While the code has been reviewed and tested, users should be aware of the following:

1. **No Warranty**: This software is provided "as is", without any express or implied warranties. The authors and Zhipu AI Technology Co., Ltd(Beijing Zhipu Huazhang Technology Co., Ltd) make no representations about the suitability of this software for any purpose.

2. **User Responsibility**: Users are responsible for:
   - Reviewing and testing the code before deployment
   - Ensuring the code meets their specific requirements
   - Maintaining and updating the code as needed
   - Backing up data before using this system

3. **Limitations**: AI-generated code may contain:
   - Undiscovered bugs or edge cases
   - Security vulnerabilities that require review
   - Performance optimizations that may be needed

4. **Recommendations**:
   - Conduct thorough testing in a controlled environment
   - Review security implications, especially for network-connected devices
   - Regularly update dependencies and firmware
   - Monitor system performance and logs

5. **Attribution**: When using this code, please acknowledge that it was generated by GLM-4.7 AI.

The authors and Zhipu AI Technology Co., Ltd. (Beijing Zhipu Huazhang Technology Co., Ltd.) are not liable for any damages arising from the use of this software.

## Important Notes

1. **No Commercial Use**: This software cannot be used for any commercial purposes
2. **Preserve Attribution**: When using or modifying this software, you must preserve the original author's attribution
3. **Share Alike**: Any derivative works must be distributed under the CC BY-NC-SA 3.0 license
4. **License Confirmation**: The license for Freenove example code is not explicitly stated in the code. Please confirm before use

---

# ESP32-S3 摄像头监控

基于ESP32-S3-CAM的网络视频监控系统，支持实时视频流、拍照、视频录制等功能。

## ⚠️ 许可证声明

**本项目采用 Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)**

### 重要限制

- ❌ **禁止商业用途**：本项目不能用于任何商业目的
- ✅ **允许非商业用途**：个人学习、研究、教育等非商业用途
- ✅ **允许修改和分发**：但必须使用相同的许可证
- ✅ **必须署名**：必须保留原作者的署名

详情请查看：https://creativecommons.org/licenses/by-nc-sa/3.0/

## 代码来源

本项目基于以下开源项目修改而来：

### 1. Freenove ESP32-S3 Camera Example
- **版权所有**：Freenove
- **网站**：www.freenove.com
- **许可证**：CC BY-NC-SA 3.0
- **说明**：本项目整体受此许可证约束

### 2. Espressif CameraWebServer
- **版权**：Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
- **许可证**：Apache License 2.0
- **说明**：虽然此代码允许商业用途，但本项目整体受 CC BY-NC-SA 3.0 限制

## 主要功能

- Web端实时视频流显示（MJPEG格式）
- Web端控制拍照并保存到SD卡
- 视频录制功能（AVI格式，MJPEG编码）
- 视频自动分段录制（2分钟一段）
- SD卡空间自动清理（55GB阈值）
- HTTP Basic Authentication认证
- NTP时间同步
- 系统运行时长显示
- 摄像头参数调节（分辨率、质量、亮度等）

## 硬件要求

- ESP32-S3-CAM开发板
- OV5640摄像头或其他支持ESP32-S3的摄像头
- SD卡（建议64GB或更大）
- SG-90舵机（可选，用于云台控制）

## 部署指南

### 第一步：硬件准备

**国内用户：**
- 使用 ESP32-S3-CAM（果云版）
- 确保摄像头模块正确连接

**国外用户：**
- 使用 ESP32-S3 WROVER Board
- 确保摄像头模块正确连接

### 第二步：软件设置

1. **安装Arduino IDE**
   - 从 https://www.arduino.cc/en/software 下载并安装最新的Arduino IDE

2. **安装ESP32开发板包**
   - 打开Arduino IDE
   - 进入 Tools > Board > Boards Manager
   - 搜索"esp32"并安装"esp32 by Espressif Systems"

3. **选择开发板**
   - 进入 Tools > Board > esp32
   - 选择"ESP32S3 Dev Module"

4. **配置开发板设置**
   - **PSRAM**: "OPI PSRAM"（用于带PSRAM的ESP32-S3-CAM）
   - **Flash Mode**: "QIO"
   - **Flash Size**: "8MB (app 3MB OTA 1.9MB)"
   - **Partition Scheme**: "Default 8MB with spiffs (3MB APP/1.5MB SPIFFS)"
   - **CPU Frequency**: "240MHz (WiFi)"
   - **Upload Speed**: "921600"

5. **修改配置**
   - 打开 `ESP32_S3_Camera_Monitor.ino`
   - 找到并修改以下行：
     ```cpp
     const char* ssid     = "your_ssid";
     const char* password = "your_password";
     ```
   - 打开 `auth.h`
   - 找到并修改以下行：
     ```cpp
     #define AUTH_USERNAME "admin"
     #define AUTH_PASSWORD "password123"
     ```
   - **重要提示**：为了安全起见，请将默认密码更改为强密码
   - 对于ESP32-S3-CAM（果云版）或ESP32-S3 WROVER Board（Freenove），确保：
     ```cpp
     #define CAMERA_MODEL_ESP32S3_EYE
     ```
   - 注意：果云版和WROVER板都使用相同的摄像头型号定义

6. **上传代码**
   - 通过USB将开发板连接到计算机
   - 在 Tools > Port 中选择正确的COM端口
   - 点击Arduino IDE中的上传按钮（→）

### 第三步：访问Web界面

1. 上传成功后，打开串口监视器（Tools > Serial Monitor）
2. 将波特率设置为115200
3. 等待WiFi连接消息
4. 记录显示的IP地址（例如："192.168.1.100"）
5. 打开Web浏览器并访问：`http://192.168.1.100`
6. 使用第二步中配置的用户名和密码登录

### 第四步：使用系统

- **查看实时视频**：点击"在新标签页打开视频流"按钮加载视频流（注意：视频流不会自动加载，这是一个已知问题）
- **拍照**：点击"拍照"按钮将照片保存到SD卡
- **调整设置**：在设置面板中修改摄像头参数（分辨率、质量、亮度等）
- **查看SD卡信息**：在仪表板上监控SD卡使用情况和系统运行时长

### 故障排除

- **摄像头不工作**：1.检查摄像头连接并确保选择了正确的摄像头型号;2.不要关闭页面，重启开发板，重启完了再打开视频流,这是一个已知问题
- **WiFi无法连接**：验证WiFi凭证并确保开发板在信号范围内
- **SD卡未检测到**：将SD卡格式化为FAT32并确保正确插入
- **视频流无法加载**：手动点击"Start Stream"按钮（注意：自动加载是一个已知问题，需要手动启动）

## 文档

详细文档请查看 [readme](/readme/) 目录：
- [技术栈](/readme/TECHNOLOGY_STACK.md) (英语 / 中文)
- [开发文档](/readme/DEVELOPMENT.md) (英语 / 中文)
- [功能更新](/readme/CHANGELOG.md) (英语 / 中文)

## 免责声明

**AI生成代码声明**

本项目的源代码由GLM-4.7（一种AI语言模型）生成。虽然代码已经过审查和测试，但用户应注意以下事项：

1. **无保证**：本软件按"原样"提供，不提供任何明示或暗示的保证。作者和北京智谱华章科技股份有限公司不对本软件适用于任何目的作任何声明。

2. **用户责任**：用户负责：
   - 在部署前审查和测试代码
   - 确保代码满足其特定要求
   - 根据需要维护和更新代码
   - 使用本系统前备份数据

3. **局限性**：AI生成的代码可能包含：
   - 未发现的错误或边缘情况
   - 需要审查的安全漏洞
   - 可能需要的性能优化

4. **建议**：
   - 在受控环境中进行彻底测试
   - 审查安全影响，特别是对于网络连接的设备
   - 定期更新依赖项和固件
   - 监控系统性能和日志

5. **署名**：使用此代码时，请确认它是由GLM-4.7 AI生成的。

作者和北京智谱华章科技股份有限公司不对使用本软件造成的任何损害承担责任。

## 注意事项

1. **禁止商业用途**：本软件不能用于任何商业目的
2. **保留署名**：使用或修改本软件时，必须保留原作者的署名
3. **相同方式共享**：任何衍生作品必须使用 CC BY-NC-SA 3.0 许可证

