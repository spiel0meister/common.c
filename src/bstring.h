#ifndef BSTRING_H
#define BSTRING_H
#include <stddef.h>

#ifndef STRING_MALLOC
    #define STRING_MALLOC malloc
#endif

typedef struct {
    char* str;
    size_t len;
}String;

String string_from_cstr(char* cstr);
// Allocates memory to create the cstring. Define STRING_MALLOC macro if you use a different allocator (ex. Arena)
char* string_to_cstr(String string);

int string_cmp(String str1, String str2);
int string_empty(String str1);

#ifdef BSTRING_IMPLEMENTATION
String string_from_cstr(char* cstr) {
    return (String) {
        cstr,
        strlen(cstr)
    };
}

char* string_to_cstr(String string) {
    char* cstr = STRING_MALLOC(string.len + 1);
    memcpy(cstr, string.str, string.len);
    cstr[string.len] = 0;
    return cstr;
}

int string_cmp(String str1, String str2) {
    if (str1.len != str2.len) return 0;

    for (size_t i = 0; i < str1.len; ++i) {
        if (str1.str[i] != str2.str[i]) return 0;
    }

    return 1;
}

int string_empty(String str1) {
    return str1.len == 0 || str1.str == NULL || str1.str[0] == 0;
}
#endif // BSTRING_IMPLEMENTATION

#endif // BSTRING_H
