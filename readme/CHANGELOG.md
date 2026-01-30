# Changelog

## License Information

This project is licensed under Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)

### Important Restrictions

- ❌ **Commercial Use Prohibited**: This project cannot be used for any commercial purposes
- ✅ **Non-Commercial Use Allowed**: Personal learning, research, education, and other non-commercial purposes
- ✅ **Modification and Distribution Allowed**: But must use the same license
- ✅ **Attribution Required**: Must retain the original author's attribution

For details, please visit: https://creativecommons.org/licenses/by-nc-sa/3.0/

## Code Sources

This project is modified from the following open-source projects:
- Freenove ESP32-S3 Camera Example (CC BY-NC-SA 3.0)
- Espressif Camera Library Example (Apache License 2.0) - from Arduino ESP32 Board Package

---

## Project Information
- **Project Name**: ESP32-S3 Monitoring Project
- **Start Date**: 2026-01-26
- **Document Version**: 1.10

## Update Log

### 2026-01-30 - Fixed Video Stream Auto-Acquisition Issue
**Updates:**
- Removed automatic video stream acquisition on page load
- Added manual control button for video stream (Start/Stop)
- Updated Web interface with bilingual support for the new button

**Modified Files:**
1. camera_index.h - Updated HTML structure and JavaScript logic

---

### 2026-01-29 - Added Invalid Video File Cleanup Function
**Updates:**
- Added automatic cleanup function for invalid video files (0KB files)
- System automatically checks and deletes 0KB video files on startup
- Improved system reliability by cleaning up corrupted video files

**Modified Files:**
1. sd_read_write.h - Added cleanInvalidVideoFiles() function declaration
2. sd_read_write.cpp - Implemented cleanInvalidVideoFiles() function
3. ESP32_S3_Camera_Monitor.ino - Added cleanup call in setup function

**Implemented Features:**
1. Invalid Video File Cleanup:
   - Automatically scans videos directory on system startup
   - Identifies video files with 0KB size
   - Deletes all invalid video files
   - Reports cleanup results via Serial output

2. Technical Implementation:
   - Function: cleanInvalidVideoFiles()
   - Location: Called after SD card initialization and directory setup
   - Returns: Number of deleted files (or -1 on error)
   - Error handling: Comprehensive error checking and reporting

3. Cleanup Process:
   - Opens videos directory
   - Iterates through all files
   - Checks file size (file.size() == 0)
   - Deletes files with 0KB size
   - Outputs cleanup summary

**Cleanup Results:**
- Success: Reports number of deleted files
- No invalid files: Reports "No invalid video files found"
- Error: Reports failure and returns -1

**Next Steps:**
- Test cleanup function with various scenarios
- Monitor system behavior after cleanup
- Optimize cleanup performance if needed

---

### 2026-01-29 - Adjusted Default Camera Parameters
**Updates:**
- Changed camera default resolution from SVGA (1280x720) to XGA (1024x768)
- Changed video recording default frame rate from 15fps to 20fps
- Changed video recording default resolution from 1280x720 to 1024x768

**Modified Files:**
1. ESP32_S3_Camera_Monitor.ino - Updated camera configuration and video recording parameters

**Technical Changes:**
- Camera default resolution: FRAMESIZE_XGA (1024x768)
  - Previous: FRAMESIZE_SVGA (1280x720)
  - Effect: Lower resolution, better performance, reduced storage usage
- Video recording frame rate: 20fps
  - Previous: 15fps
  - Effect: Smoother video, more frames per second
- Video recording resolution: 1024x768
  - Previous: 1280x720
  - Effect: Consistent with camera resolution, optimized storage

**Parameter Adjustments:**
- Camera frame size (config.frame_size): FRAMESIZE_XGA
  - Resolution: 1024x768 pixels
  - Effect: Balanced quality and performance
- Video recording frame rate: 20fps
  - Frame interval: 50ms (1000/20)
  - Effect: Smoother video playback
- Video recording resolution: 1024x768
  - Consistent with camera resolution
  - Effect: No scaling required, better quality

**Next Steps:**
- Test video recording with new parameters
- Monitor storage usage
- Adjust parameters if needed

---

### 2026-01-29 - Added Language and Theme Selection Features
**Updates:**
- Added language selection functionality (English and Chinese)
- Added theme selection functionality (Light and Dark themes)
- Implemented bilingual support for all UI elements
- Implemented smooth theme transition animations (0.3 seconds)
- Added localStorage support for language and theme preferences
- Set default language to English
- Set default theme to Light

**Modified Files:**
1. camera_index.h - Added language and theme selection UI, CSS variables, JavaScript functions

**Implemented Features:**
1. Language Selection:
   - Supports English and Chinese switching
   - Default language: English
   - Language selector in top-right corner
   - All UI elements support bilingual display
   - Language preference saved to localStorage
   - Automatic language restoration on page load

2. Theme Selection:
   - Supports Light and Dark themes
   - Default theme: Light
   - Theme selector in top-right corner (next to language selector)
   - CSS variables for easy theme switching
   - Smooth transition animations (0.3 seconds)
   - Theme preference saved to localStorage
   - Automatic theme restoration on page load

3. Technical Implementation:
   - Uses CSS custom properties (variables) for theme colors
   - Uses data-theme attribute to control theme application
   - Uses data-lang attributes for language switching
   - JavaScript functions: changeLanguage(), initializeLanguage(), changeTheme(), initializeTheme()
   - Comprehensive translation object for all UI text
   - Responsive design, language and theme selectors work on all screen sizes

**UI Elements Translated:**
- Title: ESP32-S3 Monitoring Console / ESP32-S3监控控制台
- Language label: Language / 语言
- Theme label: Theme / 主题
- Light theme option: Light / 浅色
- Dark theme option: Dark / 深色
- Live Video: Live Video / 实时视频
- Loading stream: Loading video stream... / 正在加载视频流...
- Capture Photo: Capture Photo / 拍照
- Open Stream in New Tab: Open Stream in New Tab / 在新标签页中打开视频流
- SD Card Storage Info: SD Card Storage Info / SD卡存储信息
- Total Space: Total Space / 总空间
- Used Space: Used Space / 已用空间
- Free Space: Free Space / 剩余空间
- System Uptime: System Uptime / 系统运行时长
- Uptime: Uptime / 运行时间
- Camera Settings: Camera Settings / 摄像头设置
- Resolution: Resolution / 分辨率
- Image Quality: Image Quality / 图像质量
- Low Quality: Low Quality (63) / 低质量 (63)
- Medium Quality: Medium Quality (31) / 中等质量 (31)
- High Quality: High Quality (10) / 高质量 (10)
- Very High Quality: Very High Quality (5) / 极高质量 (5)
- Brightness: Brightness / 亮度
- Contrast: Contrast / 对比度
- Saturation: Saturation / 饱和度

