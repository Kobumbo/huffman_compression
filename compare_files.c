#include <stdio.h>
#include <stdlib.h>

int compare_files(FILE *file1, FILE *file2){
    char ch1, ch2;
    while(1){
        ch1 = fgetc(file1);
        ch2 = fgetc(file2);

        if(feof(file1) != feof(file2) || ch1 != ch2){
            return 0;
        }

        if(feof(file1) && feof(file2)){
            return 1;
        }
    }
}

int main(int argc, char **argv){
    if(argc != 3){
        fprintf(stderr, "Poprawne uzycie: ./%s <plik1> <plik2>\n", argv[0]);
        exit(1);
    }

    FILE *file1 = fopen(argv[1], "r");
    FILE *file2 = fopen(argv[2], "r");

    if(!file1 || !file2){
        fprintf(stderr, "Blad otwierania plikow\n");
        exit(1);
    }

    if(compare_files(file1, file2)){
        printf("Oryginalny plik: %s | Plik po dekompresji: %s |", argv[1], argv[2]);
        printf("\033[0;32m");
        printf(" Passed!\n");
        printf("\033[0m");
    }else{
        printf("Oryginalny plik: %s | Plik po dekompresji: %s |", argv[1], argv[2]);
        printf("\033[0;31m");
        printf(" Failed!\n");
        printf("\033[0m");
    }
    fclose(file1);
    fclose(file2);
}

