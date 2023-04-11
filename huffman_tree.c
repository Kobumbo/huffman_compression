#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "node.h"
#include "min_heap.h"
#include "file_write.h"
#include "progress.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 8192
#endif

//#ifndef MAX_TREE_HEIGHT
//#define MAX_TREE_HEIGHT 200000000
//#endif

void build_huffman_tree(node_t **huffman_tree, MinHeap *min_heap) {
    while (min_heap->size > 1) {
        node_t *node1 = min_heap_extract_min(min_heap);
        node_t *node2 = min_heap_extract_min(min_heap);

        node_t *parent = merge_nodes(node1, node2);
        min_heap_insert(min_heap, parent);
        *huffman_tree = parent;
        free(parent);
    }
    *huffman_tree = min_heap_extract_min(min_heap);
}

int count_leaf_nodes(node_t *root) {
    if (!root) {
        return 0;
    }

    if (!root->left && !root->right) {
        return 1;
    }

    return count_leaf_nodes(root->left) + count_leaf_nodes(root->right);
}

int next_bit(FILE *input_file, uint8_t *input_byte, int *input_bits, int encrypted, char *password) {
    static uint8_t buffer = 0;
    static int buffer_pos = 0;


    if (*input_bits == 0) {
        int c = fgetc(input_file);
        buffer = (uint8_t)c;

        if (encrypted) {
            encrypt_xor(&buffer, password);
        }

        *input_bits = 8;
        buffer_pos = 0;
    }

    *input_bits -= 1;
    *input_byte = (buffer >> *input_bits) & 1;
    return *input_byte;
}

void write_bit(FILE *out_file, uint8_t *byte_buffer, int *bits_written, int bit) {
    if (!out_file || !byte_buffer || !bits_written) {
        fprintf(stderr, "Error with output file pointer or byte buffer or bits_written.\n");
        exit(1);
    }

    *byte_buffer |= (bit << (7 - *bits_written));
    *bits_written += 1;

    if (*bits_written == 8) {
        fwrite(byte_buffer, 1, 1, out_file);
        *byte_buffer = 0;
        *bits_written = 0;
    }
}



void rebuild_huffman_tree(node_t **huffman_tree, FILE *input_file, uint8_t *input_byte, int *input_bits, int encrypted, char *password, int compression, int *nodes_processed, int debug, long *byte_position){
    int bit1 = next_bit(input_file, input_byte, input_bits, encrypted, password);
    int bit2 = next_bit(input_file, input_byte, input_bits, encrypted, password);

    if (bit1 == 0 && bit2 == 0){
        // Internal node
        node_t *left = NULL;
        node_t *right = NULL;

        rebuild_huffman_tree(&left, input_file, input_byte, input_bits, encrypted, password, compression, nodes_processed, debug, byte_position);
        rebuild_huffman_tree(&right, input_file, input_byte, input_bits, encrypted, password, compression, nodes_processed, debug, byte_position);
        *huffman_tree = merge_nodes(left, right);
    } else {
        // Leaf node
        int code = 0;

        for (int i = 0; i < compression; i++) {
            code <<= 1;
            code |= next_bit(input_file, input_byte, input_bits, encrypted, password);
        }

        *huffman_tree = create_node(code, 0);

        if (bit1 == 1 && bit2 == 1) {
            return;
        }
    }
    (*nodes_processed)++;
    if(debug){
        progress("Odtwarzanie drzewa Huffmanna. Przetworzone wezly:", *nodes_processed, 0, 1);
    }

    // Update the byte position to store the current position in the file
    *byte_position = ftell(input_file);
}


void decompress_huffman_file(FILE *in_file, FILE *out_file, node_t *huffman_tree, int padding, int bits_left, int index, int compression, int encrypted, char *password, int debug) {
    if (!in_file || !out_file) {
        fprintf(stderr, "Error with input or output file pointers.\n");
        exit(1);
    }

    node_t *current3 = huffman_tree;
    long file_size3;

    int total_bits3;
    int input_bits3 = 0;
    int bits_written3 = 0;
    uint8_t output_byte3 = 0;


    fseek(in_file, 0, SEEK_END);
    file_size3 = ftell(in_file);
    fseek(in_file, index, SEEK_SET);

    total_bits3 = ((file_size3 - index) * 8) - padding - bits_left;

    uint8_t input_bit3;
    int bits_processed3 = 0;
    while (bits_processed3 < total_bits3) {
        input_bit3 = next_bit(in_file, &input_bit3, &input_bits3, encrypted, password);

        current3 = input_bit3 ? current3->right : current3->left;

        if (!current3->left && !current3->right) {
            uint16_t code = current3->code;


            // Write the bits of the code to the output file
            for (int i = compression - 1; i >= 0; --i) {
                int bit = (code >> i) & 1;
                write_bit(out_file, &output_byte3, &bits_written3, bit);
            }

            current3 = huffman_tree;
        }

        bits_processed3++;

    }
    if (bits_left > 0) {
        for (int i = 0; i < bits_left; ++i) {
            input_bit3 = next_bit(in_file, &input_bit3, &input_bits3, encrypted, password);
            write_bit(out_file, &output_byte3, &bits_written3, input_bit3);
        }
    }
    if(debug){
        printf("Plik zostal zdekompresowany\n");
    }
}

void free_huffman_tree(node_t *root){
    if(root == NULL){
        return;
    }
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}