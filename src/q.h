#ifndef QUEUE_H
#define QUEUE_H
#include <stddef.h>
#include <stdint.h>

#ifndef Q_ASSERT
#include <assert.h>
#define Q_ASSERT(expr) assert(expr)
#endif // Q_ASSERT

#define q_enqueue(queue, item) \
    (Q_ASSERT(sizeof((queue)->count) == sizeof(size_t) && sizeof((queue)->capacity) == sizeof(size_t)), __q_enqueue(queue, \
            (uint8_t*)&(queue)->items - (uint8_t*)(queue), \
            (uint8_t*)&(queue)->count - (uint8_t*)(queue), \
            (uint8_t*)&(queue)->capacity - (uint8_t*)(queue), item, sizeof(*(item)))) \

#define q_enqueue_many(queue, items_, item_count) \
    (Q_ASSERT(sizeof((queue)->count) == sizeof(size_t) && sizeof((queue)->capacity) == sizeof(size_t)), __q_enqueue_many(queue, \
            (uint8_t*)&(queue)->items - (uint8_t*)(queue), \
            (uint8_t*)&(queue)->count - (uint8_t*)(queue), \
            (uint8_t*)&(queue)->capacity - (uint8_t*)(queue), items_, item_count, sizeof((items_)[0])))

#define q_dequeue(queue) (Q_ASSERT((queue)->count > 0), (queue)->items[--(queue)->count])

void __q_enqueue(void* queue, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* item, size_t item_size);
void __q_enqueue_many(void* queue, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* items, size_t item_count, size_t item_size);

#endif // QUEUE_H

#ifdef QUEUE_IMPLEMENTATION
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void __q_enqueue(void* queue, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* item, size_t item_size) {
    void** items = (void**)(queue + items_field_offset);
    size_t* count = (size_t *)(queue + count_field_offset);
    size_t* capacity = (size_t *)(queue + capacity_field_offset);

    // printf("count: %ld, capacity: %ld\n", *count, *capacity);
    // printf("items: %p\n", *items);

    if (*count >= *capacity) { 
        if (*capacity == 0) *capacity = 1; 
        while (*count >= *capacity) *capacity *= 2; 
        *items = realloc(*items, *capacity * item_size);
        assert(*items != NULL);
    } 

    memmove(*items + item_size, *items, *count * item_size);
    memcpy(*items, item, item_size);
    *count += 1; 
}

void __q_enqueue_many(void* queue, size_t items_field_offset, size_t count_field_offset, size_t capacity_field_offset, void* items, size_t item_count, size_t item_size) {
    for (size_t i = 0; i < item_count; ++i) {
        void* item = items + i * item_size;
        __q_enqueue(queue, items_field_offset, count_field_offset, capacity_field_offset, item, item_size);
    }
}
#endif
