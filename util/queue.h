#pragma once
#include<stdio.h>
#include<stdlib.h>

typedef struct ch_queue ch_queue;

struct ch_queue {
    void **items;
    size_t front;
    size_t back;
    size_t size;
    size_t capacity;
};

inline ch_queue ch_queue_init() {
    return (ch_queue){ .items = (void**)malloc(sizeof(void*)), .front = 0, .back = 0, .size = 0, .capacity = 1 };
}

int ch_queue_push(ch_queue* queue, void* item);
void* ch_queue_pop(ch_queue* queue);
void* ch_queue_peek(ch_queue* queue);
int ch_queue_is_empty(ch_queue* queue);
int ch_queue_size(ch_queue* queue);
void ch_queue_destroy(ch_queue* queue, int own_items);