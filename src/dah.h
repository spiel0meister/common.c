#ifndef DAH_H_
#define DAH_H_
#endif // DAH_H_

#include <stddef.h>
#include <stdint.h>

#ifndef DAH_MALLOC
#include <stdlib.h>
#define DAH_MALLOC malloc
#endif // DAH_MALLOC

#ifndef DAH_REALLOC
#include <stdlib.h>
#define DAH_REALLOC realloc
#endif // DAH_REALLOC

typedef struct {
    size_t count, capacity;
    uint8_t data[];
}DaHeader;

#define DAH_INIT_CAPACITY 16

#define dah_init(da) ((da) = (void*)dah__default_header(sizeof(Statement))->data)

#define dah_append(da, v) ((da) = dah__maybe_resize(da, 1, sizeof(*(da))), (da)[dah_getheader(da)->count++] = (v))
#define dah_append_many(da, vs, vcount) ((da) = dah__maybe_resize(da, vcount, sizeof(*(da))), memcpy((da) + dah_getheader(da)->count, vs, vcount * sizeof(*(da))), dah_getheader(da)->count += vcount)
#define dah_append_cstr(da, cstr) dah_append_many(da, cstr, strlen(cstr))
#define dah_remove_unordered(da, i) ((da)[i] = (da)[--dah_getheader(da)->count])
#define dah_remove_ordered(da, i) dah__remove_ordered(da, i, sizeof(*(da)))
#define dah_reset(da) (da_getheader(da)->count = 0)
#define dah_free(da) ((da) = dah__free(da))

#define dah_getlen(da) (dah_getheader(da)->count)

#define dah_for(da, counter) for (size_t counter = 0; counter < dah_getheader(da)->count; ++counter)
#define dah_foreach(da, Type, ptr) for (Type* ptr = da; ptr < da + dah_getheader(da)->count; ++ptr)

DaHeader* dah_getheader(void* da);

DaHeader* dah__default_header(size_t item_size);
void* dah__maybe_resize(void* da, size_t to_add, size_t item_size);
void dah__remove_ordered(void* da, size_t i, size_t item_size);
void* dah__free(void* da);

#ifdef DAH_IMPLEMENTATION
#include <string.h>

DaHeader* dah__default_header(size_t item_size) {
    DaHeader* header = DAH_MALLOC(sizeof(DaHeader) + item_size * DAH_INIT_CAPACITY);
    header->count = 0;
    header->capacity = DAH_INIT_CAPACITY;
    return header;
}

void* dah__maybe_resize(void* da, size_t to_add, size_t item_size) {
    if (da == NULL) {
        return dah__default_header(item_size)->data;
    }

    DaHeader* header = (DaHeader*)da - 1;
    
    if (header->count + to_add >= header->capacity) {
        if (header->capacity == 0) header->capacity = 2;
        while (header->count + to_add >= header->capacity) header->capacity *= 2;
        header = DAH_REALLOC(header, sizeof(DaHeader) + header->capacity * item_size);
    }

    return header->data;
}

DaHeader* dah_getheader(void* da) {
    return (DaHeader*)da - 1;
}

void dah__remove_ordered(void* da, size_t i, size_t item_size) {
    DaHeader* header = dah_getheader(da);

    memmove(header->data + i * item_size, header->data + (i + 1) * item_size, (header->count - i - 1) * item_size);
    header->count--;
}

void* dah__free(void* da) {
    free(dah_getheader(da));
    return NULL;
}

#endif
