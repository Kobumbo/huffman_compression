#ifndef FILE_WRITE_H
#define FILE_WRITE_H

#include <stdint.h>


void compress_file(FILE *input_file, FILE *output_file, char **huffman_dict, int bits_left_value, int bits_left_size, int compression, int *padding, int input_file_size, int debug, int encrypted, char *password);
void write_huffman_tree(FILE *output_file, node_t *root, uint8_t *output_byte, int *output_bits, int compression, int *remaining_leaves, int encrypted, char *password);
void write_to_file(const char *input_filename, const char *output_filename, char **huffman_dict, int bits_left_value, int bits_left_size, int compression, node_t *root, uint8_t *output_byte, int *output_bits, int *remaining_leaves, int *padding, int encrypted, char *password, int debug);
void encrypt_xor(uint8_t *byte, const char *password);
void control_sum(FILE *output_file);
unsigned char control_sum_return(FILE *output_file);
int get_padding(FILE *input_file);
int get_left_bits(FILE *input_file);
unsigned char get_control_sum(FILE *input_file);
int get_compression(FILE *input_file);
void write_bytes_after_5th(FILE *input_file, FILE *output_file, int encrypted, char *password);
void copy_file_content(FILE *input_file, FILE *output_file, int encrypted, char *password);
void write_header(FILE *output_file, int state, int encrypted, int compression, int *padding, int bits_left_size, char *password);

#endif //FILE_WRITE_H
