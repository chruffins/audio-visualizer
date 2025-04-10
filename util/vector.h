#pragma once
#define DEF_VECTOR(TYPE) \
typedef struct { \
    TYPE** data; \
    size_t size; \
    size_t capacity; \
} TYPE##_vec;

#define VECTOR_INIT(vec) \
do { \
    (vec).data = NULL; \
    (vec).size = 0; \
    (vec).capacity = 0; \
} while (0)

#define VECTOR_PUSH(TYPE, vec, value) \
do { \
    if ((vec).size >= (vec).capacity) { \
        (vec).capacity = (vec).capacity == 0 ? 1 : (vec).capacity * 2; \
        (vec).data = realloc((vec).data, (vec).capacity * sizeof(TYPE)); \
        if (!(vec).data) { \
            fprintf(stderr, "vector push realloc failed\n"); \
            exit(1); \
        } \
    } \
    (vec).data[(vec).size++] = (value); \
} while (0)

#define VECTOR_POP(vec) \
((vec).size > 0 ? (vec).data[--(vec).size] : (TYPE){0})

#define VECTOR_AT(vec, index) \
(index < (vec).size ? (vec).data[(index)] : NULL)

#define VECTOR_RESERVE(TYPE, vec, capacity) \
do { \
    (vec).capacity = (vec).capacity < capacity ? capacity : (vec).capacity; \
    (vec).data = (vec).data == NULL ? calloc(capacity, sizeof(TYPE)) : realloc((vec).data, (vec).capacity * sizeof(TYPE)); \
    if (!(vec).data) { \
        fprintf(stderr, "vector reserve allocation failed\n"); \
        exit(1); \
    } \
} while (0)

#define VECTOR_FREE(vec) \
do { \
    for (int i = 0; i < (vec).size; i++) free((vec).data[i]);\
    free((vec).data); \
    (vec).data = NULL; \
    (vec).size = 0; \
    (vec).capacity = 0; \
} while (0)
