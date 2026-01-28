#ifndef AUTH_H
#define AUTH_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

// 最大会话数量
#define MAX_SESSIONS 20

// 会话过期时间（14天，单位：秒）
#define SESSION_EXPIRE_TIME (14 * 24 * 60 * 60)

// 用户名和密码
#define AUTH_USERNAME "admin"
#define AUTH_PASSWORD "password123"

// 认证结果
typedef enum {
    AUTH_SUCCESS = 0,           // 认证成功
    AUTH_FAILED = -1,            // 认证失败
    AUTH_NO_CREDENTIALS = -2,    // 没有提供认证信息
    AUTH_INVALID_CREDENTIALS = -3,  // 认证信息无效
    AUTH_SESSION_EXPIRED = -4,   // 会话过期
    AUTH_SESSION_INVALID = -5    // 会话无效
} auth_result_t;

// 会话结构
typedef struct {
    char session_id[33];         // 会话ID（32字符+结束符）
    char client_ip[16];          // 客户端IP地址
    uint32_t create_time;        // 创建时间（秒）
    uint32_t last_access_time;   // 最后访问时间（秒）
    bool active;                 // 是否激活
} auth_session_t;

/**
 * @brief 初始化认证模块
 * @return bool 成功返回true，失败返回false
 * @note 必须在使用其他认证函数之前调用
 */
bool auth_init(void);

/**
 * @brief 验证HTTP Basic Authentication
 * @param req HTTP请求
 * @return auth_result_t 认证结果
 * @note 验证成功后会创建会话
 */
auth_result_t auth_verify_basic(httpd_req_t *req);

/**
 * @brief 验证会话
 * @param req HTTP请求
 * @return auth_result_t 认证结果
 * @note 验证成功后会更新会话的最后访问时间
 */
auth_result_t auth_verify_session(httpd_req_t *req);

/**
 * @brief 验证认证（Basic或会话）
 * @param req HTTP请求
 * @return auth_result_t 认证结果
 * @note 优先检查会话，会话无效则检查Basic Authentication
 */
auth_result_t auth_verify(httpd_req_t *req);

/**
 * @brief 发送401未授权响应
 * @param req HTTP请求
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t auth_send_401(httpd_req_t *req);

/**
 * @brief 清理过期会话
 * @return uint32_t 清理的会话数量
 * @note 建议定期调用（如每小时一次）
 */
uint32_t auth_cleanup_expired_sessions(void);

/**
 * @brief 获取会话数量
 * @return uint32_t 当前活跃的会话数量
 */
uint32_t auth_get_session_count(void);

/**
 * @brief 清除所有会话
 * @return uint32_t 清除的会话数量
 */
uint32_t auth_clear_all_sessions(void);

#ifdef __cplusplus
}
#endif

#endif // AUTH_H
