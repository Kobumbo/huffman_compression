#include <stdio.h>

void progress(char *message, int curr, int max, int mode){
    if(mode == 0){
        double progress = 100 * ((double)curr / max);
        printf("\r%s %0.0f%%", message, progress);
        fflush(stdout); // Make sure to flush the stdout buffer to update the output
    }else if(mode == 1){
        printf("\r%s %d", message, curr);
        fflush(stdout); // Make sure to flush the stdout buffer to update the output
    }
}
