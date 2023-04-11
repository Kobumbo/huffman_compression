#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "node.h"
#include "progress.h"

void encrypt_xor(uint8_t *byte, const char *password){
    int password_len = strlen(password);

    for(int i = 0; i < password_len; i++){
        *byte ^= password[i];
    }
}

void compress_file(FILE *input_file, FILE *output_file, char **huffman_dict, int bits_left_value, int bits_left_size, int compression, int *padding, int input_file_size, int debug, int encrypted, char *password) {


    uint8_t buffer[2];
    size_t bytes_read;
    uint32_t temp_code = 0;
    int bits_read = 0;
    uint8_t output_byte = 0;
    int output_bits = 0;
    int bytes_processed = 0;

    size_t update_interval = input_file_size / 100;
    if (update_interval == 0) {
        update_interval = 1;
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), input_file)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            temp_code = (temp_code << 8) | buffer[i];
            bits_read += 8;

            while (bits_read >= compression) {
                int code = (temp_code >> (bits_read - compression)) & ((1 << compression) - 1);
                bits_read -= compression;

                char *huffman_code = huffman_dict[code];
                for (size_t j = 0; j < strlen(huffman_code); j++) {
                    output_byte = (output_byte << 1) | (huffman_code[j] - '0');
                    output_bits++;

                    if (output_bits == 8) {
                        if(encrypted){
                            encrypt_xor(&output_byte, password);
                        }
                        fwrite(&output_byte, 1, 1, output_file);
                        output_bits = 0;
                    }
                }
            }
            bytes_processed++;
            if(debug){
                if (bytes_processed % update_interval == 0) {
                    progress("Zapisywanie skompresowanych danych:", bytes_processed, input_file_size, 0);
                }
            }
        }
    }

    // Write the remaining bits left and padding zeros
    for (int i = 0; i < bits_left_size; ++i) {
        output_byte = (output_byte << 1) | ((bits_left_value >> (bits_left_size - i - 1)) & 1);
        output_bits++;

        if (output_bits == 8) {
            if(encrypted){
                encrypt_xor(&output_byte, password);
            }
            fwrite(&output_byte, 1, 1, output_file);
            output_bits = 0;
        }
    }

    while (output_bits != 0) {
        output_byte = (output_byte << 1); // Add a zero to the output_byte
        (*padding)++; // Increment the padding_zeros counter
        output_bits++;

        if (output_bits == 8) {
            if(encrypted){
                encrypt_xor(&output_byte, password);
            }
            fwrite(&output_byte, 1, 1, output_file);
            output_bits = 0;
        }
    }
}


void write_huffman_tree(FILE *output_file, node_t *root, uint8_t *output_byte, int *output_bits, int compression, int *remaining_leaves, int encrypted, char *password) {
    if (!root) {
        return;
    }

    if (!root->left && !root->right) {
        // If this is the last leaf node, output '11'
        if (*remaining_leaves == 1) {
            *output_byte = (*output_byte << 1) | 1;
            *output_byte = (*output_byte << 1) | 1;
        } else {
            // Write leaf node (01) followed by the word code
            *output_byte = (*output_byte << 1) | 0;
            *output_byte = (*output_byte << 1) | 1;
        }

        *output_bits += 2;
        if (*output_bits >= 8) {
            if(encrypted){
                encrypt_xor(output_byte, password);
            }
            fwrite(output_byte, 1, 1, output_file);
            *output_bits -= 8;
        }

        for (int i = compression - 1; i >= 0; i--) {
            *output_byte = (*output_byte << 1) | ((root->code >> i) & 1);
            *output_bits += 1;

            if (*output_bits >= 8) {
                if(encrypted){
                    encrypt_xor(output_byte, password);
                }
                fwrite(output_byte, 1, 1, output_file);
                *output_bits -= 8;
            }
        }

        // Add padding zeros if necessary
        if (*remaining_leaves == 1 && *output_bits > 0) {
            *output_byte <<= (8 - *output_bits);
            if(encrypted){
                encrypt_xor(output_byte, password);
            }
            fwrite(output_byte, 1, 1, output_file);
        }

        (*remaining_leaves)--;
    } else {
        // Write internal node (00)
        *output_byte = (*output_byte << 1) | 0;
        *output_byte = (*output_byte << 1) | 0;
        *output_bits += 2;
        if (*output_bits >= 8) {
            if(encrypted){
                encrypt_xor(output_byte, password);
            }
            fwrite(output_byte, 1, 1, output_file);
            *output_bits -= 8;
        }
    }

    write_huffman_tree(output_file, root->left, output_byte, output_bits, compression, remaining_leaves, encrypted, password);
    write_huffman_tree(output_file, root->right, output_byte, output_bits, compression, remaining_leaves, encrypted, password);
}

