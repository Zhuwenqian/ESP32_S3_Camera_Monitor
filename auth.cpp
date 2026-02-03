/**********************************************************************
  文件名称 / Filename : auth.cpp
  文件用途 / File Purpose : HTTP Basic Authentication认证实现 / HTTP Basic Authentication Implementation
               本文件实现了基于HTTP Basic Authentication的认证功能
               This file implements HTTP Basic Authentication functionality
               主要功能包括 / Main Features:
               1. HTTP Basic Authentication验证 / HTTP Basic Authentication verification
               2. 会话管理（14天过期）/ Session management (14 days expiration)
               3. IP授权管理（5分钟过期）用于视频流/img标签 / IP authorization management (5 minutes expiration) for video stream/img tags
               4. Base64编码解码 / Base64 encoding and decoding
               5. 会话ID生成 / Session ID generation
               6. 客户端IP地址获取 / Client IP address retrieval
               7. 会话查找和创建 / Session finding and creation
               8. 过期会话清理 / Expired session cleanup
  作者 / Author : Zhu Wenqian
  修改日期 / Modification Date : 2026-01-30
  硬件平台 / Hardware Platform : ESP32S3-EYE开发板 / ESP32S3-EYE Development Board
  依赖库 / Dependencies : esp_http_server.h - HTTP服务器库 / HTTP Server Library
               esp_log.h - 日志库 / Logging Library
               esp_random.h - 随机数生成库 / Random Number Generation Library
               mbedtls/base64.h - Base64编码库 / Base64 Encoding Library
               lwip/sockets.h - Socket库 / Socket Library
  使用说明 / Usage Instructions : 1. 调用auth_init()初始化认证模块 / Call auth_init() to initialize authentication module
               2. 在HTTP请求处理函数中调用auth_verify()验证认证 / Call auth_verify() in HTTP request handler to verify authentication
               3. 认证失败时调用auth_send_401()发送401响应 / Call auth_send_401() to send 401 response when authentication fails
               4. 定期调用auth_cleanup_expired_sessions()清理过期会话 / Call auth_cleanup_expired_sessions() periodically to clean up expired sessions
               5. 定期调用auth_cleanup_expired_ip_auth()清理过期IP授权 / Call auth_cleanup_expired_ip_auth() periodically to clean up expired IP authorizations
  参数调整 / Parameter Adjustment : MAX_SESSIONS - 最大会话数量（默认20）/ Maximum number of sessions (default 20)
               SESSION_EXPIRE_TIME - 会话过期时间（默认14天）/ Session expiration time (default 14 days)
               IP_AUTH_EXPIRE_TIME - IP授权过期时间（14天）/ IP authorization expiration time (default 5 minutes)
               AUTH_USERNAME - 认证用户名（默认admin）/ Authentication username (default admin)
               AUTH_PASSWORD - 认证密码（默认password123）/ Authentication password (default password123)
               调整建议：根据实际需求调整会话数量和过期时间 / Adjustment suggestion: Adjust session count and expiration time based on actual needs
  最新更新 / Latest Updates : 1. 添加IP授权管理功能，解决视频流重复认证问题 / Added IP authorization management to solve video stream repeated authentication issue
               2. 添加会话管理功能 / Added session management
               3. 添加Base64编码解码功能 / Added Base64 encoding and decoding
               4. 添加会话ID生成功能 / Added session ID generation
               5. 添加客户端IP地址获取功能 / Added client IP address retrieval
               6. 添加过期会话清理功能 / Added expired session cleanup
  注意事项 / Important Notes : 会话14天后自动过期，需要重新登录 / Sessions expire automatically after 14 days, need to re-login
               IP授权5分钟后自动过期，用于视频流等/img标签请求 / IP authorization expires automatically after 5 minutes, used for video stream and other img tag requests
               会话与客户端IP绑定，防止会话劫持 / Sessions are bound to client IP to prevent session hijacking
               所有敏感接口都需要认证 / All sensitive interfaces require authentication
**********************************************************************/

