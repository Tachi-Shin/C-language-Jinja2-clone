#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"

// URLデコード (%20 -> Space など)
void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a'-'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a'-'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

// body ("name=taro&age=20") から key に対応する値を取り出す
void get_form_data(const char *body, const char *key, char *out_value, int out_size) {
    char search_key[128];
    snprintf(search_key, sizeof(search_key), "%s=", key);

    char *p = strstr(body, search_key);
    if (!p) {
        out_value[0] = '\0';
        return;
    }

    p += strlen(search_key); // 値の開始位置
    char *end = strchr(p, '&'); // 次のパラメータまでの区切り

    int len;
    if (end) {
        len = end - p;
    } else {
        len = strlen(p);
    }

    if (len >= out_size) len = out_size - 1;

    char temp[1024];
    strncpy(temp, p, len);
    temp[len] = '\0';

    url_decode(out_value, temp);
}