void write_header(FILE *output_file, int state, int encrypted, int compression, int *padding, int bits_left_size, char *password){
    unsigned char header[5] = {0};

    header[0] = 'B';
    header[1] = 'J';

    if(encrypted){
        encrypt_xor(&header[0], password);
        encrypt_xor(&header[1], password);
    }

    if (compression == 0) {
        header[2] |= 0b00000000; // 00 for compression 0
    } else if (compression == 8) {
        header[2] |= 0b01000000; // 01 for compression 8
    } else if (compression == 12) {
        header[2] |= 0b10000000; // 10 for compression 12
    } else if (compression == 16) {
        header[2] |= 0b11000000; // 11 for compression 16
    }

    if (encrypted) {
        header[2] |= 0b00100000; // Set the 3rd bit if the file is encrypted
    }
    fseek(output_file, 0, SEEK_SET);
    fwrite(header, sizeof (unsigned char), 5, output_file);
    if(state == 1){
        if (*padding) {
            header[2] = (header[2] & 0b11110000) | (*padding & 0b00001111);
        }
        fseek(output_file, 0, SEEK_SET);
        header[3] = (bits_left_size << 4) | 0x0F; // First 4 bits for the number of bits left and last 4 bits set to 1
        fwrite(header, sizeof (unsigned char), 5, output_file);
    }
}


void control_sum(FILE *output_file){
    unsigned char byte;
    size_t byte_index = 1;
    unsigned char xor_result = 0;
    fseek(output_file, 0, SEEK_SET);
    while(fread(&byte, 1, 1, output_file) == 1){
        if(byte_index != 5){
            xor_result ^= byte;
        }
        byte_index++;
    }
    fseek(output_file, 4, SEEK_SET);
    fwrite(&xor_result, 1, 1, output_file);
}

unsigned char control_sum_return(FILE *output_file) {
    unsigned char byte;
    size_t byte_index = 1;
    unsigned char xor_result = 0;
    fseek(output_file, 0, SEEK_SET);
    while (fread(&byte, 1, 1, output_file) == 1) {
        if (byte_index != 5) {
            xor_result ^= byte;
        }
        byte_index++;
    }

    // No need to write the xor_result to the file, just return it
    return xor_result;
}

int get_padding(FILE *input_file) {
    if (!input_file) {
        fprintf(stderr, "Error with input file pointer.\n");
        exit(1);
    }

    fseek(input_file, 2, SEEK_SET); // Move the file pointer to the 3rd byte
    unsigned char byte;
    fread(&byte, 1, 1, input_file);

    int padding = byte & 0x0F; // Extract the last 4 bits
    fseek(input_file, 0, SEEK_SET); // Reset the file pointer to the beginning

    return padding;
}

int get_left_bits(FILE *input_file) {
    if (!input_file) {
        fprintf(stderr, "Error with input file pointer.\n");
        exit(1);
    }

    fseek(input_file, 3, SEEK_SET); // Move the file pointer to the 4th byte
    unsigned char byte;
    fread(&byte, 1, 1, input_file);

    int left_bits = (byte & 0xF0) >> 4; // Extract the first 4 bits
    fseek(input_file, 0, SEEK_SET); // Reset the file pointer to the beginning

    return left_bits;
}

