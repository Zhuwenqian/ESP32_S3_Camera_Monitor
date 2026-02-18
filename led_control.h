/**********************************************************************
  文件名称 / Filename : led_control.h
  文件用途 / File Purpose : WS2812B LED控制头文件 / WS2812B LED Control Header File
               声明了WS2812B LED控制相关的函数原型和宏定义
               Declares function prototypes and macro definitions related to WS2812B LED control
  作者 / Author : ESP32-S3监控项目 / ESP32-S3 Monitoring Project
  修改日期 / Modification Date : 2026-02-04
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : Arduino.h - Arduino核心库 / Arduino Core Library
  使用说明 / Usage Instructions : 1. 包含本头文件 #include "led_control.h" / Include this header file
               2. 调用led_init()初始化LED / Call led_init() to initialize LED
               3. 使用led_set_status()设置LED状态 / Use led_set_status() to set LED status
  参数调整 / Parameter Adjustment : LED_PIN - WS2812B LED引脚（默认GPIO48）/ WS2812B LED pin (default GPIO48)
               LED_NUM - LED数量（默认1）/ Number of LEDs (default 1)
  最新更新 / Latest Updates : 1. 实现WS2812B LED控制功能 / Implemented WS2812B LED control function
               2. 添加LED状态指示逻辑 / Added LED status indication logic
               3. 添加慢闪、快闪、常亮等状态 / Added slow flash, fast flash, steady on states
**********************************************************************/

#ifndef __LED_CONTROL_H
#define __LED_CONTROL_H

#include "Arduino.h"

// WS2812B LED引脚定义 / WS2812B LED pin definition
#define LED_PIN 48  // ESP32-S3-EYE开发板上的WS2812B LED引脚 / WS2812B LED pin on ESP32-S3-EYE board

// LED数量 / Number of LEDs
#define LED_NUM 1

// LED颜色定义 / LED color definitions
#define LED_COLOR_RED    0xFF0000  // 红色 / Red
#define LED_COLOR_GREEN  0x00FF00  // 绿色 / Green
#define LED_COLOR_BLUE   0x0000FF  // 蓝色 / Blue
#define LED_COLOR_OFF    0x000000  // 关闭 / Off

// LED状态枚举 / LED status enumeration
typedef enum {
    LED_OFF,              // LED关闭 / LED off
    LED_INIT_SLOW_FLASH,   // 初始化慢闪（2秒周期）/ Initialization slow flash (2s cycle)
    LED_CAMERA_READY,      // 摄像头就绪（绿色常亮）/ Camera ready (green steady)
    LED_CAMERA_ERROR,      // 摄像头错误（红色慢闪）/ Camera error (red slow flash)
    LED_SD_ERROR,         // SD卡错误（红色快闪）/ SD card error (red fast flash)
    LED_PHOTO_FLASH        // 拍照闪烁（蓝色快闪）/ Photo flash (blue fast flash)
} LEDStatus;

/**
 * @brief LED初始化函数 / LED initialization function
 * @details 功能说明 / Function Description:
 *          1. 初始化WS2812B LED引脚 / Initialize WS2812B LED pin
 *          2. 设置LED为关闭状态 / Set LED to off state
 * @note 必须在使用其他LED函数之前调用 / Must be called before using other LED functions
 *       使用Freenove WS2812 Lib for ESP32库 / Uses Freenove WS2812 Lib for ESP32 library
 */
void led_init(void);

/**
 * @brief 设置LED状态 / Set LED status
 * @param status LED状态 / LED status
 * @details 功能说明 / Function Description:
 *          1. 根据状态设置LED颜色和闪烁模式 / Set LED color and flash mode based on status
 *          2. 初始化慢闪：蓝色，2秒周期（亮1秒，灭1秒）/ Initialization slow flash: blue, 2s cycle (on 1s, off 1s)
 *          3. 摄像头就绪：绿色常亮 / Camera ready: green steady on
 *          4. 摄像头错误：红色慢闪（2秒周期）/ Camera error: red slow flash (2s cycle)
 *          5. SD卡错误：红色快闪（1秒周期）/ SD card error: red fast flash (1s cycle)
 *          6. 拍照闪烁：蓝色快闪（0.4秒周期）/ Photo flash: blue fast flash (0.4s cycle)
 * @note 状态切换响应及时 / Status switching responds promptly
 */
void led_set_status(LEDStatus status);

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
void led_task(void *pvParameters);

/**
 * @brief 设置LED颜色 / Set LED color
 * @param color LED颜色（RGB格式）/ LED color (RGB format)
 * @details 功能说明 / Function Description:
 *          1. 设置WS2812B LED的颜色 / Set WS2812B LED color
 *          2. 使用RGB格式（0xRRGGBB）/ Uses RGB format (0xRRGGBB)
 * @note 颜色格式：0xRRGGBB / Color format: 0xRRGGBB
 */
void led_set_color(uint32_t color);

/**
 * @brief 拍照闪烁函数 / Photo flash function
 * @details 功能说明 / Function Description:
 *          1. 蓝灯快速闪烁一次 / Blue LED flashes once quickly
 *          2. 亮0.2秒，灭0.2秒 / On 0.2s, off 0.2s
 *          3. 回到常亮状态 / Return to steady on state
 * @note 拍照操作执行时调用 / Called when photo capture is executed
 */
void led_photo_flash(void);

#endif // __LED_CONTROL_H