**Theme Colors:**
Light Theme:
- Background: #f5f5f5
- Container: #ffffff
- Text: #333333
- Section: #f9f9f9
- Border: #dddddd
- Shadow: rgba(0, 0, 0, 0.1)

Dark Theme:
- Background: #1a1a1a
- Container: #2d2d2d
- Text: #e0e0e0
- Section: #383838
- Border: #555555
- Shadow: rgba(0, 0, 0, 0.3)

**Next Steps:**
- Test language switching functionality
- Test theme switching functionality
- Test localStorage persistence
- Test responsive design on different devices
- Optimize performance for low-end devices

---

### 2026-01-28 - Added HTTP Basic Authentication and Session Management
**Updates:**
- Created authentication module (auth.h and auth.cpp)
- Implemented HTTP Basic Authentication
- Implemented session management (14-day expiration)
- Added authentication protection for homepage, video stream, photo capture, and camera control interfaces
- After login, users can access other interfaces without re-authentication
- Sessions expire automatically after 14 days

**Modified Files:**
1. auth.h - Authentication module header file
2. auth.cpp - Authentication module implementation
3. app_httpd.cpp - Added authentication checks for various interfaces
4. ESP32_S3_Camera_Monitor.ino - Initialize authentication module

**Technical Features:**
- HTTP Basic Authentication:
  - Uses HTTP Basic Authentication for identity verification
  - Username and password defined in auth.h (AUTH_USERNAME and AUTH_PASSWORD)
  - Creates session and sets Cookie after successful authentication

- Session Management:
  - Session ID generated using random bytes and Base64 encoding
  - Session includes client IP address, creation time, last access time
  - Sessions expire automatically after 14 days (SESSION_EXPIRE_TIME)
  - Supports up to 10 concurrent sessions (MAX_SESSIONS)
  - Periodically cleans up expired sessions (auth_cleanup_expired_sessions)

- Authentication Protection:
  - Homepage (/) requires authentication
  - Video stream (/stream) requires authentication
  - Photo capture interface (/capture) requires authentication
  - System status interface (/status) requires authentication
  - Camera control interfaces (/control, /resolution, etc.) require authentication

- Authentication Flow:
  - First access triggers browser login dialog
  - User enters username and password, server verifies
  - After successful verification, creates session and sets Cookie
  - Subsequent requests use Cookie for authentication, no need to re-enter password
  - Session expires after 14 days, requires re-login

**Username and Password Location:**
- File: auth.h
- Line: Lines 21-22
- Definition:
  ```c
  #define AUTH_USERNAME "admin"
  #define AUTH_PASSWORD "password123"
  ```

**Parameter Adjustments:**
- Username (AUTH_USERNAME): admin (customizable)
  - Recommendation: Use meaningful username
- Password (AUTH_PASSWORD): password123 (customizable)
  - Recommendation: Use strong password with uppercase, lowercase, numbers, and special characters
- Session Expiration Time (SESSION_EXPIRE_TIME): 14 days (customizable)
  - Recommendation: Adjust based on security requirements, unit in seconds
- Maximum Sessions (MAX_SESSIONS): 10 (customizable)
  - Recommendation: Adjust based on actual needs, each session uses about 100 bytes of memory

**Next Steps:**
- Implement servo control functionality
- Test video recording stability
- Optimize video quality and file size
- Add video playback functionality

---

### 2026-01-28 - Fixed Compilation Error (videoRecordTask Duplicate Definition)
**Updates:**
- Deleted videoRecordTask function definition in app_httpd.cpp
- Deleted video recording control interfaces (/record/start, /record/stop, /record/status) in app_httpd.cpp
- Deleted video recording control variables (isVideoRecording, videoRecordTaskHandle) in app_httpd.cpp
- Updated app_httpd.cpp file header description

**Modified Files:**
1. app_httpd.cpp - Deleted videoRecordTask function and related interfaces