unsigned char get_control_sum(FILE *input_file) {
    if (!input_file) {
        fprintf(stderr, "Error with input file pointer.\n");
        exit(1);
    }

    fseek(input_file, 4, SEEK_SET); // Move the file pointer to the 5th byte
    unsigned char byte;
    fread(&byte, 1, 1, input_file);

    fseek(input_file, 0, SEEK_SET); // Reset the file pointer to the beginning

    return byte;
}

int get_compression(FILE *input_file) {
    if (!input_file) {
        fprintf(stderr, "Error with input file pointer.\n");
        exit(1);
    }

    // Save the current position in the file
    long current_pos = ftell(input_file);

    // Go to the beginning of the file
    fseek(input_file, 0, SEEK_SET);

    unsigned char buffer[1];

    // Read the 3rd byte
    fseek(input_file, 2, SEEK_SET);
    fread(buffer, 1, 1, input_file);

    // Restore the file position
    fseek(input_file, current_pos, SEEK_SET);

    // Extract the first 2 bits
    unsigned char compression_bits = (buffer[0] & 0xC0) >> 6;

    // Convert the 2 bits to an int representing the compression
    int compression;
    switch (compression_bits) {
        case 0:
            compression = 0;
            break;
        case 1:
            compression = 8;
            break;
        case 2:
            compression = 12;
            break;
        case 3:
            compression = 16;
            break;
        default:
            fprintf(stderr, "Invalid compression bits.\n");
            exit(1);
    }

    return compression;
}

void write_bytes_after_5th(FILE *input_file, FILE *output_file, int encrypted, char *password) {
    if (!input_file || !output_file) {
        fprintf(stderr, "Error with input or output file pointers.\n");
        return;
    }

    // Move the file position to the 5th byte
    fseek(input_file, 5, SEEK_SET);
    fseek(output_file, 0, SEEK_SET);

    // Read data one byte at a time from the input file and write it to the output file
    uint8_t byte;
    while (fread(&byte, 1, 1, input_file) == 1) {
        if(encrypted){
            encrypt_xor(&byte, password);
        }
        fwrite(&byte, 1, 1, output_file);
    }
}

void copy_file_content(FILE *input_file, FILE *output_file, int encrypted, char *password) {
    if (!input_file || !output_file) {
        fprintf(stderr, "Error with input or output file pointers.\n");
        return;
    }

    // Reset the file positions to the beginning
    fseek(input_file, 0, SEEK_SET);

    // Read data one byte at a time from the input file and write it to the output file
    uint8_t byte;
    while (fread(&byte, 1, 1, input_file) == 1) {
        if(encrypted){
            encrypt_xor(&byte, password);
        }
        fwrite(&byte, 1, 1, output_file);
    }
}

void write_to_file(const char *input_filename, const char *output_filename, char **huffman_dict, int bits_left_value, int bits_left_size, int compression, node_t *root, uint8_t *output_byte, int *output_bits, int *remaining_leaves, int *padding, int encrypted, char *password, int debug){
    FILE *input_file = fopen(input_filename, "rb+");
    if (!input_file) {
        fprintf(stderr, "Blad otwierania pliku wejsciowego: %s\n", input_filename);
        fprintf(stderr, "Sprawdz sposob uzycia wywolujac program z flaga -h\n");
        exit(1);
    }

    FILE *output_file = fopen(output_filename, "wb+");
    if (!output_file) {
        fprintf(stderr, "Blad otwierania pliku wyjsciowego: %s\n", output_filename);
        fprintf(stderr, "Sprawdz sposob uzycia wywolujac program z flaga -h\n");
        fclose(input_file);
        exit(1);
    }

    int input_file_size;
    fseek(input_file, 0, SEEK_END);
    input_file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);

    write_header(output_file, 0, encrypted, compression, padding, bits_left_size, password);
    write_huffman_tree(output_file, root, output_byte, output_bits, compression, remaining_leaves, encrypted, password);
    compress_file(input_file, output_file, huffman_dict, bits_left_value, bits_left_size, compression, padding, input_file_size, debug, encrypted, password);
    write_header(output_file, 1, encrypted, compression, padding, bits_left_size, password);
    control_sum(output_file);

    if(debug){
        printf("\nPlik zostal skompresowany\n");
    }
    fclose(input_file);
    fclose(output_file);
}