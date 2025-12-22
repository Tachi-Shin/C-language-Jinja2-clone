#include <string.h>
#include <stdio.h>
#include "session.h"

// "Cookie: name=value; name2=val2" をパースする簡易実装
void parse_cookies(const char *raw_buffer, Request *req) {
    req->cookie_count = 0;
    const char *cookie_ptr = strstr(raw_buffer, "Cookie: ");
    if (!cookie_ptr) return;

    cookie_ptr += 8; // Skip "Cookie: "
    // ※ここでは非常に簡易的な解析にとどめます（本来はstrtokなどで分割）
    sscanf(cookie_ptr, "%[^=]=%[^;\r\n]", req->cookies[0].key, req->cookies[0].value);
    req->cookie_count = 1; 
}

void set_cookie(Response *res, const char *key, const char *value) {
    if (res->set_cookie_count < MAX_COOKIES) {
        int i = res->set_cookie_count;
        strncpy(res->set_cookies[i].key, key, 63);
        strncpy(res->set_cookies[i].value, value, 127);
        res->set_cookie_count++;
    }
}

void delete_cookie(Response *res, const char *key) {
    // Max-Age=0 を設定することでブラウザに削除を指示する
    if (res->set_cookie_count < MAX_COOKIES) {
        int i = res->set_cookie_count;
        snprintf(res->set_cookies[i].key, 63, "%s", key);
        // 値は空、Max-Age=0
        snprintf(res->set_cookies[i].value, 127, "; Max-Age=0"); 
        res->set_cookie_count++;
    }
}