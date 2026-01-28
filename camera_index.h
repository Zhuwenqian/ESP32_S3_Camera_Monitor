/*
 * 文件名：camera_index.h
 * 
 * 文件用途：
 * 本文件包含Web界面的HTML代码，提供完整的监控控制界面
 * 
 * 功能模块：
 * 1. 视频显示区域：显示实时视频流
 * 2. 拍照控制区域：控制拍照功能
 * 3. SD卡信息区域：显示SD卡存储信息（GB单位）
 * 4. 运行时长区域：显示系统运行时长（HH:MM:SS格式）
 * 5. 摄像头设置区域：调节分辨率、图像质量、亮度、对比度、饱和度
 * 
 * 技术实现：
 * - 使用HTML5和CSS3构建界面
 * - 使用JavaScript实现AJAX请求
 * - 使用Fetch API与服务器通信
 * - 支持响应式设计
 * - 支持实时状态更新
 * 
 * 界面特点：
 * - 简洁美观的UI设计
 * - 实时视频流显示（MJPEG）
 * - 拍照功能
 * - SD卡空间实时更新（GB单位）
 * - 摄像头参数动态调节
 * - 视频流加载状态显示
 * 
 * 创建日期：2026-01-26
 * 最后更新：2026-01-28
 * 最新更新：
 * 1. 添加摄像头设置界面（分辨率、图像质量、亮度、对比度、饱和度）
 * 2. 修改SD卡信息显示单位为GB
 * 3. 修复视频流加载状态显示问题
 * 4. 添加摄像头参数动态调节功能
 * 5. 移除时钟频率动态调整功能（因为需要重新初始化摄像头，会导致系统不稳定）
 * 6. 添加系统运行时长显示功能（HH:MM:SS格式，本地计时每秒更新）
 * 7. 实现运行时长实时更新（每1秒本地增加1秒）
 * 8. 页面加载时从服务器获取初始运行时长
 * 9. 移除视频录制控制界面（开始/停止按钮）
 * 10. 移除录制状态指示器和相关JavaScript函数
 * 11. 视频录制改为系统启动时自动开始，完全自动化
 * 12. 修改照片和视频文件名为时间戳格式（YYYYMMDDHHMM）
 * 13. 实现视频自动分段录制功能（2分钟一段）
 * 14. 系统启动时自动开始录制
 */