#include "auth.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <esp_log.h>
#include <esp_random.h>
#include <mbedtls/base64.h>
#include <lwip/sockets.h>

static const char *TAG = "AUTH";

// 会话数组 / Session array
static auth_session_t sessions[MAX_SESSIONS];

// IP授权数组 / IP authorization array
static ip_auth_t ip_auth_list[MAX_SESSIONS];

// 初始化标志 / Initialization flag
static bool auth_initialized = false;

/**
 * @brief 获取客户端IP地址
 * @param req HTTP请求
 * @param client_ip IP地址缓冲区（至少16字节）
 * @return bool 成功返回true，失败返回false
 */
static bool get_client_ip(httpd_req_t *req, char *client_ip, size_t client_ip_len) {
    int sockfd = httpd_req_to_sockfd(req);
    if(sockfd < 0) {
        ESP_LOGE(TAG, "Failed to get socket fd");
        return false;
    }
    
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    if(getpeername(sockfd, (struct sockaddr *)&addr, &addr_len) != 0) {
        ESP_LOGE(TAG, "Failed to get peer name");
        return false;
    }
    
    const char *ip = inet_ntoa(addr.sin_addr);
    if(ip == NULL) {
        ESP_LOGE(TAG, "Failed to convert IP address");
        return false;
    }
    
    strncpy(client_ip, ip, client_ip_len - 1);
    client_ip[client_ip_len - 1] = '\0';
    return true;
}

/**
 * @brief Base64编码
 * @param src 源数据
 * @param src_len 源数据长度
 * @param dst 目标缓冲区
 * @param dst_len 目标缓冲区长度
 * @return int 编码后的数据长度，失败返回-1
 */
static int base64_encode(const unsigned char *src, size_t src_len, char *dst, size_t dst_len) {
    size_t out_len = 0;
    int ret = mbedtls_base64_encode((unsigned char *)dst, dst_len, &out_len, src, src_len);
    if(ret != 0) {
        ESP_LOGE(TAG, "Base64 encode failed: %d", ret);
        return -1;
    }
    return (int)out_len;
}

/**
 * @brief Base64解码
 * @param src 源数据
 * @param src_len 源数据长度
 * @param dst 目标缓冲区
 * @param dst_len 目标缓冲区长度
 * @return int 解码后的数据长度，失败返回-1
 */
static int base64_decode(const char *src, size_t src_len, unsigned char *dst, size_t dst_len) {
    size_t out_len = 0;
    int ret = mbedtls_base64_decode(dst, dst_len, &out_len, (const unsigned char *)src, src_len);
    if(ret != 0) {
        ESP_LOGE(TAG, "Base64 decode failed: %d", ret);
        return -1;
    }
    return (int)out_len;
}

/**
 * @brief 生成会话ID
 * @param session_id 会话ID缓冲区（至少33字节）
 * @return bool 成功返回true，失败返回false
 */
static bool generate_session_id(char *session_id) {
    uint8_t random_bytes[16];
    
    // 生成随机字节
    esp_fill_random(random_bytes, sizeof(random_bytes));
    
    // Base64编码 / Base64 encode
    int encoded_len = base64_encode(random_bytes, sizeof(random_bytes), session_id, 32);
    if(encoded_len < 0) {
        return false;
    }
    
    // 确保以null结尾
    session_id[encoded_len] = '\0';
    
    return true;
}

/**
 * @brief 查找会话
 * @param session_id 会话ID
 * @return auth_session_t* 会话指针，未找到返回NULL
 */
static auth_session_t *find_session(const char *session_id) {
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sessions[i].active && strcmp(sessions[i].session_id, session_id) == 0) {
            return &sessions[i];
        }
    }
    return NULL;
}

// 前向声明 / Forward declaration
static auth_session_t *create_session(const char *client_ip);

/**
 * @brief 查找或创建会话
 * @param client_ip 客户端IP地址
 * @return auth_session_t* 会话指针，失败返回NULL
 * @note 如果已有相同IP的会话，则复用；否则创建新会话
 */
