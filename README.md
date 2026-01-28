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

- ESP32-S3-CAM development board
- OV5640 camera
- SD card (64GB or larger recommended)
- SG-90 servo (optional, for pan-tilt control)

## Documentation

For detailed documentation, please see the [readme](readme/) directory:
- [Project Requirements](readme/项目需求.md) (Chinese)
- [Tech Stack](readme/技术栈.md) (Chinese)
- [Development Documentation](readme/开发文档.md) (Chinese)
- [Implementation Documentation](readme/实施文档.md) (Chinese)
- [Feature Updates](readme/功能更新.md) (Chinese)

## Disclaimer

This software is provided "as is", without any express or implied warranties.

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
- OV5640摄像头
- SD卡（建议64GB或更大）
- SG-90舵机（可选，用于云台控制）

## 文档

详细文档请查看 [readme](readme/) 目录：
- [项目需求](readme/项目需求.md)
- [技术栈](readme/技术栈.md)
- [开发文档](readme/开发文档.md)
- [实施文档](readme/实施文档.md)
- [功能更新](readme/功能更新.md)

## 免责声明

本软件按"原样"提供，不提供任何明示或暗示的保证。

## 注意事项

1. **禁止商业用途**：本软件不能用于任何商业目的
2. **保留署名**：使用或修改本软件时，必须保留原作者的署名
3. **相同方式共享**：任何衍生作品必须使用 CC BY-NC-SA 3.0 许可证