**Fixed Issues:**
1. Compilation error: multiple definition of `videoRecordTask(void*)'
   - Problem: videoRecordTask function was defined in both app_httpd.cpp and ESP32_S3_Camera_Monitor.ino
   - Fix: Deleted videoRecordTask function definition in app_httpd.cpp
   - Fix: Deleted video recording control interfaces in app_httpd.cpp
   - Fix: Deleted video recording control variables in app_httpd.cpp
   - Result: Compilation successful, no more duplicate definition errors

**Technical Features:**
- Video recording fully automated:
  - Recording starts automatically on system startup
  - No longer requires web interface recording control
  - No longer requires HTTP interface recording control

- Code simplification:
  - Deleted unnecessary video recording control code
  - Reduced code complexity
  - Improved system stability

**Next Steps:**
- Implement servo control functionality
- Test video recording stability
- Optimize video quality and file size
- Add video playback functionality

---

### 2026-01-28 - Fixed Video Recording File Size 0KB Issue
**Updates:**
- Fixed video recording file size 0KB issue
- Implemented videoRecordTask function in ESP32_S3_Camera_Monitor.ino
- Created video recording task in setup function
- Added videoRecordTask function declaration

**Modified Files:**
1. ESP32_S3_Camera_Monitor.ino - Added videoRecordTask function and task creation code

**Fixed Issues:**
1. Video recording file size 0KB
   - Problem: startVideoRecording() only created AVI file and wrote file header, but no task called writeVideoFrame() to write video frames
   - Fix: Implemented videoRecordTask function in ESP32_S3_Camera_Monitor.ino
   - Fix: Created video recording task after calling startVideoRecording() in setup function
   - Fix: videoRecordTask loops to get camera frames and calls writeVideoFrame() to write video frames
   - Result: Video recording works normally, file size grows normally

**Technical Features:**
- videoRecordTask function:
  - Uses FreeRTOS task for video recording
  - Loops to get camera frames (esp_camera_fb_get)
  - Calls writeVideoFrame() to write video frames
  - Releases frame buffer (esp_camera_fb_return)
  - Delays to control frame rate (1000/fps milliseconds)
  - Checks isRecordingVideo() status, exits task when stopped

- Task creation:
  - Creates task after calling startVideoRecording() in setup function
  - Uses xTaskCreate() to create task
  - Task name: video_record
  - Task stack: 4096 bytes
  - Task priority: 5
  - Passes fps parameter (15)

**Next Steps:**
- Implement servo control functionality
- Test video recording stability
- Optimize video quality and file size
- Add video playback functionality

---

### 2026-01-28 - Fixed Time Synchronization and Auto-Segmentation
**Updates:**
- Fixed missing time.h header file in sd_read_write.cpp
- Fixed auto-segmentation function not working properly
- Optimized stopVideoRecording function to support keeping recording state
- Fixed isRecording state management during auto-segmentation

**Modified Files:**
1. sd_read_write.cpp - Added time.h header, fixed auto-segmentation logic
2. sd_read_write.h - Modified stopVideoRecording function signature, added keepRecordingState parameter

**Fixed Issues:**
1. Timestamp format issue
   - Problem: sd_read_write.cpp missing time.h header, causing NTP time sync functions unusable
   - Fix: Added #include "time.h"
   - Result: Timestamp format correctly displays as YYYYMMDDHHMM

2. Auto-segmentation function issue
   - Problem: Auto-segmentation calling stopVideoRecording() sets isRecording=false, causing subsequent recording failures
   - Fix: Modified stopVideoRecording function, added keepRecordingState parameter
   - Fix: Pass keepRecordingState=true during auto-segmentation to keep recording state
   - Fix: Temporarily set isRecording=false before calling startVideoRecording
   - Result: Auto-segmentation works normally, automatically switches to new video file every 2 minutes

**Technical Features:**
- stopVideoRecording function supports two modes:
  - stopVideoRecording(false): Completely stop recording, set isRecording=false
  - stopVideoRecording(true): Stop current segment but keep recording state, isRecording remains true
- Auto-segmentation flow:
  1. Detect 2-minute duration
  2. Call stopVideoRecording(true) to stop current segment
  3. Temporarily set isRecording=false
  4. Call startVideoRecording() to start new segment
  5. Reset segment start time
  6. Continue recording

**Next Steps:**
- Implement servo control functionality
- Test video recording stability
- Optimize video quality and file size
- Add video playback functionality

---

### 2026-01-28 - Optimized Timestamp Format and Web Interface
**Updates:**
- Changed timestamp format to YYYYMMDDHHMM (year month day hour minute)
- Added NTP time synchronization
- Removed video recording control interface from web frontend
- Video recording changed to fully automated (starts automatically on system startup)
- Optimized file naming rules, using NTP synchronized system time

**Modified Files:**
1. ESP32_S3_Camera_Monitor.ino - Added NTP time synchronization configuration
2. sd_read_write.cpp - Modified timestamp generation function, uses NTP time
3. app_httpd.cpp - Updated documentation
4. camera_index.h - Removed video recording control interface and related functions

**Implemented Features:**
1. NTP Time Synchronization
   - Uses pool.ntp.org and time.nist.gov as NTP servers
   - Timezone set to UTC+8 (Beijing time)
   - System automatically synchronizes time on startup
   - Serial output displays current time

2. Timestamp Filenames
   - Photo filename format: YYYYMMDDHHMM.jpg
   - Video filename format: YYYYMMDDHHMM.avi
   - YYYY is year (4 digits)
   - MM is month (2 digits)
   - DD is day (2 digits)
   - HH is hour (2 digits)
   - MM is minute (2 digits)

3. Web Interface Optimization
   - Removed video recording control interface (start/stop buttons)
   - Removed recording status indicator
   - Removed recording status query function
   - Video recording fully automated, no manual operation required

**Technical Features:**
- Uses NTP protocol to synchronize system time
- Unified timestamp format, easy to manage and search
- Video recording fully automated, starts automatically on system startup
- Simple web interface, only retains necessary control functions
- Filename example: 202601281041.jpg (January 28, 2026, 10:41)

**Next Steps:**
- Implement servo control functionality
- Test video recording stability
- Optimize video quality and file size
- Add video playback functionality

---

### 2026-01-27 - Optimized Video Recording
**Updates:**
- Changed photo and video filenames to timestamp format (ESP32-xxxx.xx.xx.xx)
- Implemented auto-segmented video recording (2 minutes per segment)
- Changed to start recording automatically on startup
- Optimized file naming rules, using system runtime as timestamp

**Modified Files:**
1. sd_read_write.h - Added timestamp filename generation function declaration
2. sd_read_write.cpp - Implemented timestamp filename generation and auto-segmentation
3. ESP32_S3_Camera_Monitor.ino - Start video recording automatically on startup

**Implemented Features:**
1. Timestamp Filenames
   - Photo filename format: ESP32-xxxx.xx.xx.xx.jpg
   - Video filename format: ESP32-xxxx.xx.xx.xx.avi
   - xxxx is system runtime in seconds
   - xx.xx.xx is hour:minute:second

2. Auto-Segmented Video Recording
   - Automatically segments every 2 minutes
   - Automatically stops current segment and starts new segment
   - Segment filenames automatically increment timestamp
   - Seamless transition, no impact on recording

3. Startup Auto-Recording
   - System automatically starts video recording on startup
   - Default parameters: 15fps, 1280x720
   - No manual operation required

**Technical Features:**
- Uses system runtime (millis()) as timestamp
   - Automatically detects segment duration, switches at 2 minutes
   - Automatically deletes old files during segment switch (if space insufficient)
   - Unified filename format, easy to manage and search

**Next Steps:**
- Implement servo control functionality
- Test video recording stability
- Optimize video quality and file size
- Add video playback functionality

---

### 2026-01-27 - Added Video Recording and Auto-Cleanup
**Updates:**
- Implemented video recording function (AVI format, MJPEG encoding)
- Added web video recording control interface
- Implemented auto-delete old files function (55GB threshold)
- Support custom recording parameters (frame rate, resolution)

**Modified Files:**
1. sd_read_write.h - Added AVI file structure and video recording function declarations
2. sd_read_write.cpp - Implemented video recording and auto-cleanup
3. app_httpd.cpp - Added video recording control interfaces (/record/start, /record/stop, /record/status)
4. camera_index.h - Added video recording interface and control buttons

**Implemented Features:**
1. Video Recording Function
   - Supports AVI format video recording
   - Uses MJPEG encoding
   - Customizable frame rate (default 15fps)
   - Customizable resolution (default 1280x720)
   - Automatically generates video filenames (0.avi, 1.avi, 2.avi...)

2. Auto-Cleanup Old Files
   - Automatically triggers when SD card space reaches 55GB
   - Prioritizes deleting files in videos directory
   - If videos directory is empty, deletes files in photos directory
   - Automatically checks and cleans space before starting recording

3. Web Interface Features
   - Start/Stop recording buttons
   - Recording status indicator (flashing green dot)
   - Displays current recording video filename
   - Automatically updates recording status every 2 seconds

**Technical Features:**
- Uses FreeRTOS task for video recording
- Supports real-time status query
- Comprehensive error handling and user prompts
- Automatic space management, prevents SD card from filling up

**Next Steps:**
- Implement servo control functionality
- Test video recording stability
- Optimize video quality and file size
- Add video playback functionality

---

### 2026-01-27 - Added System Uptime Display
**Updates:**
- Added system uptime display to web frontend
- Added uptime statistics to backend
- Implemented continuous uptime display (local timing, updates every second)
- Uptime format: HH:MM:SS (hours:minutes:seconds)
- Get initial uptime from server on page load, then local timing

**Modified Files:**
1. ESP32_S3_Camera_Monitor.ino - Added uptime statistics variables and functions
2. app_httpd.cpp - Added uptime information to status_handler
3. camera_index.h - Added uptime display area and real-time update function

**Implemented Features:**
1. System startup time recording
2. Uptime calculation (seconds, minutes, hours)
3. Uptime formatted display (HH:MM:SS)
4. Web interface real-time uptime display
5. Get initial uptime on page load
6. Local timing, automatically increases by 1 second every second
7. Update SD card info every 5 seconds (not uptime)

**Technical Features:**
- Uses millis() function to get system runtime
- Automatically calculates hours, minutes, seconds
- Formats as HH:MM:SS for display
- Uptime uses local timing, continuous display
- SD card info updates every 5 seconds, uptime updates every second
- Comprehensive error handling and user prompts

**Next Steps:**
- Implement video recording function
- Implement auto-delete old files (when SD card space reaches 55GB)
- Implement servo control function
- Test uptime display stability

---

### 2026-01-27 - Web Interface Camera Settings and Unit Optimization
**Updates:**
- Changed SD card info display unit from MB to GB (2 decimal places)
- Set camera default resolution to 1280x720 (FRAMESIZE_SVGA)
- Added camera settings interface to web frontend
- Support user adjustment of resolution, image quality, brightness, contrast, saturation
- Modified backend code to support dynamic camera parameter adjustment
- Fixed web interface loading status display
- Removed clock frequency dynamic adjustment (requires camera re-initialization, causes system instability)

**Modified Files:**
1. sd_read_write.cpp - Modified SD card space calculation function, returns GB unit (multiply by 100 to keep 2 decimal places)
2. app_httpd.cpp - Modified SD card info JSON format, uses %.2f format to display GB unit; removed xclk parameter support
3. camera_index.h - Added camera settings interface (resolution, image quality, brightness, contrast, saturation), removed clock frequency option
4. ESP32_S3_Camera_Monitor.ino - Changed default resolution to FRAMESIZE_SVGA (1280x720), clock frequency to 20MHz

**Implemented Features:**
1. SD card info displayed in GB (total space, used space, free space)
2. Camera default resolution set to 1280x720
3. Web interface camera settings:
   - Resolution adjustment (QVGA, SVGA, HD, XGA, SXGA, UXGA)
   - Image quality adjustment (low, medium, high, very high)
   - Brightness adjustment (-2 to +2)
   - Contrast adjustment (-2 to +2)
   - Saturation adjustment (-2 to +2)
4. Dynamically adjust camera parameters without restarting device
5. Fixed video stream loading status display

**Technical Features:**
- SD card space calculation uses floating point, keeps 2 decimal places
- Clock frequency set to 20MHz at startup, does not support runtime dynamic adjustment
- Uses JSON format to transmit camera parameters
- Responsive design, reasonable settings interface layout
- Auto-refresh video stream after settings change
- Comprehensive error handling and user prompts

**Next Steps:**
- Implement video recording function
- Implement auto-delete old files (when SD card space reaches 55GB)
- Implement servo control function
- Test camera settings stability

---

### 2026-01-27 - Optimized File Directory Structure and Web Interface
**Updates:**
- Optimized SD card file directory structure, added photos and videos subdirectories under camera directory
- Added SD card space info display to web interface (total space, used space, free space)
- Added SD card space query function (getSDFreeSpaceMB)
- Added video file save function (saveVideoToSD)
- Added file list get function (getFileList)
- Added batch delete files function (deleteAllFiles)
- Added SD card space info to system status interface

**Modified Files:**
1. sd_read_write.h - Updated directory definitions (PHOTO_DIR, VIDEO_DIR), added new function declarations
2. sd_read_write.cpp - Updated directory initialization logic, added video save and file management functions
3. app_httpd.cpp - Added SD card space info to status_handler
4. camera_index.h - Added SD card info display area, added auto-update function

**Implemented Features:**
1. Optimized file directory structure (/camera/photos and /camera/videos)
2. Real-time SD card space display (total space, used space, free space)
3. Automatically creates photos and videos subdirectories
4. Video file save function (preparation for video recording)
5. File list get function
6. Batch delete files function
7. Web interface auto-updates SD card info every 5 seconds

**Technical Features:**
- Separates photo and video storage for easy management
- Real-time display of SD card space usage
- Uses JSON format to transmit SD card info
- Responsive design, supports multiple screen sizes
- Auto-refresh mechanism, no need to manually refresh page

**Next Steps:**
- Implement video recording function
- Implement auto-delete old files (when SD card space reaches 55GB)
- Implement servo control function
- Test SD card functionality stability

---

### 2026-01-27 - Implemented SD Card File System
**Updates:**
- Created SD card read/write module (sd_read_write.h and sd_read_write.cpp)
- Implemented SD card initialization and detection
- Implemented auto-save photos to SD card
- Implemented basic file operations (directory creation, file read/write)
- Integrated SD card functionality to main program and photo capture interface

**Modified Files:**
1. sd_read_write.h - New SD card operation header, declares SD card related functions
2. sd_read_write.cpp - New SD card operation implementation, implements SD card functions
3. ESP32_S3_Camera_Monitor.ino - Integrated SD card initialization
4. app_httpd.cpp - Modified photo capture interface, implements save photos to SD card

**Implemented Features:**
1. SD card initialization and detection (supports MMC, SDSC, SDHC types)
2. SD card capacity and space usage display
3. Auto-save photos to SD card (/camera directory)
4. Auto-generate photo filenames (0.jpg, 1.jpg, 2.jpg...)
5. Basic file operations (directory creation, file read/write, delete)
6. SD card space query function

**Technical Features:**
- Uses SD_MMC interface, SDIO mode, 4-bit parallel transmission
- Supports FAT32/exFAT file systems
- Automatically creates photo save directory
- Auto-generates unique filenames
- Comprehensive error handling mechanism
- Detailed Chinese comments

**Next Steps:**
- Implement video recording function
- Implement auto-delete old files (when SD card space reaches 55GB)
- Implement servo control function
- Test SD card functionality stability

---

### 2026-01-27 - Feature Adjustment and Optimization
**Updates:**
- Adjusted project structure, removed unimplemented feature modules
- Optimized HTTP server implementation, using ESP-IDF's httpd component
- Simplified web interface, only kept core functions
- Added video stream retry mechanism, improved stability
- Added "open video stream in new tab" function
- Fixed browser tab continuous loading issue

**Modified Files:**
1. app_httpd.cpp - Simplified HTTP server implementation, removed WebSocket configuration
2. camera_index.h - Simplified web interface, added video stream retry mechanism and new tab function
3. ESP32_S3_Camera_Monitor.ino - Optimized camera configuration

**Implemented Features:**
1. Web display of monitoring video (MJPEG streaming)
2. Web control of photo capture
3. Open video stream in new tab function
4. Basic HTTP server implementation

**Technical Features:**
- Uses Arduino framework for development
- Uses ESP-IDF's httpd component for HTTP server
- Supports OV5640 camera, 22MHz clock
- Supports MJPEG video stream
- Supports web interface video display and photo capture control
- Supports video stream retry mechanism

**Next Steps:**
- Test video stream functionality
- Test photo capture functionality
- Test open video stream in new tab functionality
- Optimize system stability

---

### 2026-01-26 - Project Initialization
**Updates:**
- Created project file structure
- Wrote configuration files
- Implemented camera initialization module
- Implemented WiFi connection module
- Implemented basic HTTP server module
- Implemented web interface

**Created Files:**
1. ESP32_S3_Camera_Monitor.ino - Main program file
2. camera_pins.h - Camera pin configuration file
3. app_httpd.cpp - HTTP server implementation file
4. camera_index.h - Web interface HTML file
5. partitions.csv - Partition table file

**Implemented Features:**
1. Web display of monitoring video (streaming)
2. Web control of photo capture

**Technical Features:**
- Uses Arduino framework for development
- Supports OV5640 camera

**Next Steps:**
- Optimize feature performance
- Improve error handling
- Update documentation

---

**Document Version:** 1.10
**Created Date:** 2026-01-28
**Last Updated:** 2026-01-28

---

# 功能更新文档

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

## 项目信息
- **项目名称**：ESP32-S3监控项目
- **开始日期**：2026-01-26
- **文档版本**：1.10

## 更新记录

### 2026-01-30 - 修复视频流获取问题并改为手动控制
**更新内容：**
- 移除页面加载时自动获取视频流的逻辑
- 添加视频流手动控制按钮（开始/停止）
- 更新 Web 界面，支持新按钮的中英双语显示
- 移除 "在新标签页中打开视频流" 按钮（优化界面）

**修改的文件：**
1. camera_index.h - 更新 HTML 结构和 JavaScript 逻辑

---

### 2026-01-28 - 添加HTTP Basic Authentication和会话管理
**更新内容：**
- 创建认证模块（auth.h和auth.cpp）
- 实现HTTP Basic Authentication
- 实现会话管理（14天过期）
- 为主页、视频流、拍照接口、摄像头控制接口添加认证保护
- 登录后可以直接访问其他接口，无需重复验证
- 会话14天后自动过期

**修改的文件：**
1. auth.h - 认证模块头文件
2. auth.cpp - 认证模块实现
3. app_httpd.cpp - 为各个接口添加认证检查
4. ESP32_S3_Camera_Monitor.ino - 初始化认证模块

**技术特点：**
- HTTP Basic Authentication：
  - 使用HTTP Basic Authentication进行身份验证
  - 用户名和密码在auth.h中定义（AUTH_USERNAME和AUTH_PASSWORD）
  - 认证成功后创建会话并设置Cookie

- 会话管理：
  - 会话ID使用随机字节生成并Base64编码
  - 会话包含客户端IP地址、创建时间、最后访问时间
  - 会话14天后自动过期（SESSION_EXPIRE_TIME）
  - 支持最多10个并发会话（MAX_SESSIONS）
  - 定期清理过期会话（auth_cleanup_expired_sessions）

- 认证保护：
  - 主页（/）需要认证
  - 视频流（/stream）需要认证
  - 拍照接口（/capture）需要认证
  - 系统状态接口（/status）需要认证
  - 摄像头控制接口（/control、/resolution等）需要认证

- 认证流程：
  - 首次访问时，浏览器弹出登录对话框
  - 用户输入用户名和密码后，服务器验证
  - 验证成功后，创建会话并设置Cookie
  - 后续请求使用Cookie进行认证，无需重复输入密码
  - 会话14天后自动过期，需要重新登录

**用户名和密码位置：**
- 文件：auth.h
- 行号：第21-22行
- 定义：
  ```c
  #define AUTH_USERNAME "admin"
  #define AUTH_PASSWORD "password123"
  ```

**参数调整：**
- 用户名（AUTH_USERNAME）：admin（可自定义）
  - 调整建议：使用有意义的用户名
- 密码（AUTH_PASSWORD）：password123（可自定义）
  - 调整建议：使用强密码，包含大小写字母、数字和特殊字符
- 会话过期时间（SESSION_EXPIRE_TIME）：14天（可自定义）
  - 调整建议：根据安全需求调整，单位为秒
- 最大会话数量（MAX_SESSIONS）：10（可自定义）
  - 调整建议：根据实际需求调整，每个会话占用约100字节内存

**下一步计划：**
- 实现舵机控制功能
- 测试视频录制功能稳定性
- 优化视频质量和文件大小
- 添加视频播放功能

---

### 2026-01-28 - 修复编译错误（videoRecordTask重复定义）
**更新内容：**
- 删除app_httpd.cpp中的videoRecordTask函数定义
- 删除app_httpd.cpp中的视频录制控制接口（/record/start、/record/stop、/record/status）
- 删除app_httpd.cpp中的视频录制控制变量（isVideoRecording、videoRecordTaskHandle）
- 更新app_httpd.cpp文件头说明

**修改的文件：**
1. app_httpd.cpp - 删除videoRecordTask函数和相关接口

**修复的问题：**
1. 编译错误：multiple definition of `videoRecordTask(void*)'
   - 问题：app_httpd.cpp和ESP32_S3_Camera_Monitor.ino中都定义了videoRecordTask函数
   - 修复：删除app_httpd.cpp中的videoRecordTask函数定义
   - 修复：删除app_httpd.cpp中的视频录制控制接口
   - 修复：删除app_httpd.cpp中的视频录制控制变量
   - 结果：编译成功，不再有重复定义错误

