#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include <termios.h>

#include "ll_fwd.h"

/** Link layer settings */
enum ll_settings {
    LL_BAUDRATE     = B1152000,
    LL_TIME_OUT     = 1,
    LL_MAX_TRIES    = 3,
    LL_BUF_SIZE     = 2048
};

/** Link layer structure. */
typedef struct {

    /** Old termios structure. */
    struct termios oldtio;

    /** Baudrate */
    int baudrate;

    /** Current sequence number */
    unsigned int sequence_number;

    /** Current timeout */
    int time_out;

    /** Current number of tries */
    int max_tries;

    /** Buffer */
    uint8_t buffer[LL_BUF_SIZE];

    /** Buffer's size */
    int buffer_size;
    
} link_layer_t;

/**
 * Initializes the link layer structure.
 * @param link_layer Structure to be initialized.
 * @param oldtio Old termios structure.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int create_link_layer(link_layer_t* link_layer, struct termios oldtio);

/**
 * Sets the frame of the link layer.
 * @param link_layer Owner of the frame,
 * @param frame Frame with data to set.
 * @param frame_size Frame's size.
 * @returns Returns 0 if successful, -1 otherwise
 */
int set_link_layer_buffer(link_layer_t* link_layer, ll_frame_t* frame, int frame_size);

#endif