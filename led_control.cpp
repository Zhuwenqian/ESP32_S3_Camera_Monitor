/**********************************************************************
  文件名称 / Filename : led_control.cpp
  文件用途 / File Purpose : WS2812B LED控制实现文件 / WS2812B LED Control Implementation File
               本文件实现了WS2812B LED控制功能
               This file implements WS2812B LED control functionality
               主要功能包括 / Main Features:
               1. LED初始化 / LED initialization
               2. LED状态控制 / LED status control
               3. LED闪烁模式 / LED flash mode
               4. 拍照闪烁 / Photo flash
  作者 / Author : ESP32-S3监控项目 / ESP32-S3 Monitoring Project
  修改日期 / Modification Date : 2026-02-04
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : Freenove WS2812 Lib for ESP32 - WS2812B LED控制库 / WS2812B LED Control Library
  使用说明 / Usage Instructions : 1. 调用led_init()初始化LED / Call led_init() to initialize LED
               2. 使用led_set_status()设置LED状态 / Use led_set_status() to set LED status
               3. 使用led_photo_flash()执行拍照闪烁 / Use led_photo_flash() to execute photo flash
  参数调整 / Parameter Adjustment : LED_PIN - WS2812B LED引脚（默认GPIO48）/ WS2812B LED pin (default GPIO48)
               LED_NUM - LED数量（默认1）/ Number of LEDs (default 1)
  最新更新 / Latest Updates : 1. 实现WS2812B LED控制功能 / Implemented WS2812B LED control function
               2. 添加LED状态指示逻辑 / Added LED status indication logic
               3. 添加慢闪、快闪、常亮等状态 / Added slow flash, fast flash, steady on states
               4. 使用Freenove WS2812 Lib for ESP32库 / Uses Freenove WS2812 Lib for ESP32 library
**********************************************************************/

#include "led_control.h"
#include <Freenove_WS2812_Lib_for_ESP32.h>

// WS2812B LED对象 / WS2812B LED object
// 参数说明：LED数量，引脚号，RMT通道（0），LED类型（GRB）/ Parameters: LED count, pin number, RMT channel (0), LED type (GRB)
Freenove_ESP32_WS2812 LED(LED_NUM, LED_PIN, 0, TYPE_GRB);

// 当前LED状态 / Current LED status
static LEDStatus currentLEDStatus = LED_OFF;

// 拍照闪烁标志 / Photo flash flag
static bool photoFlashActive = false;

/**
 * @brief LED初始化函数 / LED initialization function
 * @details 功能说明 / Function Description:
 *          1. 初始化WS2812B LED / Initialize WS2812B LED
 *          2. 设置LED为关闭状态 / Set LED to off state
 *          3. 创建LED控制任务 / Create LED control task
 * @note 使用Freenove WS2812 Lib for ESP32库 / Uses Freenove WS2812 Lib for ESP32 library
 */
void led_init(void) {
    // 初始化WS2812B LED / Initialize WS2812B LED
    LED.begin();
    LED.setBrightness(50);  // 设置亮度为50% / Set brightness to 50%
    
    // 设置LED为关闭状态 / Set LED to off state
    led_set_color(LED_COLOR_OFF);
    
    Serial.println("LED initialized / LED已初始化");
    
    // 创建LED控制任务 / Create LED control task
    xTaskCreate(led_task, "led_task", 2048, NULL, 3, NULL);
}

/**
 * @brief 设置LED状态 / Set LED status
 * @param status LED状态 / LED status
 * @details 功能说明 / Function Description:
 *          1. 更新当前LED状态 / Update current LED status
 *          2. 根据状态设置LED颜色和闪烁模式 / Set LED color and flash mode based on status
 * @note 状态切换响应及时 / Status switching responds promptly
 */
void led_set_status(LEDStatus status) {
    currentLEDStatus = status;
    
    switch(status) {
        case LED_OFF:
            // LED关闭 / LED off
            led_set_color(LED_COLOR_OFF);
            break;
            
        case LED_INIT_SLOW_FLASH:
            // 初始化慢闪：蓝色，2秒周期（亮1秒，灭1秒）/ Initialization slow flash: blue, 2s cycle (on 1s, off 1s)
            Serial.println("LED status: Initialization slow flash / LED状态：初始化慢闪");
            break;
            
        case LED_CAMERA_READY:
            // 摄像头就绪：绿色常亮 / Camera ready: green steady on
            Serial.println("LED status: Camera ready / LED状态：摄像头就绪");
            led_set_color(LED_COLOR_GREEN);
            break;
            
        case LED_CAMERA_ERROR:
            // 摄像头错误：红色慢闪（2秒周期）/ Camera error: red slow flash (2s cycle)
            Serial.println("LED status: Camera error / LED状态：摄像头错误");
            break;
            
        case LED_SD_ERROR:
            // SD卡错误：红色快闪（1秒周期）/ SD card error: red fast flash (1s cycle)
            Serial.println("LED status: SD card error / LED状态：SD卡错误");
            break;
            
        case LED_PHOTO_FLASH:
            // 拍照闪烁：蓝色快闪（0.4秒周期）/ Photo flash: blue fast flash (0.4s cycle)
            Serial.println("LED status: Photo flash / LED状态：拍照闪烁");
            photoFlashActive = true;
            break;
            
        default:
            Serial.println("Unknown LED status / 未知的LED状态");
            break;
    }
}