**技术特点：**
- 视频录制完全自动化：
  - 系统启动时自动开始录制
  - 不再需要Web界面的录制控制
  - 不再需要HTTP接口控制录制

- 代码简化：
  - 删除了不必要的视频录制控制代码
  - 减少了代码复杂度
  - 提高了系统稳定性

**下一步计划：**
- 实现舵机控制功能
- 测试视频录制功能稳定性
- 优化视频质量和文件大小
- 添加视频播放功能

---

### 2026-01-28 - 修复视频录制文件大小为0KB的问题
**更新内容：**
- 修复视频录制文件大小为0KB的问题
- 在ESP32_S3_Camera_Monitor.ino中实现videoRecordTask函数
- 在setup函数中创建视频录制任务
- 添加videoRecordTask函数声明

**修改的文件：**
1. ESP32_S3_Camera_Monitor.ino - 添加videoRecordTask函数和任务创建代码

**修复的问题：**
1. 视频录制文件大小为0KB
   - 问题：startVideoRecording()只创建了AVI文件并写入了文件头，但没有任务调用writeVideoFrame()来写入视频帧
   - 修复：在ESP32_S3_Camera_Monitor.ino中实现videoRecordTask函数
   - 修复：在setup函数中调用startVideoRecording()后创建视频录制任务
   - 修复：videoRecordTask循环获取摄像头帧并调用writeVideoFrame()写入视频帧
   - 结果：视频录制正常工作，文件大小正常增长

