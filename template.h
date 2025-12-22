#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <stddef.h>

void render_template(const char *filename, char *out, size_t out_size, const char *key, const char *value);
const char* url_for(const char *endpoint);

#endif