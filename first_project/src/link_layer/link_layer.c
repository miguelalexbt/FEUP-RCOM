#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "link_layer/link_layer.h"
#include "link_layer/ll_debug.h"
#include "link_layer/ll_test.h"

int create_link_layer(link_layer_t* link_layer, struct termios oldtio) {

    if (link_layer == NULL) {
        printf(" [create_link_layer]: error in link layer.\n");
        exit(-1);
    }

    link_layer->oldtio = oldtio;
    link_layer->baudrate = LL_BAUDRATE;
    link_layer->sequence_number = 0;
    link_layer->time_out = LL_TIME_OUT;
    link_layer->max_tries = LL_MAX_TRIES;
    link_layer->buffer_size = 0;

#ifdef LL_DEBUG_LLB
    printf("\n    Initialized link layer: \n");
    printf("    Baudrate: %d\n", link_layer->baudrate);
    printf("    Sequence number: %d\n", link_layer->sequence_number);
    printf("    Time out: %d\n", link_layer->time_out);
    printf("    Max tries: %d\n", link_layer->max_tries);
    printf("    Buffer size: %d\n", link_layer->buffer_size);
#endif

    return 0;
}

int set_link_layer_buffer(link_layer_t* link_layer, ll_frame_t* frame, int frame_size) {

    if (link_layer->buffer == NULL) {
        printf(" [set_link_layer_buffer]: error in buffer.\n");
        exit(-1);
    }
    else if (frame == NULL || frame_size <= 0 || frame_size > LL_BUF_SIZE) {
        printf(" [set_link_layer_buffer]: error in frame.\n");
        exit(-1);
    }
    
    if (memmove(link_layer->buffer, frame, frame_size) == NULL) {
        perror(" [set_link_layer_buffer]");
        exit(-1);
    }

    link_layer->buffer_size = frame_size;

#ifdef DEBUG_LLB
    printf("\n    Link layer buffer's size %d:\n", link_layer->buffer_size);
#endif

    return 0;
}