#ifndef UTF8_H_
#define UTF8_H_
#include <stdint.h>
#include <stddef.h>

typedef uint8_t utf8;
typedef uint32_t utf8codepoint;

size_t utf8len(const utf8* utf8);
size_t codepoints_len(utf8codepoint* codepoints);

utf8codepoint utf8c_to_codepoint(const utf8* utf8c, size_t* codepoint_size_);
int64_t codepoint_getsize(int codepoint);

const utf8* codepoint_to_utf8c(int codepoint, size_t* codepoint_size_);

// NOTE: Allocates memory with UTF8_MALLOC
utf8codepoint* utf8_to_codepoints(const utf8* utf8);

// NOTE: Allocates memory with UTF8_MALLOC
// NOTE: Copies memory with UTF8_MEMCPY
utf8* codepoints_to_utf8(utf8codepoint* codepoints);

#endif // UTF8_H_

#ifdef UTF8_IMPLEMENTATION
#undef UTF8_IMPLEMENTATION

#ifndef UTF8_MALLOC
#include <stdlib.h>
#define UTF8_MALLOC malloc
#endif // UTF8_MALLOC

#ifndef UTF8_MEMCPY
#include <string.h>
#define UTF8_MEMCPY memcpy
#endif // UTF8_MEMCPY

#ifndef UTF8_MEMSET
#include <string.h>
#define UTF8_MEMSET memset
#endif // UTF8_MEMSET

#ifndef UTF8_ASSERT
#include <assert.h>
#define UTF8_ASSERT assert
#endif // UTF8_ASSERT

size_t utf8len(const utf8* utf8) {
    size_t len = 0;
    while (*utf8 != 0) {
        if ((utf8[0] & 0xF0) == 0xF0) utf8 += 4;
        else if ((utf8[0] & 0xF0) == 0xE0) utf8 += 3;
        else if ((utf8[0] & 0xE0) == 0xC0) utf8 += 2;
        else utf8 += 1;

        len++;
    }
    return len;
}

size_t codepoints_len(utf8codepoint* codepoints) {
    size_t len = 0;
    while (*codepoints != 0) {
        codepoints++;
        len++;
    }
    return len;
}

utf8codepoint utf8c_to_codepoint(const utf8* utf8c, size_t* codepoint_size_) {
    utf8codepoint codepoint = 0;
    size_t codepoint_size = 0;

    if ((utf8c[0] & 0xF8) == 0xF0) {
        codepoint_size = 4;
        codepoint = ((utf8c[0] & 0x07) << 6*3) | ((utf8c[1] & 0x3F) << 6*2) | ((utf8c[2] & 0x3F) << 6*1) | ((utf8c[3] & 0x3F) << 6*0);
    } else if ((utf8c[0] & 0xF0) == 0xE0) {
        codepoint_size = 3;
        codepoint = ((utf8c[0] & 0x0F) << 6*2) | ((utf8c[1] & 0x3F) << 6*1) | ((utf8c[2] & 0x3F) << 6*0);
    } else if ((utf8c[0] & 0xE0) == 0xC0) {
        codepoint_size = 2;
        codepoint = ((utf8c[0] & 0x1F) << 6*1) | ((utf8c[1] & 0x3F) << 6*0);
    } else {
        codepoint_size = 1;
        codepoint = utf8c[0];
    }

    if (codepoint_size_ != NULL) *codepoint_size_ = codepoint_size;
    return codepoint;
}

utf8codepoint* utf8_to_codepoints(const utf8* utf8) {
    size_t len = utf8len(utf8);

    utf8codepoint* codepoints = UTF8_MALLOC((len + 1) * sizeof(*codepoints));
    UTF8_MEMSET(codepoints, 0, (len + 1) * sizeof(*codepoints));

    size_t i = 0;
    while (*utf8 != 0) {
        size_t codepoint_size;
        utf8codepoint codepoint = utf8c_to_codepoint(utf8, &codepoint_size);
        codepoints[i++] = codepoint;
        utf8 += codepoint_size;
    }

    return codepoints;
}

int64_t codepoint_getsize(int codepoint) {
    if (codepoint > 0x10FFFF) return -1;
    else if (codepoint > 0xFFFF) return 4;
    else if (codepoint > 0x07FF) return 3;
    else if (codepoint > 0x007F) return 2;
    else return 1;
}

const utf8* codepoint_to_utf8c(int codepoint, size_t* codepoint_size_) {
static utf8 utf8[5] = {0, 0, 0, 0, 0};

    int64_t codepoint_size = codepoint_getsize(codepoint);
    if (codepoint_size == -1) return NULL;

    switch (codepoint_size) {
        case 1:
            utf8[0] = codepoint;
            break;
        case 2:
            utf8[0] = 0xC0 | ((codepoint >> 6*1) & 0x1F);
            utf8[1] = 0x80 | ((codepoint >> 6*0) & 0x3F);
            break;
        case 3:
            utf8[0] = 0xE0 | ((codepoint >> 6*2) & 0x0F);
            utf8[1] = 0x80 | ((codepoint >> 6*1) & 0x3F);
            utf8[2] = 0x80 | ((codepoint >> 6*0) & 0x3F);
            break;
        case 4:
            utf8[0] = 0xF0 | ((codepoint >> 6*3) & 0x07);
            utf8[1] = 0x80 | ((codepoint >> 6*2) & 0x3F);
            utf8[2] = 0x80 | ((codepoint >> 6*1) & 0x3F);
            utf8[3] = 0x80 | ((codepoint >> 6*0) & 0x3F);
            break;
        default:
            UTF8_ASSERT(0);
    }
    utf8[codepoint_size] = 0;

    if (codepoint_size_ != NULL) *codepoint_size_ = codepoint_size;
    return utf8;
}

utf8* codepoints_to_utf8(utf8codepoint* codepoints) {
    size_t len = 0;
    for (size_t i = 0; i < codepoints_len(codepoints); ++i) {
        len += codepoint_getsize(codepoints[i]);
    }

    utf8* out = UTF8_MALLOC((len + 1) * sizeof(*out));
    UTF8_MEMSET(out, 0, (len + 1) * sizeof(*out));

    size_t offset = 0;
    for (size_t i = 0; i < codepoints_len(codepoints); ++i) {
        size_t codepoint_size;
        const utf8* utf8c = codepoint_to_utf8c(codepoints[i], &codepoint_size);
        memcpy(out + offset, utf8c, codepoint_size);
        offset += codepoint_size;
    }

    out[len] = 0;
    return out;
}
#endif // UTF8_IMPLEMENTATION
