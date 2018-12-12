#ifndef APP_LAYER_H
#define APP_LAYER_H

#include "app_layer/app_fwd.h"

/** App layer status */
typedef enum {

    /** Transmitter role */
    TRANSMITTER = 0,

    /** Receiver role */
    RECEIVER    = 1
} app_status_t;

/** App layer structure */
typedef struct {
    
    /** File descriptor */
    int fd;

    /** Status */
    app_status_t status;

    /** File path */
    char file_path[256];

    /** File data */
    app_data_t file_data;

    /** File size */
    int file_size;
} app_layer_t;

/**
 * Initializes the app layer structure.
 * @param app_layer Structure to be initialized.
 * @param port Serial port to be used.
 * @param status Status of the host.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int create_app_layer(app_layer_t* app_layer, int port, app_status_t status);

/**
 * Sets the app layer file.
 * @param app_layer Structure to be set.
 * @param file_path File path to set.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int set_app_layer_file(app_layer_t* app_layer, char* file_path);

/**
 * Destroys the link layer structure.
 * @param app_layer Structure to be destroyed.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int destroy_app_layer(app_layer_t* app_layer);

#endif
