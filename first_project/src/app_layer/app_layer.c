#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_layer/app_layer.h"
#include "api/api.h"

int create_app_layer(app_layer_t* app_layer, int port, app_status_t status) {

    if (app_layer == NULL) {
        printf(" [create_app_layer]: error in app layer.\n");
        exit(-1);
    }
    else if (port < 0 || port > 3) {
        printf(" [create_app_layer]: error in port.\n");
        exit(-1);
    }

    app_layer->status = status;

    if ((app_layer->fd = llopen(port, app_layer->status)) == -1) {
        printf(" [create_app_layer]: error opening port.\n");
        exit(-1);
    }

    app_layer->file_data = NULL;

    app_layer->file_size = 0;

    return 0;
}

int set_app_layer_file(app_layer_t* app_layer, char* file_path) {

    if (app_layer == NULL || file_path == NULL) {
        printf(" [set_app_layer_file]: error in arguments.\n");
        exit(-1);
    }

    if (memmove(app_layer->file_path, file_path, strlen(file_path)) == NULL) {
        printf(" [set_app_layer_file]: error setting app layer file.\n");
        exit(-1);
    }

    return 0;
}

int destroy_app_layer(app_layer_t* app_layer) {

    if (app_layer == NULL) {
        printf(" [destroy_app_layer]: error in app layer.\n");
        exit(-1);
    }

    if (llclose(app_layer->fd) == -1) {
        printf(" [destroy_app_layer]: error closing port.\n");
        exit(-1);
    }

    if (app_layer->file_data != NULL) {
        free(app_layer->file_data);
        app_layer->file_data = NULL;
        app_layer->file_size = 0;
    }

    return 0;
}