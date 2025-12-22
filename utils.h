#ifndef UTILS_H
#define UTILS_H

void url_decode(char *dst, const char *src);
void get_form_data(const char *body, const char *key, char *out_value, int out_size);

#endif