**技术特点：**
- videoRecordTask函数：
  - 使用FreeRTOS任务进行视频录制
  - 循环获取摄像头帧（esp_camera_fb_get）
  - 调用writeVideoFrame()写入视频帧
  - 释放帧缓冲区（esp_camera_fb_return）
  - 延迟以控制帧率（1000/fps毫秒）
  - 检查isRecordingVideo()状态，停止时退出任务

- 任务创建：
  - 在setup函数中调用startVideoRecording()后创建任务
  - 使用xTaskCreate()创建任务
  - 任务名称：video_record
  - 任务堆栈：4096字节
  - 任务优先级：5
  - 传递fps参数（15）

**下一步计划：**
- 实现舵机控制功能
- 测试视频录制功能稳定性
- 优化视频质量和文件大小
- 添加视频播放功能

---

### 2026-01-28 - 修复时间同步和自动分段功能
**更新内容：**
- 修复sd_read_write.cpp缺少time.h头文件的问题
- 修复自动分段功能无法正常工作的问题
- 优化stopVideoRecording函数，支持保持录制状态
- 修复自动分段时isRecording状态管理问题

**修改的文件：**
1. sd_read_write.cpp - 添加time.h头文件，修复自动分段逻辑
2. sd_read_write.h - 修改stopVideoRecording函数签名，添加keepRecordingState参数

