#include <semaphore.h>
#include <stdlib.h>
#include "graph.h"

typedef struct {
    semt_t write_sem;
    semt_t read_sem;
    size_t matrix_count;
    size_t matrix_size;
    bool matrix[matrix_count][matrix_size];
} ringbuffer_t;

ringbuffer_t* ringbuffer_create(char* shm_name, size_t size);

ringbuffer_t* ringbuffer_open(char* shm_name, size_t size);

void ringbuffer_close(ringbuffer_t* ring);

void ringbuffer_push(ringbuffer_t* ring, edge_matrix_t edges);

void ringbuffer_pop(ringbuffer_t* ring, edge_matrix_t* buffer);