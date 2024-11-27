#ifndef UTF8_H_
#define UTF8_H_
#include <stddef.h>

size_t utf8len(const char* utf8);

int utf8c_to_codepoint(const char* utf8c, size_t* codepoint_size);
int* utf8_to_codepoints(const char* utf8);

#endif // UTF8_H_

#ifdef UTF8_IMPLEMENTATION
size_t utf8len(const char* utf8) {
    size_t len = 0;
    while (*utf8 != 0) {
        if ((*utf8 & 0xC0) != 0x80) len++;
        utf8++;
    }
    return len;
}

int utf8c_to_codepoint(const char* utf8c, size_t* codepoint_size) {
    int codepoint = 0;
    size_t codepoint_size_ = 0;

    if ((*utf8c & 0xF8) == 0xF0) { // 4-bytes
        if (((utf8c[1] & 0xC0) ^ 0x80) || ((utf8c[2] & 0xC0) ^ 0x80) || ((utf8c[3] & 0xC0) ^ 0x80)) return codepoint;
        codepoint = ((utf8c[0] & 0x07) << 6*3) | ((utf8c[1] & 0x3F) << 6*2) | ((utf8c[2] & 0x3F) << 6*1) | (utf8c[3] & 0x3F);
        codepoint_size_ = 4;
    } else if ((*utf8c & 0xF0) == 0xE0) { // 3-bytes
        if (((utf8c[1] & 0xC0) ^ 0x80) || ((utf8c[2] & 0xC0) ^ 0x80)) return codepoint;
        codepoint = ((utf8c[0] & 0x0F) << 6*2) | ((utf8c[1] & 0x3F) << 6*1) | (utf8c[2] & 0x3F);
        codepoint_size_ = 3;
    } else if ((*utf8c & 0xE0) == 0xC0) { // 2-bytes
        if ((utf8c[1] & 0xC0) ^ 0x80) return codepoint;
        codepoint = ((utf8c[0] & 0x1F) << 6*1) | (utf8c[1] & 0x3F);
        codepoint_size_ = 2;
    } else if ((*utf8c & 0x80) == 0) { // 1-byte
        codepoint = utf8c[0];
        codepoint_size_ = 1;
    }

    if (codepoint_size != NULL) *codepoint_size = codepoint_size_;
    return codepoint;
}

int* utf8_to_codepoints(const char* utf8) {
    size_t len = utf8len(utf8);
    int* codepoints = malloc(len * sizeof(*codepoints));

    for (size_t i = 0; i < len; ++i) {
        size_t codepoint_size = 0;
        int codepoint = utf8c_to_codepoint(utf8, &codepoint_size);
        codepoints[i] = codepoint;
        utf8 += codepoint_size;
    }

    return codepoints;
}

#endif // UTF8_IMPLEMENTATION
