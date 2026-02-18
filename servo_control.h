/**********************************************************************
  Filename    : servo_control.h / 云台控制头文件
  Description : SG-90 servo control header for pan-tilt camera mount
                SG-90舵机云台控制头文件，用于摄像头云台控制
                支持两个SG-90舵机：水平旋转(Pan)和垂直旋转(Tilt)
  Auther      : Zhu Wenqian
  Modification: 2026-01-30
  
  硬件连接 / Hardware Connection:
  - Pan Servo (水平舵机): GPIO 14
  - Tilt Servo (垂直舵机): GPIO 21
  - Power (电源): 5V (外部供电推荐，避免ESP32供电不足)
  - GND (地线): 共地连接
  
  依赖库 / Dependencies:
  - ESP32Servo library (v1.1.0 or later)
    GitHub: https://github.com/madhephaestus/ESP32Servo
  
  许可证 / License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)
**********************************************************************/
#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <ESP32Servo.h>

// =================== / ===================
// Servo Pin Configuration / 舵机引脚配置
// =================== / ===================
// 重要说明 / Important Note:
// ESP32-S3-CAM的摄像头使用了GPIO 4,5,6,7,8,9,10,11,12,13,15,16,17,18
// 舵机引脚必须选择摄像头未使用的引脚
// Camera uses GPIO 4,5,6,7,8,9,10,11,12,13,15,16,17,18
// Servo pins must use pins not used by camera
//
// 可用引脚 / Available pins: 1, 2, 3, 14, 19, 20, 21, 35-48
// 推荐引脚 / Recommended pins: 14, 21 (靠近GND和3.3V，方便接线)
// Recommended pins: 14, 21 (close to GND and 3.3V for easy wiring)
#define PAN_SERVO_PIN   14  // Horizontal servo pin / 水平舵机引脚 (左右旋转 / Left-Right rotation)
#define TILT_SERVO_PIN  21  // Vertical servo pin / 垂直舵机引脚 (上下旋转 / Up-Down rotation)

// =================== / ===================
// Servo Angle Limits / 舵机角度限制
// =================== / ===================
#define SERVO_MIN_ANGLE     0   // Minimum angle / 最小角度 (degrees / 度)
#define SERVO_MAX_ANGLE     180 // Maximum angle / 最大角度 (degrees / 度)
#define SERVO_DEFAULT_PAN   90  // Default pan angle / 默认水平角度 (center / 居中)
#define SERVO_DEFAULT_TILT  90  // Default tilt angle / 默认垂直角度 (center / 居中)

// =================== / ===================
// Servo Speed Control / 舵机速度控制
// =================== / ===================
#define SERVO_STEP_DELAY    15  // Delay between steps (ms) / 每步延迟 (毫秒)
                                // 值越大，舵机转动越慢越平滑 / Larger value = slower and smoother movement
                                // 建议范围：10-30ms / Recommended range: 10-30ms
#define SERVO_STEP_SIZE     2   // Angle change per step / 每步角度变化量
                                // 值越大，舵机转动越快 / Larger value = faster movement
                                // 建议范围：1-5度 / Recommended range: 1-5 degrees

// Servo objects / 舵机对象
extern Servo panServo;   // Horizontal servo / 水平舵机
extern Servo tiltServo;  // Vertical servo / 垂直舵机

// Current servo angles / 当前舵机角度
extern int currentPanAngle;
extern int currentTiltAngle;

// =================== / ===================
// Function Declarations / 函数声明
// =================== / ===================

/**
 * Initialize servo control module / 初始化云台控制模块
 * 
 * 功能 / Function:
 * - 初始化舵机引脚和PWM设置
 * - 将舵机移动到默认位置（居中）
 * 
 * 返回值 / Return:
 * - true: 初始化成功 / Initialization successful
 * - false: 初始化失败 / Initialization failed
 */
bool servo_init(void);

/**
 * Set pan servo angle / 设置水平舵机角度
 * 
 * 参数 / Parameters:
 * - angle: 目标角度 (0-180度) / Target angle (0-180 degrees)
 * - smooth: 是否平滑移动 / Whether to move smoothly
 *           true: 平滑移动（使用步进）/ Smooth movement (using steps)
 *           false: 直接移动到目标角度 / Direct movement to target angle
 * 
 * 返回值 / Return:
 * - true: 设置成功 / Setting successful
 * - false: 角度超出范围 / Angle out of range
 */
bool servo_setPan(int angle, bool smooth = true);

/**
 * Set tilt servo angle / 设置垂直舵机角度
 * 
 * 参数 / Parameters:
 * - angle: 目标角度 (0-180度) / Target angle (0-180 degrees)
 * - smooth: 是否平滑移动 / Whether to move smoothly
 *           true: 平滑移动（使用步进）/ Smooth movement (using steps)
 *           false: 直接移动到目标角度 / Direct movement to target angle
 * 
 * 返回值 / Return:
 * - true: 设置成功 / Setting successful
 * - false: 角度超出范围 / Angle out of range
 */
bool servo_setTilt(int angle, bool smooth = true);

/**
 * Set both servo angles / 同时设置两个舵机角度
 * 
 * 参数 / Parameters:
 * - panAngle: 水平角度 (0-180度) / Pan angle (0-180 degrees)
 * - tiltAngle: 垂直角度 (0-180度) / Tilt angle (0-180 degrees)
 * - smooth: 是否平滑移动 / Whether to move smoothly
 * 
 * 返回值 / Return:
 * - true: 设置成功 / Setting successful
 * - false: 角度超出范围 / Angle out of range
 */
bool servo_setPosition(int panAngle, int tiltAngle, bool smooth = true);

/**
 * Get current pan angle / 获取当前水平角度
 * 
 * 返回值 / Return:
 * - 当前水平角度 (0-180度) / Current pan angle (0-180 degrees)
 */
int servo_getPanAngle(void);

/**
 * Get current tilt angle / 获取当前垂直角度
 * 
 * 返回值 / Return:
 * - 当前垂直角度 (0-180度) / Current tilt angle (0-180 degrees)
 */
int servo_getTiltAngle(void);

/**
 * Reset servos to default position / 重置舵机到默认位置
 * 
 * 功能 / Function:
 * - 将两个舵机都移动到90度（居中位置）
 * - 使用平滑移动方式
 */
void servo_resetPosition(void);

/**
 * Move servos by relative angle / 相对移动舵机
 * 
 * 参数 / Parameters:
 * - panDelta: 水平角度变化量（可为负数）/ Pan angle delta (can be negative)
 * - tiltDelta: 垂直角度变化量（可为负数）/ Tilt angle delta (can be negative)
 * 
 * 返回值 / Return:
 * - true: 移动成功 / Movement successful
 * - false: 目标角度超出范围 / Target angle out of range
 */
bool servo_moveRelative(int panDelta, int tiltDelta);

/**
 * Cleanup servo resources / 清理舵机资源
 * 
 * 功能 / Function:
 * - 分离舵机对象
 * - 释放PWM资源
 */
void servo_cleanup(void);

#endif // SERVO_CONTROL_H
