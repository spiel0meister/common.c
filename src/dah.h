#ifndef DAH_H_
#define DAH_H_
#endif // DAH_H_

#include <stddef.h>
#include <stdint.h>

#ifndef DAH_MALLOC
#define DAH_MALLOC malloc
#endif // DAH_MALLOC

#ifndef DAH_REALLOC
#define DAH_REALLOC realloc
#endif // DAH_REALLOC

typedef struct {
    size_t count, capacity;
    uint8_t data[];
}DaHeader;

#define DAH_INIT_CAPACITY 16

#define dah_append(da, v) ((da) = dah__maybe_resize(da, 1, sizeof(*(da))), (da)[dah__getheader(da)->count++] = (v))
#define dah_append_many(da, vs, vcount) ((da) = dah__maybe_resize(da, vcount, sizeof(*(da))), memcpy((da) + dah__getheader(da)->count, vs, vcount * sizeof(*(da))), dah__getheader(da)->count += vcount)
#define dah_remove_unordered(da, i) ((da)[i] = (da)[--dah__getheader(da)->count])
#define dah_remove_ordered(da, i) dah__remove_ordered(da, i, sizeof(*(da)))

#define da_free(da) ((da) = dah__free(da))

size_t dah_getlen(void* da);
void dah_setlen(void* da, size_t count);

DaHeader* dah__default_header(size_t item_size);
DaHeader* dah__default_header(size_t item_size);
void* dah__maybe_resize(void* da, size_t to_add, size_t item_size);
DaHeader* dah__getheader(void* da);
void dah__remove_ordered(void* da, size_t i, size_t item_size);
void* dah__free(void* da);

#ifdef DAH_IMPLEMENTATION

DaHeader* dah__default_header(size_t item_size) {
    return DAH_MALLOC(sizeof(DaHeader) + item_size * DA_INIT_CAPACITY);
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

DaHeader* dah__getheader(void* da) {
    return (DaHeader*)da - 1;
}

void dah__remove_ordered(void* da, size_t i, size_t item_size) {
    DaHeader* header = dah__getheader(da);

    memmove(header->data + i * item_size, header->data + (i + 1) * item_size, (header->count - i - 1) * item_size);
    header->count--;
}

size_t dah_getlen(void* da) {
    return dah__getheader(da)->count;
}

void dah_setlen(void* da, size_t count) {
    dah__getheader(da)->count = count;
}

void* dah__free(void* da) {
    free(dah__getheader(da));
    return NULL;
}

#endif
