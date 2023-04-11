#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "node.h"
#include "progress.h"

void generate_huffman_codes(node_t *root, char *huffman_code, int depth, char **huffman_dict) {
    if (!root->left && !root->right) {
        if (depth == 0) {
            huffman_code[depth++] = '0'; // Assign a single bit Huffman code to a single node tree
        }
        huffman_code[depth] = '\0';
        huffman_dict[root->code] = strdup(huffman_code);
        return;
    }

    if (root->left) {
        huffman_code[depth] = '0';
        generate_huffman_codes(root->left, huffman_code, depth + 1, huffman_dict);
    }

    if (root->right) {
        huffman_code[depth] = '1';
        generate_huffman_codes(root->right, huffman_code, depth + 1, huffman_dict);
    }
}

void free_huffman_dict(char **huffman_dict, int dict_size) {
    for (int i = 0; i < dict_size; i++) {
        if (huffman_dict[i] != NULL) {
            free(huffman_dict[i]);
            huffman_dict[i] = NULL;
        }
    }
    free(huffman_dict);
}


void print_huffman_codes(char **huffman_dict, int size, int compression) {
    for (int i = 0; i < size; i++) {
        if (huffman_dict[i]) {
            printf("Code: ");
            for (int j = compression - 1; j >= 0; j--) {
                printf("%d", (i >> j) & 1);
            }
            printf(" | Huffman Code: %s\n", huffman_dict[i]);
        }
    }
}