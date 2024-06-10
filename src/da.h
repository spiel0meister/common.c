#ifndef DA_H
#define DA_H

#define DA_INIT_CAP 256

#define da_append(da, item) \
    if ((da)->count == (da)->capacity) { \
        (da)->capacity = (da)->capacity == 0? DA_INIT_CAP : (da)->capacity * 2; \
        (da)->items = realloc((da)->items, (da)->capacity * sizeof((da)->items[0])); \
        assert((da)->items != NULL); \
    } \
    (da)->items[(da)->count++] = (item)

#define da_append_many(da, items, items_size) do { \
        if ((da)->count + (items_size) > (da)->capacity) {\
            if ((da)->capacity == 0) {\
                (da)->capacity = DA_INIT_CAP; \
            } \
            while ((da)->count + (items_size) > (da)->capacity) {\
                (da)->capacity = (da)->capacity * 2; \
            } \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof((da)->items[0])); \
            assert((da)->items != NULL); \
        } \
        memcpy((da)->items + (da)->count, items, (items_size) * sizeof(*((da)->items))); \
        (da)->count += items_size; \
    } while (0)

#define da_get(da, i) (da)->items[i]
#define da_set(da, i, value) (da)->items[i] = value

#define da_first(da) (da)->items[0]
#define da_last(da) (da)->items[(da)->count - 1]

#define da_remove(da, i, n) \
    assert((i) + (n) <= (da)->count); \
    memmove((da)->items + (i), (da)->items + (i) + 1, (n) * sizeof((da)->items[0]))

#define da_free(da) \
    free((da)->items); \
    (da)->items = NULL; \
    (da)->count = 0; \
    (da)->capacity = 0

#define da_foreach(da, type, ptr) for (type* ptr = (da)->items; ptr != (da)->items + (da)->count; ptr++)

#endif // DA_H