static auth_session_t *find_or_create_session(const char *client_ip) {
    // 首先查找是否已有相同IP的会话
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sessions[i].active && strcmp(sessions[i].client_ip, client_ip) == 0) {
            // 更新最后访问时间
            uint32_t current_time = time(NULL);
            sessions[i].last_access_time = current_time;
            ESP_LOGI(TAG, "Reusing existing session: %s, IP: %s", sessions[i].session_id, client_ip);
            return &sessions[i];
        }
    }
    
    // 没有找到，创建新会话
    return create_session(client_ip);
}

/**
 * @brief 创建新会话
 * @param client_ip 客户端IP地址
 * @return auth_session_t* 会话指针，失败返回NULL
 */
static auth_session_t *create_session(const char *client_ip) {
    // 查找空闲会话槽
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(!sessions[i].active) {
            // 生成会话ID / Generate session ID
            if(!generate_session_id(sessions[i].session_id)) {
                ESP_LOGE(TAG, "Failed to generate session ID");
                return NULL;
            }
            
            // 设置客户端IP
            strncpy(sessions[i].client_ip, client_ip, sizeof(sessions[i].client_ip) - 1);
            sessions[i].client_ip[sizeof(sessions[i].client_ip) - 1] = '\0';
            
            // 设置时间
            uint32_t current_time = time(NULL);
            sessions[i].create_time = current_time;
            sessions[i].last_access_time = current_time;
            
            // 激活会话
            sessions[i].active = true;
            
            ESP_LOGI(TAG, "Session created: %s, IP: %s", sessions[i].session_id, sessions[i].client_ip);
            return &sessions[i];
        }
    }
    
    ESP_LOGE(TAG, "No free session slots");
    return NULL;
}

/**
 * @brief 删除会话
 * @param session 会话指针
 */
static void delete_session(auth_session_t *session) {
    if(session && session->active) {
        ESP_LOGI(TAG, "Session deleted: %s, IP: %s", session->session_id, session->client_ip);
        session->active = false;
        memset(session, 0, sizeof(auth_session_t));
    }
}

/**
 * @brief 初始化认证模块
 */
bool auth_init(void) {
    if(auth_initialized) {
        ESP_LOGW(TAG, "Auth already initialized");
        return true;
    }
    
    // 初始化会话数组 / Initialize session array
    memset(sessions, 0, sizeof(sessions));
    
    // 初始化IP授权数组 / Initialize IP authorization array
    memset(ip_auth_list, 0, sizeof(ip_auth_list));
    
    auth_initialized = true;
    ESP_LOGI(TAG, "Auth initialized with IP authorization support");
    return true;
}

/**
 * @brief 验证HTTP Basic Authentication
 */
