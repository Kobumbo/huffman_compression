#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include "node.h"
#include "min_heap.h"

void build_huffman_tree(node_t **huffman_tree, MinHeap *min_heap);
int count_leaf_nodes(node_t *root);
int next_bit(FILE *input_file, uint8_t *input_byte, int *input_bits, int encrypted, char *password);
void write_bit(FILE *out_file, uint8_t *byte_buffer, int *bits_written, int bit);
void rebuild_huffman_tree(node_t **huffman_tree, FILE *input_file, uint8_t *input_byte, int *input_bits, int encrypted, char *password, int compression, int *nodes_processed, int debug, long *byte_position);
void decompress_huffman_file(FILE *in_file, FILE *out_file, node_t *huffman_tree, int padding, int bits_left, int index, int compression, int encrypted, char *password, int debug);
void free_huffman_tree(node_t *root);

#endif //HUFFMAN_TREE_H
