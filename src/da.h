#ifndef DA_H
#define DA_H

#define da_append(da, item) \
    if ((da)->size == (da)->capacity) { \
        (da)->capacity = (da)->capacity == 0? 2 : (da)->capacity * 2; \
        (da)->items = realloc((da)->items, (da)->capacity * sizeof((da)->items[0])); \
        assert((da)->items != NULL); \
    } \
    (da)->items[(da)->size++] = (item)

#define da_remove(da, i, n) \
    assert((i) + (n) <= (da)->size); \
    memmove((da)->items + (i), (da)->items + (i) + 1, (n) * sizeof((da)->items[0]))

#define da_foreach(da, type, ptr) for (type* ptr = (da)->items; ptr != (da)->items + (da)->size; ptr++)

#endif // DA_H
