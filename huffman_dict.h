#ifndef HUFFMAN_DICT_H
#define HUFFMAN_DICT_H

#include "node.h"

void generate_huffman_codes(node_t *root, char *huffman_code, int depth, char **huffman_dict);
void free_huffman_dict(char** huffman_dict, int dict_size);
void print_huffman_codes(char **huffman_dict, int size, int compression);

#endif //HUFFMAN_DICT_H
