#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

typedef struct {
    char* items;
    int size;
    int capacity;
}StringBuilder;

#define sbuilder_append(sbuilder, c) \
    if ((sbuilder)->size == (sbuilder)->capacity) { \
        (sbuilder)->capacity = (sbuilder)->capacity == 0 ? 2 : (sbuilder)->capacity * 2; \
        (sbuilder)->items = realloc((sbuilder)->items, sizeof((sbuilder)->items[0]) * (sbuilder)->capacity); \
    } \
    (sbuilder)->items[(sbuilder)->size++] = (c)

#define sbuilder_cstr_append(sbuilder, cstr) \
    while ((sbuilder)->size + strlen(cstr) >= (sbuilder)->capacity) { \
        (sbuilder)->capacity = (sbuilder)->capacity == 0 ? 2 : (sbuilder)->capacity * 2; \
    } \
    memcpy((sbuilder)->items + (sbuilder)->size, cstr, strlen(cstr)); \
    (sbuilder)->size += strlen(cstr)

#define sbuilder_cstr_append_many(sbuilder, first, ...) do {\
        char* cstrs[] = { first, __VA_ARGS__ }; \
        for (int i = 0; i < sizeof(cstrs) / sizeof(*cstrs); i++) { \
            sbuilder_cstr_append(sbuilder, cstrs[i]); \
        } \
    } while (0)

#endif // STRING_BUILDER_H

#ifdef STRING_BUILDER_IMPLENTATION
#endif // STRING_BUILDER_IMPLENTATION
