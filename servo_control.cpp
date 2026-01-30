/**********************************************************************
  Filename    : servo_control.cpp / 云台控制实现文件
  Description : SG-90 servo control implementation for pan-tilt camera mount
                SG-90舵机云台控制实现文件
                实现水平(Pan)和垂直(Tilt)两个舵机的控制
  Auther      : ESP32-S3 Monitoring Project
  Modification: 2026-01-30
  
  依赖库 / Dependencies:
  - ESP32Servo library (v1.1.0 or later)
    调用项目和版本 / Called project and version: ESP32Servo v1.1.0
    GitHub: https://github.com/madhephaestus/ESP32Servo
  
  许可证 / License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License (CC BY-NC-SA 3.0)
**********************************************************************/
#include "servo_control.h"
#include <Arduino.h>

// Servo objects / 舵机对象
Servo panServo;   // Horizontal servo / 水平舵机
Servo tiltServo;  // Vertical servo / 垂直舵机

// Current servo angles / 当前舵机角度
int currentPanAngle = SERVO_DEFAULT_PAN;
int currentTiltAngle = SERVO_DEFAULT_TILT;

// =================== / ===================
// Internal Helper Functions / 内部辅助函数
// =================== / ===================

/**
 * Smooth servo movement / 平滑移动舵机
 * 
 * 原理 / Principle:
 * 通过逐步改变角度实现平滑移动，避免舵机突然转动造成抖动
 * 每步移动SERVO_STEP_SIZE度，每步延迟SERVO_STEP_DELAY毫秒
 * 
 * 参数 / Parameters:
 * - servo: 舵机对象 / Servo object
 * - currentAngle: 当前角度 / Current angle
 * - targetAngle: 目标角度 / Target angle
 */
static void smoothMove(Servo &servo, int currentAngle, int targetAngle) {
    // 确定移动方向 / Determine movement direction
    int step = (targetAngle > currentAngle) ? SERVO_STEP_SIZE : -SERVO_STEP_SIZE;
    
    // 逐步移动 / Move step by step
    for (int angle = currentAngle; angle != targetAngle; ) {
        // 计算下一步角度 / Calculate next angle
        angle += step;
        
        // 检查是否超过目标角度 / Check if passed target angle
        if ((step > 0 && angle > targetAngle) || (step < 0 && angle < targetAngle)) {
            angle = targetAngle;
        }
        
        // 写入角度 / Write angle
        servo.write(angle);
        
        // 延迟以实现平滑效果 / Delay for smooth effect
        delay(SERVO_STEP_DELAY);
    }
}

/**
 * Constrain angle to valid range / 限制角度在有效范围内
 * 
 * 参数 / Parameters:
 * - angle: 输入角度 / Input angle
 * 
 * 返回值 / Return:
 * - 限制后的角度 (0-180度) / Constrained angle (0-180 degrees)
 */
static int constrainAngle(int angle) {
    if (angle < SERVO_MIN_ANGLE) return SERVO_MIN_ANGLE;
    if (angle > SERVO_MAX_ANGLE) return SERVO_MAX_ANGLE;
    return angle;
}

// =================== / ===================
// Public Functions / 公共函数
// =================== / ===================

/**
 * Initialize servo control module / 初始化云台控制模块
 */
bool servo_init(void) {
    // 设置PWM频率（ESP32Servo库会自动处理）
    // ESP32Servo库使用LEDC定时器，频率为50Hz（标准舵机频率）
    // ESP32Servo library uses LEDC timer with 50Hz frequency (standard servo frequency)
    
    // 分配定时器 / Allocate timers
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    
    // 附加舵机到引脚 / Attach servos to pins
    panServo.setPeriodHertz(50);  // 标准舵机频率 50Hz / Standard servo frequency 50Hz
    tiltServo.setPeriodHertz(50);
    
    bool panAttached = panServo.attach(PAN_SERVO_PIN, 500, 2400);   // 500-2400us pulse width / 脉冲宽度
    bool tiltAttached = tiltServo.attach(TILT_SERVO_PIN, 500, 2400);
    
    if (!panAttached || !tiltAttached) {
        Serial.println("[Servo] Failed to attach servos / 舵机附加失败");
        return false;
    }
    
    // 移动到默认位置 / Move to default position
    panServo.write(SERVO_DEFAULT_PAN);
    tiltServo.write(SERVO_DEFAULT_TILT);
    
    currentPanAngle = SERVO_DEFAULT_PAN;
    currentTiltAngle = SERVO_DEFAULT_TILT;
    
    Serial.printf("[Servo] Initialized - Pan: GPIO%d, Tilt: GPIO%d / 云台初始化完成 - 水平: GPIO%d, 垂直: GPIO%d\n", 
                  PAN_SERVO_PIN, TILT_SERVO_PIN, PAN_SERVO_PIN, TILT_SERVO_PIN);
    Serial.printf("[Servo] Default position - Pan: %d°, Tilt: %d° / 默认位置 - 水平: %d°, 垂直: %d°\n",
                  SERVO_DEFAULT_PAN, SERVO_DEFAULT_TILT, SERVO_DEFAULT_PAN, SERVO_DEFAULT_TILT);
    
    return true;
}