auth_result_t auth_verify_basic(httpd_req_t *req) {
    char auth_header[256];
    size_t auth_header_len = httpd_req_get_hdr_value_str(req, "Authorization", auth_header, sizeof(auth_header));
    
    // 检查是否有Authorization头
    if(auth_header_len == ESP_ERR_NOT_FOUND) {
        ESP_LOGW(TAG, "No Authorization header");
        return AUTH_NO_CREDENTIALS;
    }
    
    // 检查是否为Basic Authentication
    if(strncmp(auth_header, "Basic ", 6) != 0) {
        ESP_LOGW(TAG, "Not Basic Authentication");
        return AUTH_INVALID_CREDENTIALS;
    }
    
    // 提取Base64编码的字符串
    char *base64_str = auth_header + 6;
    
    // Base64解码 / Base64 decode
    unsigned char decoded[128];
    int decoded_len = base64_decode(base64_str, strlen(base64_str), decoded, sizeof(decoded));
    if(decoded_len < 0) {
        ESP_LOGE(TAG, "Base64 decode failed");
        return AUTH_INVALID_CREDENTIALS;
    }
    
    // 确保以null结尾
    decoded[decoded_len] = '\0';
    
    // 解析用户名和密码
    char username[64], password[64];
    int parsed = sscanf((char *)decoded, "%63[^:]:%63s", username, password);
    if(parsed != 2) {
        ESP_LOGE(TAG, "Failed to parse credentials");
        return AUTH_INVALID_CREDENTIALS;
    }
    
    // 验证用户名和密码
    if(strcmp(username, AUTH_USERNAME) != 0 || strcmp(password, AUTH_PASSWORD) != 0) {
        ESP_LOGW(TAG, "Invalid credentials: %s", username);
        return AUTH_FAILED;
    }
    
    // 获取客户端IP
    char client_ip[16];
    if(!get_client_ip(req, client_ip, sizeof(client_ip))) {
        ESP_LOGE(TAG, "Failed to get client IP");
        return AUTH_FAILED;
    }
    
    // 查找或创建会话 / Find or create session（复用相同IP的会话）
    auth_session_t *session = find_or_create_session(client_ip);
    if(!session) {
        ESP_LOGE(TAG, "Failed to find or create session");
        return AUTH_FAILED;
    }
    
    // 设置会话Cookie（只在创建新会话时设置）
    char cookie_header[256];
    snprintf(cookie_header, sizeof(cookie_header), "session_id=%s; Max-Age=%d; Path=/; HttpOnly; SameSite=Lax", 
             session->session_id, SESSION_EXPIRE_TIME);
    httpd_resp_set_hdr(req, "Set-Cookie", cookie_header);
    
    // 添加IP授权，用于视频流等/img标签请求 / Add IP authorization for video stream and other img tag requests
    auth_add_ip_auth(client_ip);
    
    ESP_LOGI(TAG, "Basic authentication successful: %s, IP: %s, Session: %s", username, client_ip, session->session_id);
    ESP_LOGI(TAG, "Set-Cookie header: %s", cookie_header);
    return AUTH_SUCCESS;
}

/**
 * @brief 验证会话
 */
auth_result_t auth_verify_session(httpd_req_t *req) {
    char cookie_header[256];
    size_t cookie_header_len = httpd_req_get_hdr_value_str(req, "Cookie", cookie_header, sizeof(cookie_header));
    
    // 检查是否有Cookie头
    if(cookie_header_len == ESP_ERR_NOT_FOUND) {
        ESP_LOGW(TAG, "No Cookie header found");
        return AUTH_NO_CREDENTIALS;
    }
    
    ESP_LOGI(TAG, "Cookie header received: %s", cookie_header);
    
    // 解析Cookie，查找session_id
    char *session_id = strstr(cookie_header, "session_id=");
    if(!session_id) {
        ESP_LOGW(TAG, "No session_id in cookie");
        return AUTH_NO_CREDENTIALS;
    }
    
    // 提取session_id
    session_id += 11; // 跳过"session_id="
    char session_id_end[33];
    int i = 0;
    while(session_id[i] != '\0' && session_id[i] != ';' && session_id[i] != ' ' && i < 32) {
        session_id_end[i] = session_id[i];
        i++;
    }
    session_id_end[i] = '\0';
    
    ESP_LOGI(TAG, "Extracted session_id: %s", session_id_end);
    
    // 查找会话 / Find session
    auth_session_t *session = find_session(session_id_end);
    if(!session) {
        ESP_LOGW(TAG, "Session not found: %s", session_id_end);
        return AUTH_SESSION_INVALID;
    }
    
    // 检查会话是否过期
    uint32_t current_time = time(NULL);
    if(current_time - session->last_access_time > SESSION_EXPIRE_TIME) {
        ESP_LOGW(TAG, "Session expired: %s", session_id_end);
        delete_session(session);
        return AUTH_SESSION_EXPIRED;
    }
    
    // 更新最后访问时间
    session->last_access_time = current_time;
    
    // 检查客户端IP是否匹配
    char client_ip[16];
    if(!get_client_ip(req, client_ip, sizeof(client_ip))) {
        ESP_LOGE(TAG, "Failed to get client IP");
        return AUTH_SESSION_INVALID;
    }
    if(strcmp(session->client_ip, client_ip) != 0) {
        ESP_LOGW(TAG, "IP mismatch: %s != %s", session->client_ip, client_ip);
        return AUTH_SESSION_INVALID;
    }
    
    ESP_LOGD(TAG, "Session valid: %s, IP: %s", session_id_end, client_ip);
    return AUTH_SUCCESS;
}