/**
 * @brief LED任务 / LED task
 * @param pvParameters 任务参数 / Task parameter
 * @details 功能说明 / Function Description:
 *          1. 根据当前LED状态控制LED闪烁 / Control LED flashing based on current LED status
 *          2. 慢闪：亮1秒，灭1秒 / Slow flash: on 1s, off 1s
 *          3. 快闪：亮0.5秒，灭0.5秒 / Fast flash: on 0.5s, off 0.5s
 *          4. 拍照闪烁：亮0.2秒，灭0.2秒 / Photo flash: on 0.2s, off 0.2s
 * @note 在FreeRTOS任务中运行 / Runs in FreeRTOS task
 */
void led_task(void *pvParameters) {
    bool ledOn = false;
    unsigned long lastToggleTime = millis();
    unsigned long flashInterval = 1000;  // 默认1秒 / Default 1 second
    
    while(true) {
        unsigned long currentTime = millis();
        
        // 处理拍照闪烁 / Handle photo flash
        if(currentLEDStatus == LED_PHOTO_FLASH && photoFlashActive) {
            // 蓝色快闪：亮0.2秒，灭0.2秒 / Blue fast flash: on 0.2s, off 0.2s
            flashInterval = 200;
            
            if(currentTime - lastToggleTime >= flashInterval) {
                ledOn = !ledOn;
                led_set_color(ledOn ? LED_COLOR_BLUE : LED_COLOR_OFF);
                lastToggleTime = currentTime;
                
                // 拍照闪烁只执行一次 / Photo flash only executes once
                if(!ledOn) {
                    photoFlashActive = false;
                    // 回到摄像头就绪状态 / Return to camera ready status
                    led_set_status(LED_CAMERA_READY);
                }
            }
        }
        // 处理初始化慢闪 / Handle initialization slow flash
        else if(currentLEDStatus == LED_INIT_SLOW_FLASH) {
            // 蓝色慢闪：亮1秒，灭1秒 / Blue slow flash: on 1s, off 1s
            flashInterval = 1000;
            
            if(currentTime - lastToggleTime >= flashInterval) {
                ledOn = !ledOn;
                led_set_color(ledOn ? LED_COLOR_BLUE : LED_COLOR_OFF);
                lastToggleTime = currentTime;
            }
        }
        // 处理摄像头错误慢闪 / Handle camera error slow flash
        else if(currentLEDStatus == LED_CAMERA_ERROR) {
            // 红色慢闪：亮1秒，灭1秒 / Red slow flash: on 1s, off 1s
            flashInterval = 1000;
            
            if(currentTime - lastToggleTime >= flashInterval) {
                ledOn = !ledOn;
                led_set_color(ledOn ? LED_COLOR_RED : LED_COLOR_OFF);
                lastToggleTime = currentTime;
            }
        }
        // 处理SD卡错误快闪 / Handle SD card error fast flash
        else if(currentLEDStatus == LED_SD_ERROR) {
            // 红色快闪：亮0.5秒，灭0.5秒 / Red fast flash: on 0.5s, off 0.5s
            flashInterval = 500;
            
            if(currentTime - lastToggleTime >= flashInterval) {
                ledOn = !ledOn;
                led_set_color(ledOn ? LED_COLOR_RED : LED_COLOR_OFF);
                lastToggleTime = currentTime;
            }
        }
        // 其他状态（常亮或关闭）/ Other states (steady on or off)
        else {
            // 不需要闪烁，保持当前状态 / No flashing needed, maintain current state
            delay(100);
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));  // 50ms延迟 / 50ms delay
    }
}

/**
 * @brief 设置LED颜色 / Set LED color
 * @param color LED颜色（RGB格式）/ LED color (RGB format)
 * @details 功能说明 / Function Description:
 *          1. 设置WS2812B LED的颜色 / Set WS2812B LED color
 *          2. 使用RGB格式（0xRRGGBB）/ Uses RGB format (0xRRGGBB)
 * @note 颜色格式：0xRRGGBB / Color format: 0xRRGGBB
 */
void led_set_color(uint32_t color) {
    // 解析RGB颜色 / Parse RGB color
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    // 设置WS2812B LED颜色 / Set WS2812B LED color
    LED.setLedColorData(0, r, g, b);
    LED.show();
}

/**
 * @brief 拍照闪烁函数 / Photo flash function
 * @details 功能说明 / Function Description:
 *          1. 蓝灯快速闪烁一次 / Blue LED flashes once quickly
 *          2. 亮0.2秒，灭0.2秒 / On 0.2s, off 0.2s
 *          3. 回到常亮状态 / Return to steady on state
 * @note 拍照操作执行时调用 / Called when photo capture is executed
 */
void led_photo_flash(void) {
    led_set_status(LED_PHOTO_FLASH);
}
