#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "template.h"

// 簡易版 render_template: ファイルを読み込み、{{key}} を value に置換
void render_template(const char *filename, char *out, size_t out_size, const char *key, const char *value) {
    char file_buf[4096];
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        snprintf(out, out_size, "<h1>Template not found: %s</h1>", filename);
        return;
    }
    ssize_t len = read(fd, file_buf, sizeof(file_buf) - 1);
    file_buf[len] = '\0';
    close(fd);

    // プレースホルダーの作成 (例: {{get_data}})
    char placeholder[64];
    snprintf(placeholder, sizeof(placeholder), "{{%s}}", key);

    char *p = strstr(file_buf, placeholder);
    if (!p) {
        strncpy(out, file_buf, out_size);
        return;
    }

    // 文字列置換処理
    size_t before_len = p - file_buf;
    size_t value_len = strlen(value);
    
    // バッファオーバーフロー対策は簡易的に省略していますが、実用時は厳密にチェックしてください
    memcpy(out, file_buf, before_len);
    memcpy(out + before_len, value, value_len);
    strcpy(out + before_len + value_len, p + strlen(placeholder));
}

// 簡易版 url_for: 本来はルーティングテーブルを逆引きしますが、ここでは単純に返します
const char* url_for(const char *endpoint) {
    // 本格的にやるならRouterの登録情報から検索する
    return endpoint; 
}