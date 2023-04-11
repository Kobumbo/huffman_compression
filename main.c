#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include "codes_counter.h"
#include "min_heap.h"
#include "huffman_tree.h"
#include "huffman_dict.h"
#include "file_write.h"



int main(int argc, char **argv)
{
    int opt;
    int compression = 0;
    char *password;
    char *output_file_compression;
    int output_file_compression_given = 0;
    char *output_file_decompression;
    int output_file_decompression_given = 0;
    int encrypted = 0;
    int debug = 0;

    char *usage =
            "\n\nDostepne parametry przy wywolywaniu: \n"
            "   -z [nazwa_pliku] -> kompresuje plik podany w argumencie [nazwa_pliku].\n"
            "       Przyklad:\n"
            "           -z i.txt     -> kompresuje plik o nazwie i.txt\n"
            "   -c [nazwa_pliku] -> plik wyjsciowy kompresji\n"
            "   -x [nazwa_pliku] -> dekompresuje plik podany w argumencie [nazwa_pliku]\n"
            "       Przylad:\n"
            "           -x i.txt     -> dekompresuje plik o nazwie i.txt\n"
            "   -d [nazwa_pliku] -> plik wyjsciowy dekompresji"
            "   -o[0|1|2|3] -> okreslenie stopnia kompresji pliku\n"
            "       Przyklad:\n"
            "           -o0             "
            "  -> compression zerowa - przepisanie pliku z dodaniem flag\n"
            "           -o1            -> slownik 8 bitowy\n"
            "           -o2            -> slownik 12 bitowy\n"
            "           -o3            -> slownik 16 bitowy\n"
            "   -p [password] -> plik wyjsciowy zostanie zaszyfrowany przy uzyciu hasla podanego w argumencie [halso] za pomoca szyfru XOR\n"
            "       Przyklad:\n"
            "           -c haslo123   -> plik wyjsciowy zostanie zaszyfrowany haslem: haslo123\n"
            "   -v                    -> powoduje wypisanie dodatkowych informacji diagnostycznych\n"
            "   -h                    -> powoduje wyswietlenie powyzszej instrukcji\n\n\n";




    while((opt = getopt(argc, argv, ":z:x:p:vho:c:d:")) != -1){
        switch(opt){
            case 'o':
                if(*optarg == '0'){
                    compression = 0;
                }else if(*optarg == '1'){
                    compression = 8;
                }else if(*optarg == '2'){
                    compression = 12;
                }else if(*optarg == '3'){
                    compression = 16;
                }else{
                    fprintf(stderr, "Niepoprawny stopien kompresji. Sprawdz sposob uzycia wywolujac program z flaga -h\n");
                    exit(1);
                }
                break;
            case 'c':
                output_file_compression = optarg;
                output_file_compression_given = 1;
                break;
            case 'd':
                output_file_decompression = optarg;
                output_file_decompression_given = 1;
                break;
            case 'p':
                password = optarg;
                encrypted = 1;
                break;
            case 'v':
                debug = 1;
                break;
            }
    }

    optind = 1;

    while((opt = getopt(argc, argv, ":z:x:p:vho:c:d:")) != -1){
        switch(opt){
            case 'h':
                printf("%s", usage);
                break;
            case 'z':
            {
                if(output_file_compression_given == 0){
                    fprintf(stderr, "Nie podano pliku wyjsciowego do kompresji\n");
                    exit(1);
                }
                if(compression == 0){
                    FILE *input_file = fopen(optarg, "rb+");
                    if (!input_file) {
                        fprintf(stderr, "Blad otwierania pliku wejsciowego: %s\n", optarg);
                        fprintf(stderr, "Sprawdz sposob uzycia wywolujac program z flaga -h\n");
                        exit(1);
                    }

                    FILE *output_file = fopen(output_file_compression, "wb+");
                    if (!output_file) {
                        fprintf(stderr, "Blad otwierania pliku wyjsciowego: %s\n", output_file_compression);
                        fprintf(stderr, "Sprawdz sposob uzycia wywolujac program z flaga -h\n");
                        exit(1);
                    }
                    int padding = 0;
                    write_header(output_file, 0, encrypted, compression, &padding, 0, password);
                    copy_file_content(input_file, output_file, encrypted, password);
                    write_header(output_file, 1, encrypted, compression, &padding, 0, password);
                    control_sum(output_file);

                    fclose(input_file);
                    fclose(output_file);
                    break;
                }


                int *codes_counter = calloc(pow(2, compression), sizeof(int));
                int bits_left_value = 0;
                int number_of_codes = 0;
                int bits_left_size = 0;
                if(debug){
                    printf("\n");
                    printf("Informacje diagnostyczne kompresja:\n");
                }
                count_codes(optarg, compression, &bits_left_value, &codes_counter, &number_of_codes, &bits_left_size, debug);
                if(debug){
                    printf("\n");
                }
                if(number_of_codes < 1){
                    fprintf(stderr, "Nie mozna odczytac zadnego kodu\n");
                    exit(1);
                }
                MinHeap *min_heap = NULL;
                min_heap = min_heap_create(pow(2, compression));
                min_heap_fill(&min_heap, codes_counter, pow(2, compression));
                node_t *huffman_tree = NULL;
                build_huffman_tree(&huffman_tree, min_heap);
                int total_leaves = count_leaf_nodes(huffman_tree);
                char **huffman_dict = (char **)calloc(pow(2,compression), sizeof(char *));
                char *huffman_code = (char *)calloc(compression + 10, sizeof(char));
                generate_huffman_codes(huffman_tree, huffman_code, 0, huffman_dict);
//                print_huffman_codes(huffman_dict, pow(2, compression), compression);
                int huffman_output_bits = 0;
                uint8_t output_byte = 0;
                int padding = 0;
                unsigned char *header = calloc(5, sizeof(unsigned  char));
                write_to_file(optarg, output_file_compression, huffman_dict, bits_left_value, bits_left_size, compression, huffman_tree, &output_byte, &huffman_output_bits, &total_leaves, &padding, encrypted, password, debug);



                min_heap_free(min_heap);
                free(codes_counter);
                free(huffman_code);
                free(header);
                free_huffman_dict(huffman_dict, pow(2, compression));
                free_huffman_tree(huffman_tree);
                break;
            }
            case 'x':{
                if(!output_file_decompression_given){
                    fprintf(stderr, "Nie podano pliku wyjsciowego do dekompresji\n");
                    exit(1);
                }
                node_t *huffman_tree2 = NULL;
                uint8_t input_byte = 0;
                int input_bits = 0;
                int bits_read = 0;
                FILE *input_file = fopen(optarg, "rb+");
                if (!input_file) {
                    fprintf(stderr, "Blad otwierania pliku wejsciowego: %s\n", optarg);
                    fprintf(stderr, "Sprawdz sposob uzycia wywolujac program z flaga -h\n");
                    exit(1);
                }

                unsigned char a = control_sum_return(input_file);
                unsigned char b = get_control_sum(input_file);
                if(a != b){
                    fprintf(stderr, "Plik jest uszkodzony\n");
                    exit(1);
                }

                FILE *output_file = fopen(output_file_decompression, "wb+");
                if (!output_file) {
                    fprintf(stderr, "Blad otwierania pliku wyjsciowego: %s\n", output_file_decompression);
                    fprintf(stderr, "Sprawdz sposob uzycia wywolujac program z flaga -h\n");
                    exit(1);
                }

                int compression_decoding = get_compression(input_file);

                if(compression_decoding == 0){
                    write_bytes_after_5th(input_file, output_file, encrypted, password);
                    fclose(input_file);
                    fclose(output_file);
                    exit(1);
                }

                if(debug){
                    printf("\n\nInformacje diagnostyczne dekompresja:\n");
                }
                fseek(input_file, 5, SEEK_SET);
                int nodes_processed = 1;
                long byte_position = 0;
                rebuild_huffman_tree(&huffman_tree2, input_file, &input_byte, &input_bits,encrypted, password, compression, &nodes_processed, debug, &byte_position);
                if(debug){
                    printf("\n");
                }
                int padding = get_padding(input_file);
                int bits_left = get_left_bits(input_file);
                fseek(input_file, byte_position+1, SEEK_SET);
                decompress_huffman_file(input_file, output_file, huffman_tree2, padding, bits_left, byte_position, compression_decoding, encrypted, password, debug);

                free_huffman_tree(huffman_tree2);
                fclose(input_file);
                fclose(output_file);
                break;
            }


        }
    }
}
