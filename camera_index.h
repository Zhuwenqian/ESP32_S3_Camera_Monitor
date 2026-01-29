/*
 * 文件名 / Filename : camera_index.h
 * 
 * 文件用途 / File Purpose :
 * 本文件包含Web界面的HTML代码，提供完整的监控控制界面
 * This file contains HTML code for the web interface, providing a complete monitoring control interface
 * 
 * 功能模块 / Function Modules:
 * 1. 视频显示区域：显示实时视频流 / Video display area: displays real-time video stream
 * 2. 拍照控制区域：控制拍照功能 / Photo capture control area: controls photo capture function
 * 3. SD卡信息区域：显示SD卡存储信息（GB单位）/ SD card info area: displays SD card storage information (GB unit)
 * 4. 运行时长区域：显示系统运行时长（HH:MM:SS格式）/ Uptime area: displays system uptime (HH:MM:SS format)
 * 5. 摄像头设置区域：调节分辨率、图像质量、亮度、对比度、饱和度 / Camera settings area: adjusts resolution, quality, brightness, contrast, saturation
 * 6. 语言选择功能：支持英语和中文切换，默认为英语 / Language selection: supports English and Chinese switching, default is English
 * 7. 主题选择功能：支持深色和浅色主题切换，默认为浅色 / Theme selection: supports dark and light theme switching, default is light
 * 
 * 技术实现 / Technical Implementation:
 * - 使用HTML5和CSS3构建界面 / Uses HTML5 and CSS3 to build the interface
 * - 使用JavaScript实现AJAX请求 / Uses JavaScript to implement AJAX requests
 * - 使用Fetch API与服务器通信 / Uses Fetch API to communicate with server
 * - 支持响应式设计 / Supports responsive design
 * - 支持实时状态更新 / Supports real-time status updates
 * - 使用localStorage保存语言偏好 / Uses localStorage to save language preference
 * - 使用localStorage保存主题偏好 / Uses localStorage to save theme preference
 * - 使用data-lang属性实现多语言切换 / Uses data-lang attributes for multi-language switching
 * - 使用CSS变量实现主题切换 / Uses CSS variables for theme switching
 * - 使用data-theme属性控制主题 / Uses data-theme attribute to control theme
 * 
 * 界面特点 / Interface Features:
 * - 简洁美观的UI设计 / Clean and beautiful UI design
 * - 实时视频流显示（MJPEG）/ Real-time video stream display (MJPEG)
 * - 拍照功能 / Photo capture function
 * - SD卡空间实时更新（GB单位）/ Real-time SD card space update (GB unit)
 * - 摄像头参数动态调节 / Dynamic camera parameter adjustment
 * - 视频流加载状态显示 / Video stream loading status display
 * - 双语支持（英语和中文）/ Bilingual support (English and Chinese)
 * - 语言偏好记忆功能 / Language preference memory
 * - 深色/浅色主题支持 / Dark/Light theme support
 * - 主题偏好记忆功能 / Theme preference memory
 * - 平滑的主题切换动画 / Smooth theme transition animation
 * 
 * 创建日期 / Creation Date : 2026-01-26
 * 最后更新 / Last Update : 2026-01-29
 * 最新更新 / Latest Updates:
 * 1. 添加摄像头设置界面（分辨率、图像质量、亮度、对比度、饱和度）/ Added camera settings interface (resolution, quality, brightness, contrast, saturation)
 * 2. 修改SD卡信息显示单位为GB / Changed SD card info display unit to GB
 * 3. 修复视频流加载状态显示问题 / Fixed video stream loading status display issue
 * 4. 添加摄像头参数动态调节功能 / Added dynamic camera parameter adjustment function
 * 5. 移除时钟频率动态调整功能（因为需要重新初始化摄像头，会导致系统不稳定）/ Removed dynamic clock frequency adjustment (because it requires re-initializing the camera, which can cause system instability)
 * 6. 添加系统运行时长显示功能（HH:MM:SS格式，本地计时每秒更新）/ Added system uptime display function (HH:MM:SS format, local timer updates every second)
 * 7. 实现运行时长实时更新（每1秒本地增加1秒）/ Implemented real-time uptime update (adds 1 second locally every 1 second)
 * 8. 页面加载时从服务器获取初始运行时长 / Fetches initial uptime from server on page load
 * 9. 移除视频录制控制界面（开始/停止按钮）/ Removed video recording control interface (start/stop buttons)
 * 10. 移除录制状态指示器和相关JavaScript函数 / Removed recording status indicator and related JavaScript functions
 * 11. 视频录制改为系统启动时自动开始，完全自动化 / Video recording changed to auto-start on boot, fully automated
 * 12. 修改照片和视频文件名为时间戳格式（YYYYMMDDHHMM）/ Changed photo and video filename format to timestamp (YYYYMMDDHHMM)
 * 13. 实现视频自动分段录制功能（2分钟一段）/ Implemented auto-segmented video recording (2 minutes per segment)
 * 14. 系统启动时自动开始录制 / Recording starts automatically on boot
 * 15. 添加语言选择功能（英语和中文，默认英语）/ Added language selection (English and Chinese, default is English)
 * 16. 使用localStorage保存用户语言偏好 / Uses localStorage to save user language preference
 * 17. 实现页面加载时自动应用保存的语言 / Implements automatic application of saved language on page load
 * 18. 添加主题选择功能（深色和浅色，默认浅色）/ Added theme selection (dark and light, default is light)
 * 19. 使用CSS变量实现深色和浅色主题 / Uses CSS variables for dark and light themes
 * 20. 使用localStorage保存用户主题偏好 / Uses localStorage to save user theme preference
 * 21. 实现页面加载时自动应用保存的主题 / Implements automatic application of saved theme on page load
 * 22. 添加平滑的主题切换过渡动画（0.3秒）/ Added smooth theme transition animation (0.3 seconds)
 */

