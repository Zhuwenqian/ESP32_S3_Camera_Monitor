/**********************************************************************
  文件名称 / Filename : auth.h
  文件用途 / File Purpose : HTTP Basic Authentication认证头文件 / HTTP Basic Authentication Header File
               本文件声明了认证模块的函数原型和宏定义
               This file declares function prototypes and macro definitions for the authentication module
               主要功能包括 / Main Features:
               1. 认证模块初始化 / Authentication module initialization
               2. HTTP Basic Authentication验证 / HTTP Basic Authentication verification
               3. 会话验证 / Session verification
               4. 认证验证（Basic或会话）/ Authentication verification (Basic or session)
               5. 401未授权响应发送 / 401 Unauthorized response sending
               6. 过期会话清理 / Expired session cleanup
               7. 会话数量获取 / Session count retrieval
               8. 所有会话清除 / All sessions clearing
  作者 / Author : Zhu Wenqian
  修改日期 / Modification Date : 2026-01-28
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : esp_http_server.h - HTTP服务器库 / HTTP Server Library
  使用说明 / Usage Instructions : 1. 包含本头文件 #include "auth.h" / Include this header file
               2. 调用auth_init()初始化认证模块 / Call auth_init() to initialize authentication module
               3. 在HTTP请求处理函数中调用auth_verify()验证认证 / Call auth_verify() in HTTP request handler to verify authentication
               4. 认证失败时调用auth_send_401()发送401响应 / Call auth_send_401() to send 401 response when authentication fails
               5. 定期调用auth_cleanup_expired_sessions()清理过期会话 / Call auth_cleanup_expired_sessions() periodically to clean up expired sessions
  参数调整 / Parameter Adjustment : MAX_SESSIONS - 最大会话数量（默认20）/ Maximum number of sessions (default 20)
               SESSION_EXPIRE_TIME - 会话过期时间（默认14天）/ Session expiration time (default 14 days)
               AUTH_USERNAME - 认证用户名（默认admin）/ Authentication username (default admin)
               AUTH_PASSWORD - 认证密码（默认password123）/ Authentication password (default password123)
               调整建议：根据实际需求调整会话数量和过期时间 / Adjustment suggestion: Adjust session count and expiration time based on actual needs
  最新更新 / Latest Updates : 1. 添加会话管理功能 / Added session management
               2. 添加Base64编码解码功能 / Added Base64 encoding and decoding
               3. 添加会话ID生成功能 / Added session ID generation
               4. 添加客户端IP地址获取功能 / Added client IP address retrieval
               5. 添加过期会话清理功能 / Added expired session cleanup
  注意事项 / Important Notes : 会话14天后自动过期，需要重新登录 / Sessions expire automatically after 14 days, need to re-login
               会话与客户端IP绑定，防止会话劫持 / Sessions are bound to client IP to prevent session hijacking
               所有敏感接口都需要认证 / All sensitive interfaces require authentication
**********************************************************************/

#ifndef AUTH_H
#define AUTH_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

// 最大会话数量 / Maximum number of sessions
#define MAX_SESSIONS 20

// 会话过期时间（14天，单位：秒）/ Session expiration time (14 days, unit: seconds)
#define SESSION_EXPIRE_TIME (14 * 24 * 60 * 60)

// IP授权过期时间（14天，单位：秒）用于视频流等/img标签请求 / IP authorization expiration time (14 days) for video stream and other img tag requests
#define IP_AUTH_EXPIRE_TIME (60 * 60 * 24 * 14)

// 用户名和密码 / Username and password
#define AUTH_USERNAME "admin"
#define AUTH_PASSWORD "password123"

// 认证结果 / Authentication result
typedef enum {
    AUTH_SUCCESS = 0,           // 认证成功 / Authentication successful
    AUTH_FAILED = -1,            // 认证失败 / Authentication failed
    AUTH_NO_CREDENTIALS = -2,    // 没有提供认证信息 / No credentials provided
    AUTH_INVALID_CREDENTIALS = -3,  // 认证信息无效 / Invalid credentials
    AUTH_SESSION_EXPIRED = -4,   // 会话过期 / Session expired
    AUTH_SESSION_INVALID = -5    // 会话无效 / Session invalid
} auth_result_t;