/**
 * Set pan servo angle / 设置水平舵机角度
 */
bool servo_setPan(int angle, bool smooth) {
    // 限制角度范围 / Constrain angle
    angle = constrainAngle(angle);
    
    if (smooth) {
        smoothMove(panServo, currentPanAngle, angle);
    } else {
        panServo.write(angle);
    }
    
    currentPanAngle = angle;
    
    Serial.printf("[Servo] Pan angle set to %d° / 水平角度设置为 %d°\n", angle, angle);
    return true;
}

/**
 * Set tilt servo angle / 设置垂直舵机角度
 */
bool servo_setTilt(int angle, bool smooth) {
    // 限制角度范围 / Constrain angle
    angle = constrainAngle(angle);
    
    if (smooth) {
        smoothMove(tiltServo, currentTiltAngle, angle);
    } else {
        tiltServo.write(angle);
    }
    
    currentTiltAngle = angle;
    
    Serial.printf("[Servo] Tilt angle set to %d° / 垂直角度设置为 %d°\n", angle, angle);
    return true;
}

/**
 * Set both servo angles / 同时设置两个舵机角度
 */
bool servo_setPosition(int panAngle, int tiltAngle, bool smooth) {
    // 限制角度范围 / Constrain angles
    panAngle = constrainAngle(panAngle);
    tiltAngle = constrainAngle(tiltAngle);
    
    if (smooth) {
        // 同时平滑移动两个舵机 / Smoothly move both servos simultaneously
        int panStep = (panAngle > currentPanAngle) ? SERVO_STEP_SIZE : -SERVO_STEP_SIZE;
        int tiltStep = (tiltAngle > currentTiltAngle) ? SERVO_STEP_SIZE : -SERVO_STEP_SIZE;
        
        int currentPan = currentPanAngle;
        int currentTilt = currentTiltAngle;
        
        bool panDone = false;
        bool tiltDone = false;
        
        while (!panDone || !tiltDone) {
            // 移动水平舵机 / Move pan servo
            if (!panDone) {
                currentPan += panStep;
                if ((panStep > 0 && currentPan > panAngle) || (panStep < 0 && currentPan < panAngle)) {
                    currentPan = panAngle;
                    panDone = true;
                }
                panServo.write(currentPan);
            }
            
            // 移动垂直舵机 / Move tilt servo
            if (!tiltDone) {
                currentTilt += tiltStep;
                if ((tiltStep > 0 && currentTilt > tiltAngle) || (tiltStep < 0 && currentTilt < tiltAngle)) {
                    currentTilt = tiltAngle;
                    tiltDone = true;
                }
                tiltServo.write(currentTilt);
            }
            
            delay(SERVO_STEP_DELAY);
        }
    } else {
        // 直接移动 / Direct movement
        panServo.write(panAngle);
        tiltServo.write(tiltAngle);
    }
    
    currentPanAngle = panAngle;
    currentTiltAngle = tiltAngle;
    
    Serial.printf("[Servo] Position set - Pan: %d°, Tilt: %d° / 位置设置 - 水平: %d°, 垂直: %d°\n",
                  panAngle, tiltAngle, panAngle, tiltAngle);
    return true;
}

/**
 * Get current pan angle / 获取当前水平角度
 */
int servo_getPanAngle(void) {
    return currentPanAngle;
}

/**
 * Get current tilt angle / 获取当前垂直角度
 */
int servo_getTiltAngle(void) {
    return currentTiltAngle;
}

/**
 * Reset servos to default position / 重置舵机到默认位置
 */
void servo_resetPosition(void) {
    servo_setPosition(SERVO_DEFAULT_PAN, SERVO_DEFAULT_TILT, true);
    Serial.println("[Servo] Reset to default position / 舵机重置到默认位置");
}

/**
 * Move servos by relative angle / 相对移动舵机
 */
bool servo_moveRelative(int panDelta, int tiltDelta) {
    int newPan = constrainAngle(currentPanAngle + panDelta);
    int newTilt = constrainAngle(currentTiltAngle + tiltDelta);
    
    return servo_setPosition(newPan, newTilt, true);
}

/**
 * Cleanup servo resources / 清理舵机资源
 */
void servo_cleanup(void) {
    panServo.detach();
    tiltServo.detach();
    Serial.println("[Servo] Servos detached / 舵机已分离");
}
