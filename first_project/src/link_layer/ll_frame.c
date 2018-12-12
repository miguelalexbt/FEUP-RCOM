#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "link_layer/ll_frame.h"
#include "link_layer/ll_debug.h"
#include "link_layer/ll_utils.h"

int set_frame_header(ll_frame_t* frame, ll_addr_t address, ll_ctrl_t control) {

    if (frame == NULL) {
        printf(" [set_frame_header]: error in frame.\n");
        exit(-1);
    }

    // Delimitation flag
    frame[FLAG_INDEX] = FLAG_DELIM;

    // Address flag
    frame[ADDR_INDEX] = address;

    // Control flag
    frame[CTRL_INDEX] = control;

    // Header's BCC
    frame[BCC_INDEX] = generate_bcc(frame, ADDR_INDEX, BCC_INDEX);

    return 0;
}

int set_frame_data(ll_frame_t* frame, const ll_packet_t packet, int packet_size) {

    if (frame == NULL) {
        printf(" [set_frame_data]: error in frame.\n");
        exit(-1);
    }
    else if (packet == NULL || packet_size < 0) {
        printf(" [set_frame_data]: error in packet.\n");
        exit(-1);
    }

    if (memmove(frame + DATA_INDEX, packet, packet_size) == NULL) {
        perror(" [set_frame_data]");
        exit(-1);
    }

    return 0;
}

int set_frame_trailer(ll_frame_t* frame, int offset) {

    if (frame == NULL) {
        printf(" [set_frame_trailer]: error in frame.\n");
        exit(-1);
    }
    else if (offset < 0) {
        printf(" [set_frame_trailer]: error in offset.\n");
        exit(-1);
    }

    if (offset > DATA_INDEX) {
        
        // Packet's BCC
        frame[offset] = generate_bcc(frame, DATA_INDEX, offset);
        ++offset;
    }

    // Delimitation flag
    frame[offset] = FLAG_DELIM;

    return 0;
}

int create_iframe(ll_frame_t* frame, unsigned int s, ll_addr_t address, const ll_packet_t packet, int packet_size) {

    if (frame == NULL) {
        printf(" [create_iframe]: error in frame.\n");
        exit(-1);
    }
    else if (packet == NULL || packet_size < 0 || packet_size > FRAME_SIZE - 6) {
        printf(" [set_frame_data]: error in packet.\n");
        exit(-1);
    }
    else if (s != 0 && s != 1) {
        printf(" [set_frame_data]: error in sequence number.\n");
        exit(-1);
    }

    int frame_size = 6 + packet_size;

    if (frame_size > FRAME_SIZE) {
        printf(" [create_iframe]: error in frame size.\n");
        exit(-1);
    }

    if (set_frame_header(frame, address, s ? CMND_INFO_S : CMND_INFO) == -1) {
        printf(" [create_iframe]: error setting frame's header.\n");
        exit(-1);
    }

    if (set_frame_data(frame, packet, packet_size) == -1) {
        printf(" [create_iframe]: error setting frame data.\n");
        exit(-1);
    }

    if (set_frame_trailer(frame, DATA_INDEX + packet_size) == -1) {
        printf(" [create_iframe]: error setting frame trailer.\n");
        exit(-1);
    }

    if ((frame_size = byte_stuffing(frame, frame_size)) == -1) {
        printf(" [create_iframe]: error byte stuffing.\n");
        exit(-1);
    }

    if (frame_size > FRAME_SIZE) {
        printf(" [create_iframe]: error in frame size.\n");
        exit(-1);
    }

#ifdef LL_DEBUG_FRAME
    printf("\n    Created I frame (size: %d):\n", frame_size);
    int i;
    for (i = 0; i < frame_size; ++i) printf("    [%.2d] -> 0x%.2x\n", i, frame[i]);
#endif

    return frame_size;
}

int create_suframe(ll_frame_t* frame, unsigned int r, ll_addr_t address, ll_ctrl_t control) {

    if (frame == NULL) {
        printf(" [create_suframe]: error in frame.\n");
        exit(-1);
    }
    else if (r != 0 && r != 1) {
        printf(" [set_frame_data]: error in sequence number.\n");
        exit(-1);
    }

    int frame_size = 5;

    // Ensure r = 0 for these control flags
    if (control == CMND_SET || control == CMND_DISC || control == RESP_UA)
        r = 0;

    if (set_frame_header(frame, address, r ? control | (uint8_t)(0x01 << 7) : control) == -1) {
        printf(" [create_suframe]: error setting frame's header.\n");
        exit(-1);
    }

    if (set_frame_trailer(frame, DATA_INDEX) == -1) {
        printf(" [create_suframe]: error setting frame's trailer.\n");
        exit(-1);
    }

#ifdef LL_DEBUG_FRAME
    printf("\n    Created S/U frame (size: %d):\n", frame_size);
    int i;
    for (i = 0; i < frame_size; ++i) printf("    [%.2d] -> 0x%.2x\n", i, frame[i]);
#endif

    return frame_size;
}