**修复的问题：**
1. 时间戳格式问题
   - 问题：sd_read_write.cpp缺少time.h头文件，导致NTP时间同步函数无法使用
   - 修复：添加#include "time.h"
   - 结果：时间戳格式正确显示为YYYYMMDDHHMM

2. 自动分段功能问题
   - 问题：自动分段时调用stopVideoRecording()会设置isRecording=false，导致后续录制失败
   - 修复：修改stopVideoRecording函数，添加keepRecordingState参数
   - 修复：在自动分段时传递keepRecordingState=true，保持录制状态
   - 修复：在startVideoRecording前临时设置isRecording=false
   - 结果：自动分段功能正常工作，每2分钟自动切换到新的视频文件

**技术特点：**
- stopVideoRecording函数支持两种模式：
  - stopVideoRecording(false)：完全停止录制，设置isRecording=false
  - stopVideoRecording(true)：停止当前分段但保持录制状态，isRecording保持true
- 自动分段流程：
  1. 检测到2分钟时长
  2. 调用stopVideoRecording(true)停止当前分段
  3. 临时设置isRecording=false
  4. 调用startVideoRecording()开始新的分段
  5. 重置分段开始时间
  6. 继续录制

**下一步计划：**
- 实现舵机控制功能
- 测试视频录制功能稳定性
- 优化视频质量和文件大小
- 添加视频播放功能

---

### 2026-01-28 - 优化时间戳格式和Web界面
**更新内容：**
- 修改时间戳格式为YYYYMMDDHHMM（年月日时分）
- 添加NTP时间同步功能
- 移除Web前端的视频录制控制界面
- 视频录制改为完全自动化（系统启动时自动开始）
- 优化文件命名规则，使用NTP同步的系统时间

**修改的文件：**
1. ESP32_S3_Camera_Monitor.ino - 添加NTP时间同步配置
2. sd_read_write.cpp - 修改时间戳生成函数，使用NTP时间
3. app_httpd.cpp - 更新文档说明
4. camera_index.h - 移除视频录制控制界面和相关函数

**实现的功能：**
1. NTP时间同步
   - 使用pool.ntp.org和time.nist.gov作为NTP服务器
   - 时区设置为UTC+8（北京时间）
   - 系统启动后自动同步时间
   - 串口输出当前时间

2. 时间戳文件名
   - 照片文件名格式：YYYYMMDDHHMM.jpg
   - 视频文件名格式：YYYYMMDDHHMM.avi
   - YYYY为年份（4位）
   - MM为月份（2位）
   - DD为日期（2位）
   - HH为小时（2位）
   - MM为分钟（2位）

3. Web界面优化
   - 移除视频录制控制界面（开始/停止按钮）
   - 移除录制状态指示器
   - 移除录制状态查询功能
   - 视频录制完全自动化，无需手动操作

