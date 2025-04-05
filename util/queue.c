#include "queue.h"

int ch_queue_push(ch_queue* queue, void* item) {
    if (queue->size == queue->capacity) {
        size_t new_capacity = queue->capacity * 1.5;
        void **new_container = malloc(new_capacity * sizeof(void*));

        if (!new_container) {
            return 1;
        }

        for (int i = 0; i < queue->size; i++) {
            size_t pos = (queue->front + i) % queue->capacity;
            new_container[i] = queue->items[pos];
        }

        free(queue->items);
        queue->items = new_container;
        queue->front = 0;
        queue->back = queue->size;
        queue->capacity = new_capacity;
    }

    queue->items[queue->back] = item;
    queue->back = (queue->back + 1) % queue->capacity;
    queue->size++;

    return 0;
}

void* ch_queue_pop(ch_queue* queue) {
    if (queue->size == 0) {
        return NULL;
    }
    void *item = queue->items[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    // shrink when capacity is too small
    if (queue->size > 0 && queue->size == queue->capacity / 4 && queue->capacity >= 4) {
        size_t new_capacity = queue->capacity / 2;
        void **new_container = malloc(new_capacity * sizeof(void*));

        for (int i = 0; i < queue->size; i++) {
            size_t pos = (queue->front + i) % queue->capacity;
            new_container[i] = queue->items[pos];
        }

        free(queue->items);
        queue->items = new_container;
        queue->front = 0;
        queue->back = queue->size;
        queue->capacity = new_capacity;
    }

    return item;
}

void* ch_queue_peek(ch_queue* queue) {
    if (queue->items == NULL) return NULL;

    if (queue->size > 0) {
        return queue->items[queue->front];
    }
}

int ch_queue_is_empty(ch_queue* queue) {
    return queue->size == 0;
}

int ch_queue_size(ch_queue* queue) {
    return queue->size;
}

void ch_queue_destroy(ch_queue* queue, int own_items) {
    if (own_items) {
        for (size_t i = 0; i < queue->size; i++) {
            int pos = (queue->front + i) % queue->capacity;
            free(queue->items[pos]);
        }
    }
    free(queue->items);
    free(queue);
}