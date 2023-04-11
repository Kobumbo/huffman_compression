#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "progress.h"

void count_codes(const char *input_filename, int compression, int *bits_left_value, int **codes_counter, int *number_of_codes, int *bits_left_size, int debug){
    FILE *file = fopen(input_filename, "rb");
    if(!file){
        fprintf(stderr, "Blad otwierania pliku wejsciowego: %s\n", input_filename);
        fprintf(stderr, "Sprawdz sposob uzycia wywolujac program z flaga -h\n");
        exit(1);
    }

    size_t size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t buffer[2];
    size_t bytes_read;
    uint32_t temp_code = 0;
    int bits_read = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            temp_code = (temp_code << 8) | buffer[i];
            bits_read += 8;

            while (bits_read >= compression) {
                int code = (temp_code >> (bits_read - compression)) & ((1 << compression) - 1);
                bits_read -= compression;
                (*codes_counter)[code]++;
                (*number_of_codes)++;
                if(debug){
                    progress("Czytanie kodow. Przetworzone kody:", *number_of_codes, 0, 1);
                }
            }
        }
    }

    fclose(file);

    *bits_left_value = 0;
    if(bits_read > 0){
        for(int i = bits_read - 1; i >=0; i--){
            int bit_value = (temp_code >> i) & 1;
            *bits_left_value = (*bits_left_value << 1) | bit_value;
        }
    }
    *bits_left_size = bits_read;
}