// 会话结构 / Session structure
typedef struct {
    char session_id[33];         // 会话ID（32字符+结束符）/ Session ID (32 chars + null terminator)
    char client_ip[16];          // 客户端IP地址 / Client IP address
    uint32_t create_time;        // 创建时间（秒）/ Creation time (seconds)
    uint32_t last_access_time;   // 最后访问时间（秒）/ Last access time (seconds)
    bool active;                 // 是否激活 / Is active
} auth_session_t;

// IP授权结构 / IP authorization structure
typedef struct {
    char client_ip[16];          // 客户端IP地址 / Client IP address
    uint32_t auth_time;          // 授权时间（秒）/ Authorization time (seconds)
    uint32_t last_access_time;   // 最后访问时间（秒）/ Last access time (seconds)
    bool active;                 // 是否激活 / Is active
} ip_auth_t;

/**
 * @brief 初始化认证模块 / Initialize authentication module
 * @return bool 成功返回true，失败返回false / Returns true on success, false on failure
 * @note 必须在使用其他认证函数之前调用 / Must be called before using other authentication functions
 */
bool auth_init(void);

/**
 * @brief 验证HTTP Basic Authentication / Verify HTTP Basic Authentication
 * @param req HTTP请求 / HTTP request
 * @return auth_result_t 认证结果 / Authentication result
 * @note 验证成功后会创建会话 / Creates session after successful verification
 */
auth_result_t auth_verify_basic(httpd_req_t *req);

/**
 * @brief 验证会话 / Verify session
 * @param req HTTP请求 / HTTP request
 * @return auth_result_t 认证结果 / Authentication result
 * @note 验证成功后会更新会话的最后访问时间 / Updates session last access time after successful verification
 */
auth_result_t auth_verify_session(httpd_req_t *req);

/**
 * @brief 验证认证（Basic或会话）/ Verify authentication (Basic or session)
 * @param req HTTP请求 / HTTP request
 * @return auth_result_t 认证结果 / Authentication result
 * @note 优先检查会话，会话无效则检查Basic Authentication / Checks session first, then Basic Authentication if session is invalid
 */
auth_result_t auth_verify(httpd_req_t *req);

/**
 * @brief 发送401未授权响应 / Send 401 Unauthorized response
 * @param req HTTP请求 / HTTP request
 * @return esp_err_t ESP_OK表示成功 / ESP_OK indicates success
 */
esp_err_t auth_send_401(httpd_req_t *req);

/**
 * @brief 清理过期会话 / Clean up expired sessions
 * @return uint32_t 清理的会话数量 / Number of sessions cleaned
 * @note 建议定期调用（如每小时一次）/ Recommended to call periodically (e.g., once per hour)
 */
uint32_t auth_cleanup_expired_sessions(void);

/**
 * @brief 获取会话数量 / Get session count
 * @return uint32_t 当前活跃的会话数量 / Current number of active sessions
 */
uint32_t auth_get_session_count(void);

/**
 * @brief 清除所有会话 / Clear all sessions
 * @return uint32_t 清除的会话数量 / Number of sessions cleared
 */
uint32_t auth_clear_all_sessions(void);

/**
 * @brief 验证IP授权 / Verify IP authorization
 * @param req HTTP请求 / HTTP request
 * @return auth_result_t 认证结果 / Authentication result
 * @note 检查该IP是否在授权时间内 / Checks if the IP is within authorization time
 */
auth_result_t auth_verify_ip(httpd_req_t *req);

/**
 * @brief 添加IP授权 / Add IP authorization
 * @param client_ip 客户端IP地址 / Client IP address
 * @return bool 成功返回true，失败返回false / Returns true on success, false on failure
 * @note 在Basic认证成功后调用 / Called after successful Basic authentication
 */
bool auth_add_ip_auth(const char *client_ip);

/**
 * @brief 清理过期IP授权 / Clean up expired IP authorizations
 * @return uint32_t 清理的授权数量 / Number of authorizations cleaned
 * @note 建议定期调用（如每分钟一次）/ Recommended to call periodically (e.g., once per minute)
 */
uint32_t auth_cleanup_expired_ip_auth(void);

/**
 * @brief 获取IP授权数量 / Get IP authorization count
 * @return uint32_t 当前活跃的IP授权数量 / Current number of active IP authorizations
 */
uint32_t auth_get_ip_auth_count(void);

/**
 * @brief 清除所有IP授权 / Clear all IP authorizations
 * @return uint32_t 清除的授权数量 / Number of authorizations cleared
 */
uint32_t auth_clear_all_ip_auth(void);

#ifdef __cplusplus
}
#endif

#endif // AUTH_H