/**
 * @brief 验证认证（Basic、会话或IP授权）
 */
auth_result_t auth_verify(httpd_req_t *req) {
    // 优先检查会话 / Check session first
    auth_result_t session_result = auth_verify_session(req);
    if(session_result == AUTH_SUCCESS) {
        return AUTH_SUCCESS;
    }
    
    // 会话无效，检查IP授权（用于视频流等/img标签请求）/ Check IP authorization (for video stream and other img tag requests)
    auth_result_t ip_result = auth_verify_ip(req);
    if(ip_result == AUTH_SUCCESS) {
        ESP_LOGD(TAG, "IP authorization verified successfully");
        return AUTH_SUCCESS;
    }
    
    // 会话和IP授权都无效，检查Basic Authentication
    auth_result_t basic_result = auth_verify_basic(req);
    if(basic_result == AUTH_SUCCESS) {
        return AUTH_SUCCESS;
    }
    
    // 都失败，返回最具体的错误 / Return the most specific error
    if(session_result != AUTH_NO_CREDENTIALS) {
        return session_result;
    }
    if(ip_result != AUTH_NO_CREDENTIALS) {
        return ip_result;
    }
    return basic_result;
}

/**
 * @brief 发送401未授权响应
 */
esp_err_t auth_send_401(httpd_req_t *req) {
    httpd_resp_set_status(req, "401 Unauthorized");
    httpd_resp_set_hdr(req, "WWW-Authenticate", "Basic realm=\"ESP32 Camera\"");
    httpd_resp_set_type(req, "text/html");
    
    const char *response = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset=\"UTF-8\">"
        "<title>401 Unauthorized</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f0f0f0; }"
        ".container { text-align: center; padding: 40px; background: white; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
        "h1 { color: #d9534f; }"
        "p { color: #666; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class=\"container\">"
        "<h1>401 Unauthorized</h1>"
        "<p>您需要登录才能访问此资源</p>"
        "</div>"
        "</body>"
        "</html>";
    
    return httpd_resp_send(req, response, strlen(response));
}

/**
 * @brief 清理过期会话
 */
uint32_t auth_cleanup_expired_sessions(void) {
    uint32_t cleaned_count = 0;
    uint32_t current_time = time(NULL);
    
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sessions[i].active) {
            if(current_time - sessions[i].last_access_time > SESSION_EXPIRE_TIME) {
                ESP_LOGI(TAG, "Cleaning up expired session: %s", sessions[i].session_id);
                delete_session(&sessions[i]);
                cleaned_count++;
            }
        }
    }
    
    if(cleaned_count > 0) {
        ESP_LOGI(TAG, "Cleaned up %u expired sessions", cleaned_count);
    }
    
    return cleaned_count;
}

/**
 * @brief 获取会话数量
 */
uint32_t auth_get_session_count(void) {
    uint32_t count = 0;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sessions[i].active) {
            count++;
        }
    }
    return count;
}

/**
 * @brief 清除所有会话
 */
uint32_t auth_clear_all_sessions(void) {
    uint32_t cleared_count = 0;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(sessions[i].active) {
            delete_session(&sessions[i]);
            cleared_count++;
        }
    }
    ESP_LOGI(TAG, "Cleared all sessions: %u", cleared_count);
    return cleared_count;
}

/**
 * @brief 查找IP授权
 * @param client_ip 客户端IP地址
 * @return ip_auth_t* IP授权指针，未找到返回NULL
 */
static ip_auth_t *find_ip_auth(const char *client_ip) {
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(ip_auth_list[i].active && strcmp(ip_auth_list[i].client_ip, client_ip) == 0) {
            return &ip_auth_list[i];
        }
    }
    return NULL;
}

