#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "node.h"

typedef struct MinHeap {
    node_t *data;
    int size;
    int capacity;
} MinHeap;

MinHeap *min_heap_create(int capacity);
void min_heap_insert(MinHeap *heap, node_t *node);
node_t *min_heap_extract_min(MinHeap *heap);
void min_heap_free(MinHeap *heap);
void min_heap_fill(MinHeap **heap, int *codes_counter, int compression);

#endif //MIN_HEAP_H