// Web界面HTML代码
const char index_ov2640_html[] = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-S3监控控制台</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: Arial, sans-serif;
            background-color: #f5f5f5;
            padding: 20px;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.1);
        }
        
        h1 {
            text-align: center;
            color: #333;
            margin-bottom: 30px;
        }
        
        .control-section {
            margin-bottom: 30px;
            padding: 20px;
            background-color: #f9f9f9;
            border-radius: 5px;
        }
        
        .section-title {
            font-size: 18px;
            font-weight: bold;
            color: #555;
            margin-bottom: 15px;
        }
        
        .video-container {
            text-align: center;
        }
        
        #stream {
            max-width: 100%;
            height: auto;
            border: 2px solid #ddd;
            border-radius: 5px;
        }
        
        .control-buttons {
            display: flex;
            gap: 10px;
            margin-bottom: 15px;
            flex-wrap: wrap;
        }
        
        .btn-primary {
            background-color: #007bff;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
        }
        
        .btn-primary:hover {
            background-color: #0056b3;
        }
        
        .btn-success {
            background-color: #28a745;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
        }
        
        .btn-success:hover {
            background-color: #218838;
        }
        
        .btn-danger {
            background-color: #dc3545;
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
        }
        
        .btn-danger:hover {
            background-color: #c82333;
        }
        
        .btn-warning {
            background-color: #ffc107;
            color: #212529;
            border: none;
            padding: 10px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 14px;
        }
        
        .btn-warning:hover {
            background-color: #e0a800;
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
            color: #555;
        }
        
        .info-display {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 15px;
        }
        
        .info-item {
            padding: 15px;
            background-color: white;
            border-radius: 5px;
            border: 1px solid #ddd;
        }
        
        .info-label {
            font-size: 14px;
            color: #666;
            margin-bottom: 5px;
        }
        
        .info-value {
            font-size: 16px;
            font-weight: bold;
            color: #333;
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
            background-color: white;
            border-radius: 5px;
            border: 1px solid #ddd;
            text-align: center;
        }
        
        .sd-info-label {
            font-size: 14px;
            color: #666;
            margin-bottom: 5px;
        }
        
        .sd-info-value {
            font-size: 20px;
            font-weight: bold;
            color: #333;
        }
        
        .sd-info-unit {
            font-size: 14px;
            color: #999;
            margin-left: 5px;
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
            color: #666;
            margin-bottom: 5px;
        }
        
        .setting-select {
            padding: 8px;
            border: 1px solid #ddd;
            border-radius: 5px;
            font-size: 14px;
            background-color: white;
            cursor: pointer;
        }
        
        .setting-select:hover {
            border-color: #007bff;
        }
        
        .setting-select:focus {
            outline: none;
            border-color: #007bff;
            box-shadow: 0 0 0 2px rgba(0, 123, 255, 0.25);
        }
        
        .uptime-info {
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
            background-color: white;
            border-radius: 5px;
            border: 1px solid #ddd;
        }
        
        .uptime-value {
            font-size: 24px;
            font-weight: bold;
            color: #333;
        }
        
        .uptime-unit {
            font-size: 14px;
            color: #666;
            margin-left: 10px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32-S3监控控制台</h1>
        
        <!-- 视频显示区域 -->
        <div class="control-section">
            <div class="section-title">实时视频</div>
            <div class="video-container">
                <img id="stream" src="" alt="视频流" style="display:none;" onload="this.style.display='block'; document.getElementById('loading').style.display='none';">
                <div id="loading" style="text-align:center; padding:20px; color:#666;">正在加载视频流...</div>
            </div>
            <div style="margin-top: 15px; text-align: center;">
                <button class="btn-primary" onclick="capturePhoto()">拍照</button>
                <button class="btn-success" onclick="openStreamInNewTab()">在新标签页中打开视频流</button>
            </div>
        </div>
        
        <!-- SD卡信息区域 -->
        <div class="control-section">
            <div class="section-title">SD卡存储信息</div>
            <div class="sd-info">
                <div class="sd-info-item">
                    <div class="sd-info-label">总空间</div>
                    <div class="sd-info-value">
                        <span id="sd-total">--</span>
                        <span class="sd-info-unit">GB</span>
                    </div>
                </div>
                <div class="sd-info-item">
                    <div class="sd-info-label">已用空间</div>
                    <div class="sd-info-value">
                        <span id="sd-used">--</span>
                        <span class="sd-info-unit">GB</span>
                    </div>
                </div>
                <div class="sd-info-item">
                    <div class="sd-info-label">剩余空间</div>
                    <div class="sd-info-value">
                        <span id="sd-free">--</span>
                        <span class="sd-info-unit">GB</span>
                    </div>
                </div>
            </div>
        </div>
        
        <!-- 运行时长区域 -->
        <div class="control-section">
            <div class="section-title">系统运行时长</div>
            <div class="uptime-info">
                <div class="uptime-value">
                    <span id="uptime">--:--:--</span>
                    <span class="uptime-unit">运行时间</span>
                </div>
            </div>
        </div>
        
        <!-- 摄像头设置区域 -->
        <div class="control-section">
            <div class="section-title">摄像头设置</div>
            <div class="camera-settings">
                <div class="setting-item">
                    <label class="setting-label">分辨率</label>
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
                    <label class="setting-label">图像质量</label>
                    <select id="quality" class="setting-select" onchange="applyCameraSettings()">
                        <option value="63">低质量 (63)</option>
                        <option value="31">中等质量 (31)</option>
                        <option value="10" selected>高质量 (10)</option>
                        <option value="5">极高质量 (5)</option>
                    </select>
                </div>
                <div class="setting-item">
                    <label class="setting-label">亮度</label>
                    <select id="brightness" class="setting-select" onchange="applyCameraSettings()">
                        <option value="-2">-2</option>
                        <option value="-1">-1</option>
                        <option value="0" selected>0</option>
                        <option value="1">1</option>
                        <option value="2">2</option>
                    </select>
                </div>
                <div class="setting-item">
                    <label class="setting-label">对比度</label>
                    <select id="contrast" class="setting-select" onchange="applyCameraSettings()">
                        <option value="-2">-2</option>
                        <option value="-1">-1</option>
                        <option value="0" selected>0</option>
                        <option value="1">1</option>
                        <option value="2">2</option>
                    </select>
                </div>
                <div class="setting-item">
                    <label class="setting-label">饱和度</label>
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
        // 应用摄像头设置
        function applyCameraSettings() {
            const framesize = document.getElementById('framesize').value;
            const quality = document.getElementById('quality').value;
            const brightness = document.getElementById('brightness').value;
            const contrast = document.getElementById('contrast').value;
            const saturation = document.getElementById('saturation').value;
            
            // 应用分辨率设置
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
            
            // 应用图像质量设置
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
            
            // 应用亮度设置
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
            
            // 应用对比度设置
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
            
            // 应用饱和度设置
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
            
            // 重新加载视频流以应用新设置
            setTimeout(() => {
                const streamImg = document.getElementById('stream');
                const currentSrc = streamImg.src;
                streamImg.src = '';
                setTimeout(() => {
                    streamImg.src = currentSrc;
                }, 100);
            }, 500);
        }
        
        // 拍照功能
        function capturePhoto() {
            // 使用GET请求，因为app_httpd.cpp中的capture_handler是GET方法
            fetch('/capture')
            .then(response => {
                // 创建图片URL并在新窗口打开
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
        
        // 在新标签页中打开视频流
        function openStreamInNewTab() {
            const streamUrl = 'http://' + window.location.hostname + ':81/stream';
            console.log('在新标签页中打开视频流:', streamUrl);
            window.open(streamUrl, '_blank');
        }
        
        // 视频流重试机制
        let retryCount = 0;
        const maxRetries = 10;
        const retryInterval = 3000; // 3秒重试一次
        
        // 设置视频流URL
        function setStreamUrl() {
            const streamImg = document.getElementById('stream');
            const streamUrl = 'http://' + window.location.hostname + ':81/stream';
            streamImg.src = streamUrl;
            console.log('设置视频流URL:', streamUrl, '重试次数:', retryCount);
        }
        
        // 视频流加载失败处理
        function handleStreamError() {
            const streamImg = document.getElementById('stream');
            console.error('视频流加载失败，准备重试...');
            
            retryCount++;
            if (retryCount <= maxRetries) {
                // 延迟重试
                setTimeout(() => {
                    setStreamUrl();
                }, retryInterval);
            } else {
                console.error('已达到最大重试次数，停止重试');
                alert('视频流加载失败，请刷新页面重试');
            }
        }
        
        // 视频流加载成功处理
        function handleStreamLoad() {
            console.log('视频流加载成功');
            retryCount = 0; // 重置重试计数
        }
        
        // 页面加载完成后启动
        window.onload = function() {
            const streamImg = document.getElementById('stream');
            // 添加事件监听器
            streamImg.onerror = handleStreamError;
            streamImg.onload = handleStreamLoad;
            
            // 初始加载视频流
            setStreamUrl();
            
            // 初始更新SD卡信息和运行时长
            updateSDInfo();
            
            // 每5秒更新一次SD卡信息
            setInterval(updateSDInfo, 5000);
            
            // 每秒更新运行时长
            setInterval(updateUptime, 1000);
        };
        
        // 更新运行时长（本地计时）
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
        
        // 视频流加载成功处理
        function handleStreamLoad() {
            console.log('视频流加载成功');
            retryCount = 0; // 重置重试计数
        }
        
        // 更新SD卡信息和运行时长
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
                // 更新SD卡信息
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