/**
 * @brief 验证IP授权
 */
auth_result_t auth_verify_ip(httpd_req_t *req) {
    // 获取客户端IP
    char client_ip[16];
    if(!get_client_ip(req, client_ip, sizeof(client_ip))) {
        ESP_LOGE(TAG, "Failed to get client IP for IP auth verification");
        return AUTH_FAILED;
    }
    
    // 查找IP授权
    ip_auth_t *ip_auth = find_ip_auth(client_ip);
    if(!ip_auth) {
        ESP_LOGD(TAG, "No IP authorization found for: %s", client_ip);
        return AUTH_NO_CREDENTIALS;
    }
    
    // 检查IP授权是否过期
    uint32_t current_time = time(NULL);
    if(current_time - ip_auth->auth_time > IP_AUTH_EXPIRE_TIME) {
        ESP_LOGW(TAG, "IP authorization expired for: %s", client_ip);
        ip_auth->active = false;
        return AUTH_SESSION_EXPIRED;
    }
    
    // 更新最后访问时间
    ip_auth->last_access_time = current_time;
    
    ESP_LOGD(TAG, "IP authorization valid for: %s", client_ip);
    return AUTH_SUCCESS;
}

/**
 * @brief 添加IP授权
 */
bool auth_add_ip_auth(const char *client_ip) {
    // 首先检查是否已有该IP的授权
    ip_auth_t *existing = find_ip_auth(client_ip);
    if(existing) {
        // 更新现有授权
        uint32_t current_time = time(NULL);
        existing->auth_time = current_time;
        existing->last_access_time = current_time;
        ESP_LOGI(TAG, "Updated IP authorization for: %s", client_ip);
        return true;
    }
    
    // 查找空闲槽位
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(!ip_auth_list[i].active) {
            // 设置客户端IP
            strncpy(ip_auth_list[i].client_ip, client_ip, sizeof(ip_auth_list[i].client_ip) - 1);
            ip_auth_list[i].client_ip[sizeof(ip_auth_list[i].client_ip) - 1] = '\0';
            
            // 设置时间
            uint32_t current_time = time(NULL);
            ip_auth_list[i].auth_time = current_time;
            ip_auth_list[i].last_access_time = current_time;
            
            // 激活授权
            ip_auth_list[i].active = true;
            
            ESP_LOGI(TAG, "Added IP authorization for: %s", client_ip);
            return true;
        }
    }
    
    ESP_LOGE(TAG, "No free IP authorization slots");
    return false;
}

/**
 * @brief 清理过期IP授权
 */
uint32_t auth_cleanup_expired_ip_auth(void) {
    uint32_t cleaned_count = 0;
    uint32_t current_time = time(NULL);
    
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(ip_auth_list[i].active) {
            if(current_time - ip_auth_list[i].auth_time > IP_AUTH_EXPIRE_TIME) {
                ESP_LOGI(TAG, "Cleaning up expired IP authorization: %s", ip_auth_list[i].client_ip);
                ip_auth_list[i].active = false;
                cleaned_count++;
            }
        }
    }
    
    if(cleaned_count > 0) {
        ESP_LOGI(TAG, "Cleaned up %u expired IP authorizations", cleaned_count);
    }
    
    return cleaned_count;
}

/**
 * @brief 获取IP授权数量
 */
uint32_t auth_get_ip_auth_count(void) {
    uint32_t count = 0;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(ip_auth_list[i].active) {
            count++;
        }
    }
    return count;
}

/**
 * @brief 清除所有IP授权
 */
uint32_t auth_clear_all_ip_auth(void) {
    uint32_t cleared_count = 0;
    for(int i = 0; i < MAX_SESSIONS; i++) {
        if(ip_auth_list[i].active) {
            ip_auth_list[i].active = false;
            cleared_count++;
        }
    }
    ESP_LOGI(TAG, "Cleared all IP authorizations: %u", cleared_count);
    return cleared_count;
}
