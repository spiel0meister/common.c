#ifndef DA_H_
#define DA_H_
#include <stdint.h>

#ifndef DA_SIZE_TYPE
#include <stddef.h>
#define DA_SIZE_TYPE size_t
#endif // DA_INT_TYPE

#ifndef DA_SORT
#include <stdlib.h>
#define DA_SORT qsort
#endif

#ifndef DA_ASSERT
#include <assert.h>
#define DA_ASSERT(expr) assert(expr)
#endif // DA_ASSERT

#define DA(Type) Type* items; DA_SIZE_TYPE count; DA_SIZE_TYPE capacity

#define da_maybe_resize(da, to_add_count) do {\
    if ((da)->count + (to_add_count) > (da)->capacity) { \
        if ((da)->capacity == 0) ((da)->capacity) = 16; \
        while ((da)->count >= (da)->capacity) (da)->capacity *= 2; \
        (da)->items = realloc((da)->items, sizeof((da)->items[0]) * (da)->capacity); \
    } \
} while (0)

#define da_append(da, item) do {\
        da_maybe_resize(da, 1); \
        (da)->items[(da)->count++] = item; \
    } while (0)

#define da_append_many(da, items, item_count) do {\
        da_maybe_resize(da, item_count); \
        memcpy((da)->items + (da)->count, items, sizeof((da)->items[0]) * item_count); \
        (da)->count += item_count; \
    } while (0)

#define da_remove_unsorted(da, i) do { \
    (da)->items[i] = (da)->items[--(da)->count]; \
} while (0)

#define da_remove_sorted(da, index) do { \
    memmove((da)->items + index, (da)->items + index + 1, ((da)->count - index - 1) * sizeof((da)->items[0])); \
    (da)->count--; \
} while (0)

#define da_first(da) (DA_ASSERT((da)->count > 0), (da)->items[0])
#define da_last(da) (DA_ASSERT((da)->count > 0), (da)->items[(da)->count - 1])

#define da_pop(da) (DA_ASSERT((da)->count > 0), (da)->items[--(da)->count])
#define da_sort(da, compare_fn) DA_SORT((da)->items, (da)->count, sizeof((da)->items[0]), compare_fn)

#define da_free(da) do { \
        free((da)->items); \
        (da)->items = NULL; \
        (da)->count = 0; \
        (da)->capacity = 0; \
    } while (0)

#define da_foreach(da, Type, ptr) for (Type* ptr = (da)->items; ptr < (da)->items + (da)->count; ptr++)
#define da_for(da, it) for (size_t it = 0; it < (da)->count; it++)

#endif // DA_H_
