#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <math.h>

#include "app_layer/app_io.h"
#include "app_layer/app_debug.h"
#include "app_layer/app_layer.h"

/** App layer structure */
app_layer_t app_layer;

/** File to write/read data */
static FILE* file;

int open_file(char* file_path, char* flags) {

    if (file_path == NULL || flags == NULL) {
        printf(" [open_file]: error in arguments.\n");
        exit(-1);
    }

    if (strlen(basename(file_path)) > 256) {
        printf(" [open_file]: file name is too long.\n");
        exit(-1);
    }

    if ((file = fopen(file_path, flags)) == NULL) {
        perror(" [open_file]");
        exit(-1);
    }

    if (memmove(app_layer.file_path, file_path, strlen(file_path)) == NULL) {
        printf(" [open_file]");
        exit(-1);
    }


#ifdef APP_DEBUG_TRANSMITTER
    printf("\n [Application]: Opening file.\n");
#endif

    return 0;
}

int close_file() {

    if (file == NULL) {
        printf(" [close_file]: error in file.\n");
        exit(-1);
    }

    if (fclose(file) == -1) {
        perror(" [close_file]");
        exit(-1);
    }

#ifdef APP_DEBUG_TRANSMITTER
    printf("\n [Application]: Closing file.\n");
#endif

    return 0;
}

int write_file_data() {

    if (open_file(app_layer.file_path, "w") == -1) {
        printf(" [write_file_data]: error opening file.\n");
        exit(-1);
    }

    if (fwrite(app_layer.file_data, app_layer.file_size, 1, file) != 1) {
        perror(" [write_file_data]");
        exit(-1);
    }

    if (close_file() == -1) {
        printf(" [receive_file]: error closing file.\n");
        exit(-1);
    }


#ifdef APP_DEBUG_TRANSMITTER
    printf("\n [Application]: Wrote data to file.\n");
#endif


    return 0;
}

int read_file_data() {

    if (open_file(app_layer.file_path, "rb") == -1) {
        printf(" [read_file_data]: error opening file.\n");
        exit(-1);
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror(" [read_file_data]");
        exit(-1);
    }

    if ((app_layer.file_size = ftell(file)) == -1L) {
        perror(" [read_file_data]");
        exit(-1);
    }

    rewind(file);

    if ((app_layer.file_data = (app_data_t) malloc(app_layer.file_size * sizeof(uint8_t))) == NULL) {
        perror(" [read_file_data]");
        exit(-1);
    }

    if (fread(app_layer.file_data, app_layer.file_size, 1, file) != 1) {
        printf(" [read_file_data]: error reading file.\n");
        exit(-1);
    }

    if (close_file() == -1) {
        printf(" [read_file_data]: error closing file.\n");
        exit(-1);
    }

#ifdef APP_DEBUG_TRANSMITTER
    printf("\n [Application]: Read data from file.\n");
#endif

    return 0;
}

void update_progress_bar(int bytes_per_packet) {

    static int current = 0;
    current++;

    const int max = ceil((double)app_layer.file_size / bytes_per_packet);

    int n = (current * 100) / max;

    printf(" %d%% [", n);
    
    int i = 0;
    do {
        if (i <= ceil((double)n / 2)) printf("#");
        else printf(" ");
    } while (i++ < 50);

    printf("] (%d, %d)", current, max);

    fflush(stdout);
}  