// Web界面HTML代码 / Web interface HTML code
const char index_ov2640_html[] = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-S3监控控制台</title>
    <style>
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
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: Arial, sans-serif;
            background-color: var(--bg-color);
            padding: 20px;
            transition: background-color 0.3s ease;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background-color: var(--container-bg);
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px var(--shadow-color);
            transition: background-color 0.3s ease, box-shadow 0.3s ease;
        }
        
        h1 {
            text-align: center;
            color: var(--text-color);
            margin-bottom: 30px;
            transition: color 0.3s ease;
        }
        
        .control-section {
            margin-bottom: 30px;
            padding: 20px;
            background-color: var(--section-bg);
            border-radius: 5px;
            transition: background-color 0.3s ease;
        }
        
        .section-title {
            font-size: 18px;
            font-weight: bold;
            color: var(--section-title-color);
            margin-bottom: 15px;
            transition: color 0.3s ease;
        }
        
        .video-container {
            text-align: center;
        }
        
        #stream {
            max-width: 100%;
            height: auto;
            border: 2px solid var(--border-color);
            border-radius: 5px;
            transition: border-color 0.3s ease;
        }
        
        .control-buttons {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            flex-wrap: wrap;
        }
        
        .btn-primary {
            background-color: var(--btn-primary-bg);
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
            transition: background-color 0.3s ease;
        }
        
        .btn-primary:hover {
            background-color: var(--btn-primary-hover);
        }
        
        .btn-success {
            background-color: var(--btn-success-bg);
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
            transition: background-color 0.3s ease;
        }
        
        .btn-success:hover {
            background-color: var(--btn-success-hover);
        }
        
        .btn-danger {
            background-color: var(--btn-danger-bg);
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
            transition: background-color 0.3s ease;
        }
        
        .btn-danger:hover {
            background-color: var(--btn-danger-hover);
        }
        
        .btn-warning {
            background-color: var(--btn-warning-bg);
            color: var(--btn-warning-text);
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
            transition: background-color 0.3s ease;
        }
        
        .btn-warning:hover {
            background-color: var(--btn-warning-hover);
        }
        
        .slider-container {
            margin-bottom: 15px;
        }
        
        input[type="range"] {
            width: 100%;
            margin-bottom: 10px;
        }
        
        .slider-value {
            text-align: center;
            color: var(--text-color);
            transition: color 0.3s ease;
        }
        
        .info-display {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
        }
        
        .info-item {
            padding: 15px;
            background-color: var(--container-bg);
            border-radius: 5px;
            border: 1px solid var(--border-color);
            transition: background-color 0.3s ease, border-color 0.3s ease;
        }
        
        .info-label {
            font-size: 14px;
            color: var(--info-label-color);
            margin-bottom: 5px;
            transition: color 0.3s ease;
        }
        
        .info-value {
            font-size: 16px;
            font-weight: bold;
            color: var(--info-value-color);
            transition: color 0.3s ease;
        }
        
        .status-indicator {
            display: inline-block;
            width: 10px;
            height: 10px;
            border-radius: 50%;
            margin-right: 5px;
        }
        
        .status-online {
            background-color: #28a745;
        }
        
        .status-offline {
            background-color: #dc3545;
        }
        
        .sd-info {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            margin-top: 15px;
        }
        
        .sd-info-item {
            padding: 15px;
            background-color: var(--container-bg);
            border-radius: 5px;
            border: 1px solid var(--border-color);
            text-align: center;
            transition: background-color 0.3s ease, border-color 0.3s ease;
        }
        
        .sd-info-label {
            font-size: 14px;
            color: var(--info-label-color);
            margin-bottom: 5px;
            transition: color 0.3s ease;
        }
        
        .sd-info-value {
            font-size: 20px;
            font-weight: bold;
            color: var(--sd-info-value-color);
            transition: color 0.3s ease;
        }
        
        .sd-info-unit {
            font-size: 14px;
            color: var(--sd-info-unit-color);
            margin-left: 5px;
            transition: color 0.3s ease;
        }
        
        .camera-settings {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 15px;
            margin-top: 15px;
        }
        
        .setting-item {
            display: flex;
            flex-direction: column;
        }
        
        .setting-label {
            font-size: 14px;
            color: var(--setting-label-color);
            margin-bottom: 5px;
            transition: color 0.3s ease;
        }
        
        .setting-select {
            padding: 8px;
            border: 1px solid var(--setting-border-color);
            border-radius: 5px;
            font-size: 14px;
            background-color: var(--container-bg);
            color: var(--text-color);
            cursor: pointer;
            transition: border-color 0.3s ease, background-color 0.3s ease, color 0.3s ease;
        }
        
        .setting-select:hover {
            border-color: var(--setting-border-hover);
        }
        
        .setting-select:focus {
            outline: none;
            border-color: var(--setting-border-hover);
            box-shadow: 0 0 0 2px rgba(0, 123, 255, 0.25);
        }
        
        .uptime-info {
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
            background-color: var(--container-bg);
            border-radius: 5px;
            border: 1px solid var(--border-color);
            transition: background-color 0.3s ease, border-color 0.3s ease;
        }
        
        .uptime-value {
            font-size: 24px;
            font-weight: bold;
            color: var(--uptime-value-color);
            transition: color 0.3s ease;
        }
        
        .uptime-unit {
            font-size: 14px;
            color: var(--uptime-unit-color);
            margin-left: 10px;
            transition: color 0.3s ease;
        }
        
        .language-selector {
            display: flex;
            justify-content: flex-end;
            align-items: center;
            margin-bottom: 20px;
            padding: 10px;
            background-color: var(--section-bg);
            border-radius: 5px;
            transition: background-color 0.3s ease;
        }
        
        .language-label {
            font-size: 14px;
            color: var(--language-label-color);
            margin-right: 10px;
            transition: color 0.3s ease;
        }
        
        .language-select {
            padding: 8px 12px;
            border: 1px solid var(--setting-border-color);
            border-radius: 5px;
            font-size: 14px;
            background-color: var(--container-bg);
            color: var(--text-color);
            cursor: pointer;
            transition: border-color 0.3s ease, background-color 0.3s ease, color 0.3s ease;
        }
        
        .language-select:hover {
            border-color: var(--setting-border-hover);
        }
        
        .language-select:focus {
            outline: none;
            border-color: var(--setting-border-hover);
            box-shadow: 0 0 0 2px rgba(0, 123, 255, 0.25);
        }
        
        .theme-selector {
            display: flex;
            justify-content: flex-end;
            align-items: center;
            margin-bottom: 20px;
            padding: 10px;
            background-color: var(--section-bg);
            border-radius: 5px;
            transition: background-color 0.3s ease;
        }
        
        .theme-label {
            font-size: 14px;
            color: var(--language-label-color);
            margin-right: 10px;
            transition: color 0.3s ease;
        }
        
        .theme-select {
            padding: 8px 12px;
            border: 1px solid var(--setting-border-color);
            border-radius: 5px;
            font-size: 14px;
            background-color: var(--container-bg);
            color: var(--text-color);
            cursor: pointer;
            transition: border-color 0.3s ease, background-color 0.3s ease, color 0.3s ease;
        }
        
        .theme-select:hover {
            border-color: var(--setting-border-hover);
        }
        
        .theme-select:focus {
            outline: none;
            border-color: var(--setting-border-hover);
            box-shadow: 0 0 0 2px rgba(0, 123, 255, 0.25);
        }
        
        .selectors-container {
            display: flex;
            gap: 10px;
            justify-content: flex-end;
            margin-bottom: 20px;
        }
        
        .selector-item {
            flex: 1;
            max-width: 200px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1 data-lang="title">ESP32-S3监控控制台</h1>
        
        <!-- 语言和主题选择器 -->
        <div class="selectors-container">
            <div class="selector-item">
                <div class="language-selector">
                    <span class="language-label" data-lang="language">Language / 语言</span>
                    <select id="language-select" class="language-select" onchange="changeLanguage()">
                        <option value="en">English</option>
                        <option value="zh">中文</option>
                    </select>
                </div>
            </div>
            <div class="selector-item">
                <div class="theme-selector">
                    <span class="theme-label" data-lang="theme">Theme / 主题</span>
                    <select id="theme-select" class="theme-select" onchange="changeTheme()">
                        <option value="light" data-lang="lightTheme">Light / 浅色</option>
                        <option value="dark" data-lang="darkTheme">Dark / 深色</option>
                    </select>
                </div>
            </div>
        </div>
        
        <!-- 视频显示区域 -->
        <div class="control-section">
            <div class="section-title" data-lang="liveVideo">实时视频</div>
            <div class="video-container">
                <img id="stream" src="" alt="视频流" style="display:none;" onload="this.style.display='block'; document.getElementById('loading').style.display='none';">
                <div id="loading" style="text-align:center; padding:20px; color:#666;" data-lang="loadingStream">正在加载视频流...</div>
            </div>
            <div style="margin-top: 15px; text-align: center;">
                <button class="btn-primary" onclick="capturePhoto()" data-lang="capturePhoto">拍照</button>
                <button class="btn-success" onclick="openStreamInNewTab()" data-lang="openStreamNewTab">在新标签页中打开视频流</button>
            </div>
        </div>
        
        <!-- SD卡信息区域 -->
        <div class="control-section">
            <div class="section-title" data-lang="sdCardInfo">SD卡存储信息</div>
            <div class="sd-info">
                <div class="sd-info-item">
                    <div class="sd-info-label" data-lang="totalSpace">总空间</div>
                    <div class="sd-info-value">
                        <span id="sd-total">--</span>
                        <span class="sd-info-unit">GB</span>
                    </div>
                </div>
                <div class="sd-info-item">
                    <div class="sd-info-label" data-lang="usedSpace">已用空间</div>
                    <div class="sd-info-value">
                        <span id="sd-used">--</span>
                        <span class="sd-info-unit">GB</span>
                    </div>
                </div>
                <div class="sd-info-item">
                    <div class="sd-info-label" data-lang="freeSpace">剩余空间</div>
                    <div class="sd-info-value">
                        <span id="sd-free">--</span>
                        <span class="sd-info-unit">GB</span>
                    </div>
                </div>
            </div>
        </div>
        
        <!-- 运行时长区域 -->
        <div class="control-section">
            <div class="section-title" data-lang="systemUptime">系统运行时长</div>
            <div class="uptime-info">
                <div class="uptime-value">
                    <span id="uptime">--:--:--</span>
                    <span class="uptime-unit" data-lang="uptimeUnit">运行时间</span>
                </div>
            </div>
        </div>
        
        <!-- 摄像头设置区域 -->
        <div class="control-section">
            <div class="section-title" data-lang="cameraSettings">摄像头设置</div>
            <div class="camera-settings">
                <div class="setting-item">
                    <label class="setting-label" data-lang="resolution">分辨率</label>
                    <select id="framesize" class="setting-select" onchange="applyCameraSettings()">
                        <option value="10">QVGA (320x240)</option>
                        <option value="7">SVGA (800x600)</option>
                        <option value="6" selected>HD (1280x720)</option>
                        <option value="5">XGA (1024x768)</option>
                        <option value="3">SXGA (1280x1024)</option>
                        <option value="4">UXGA (1600x1200)</option>
                    </select>
                </div>
                <div class="setting-item">
                    <label class="setting-label" data-lang="imageQuality">图像质量</label>
                    <select id="quality" class="setting-select" onchange="applyCameraSettings()">
                        <option value="63" data-lang="lowQuality">低质量 (63)</option>
                        <option value="31" data-lang="mediumQuality">中等质量 (31)</option>
                        <option value="10" selected data-lang="highQuality">高质量 (10)</option>
                        <option value="5" data-lang="veryHighQuality">极高质量 (5)</option>
                    </select>
                </div>
                <div class="setting-item">
                    <label class="setting-label" data-lang="brightness">亮度</label>
                    <select id="brightness" class="setting-select" onchange="applyCameraSettings()">
                        <option value="-2">-2</option>
                        <option value="-1">-1</option>
                        <option value="0" selected>0</option>
                        <option value="1">1</option>
                        <option value="2">2</option>
                    </select>
                </div>
                <div class="setting-item">
                    <label class="setting-label" data-lang="contrast">对比度</label>
                    <select id="contrast" class="setting-select" onchange="applyCameraSettings()">
                        <option value="-2">-2</option>
                        <option value="-1">-1</option>
                        <option value="0" selected>0</option>
                        <option value="1">1</option>
                        <option value="2">2</option>
                    </select>
                </div>
                <div class="setting-item">
                    <label class="setting-label" data-lang="saturation">饱和度</label>
                    <select id="saturation" class="setting-select" onchange="applyCameraSettings()">
                        <option value="-2">-2</option>
                        <option value="-1">-1</option>
                        <option value="0" selected>0</option>
                        <option value="1">1</option>
                        <option value="2">2</option>
                    </select>
                </div>
            </div>
        </div>
    </div>
    
    <script>
        // 语言翻译对象 / Language translation object
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
        
        // 当前语言 / Current language
        let currentLanguage = 'en';
        
        // 当前主题 / Current theme
        let currentTheme = 'light';
        
        // 切换语言 / Change language
        function changeLanguage() {
            const langSelect = document.getElementById('language-select');
            currentLanguage = langSelect.value;
            
            // 更新所有带有data-lang属性的元素 / Update all elements with data-lang attribute
            const elements = document.querySelectorAll('[data-lang]');
            elements.forEach(element => {
                const key = element.getAttribute('data-lang');
                if (translations[currentLanguage][key]) {
                    element.textContent = translations[currentLanguage][key];
                }
            });
            
            // 保存语言偏好到localStorage / Save language preference to localStorage
            localStorage.setItem('preferredLanguage', currentLanguage);
        }
        
        // 切换主题 / Change theme
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
        
        // 初始化语言 / Initialize language
        function initializeLanguage() {
            // 从localStorage读取语言偏好，如果没有则使用默认语言英语 / Read language preference from localStorage, use default English if not set
            const savedLanguage = localStorage.getItem('preferredLanguage');
            if (savedLanguage && translations[savedLanguage]) {
                currentLanguage = savedLanguage;
            } else {
                currentLanguage = 'en';
            }
            
            // 设置语言选择器的值 / Set language selector value
            const langSelect = document.getElementById('language-select');
            if (langSelect) {
                langSelect.value = currentLanguage;
            }
            
            // 应用语言 / Apply language
            changeLanguage();
        }
        
        // 初始化主题 / Initialize theme
        function initializeTheme() {
            // 从localStorage读取主题偏好，如果没有则使用默认主题浅色 / Read theme preference from localStorage, use default light if not set
            const savedTheme = localStorage.getItem('preferredTheme');
            if (savedTheme && (savedTheme === 'light' || savedTheme === 'dark')) {
                currentTheme = savedTheme;
            } else {
                currentTheme = 'light';
            }
            
            // 设置主题选择器的值 / Set theme selector value
            const themeSelect = document.getElementById('theme-select');
            if (themeSelect) {
                themeSelect.value = currentTheme;
            }
            
            // 应用主题 / Apply theme
            changeTheme();
        }
        
        // 页面加载时初始化语言和主题 / Initialize language and theme on page load
        window.addEventListener('load', function() {
            initializeLanguage();
            initializeTheme();
        });
        
        // 应用摄像头设置 / Apply camera settings
        function applyCameraSettings() {
            const framesize = document.getElementById('framesize').value;
            const quality = document.getElementById('quality').value;
            const brightness = document.getElementById('brightness').value;
            const contrast = document.getElementById('contrast').value;
            const saturation = document.getElementById('saturation').value;
            
            // 应用分辨率设置 / Apply resolution setting
            fetch('/control?var=framesize&val=' + framesize)
            .then(response => {
                if (!response.ok) {
                    throw new Error('设置分辨率失败');
                }
                console.log('分辨率设置成功:', framesize);
            })
            .catch(error => {
                console.error('设置分辨率失败:', error);
                alert('设置分辨率失败！');
            });
            
            // 应用图像质量设置 / Apply image quality setting
            fetch('/control?var=quality&val=' + quality)
            .then(response => {
                if (!response.ok) {
                    throw new Error('设置图像质量失败');
                }
                console.log('图像质量设置成功:', quality);
            })
            .catch(error => {
                console.error('设置图像质量失败:', error);
                alert('设置图像质量失败！');
            });
            
            // 应用亮度设置 / Apply brightness setting
            fetch('/control?var=brightness&val=' + brightness)
            .then(response => {
                if (!response.ok) {
                    throw new Error('设置亮度失败');
                }
                console.log('亮度设置成功:', brightness);
            })
            .catch(error => {
                console.error('设置亮度失败:', error);
                alert('设置亮度失败！');
            });
            
            // 应用对比度设置 / Apply contrast setting
            fetch('/control?var=contrast&val=' + contrast)
            .then(response => {
                if (!response.ok) {
                    throw new Error('设置对比度失败');
                }
                console.log('对比度设置成功:', contrast);
            })
            .catch(error => {
                console.error('设置对比度失败:', error);
                alert('设置对比度失败！');
            });
            
            // 应用饱和度设置 / Apply saturation setting
            fetch('/control?var=saturation&val=' + saturation)
            .then(response => {
                if (!response.ok) {
                    throw new Error('设置饱和度失败');
                }
                console.log('饱和度设置成功:', saturation);
            })
            .catch(error => {
                console.error('设置饱和度失败:', error);
                alert('设置饱和度失败！');
            });
            
            // 重新加载视频流以应用新设置 / Reload video stream to apply new settings
            setTimeout(() => {
                const streamImg = document.getElementById('stream');
                const currentSrc = streamImg.src;
                streamImg.src = '';
                setTimeout(() => {
                    streamImg.src = currentSrc;
                }, 100);
            }, 500);
        }
        
        // 拍照功能 / Photo capture function
        function capturePhoto() {
            // 使用GET请求，因为app_httpd.cpp中的capture_handler是GET方法 / Use GET request because capture_handler in app_httpd.cpp is GET method
            fetch('/capture')
            .then(response => {
                // 创建图片URL并在新窗口打开 / Create image URL and open in new window
                const blob = response.blob();
                return blob.then(blob => {
                    const url = URL.createObjectURL(blob);
                    window.open(url);
                });
            })
            .catch(error => {
                console.error('拍照失败:', error);
                alert('拍照失败！');
            });
        }
        
        // 在新标签页中打开视频流 / Open video stream in new tab
        function openStreamInNewTab() {
            const streamUrl = 'http://' + window.location.hostname + ':81/stream';
            console.log('在新标签页中打开视频流:', streamUrl);
            window.open(streamUrl, '_blank');
        }
        
        // 视频流重试机制 / Video stream retry mechanism
        let retryCount = 0;
        const maxRetries = 10;
        const retryInterval = 3000; // 3秒重试一次
        
        // 设置视频流URL / Set video stream URL
        function setStreamUrl() {
            const streamImg = document.getElementById('stream');
            const streamUrl = 'http://' + window.location.hostname + ':81/stream';
            streamImg.src = streamUrl;
            console.log('设置视频流URL:', streamUrl, '重试次数:', retryCount);
        }
        
        // 视频流加载失败处理 / Video stream load failure handling
        function handleStreamError() {
            const streamImg = document.getElementById('stream');
            console.error('视频流加载失败，准备重试...');
            
            retryCount++;
            if (retryCount <= maxRetries) {
                // 延迟重试 / Delay before retry
                setTimeout(() => {
                    setStreamUrl();
                }, retryInterval);
            } else {
                console.error('已达到最大重试次数，停止重试');
                alert('视频流加载失败，请刷新页面重试');
            }
        }
        
        // 视频流加载成功处理 / Video stream load success handling
        function handleStreamLoad() {
            console.log('视频流加载成功');
            retryCount = 0; // 重置重试计数
        }
        
        // 页面加载完成后启动 / Start after page load completes
        window.onload = function() {
            const streamImg = document.getElementById('stream');
            // 添加事件监听器 / Add event listeners
            streamImg.onerror = handleStreamError;
            streamImg.onload = handleStreamLoad;
            
            // 初始加载视频流 / Initial load video stream
            setStreamUrl();
            
            // 初始更新SD卡信息和运行时长 / Initial update SD card info and uptime
            updateSDInfo();
            
            // 每5秒更新一次SD卡信息 / Update SD card info every 5 seconds
            setInterval(updateSDInfo, 5000);
            
            // 每秒更新运行时长 / Update uptime every second
            setInterval(updateUptime, 1000);
        };
        
        // 更新运行时长（本地计时）/ Update uptime (local timer)
        let uptimeSeconds = 0;
        function updateUptime() {
            uptimeSeconds++;
            const hours = Math.floor(uptimeSeconds / 3600);
            const minutes = Math.floor((uptimeSeconds % 3600) / 60);
            const seconds = uptimeSeconds % 60;
            
            const uptime = document.getElementById('uptime');
            if (uptime) {
                uptime.textContent = 
                    String(hours).padStart(2, '0') + ':' + 
                    String(minutes).padStart(2, '0') + ':' + 
                    String(seconds).padStart(2, '0');
            }
        }
        
        // 视频流加载成功处理 / Video stream load success handling
        function handleStreamLoad() {
            console.log('视频流加载成功');
            retryCount = 0; // 重置重试计数
        }
        
        // 更新SD卡信息 / Update SD card information和运行时长
        function updateSDInfo() {
            fetch('/status')
            .then(response => {
                if (!response.ok) {
                    throw new Error('网络响应错误');
                }
                return response.json();
            })
            .then(data => {
                console.log('系统状态:', data);
                // 更新SD卡信息 / Update SD card information
                const totalSpace = document.getElementById('sd-total');
                const usedSpace = document.getElementById('sd-used');
                const freeSpace = document.getElementById('sd-free');
                
                if (totalSpace) totalSpace.textContent = data.sd_total || '--';
                if (usedSpace) usedSpace.textContent = data.sd_used || '--';
                if (freeSpace) freeSpace.textContent = data.sd_free || '--';
                
                // 初始化运行时长（从服务器获取初始值）
                if (data.uptime && uptimeSeconds === 0) {
                    const parts = data.uptime.split(':');
                    if (parts.length === 3) {
                        uptimeSeconds = parseInt(parts[0]) * 3600 + parseInt(parts[1]) * 60 + parseInt(parts[2]);
                    }
                }
            })
            .catch(error => {
                console.error('获取系统状态失败:', error);
                // 显示错误信息
                const totalSpace = document.getElementById('sd-total');
                const usedSpace = document.getElementById('sd-used');
                const freeSpace = document.getElementById('sd-free');
                
                if (totalSpace) totalSpace.textContent = '错误';
                if (usedSpace) usedSpace.textContent = '错误';
                if (freeSpace) freeSpace.textContent = '错误';
            });
        }
    </script>
</body>
</html>
)rawliteral";