**技术特点：**
- 使用NTP协议同步系统时间
- 时间戳格式统一，便于管理和查找
- 视频录制完全自动化，系统启动后自动开始
- Web界面简洁，只保留必要的控制功能
- 文件名示例：202601281041.jpg（2026年1月28日10:41）

**下一步计划：**
- 实现舵机控制功能
- 测试视频录制功能稳定性
- 优化视频质量和文件大小
- 添加视频播放功能

---

### 2026-01-27 - 优化视频录制功能
**更新内容：**
- 修改照片和视频文件名为时间戳格式（ESP32-xxxx.xx.xx.xx）
- 实现视频自动分段录制（2分钟一段）
- 修改为启动时自动开始录制
- 优化文件命名规则，使用系统运行时间作为时间戳

**修改的文件：**
1. sd_read_write.h - 添加时间戳文件名生成函数声明
2. sd_read_write.cpp - 实现时间戳文件名生成和自动分段功能
3. ESP32_S3_Camera_Monitor.ino - 在启动时自动开始视频录制

**实现的功能：**
1. 时间戳文件名
   - 照片文件名格式：ESP32-xxxx.xx.xx.xx.jpg
   - 视频文件名格式：ESP32-xxxx.xx.xx.xx.avi
   - xxxx为系统运行秒数
   - xx.xx.xx为时:分:秒

2. 视频自动分段
   - 每2分钟自动分段
   - 自动停止当前分段并开始新的分段
   - 分段文件名自动递增时间戳
   - 无缝衔接，不影响录制

3. 启动时自动录制
   - 系统启动时自动开始视频录制
   - 默认参数：15fps，1280x720
   - 无需手动操作

**技术特点：**
- 使用系统运行时间（millis()）作为时间戳
- 自动检测分段时长，达到2分钟自动切换
- 分段切换时自动清理旧文件（如果空间不足）
- 文件名格式统一，便于管理和查找

**下一步计划：**
- 实现舵机控制功能
- 测试视频录制功能稳定性
- 优化视频质量和文件大小
- 添加视频播放功能

---

### 2026-01-27 - 添加视频录制和自动清理功能
**更新内容：**
- 实现视频录制功能（AVI格式，MJPEG编码）
- 添加Web端视频录制控制界面
- 实现自动删除旧文件功能（55GB阈值）
- 支持自定义录制参数（帧率、分辨率）

**修改的文件：**
1. sd_read_write.h - 添加AVI文件结构体和视频录制函数声明
2. sd_read_write.cpp - 实现视频录制和自动清理功能
3. app_httpd.cpp - 添加视频录制控制接口（/record/start、/record/stop、/record/status）
4. camera_index.h - 添加视频录制界面和控制按钮

**实现的功能：**
1. 视频录制功能
   - 支持AVI格式视频录制
   - 使用MJPEG编码
   - 可自定义帧率（默认15fps）
   - 可自定义分辨率（默认1280x720）
   - 自动生成视频文件名（0.avi, 1.avi, 2.avi...）

2. 自动清理旧文件功能
   - SD卡空间达到55GB时自动触发
   - 优先删除videos目录中的文件
   - 如果videos目录为空，删除photos目录中的文件
   - 在开始录制前自动检查并清理空间

3. Web界面功能
   - 开始/停止录制按钮
   - 录制状态指示器（闪烁的绿色圆点）
   - 显示当前录制的视频文件名
   - 每2秒自动更新录制状态

**技术特点：**
- 使用FreeRTOS任务进行视频录制
- 支持实时状态查询
- 完善的错误处理和用户提示
- 自动空间管理，避免SD卡写满

**下一步计划：**
- 实现舵机控制功能
- 测试视频录制功能稳定性
- 优化视频质量和文件大小
- 添加视频播放功能

---

### 2026-01-27 - 添加系统运行时长显示
**更新内容：**
- 在Web前端添加系统运行时长显示功能
- 在后端添加运行时长统计功能
- 实现运行时长连续显示（本地计时，每秒更新）
- 运行时长格式：HH:MM:SS（时:分:秒）
- 在页面加载时从服务器获取初始运行时长，然后本地计时

**修改的文件：**
1. ESP32_S3_Camera_Monitor.ino - 添加运行时长统计变量和函数
2. app_httpd.cpp - 在status_handler中添加运行时长信息
3. camera_index.h - 添加运行时长显示区域和实时更新功能

**实现的功能：**
1. 系统启动时间记录
2. 运行时长计算（秒、分、时）
3. 运行时长格式化显示（HH:MM:SS）
4. Web界面实时显示运行时长
5. 页面加载时获取初始运行时长
6. 本地计时，每秒自动增加1秒
7. 每5秒更新SD卡信息（不更新运行时长）

**技术特点：**
- 使用millis()函数获取系统运行时间
- 自动计算时、分、秒
- 格式化为HH:MM:SS格式显示
- 运行时长使用本地计时，连续显示
- SD卡信息每5秒更新，运行时长每秒更新
- 完善的错误处理和用户提示

**下一步计划：**
- 实现视频录制功能
- 实现自动删除旧文件功能（当SD卡空间达到55GB时）
- 实现舵机控制功能
- 测试运行时长显示功能稳定性

---

### 2026-01-27 - Web界面摄像头设置和单位优化
**更新内容：**
- 将SD卡信息显示单位从MB改为GB（保留2位小数）
- 将摄像头默认分辨率设置为1280x720（FRAMESIZE_SVGA）
- 在Web前端添加摄像头设置界面
- 支持用户自行调节分辨率、图像质量、亮度、对比度、饱和度
- 修改后端代码支持动态调整摄像头参数
- 修复Web界面加载状态显示问题
- 移除时钟频率动态调整功能（因为需要重新初始化摄像头，会导致系统不稳定）

**修改的文件：**
1. sd_read_write.cpp - 修改SD卡空间计算函数，返回GB单位（乘以100保留2位小数）
2. app_httpd.cpp - 修改SD卡信息JSON格式，使用%.2f格式显示GB单位；移除xclk参数支持
3. camera_index.h - 添加摄像头设置界面（分辨率、图像质量、亮度、对比度、饱和度），移除时钟频率选项
4. ESP32_S3_Camera_Monitor.ino - 修改默认分辨率为FRAMESIZE_SVGA（1280x720），时钟频率改为20MHz

