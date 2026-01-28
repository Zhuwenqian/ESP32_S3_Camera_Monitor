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

// 会话数组
static auth_session_t sessions[MAX_SESSIONS];

// 初始化标志
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
    
    // Base64编码
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

// 前向声明
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
            // 生成会话ID
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
    
    // 初始化会话数组
    memset(sessions, 0, sizeof(sessions));
    
    auth_initialized = true;
    ESP_LOGI(TAG, "Auth initialized");
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
    
    // Base64解码
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
    
    // 查找或创建会话（复用相同IP的会话）
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
    
    // 查找会话
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
 * @brief 验证认证（Basic或会话）
 */
auth_result_t auth_verify(httpd_req_t *req) {
    // 优先检查会话
    auth_result_t session_result = auth_verify_session(req);
    if(session_result == AUTH_SUCCESS) {
        return AUTH_SUCCESS;
    }
    
    // 会话无效，检查Basic Authentication
    auth_result_t basic_result = auth_verify_basic(req);
    if(basic_result == AUTH_SUCCESS) {
        return AUTH_SUCCESS;
    }
    
    // 都失败，返回会话结果（优先返回会话错误）
    return session_result != AUTH_NO_CREDENTIALS ? session_result : basic_result;
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
