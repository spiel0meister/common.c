#ifndef DA_H
#define DA_H

#define da_append(da, item) \
    if ((da)->size == (da)->capacity) { \
        (da)->capacity = (da)->capacity == 0 ? 2 : (da)->capacity * 2; \
        (da)->items = realloc((da)->items, sizeof((da)->items[0]) * (da)->capacity); \
    } \
    (da)->items[(da)->size++] = (item)

#define da_foreach(da, type, ptr) for (type* ptr = (da)->items; ptr != (da)->items + (da)->size; ptr++)

#endif // DA_H