**实现的功能：**
1. SD卡信息以GB为单位显示（总空间、已用空间、剩余空间）
2. 摄像头默认分辨率设置为1280x720
3. Web界面摄像头设置功能：
   - 分辨率调节（QVGA、SVGA、HD、XGA、SXGA、UXGA）
   - 图像质量调节（低、中、高、极高）
   - 亮度调节（-2到+2）
   - 对比度调节（-2到+2）
   - 饱和度调节（-2到+2）
4. 动态调整摄像头参数，无需重启设备
5. 修复视频流加载状态显示问题

**技术特点：**
- SD卡空间计算使用浮点数运算，保留2位小数
- 时钟频率在启动时设置为20MHz，不支持运行时动态调整
- 使用JSON格式传输摄像头参数
- 响应式设计，设置界面布局合理
- 设置更改后自动刷新视频流
- 完善的错误处理和用户提示

**下一步计划：**
- 实现视频录制功能
- 实现自动删除旧文件功能（当SD卡空间达到55GB时）
- 实现舵机控制功能
- 测试摄像头设置功能稳定性

---

### 2026-01-27 - 优化文件目录结构和Web界面
**更新内容：**
- 优化SD卡文件目录结构，在camera目录下增加photos和videos子目录
- 在Web界面添加SD卡空间信息显示（总空间、已用空间、剩余空间）
- 添加SD卡空间查询功能（getSDFreeSpaceMB）
- 添加视频文件保存功能（saveVideoToSD）
- 添加文件列表获取功能（getFileList）
- 添加批量删除文件功能（deleteAllFiles）
- 在系统状态接口中添加SD卡空间信息

**修改的文件：**
1. sd_read_write.h - 更新目录定义（PHOTO_DIR、VIDEO_DIR），添加新函数声明
2. sd_read_write.cpp - 更新目录初始化逻辑，添加视频保存和文件管理功能
3. app_httpd.cpp - 在status_handler中添加SD卡空间信息
4. camera_index.h - 添加SD卡信息显示区域，添加自动更新功能

**实现的功能：**
1. 优化的文件目录结构（/camera/photos和/camera/videos）
2. SD卡空间实时显示（总空间、已用空间、剩余空间）
3. 自动创建photos和videos子目录
4. 视频文件保存功能（为后续视频录制做准备）
5. 文件列表获取功能
6. 批量删除文件功能
7. Web界面每5秒自动更新SD卡信息

**技术特点：**
- 分离照片和视频存储，便于管理
- 实时显示SD卡空间使用情况
- 使用JSON格式传输SD卡信息
- 响应式设计，支持多种屏幕尺寸
- 自动刷新机制，无需手动刷新页面

**下一步计划：**
- 实现视频录制功能
- 实现自动删除旧文件功能（当SD卡空间达到55GB时）
- 实现舵机控制功能
- 测试SD卡功能稳定性

---

### 2026-01-27 - 实现SD卡文件系统功能
**更新内容：**
- 创建SD卡读写模块（sd_read_write.h和sd_read_write.cpp）
- 实现SD卡初始化和检测功能
- 实现照片自动保存到SD卡功能
- 实现目录创建、文件读写等基础文件操作
- 集成SD卡功能到主程序和拍照接口

**修改的文件：**
1. sd_read_write.h - 新建SD卡操作头文件，声明SD卡相关函数
2. sd_read_write.cpp - 新建SD卡操作实现文件，实现SD卡功能
3. ESP32_S3_Camera_Monitor.ino - 集成SD卡初始化功能
4. app_httpd.cpp - 修改拍照接口，实现保存照片到SD卡

**实现的功能：**
1. SD卡初始化和检测（支持MMC、SDSC、SDHC类型）
2. SD卡容量和空间使用情况显示
3. 照片自动保存到SD卡（/camera目录）
4. 自动生成照片文件名（0.jpg, 1.jpg, 2.jpg...）
5. 目录创建、文件读写、删除等基础文件操作
6. SD卡空间查询功能

**技术特点：**
- 使用SD_MMC接口，SDIO模式，4位并行传输
- 支持FAT32/exFAT文件系统
- 自动创建照片保存目录
- 自动生成唯一文件名
- 完整的错误处理机制
- 详细的中文注释

**下一步计划：**
- 实现视频录制功能
- 实现自动删除旧文件功能（当SD卡空间达到55GB时）
- 实现舵机控制功能
- 测试SD卡功能稳定性

---

### 2026-01-27 - 功能调整与优化
**更新内容：**
- 调整项目结构，移除未实现的功能模块
- 优化HTTP服务器实现，使用ESP-IDF的httpd组件
- 简化Web界面，只保留核心功能
- 添加视频流重试机制，提高稳定性
- 添加"在新标签页中打开视频流"功能
- 修复浏览器标签页持续加载的问题

**修改的文件：**
1. app_httpd.cpp - 简化HTTP服务器实现，移除WebSocket配置
2. camera_index.h - 简化Web界面，添加视频流重试机制和新标签页功能
3. ESP32_S3_Camera_Monitor.ino - 优化摄像头配置

**实现的功能：**
1. Web端显示监控视频（MJPEG流式输出）
2. Web端控制拍照功能
3. 在新标签页中打开视频流功能
4. 基础HTTP服务器实现

**技术特点：**
- 使用Arduino框架开发
- 使用ESP-IDF的httpd组件实现HTTP服务器
- 支持OV5640摄像头，22MHz时钟
- 支持MJPEG视频流
- 支持Web界面视频显示和拍照控制
- 支持视频流重试机制

**下一步计划：**
- 测试视频流功能
- 测试拍照功能
- 测试在新标签页中打开视频流功能
- 优化系统稳定性

---

### 2026-01-26 - 项目初始化
**更新内容：**
- 创建项目文件结构
- 编写配置文件
- 实现摄像头初始化模块
- 实现WiFi连接模块
- 实现基础HTTP服务器模块
- 实现Web界面

**创建的文件：**
1. ESP32_S3_Camera_Monitor.ino - 主程序文件
2. camera_pins.h - 摄像头引脚配置文件
3. app_httpd.cpp - HTTP服务器实现文件
4. camera_index.h - Web界面HTML文件
5. partitions.csv - 分区表文件

**实现的功能：**
1. Web端显示监控视频（流式输出）
2. Web端控制拍照功能

**技术特点：**
- 使用Arduino框架开发
- 支持OV5640摄像头

**下一步计划：**
- 优化功能性能
- 完善错误处理
- 更新文档

---

**文档版本：** 1.10
**创建日期：** 2026-01-28
**最后更新：** 2026-01-28
