#include <stdio.h>
#include <stdlib.h>
#include "min_heap.h"



void min_heapify(MinHeap *heap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && heap->data[left].occurrences < heap->data[smallest].occurrences) {
        smallest = left;
    }

    if (right < heap->size && heap->data[right].occurrences < heap->data[smallest].occurrences) {
        smallest = right;
    }

    if (smallest != index) {
        node_t temp = heap->data[index];
        heap->data[index] = heap->data[smallest];
        heap->data[smallest] = temp;
        min_heapify(heap, smallest);
    }
}

MinHeap *min_heap_create(int capacity) {
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    heap->data = (node_t *)malloc(capacity * sizeof(node_t *)*50);
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void min_heap_insert(MinHeap *heap, node_t *node) {
    if (heap->size == heap->capacity) {
        return; // Heap is full
    }

    int index = heap->size++;
    heap->data[index] = *node;

    while (index != 0 && heap->data[(index - 1) / 2].occurrences > heap->data[index].occurrences) {
        node_t temp = heap->data[index];
        heap->data[index] = heap->data[(index - 1) / 2];
        heap->data[(index - 1) / 2] = temp;
        index = (index - 1) / 2;
    }
}

void min_heap_fill(MinHeap **heap, int *codes_counter, int cap){
    for(int i = 0; i < cap; i++){
        if(codes_counter[i] > 0){
            node_t *node = create_node(i, codes_counter[i]);
            min_heap_insert(*heap, node);
            free(node);
        }
    }
}

node_t *min_heap_extract_min(MinHeap *heap) {
    if (heap->size == 0) {
        return NULL;
    }

    node_t *min = (node_t *)malloc(sizeof(node_t));
    *min = heap->data[0];
    heap->data[0] = heap->data[--heap->size];
    min_heapify(heap, 0);

    return min;
}

void min_heap_free(MinHeap *heap) {
    free(heap->data);
    free(heap);
}

