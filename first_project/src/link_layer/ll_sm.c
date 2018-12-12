#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "link_layer/ll_sm.h"
#include "link_layer/ll_test.h"
#include "link_layer/ll_utils.h"

/** State machine's current state */
static ll_sm_state_t current_state;

/** Sequence number */
static unsigned int seq_number;

void ll_initialize_sm(unsigned int number) {
    current_state = RCV_NIL;
    seq_number = number;
}

int ll_sm_handler(uint8_t byte, ll_frame_t* frame, int* offset) {

    if (frame == NULL) {
        printf(" [ll_sm_handler]: error in frame.\n");
        exit(-1);
    }
    else if (offset == NULL || *offset < 0) {
        printf(" [ll_sm_handler]: error in offset.\n");
        exit(-1);
    }

#ifdef LL_TEST
    srand(time(NULL));

    if (current_state > RCV_NIL && current_state < RCV_BCC) {
        if (rand() < (1 - pow(1 - HEADER_BER, 8)) * ((double) RAND_MAX + 1.0))
            byte = rand() % 256;
    }
    else if (current_state == RCV_DATA) {
        if (rand() < (1 - pow(1 - DATA_BER, 8)) * ((double) RAND_MAX + 1.0))
            byte = rand() % 256;
    }
#endif

    switch (current_state) {
        case RCV_NIL:
            if (byte == FLAG_DELIM) {
                frame[(*offset)++] = byte; 
                current_state = RCV_F;
            }
            break;
        case RCV_F:
            if (byte == ADDR_EMI || byte == ADDR_REC) {
                frame[(*offset)++] = byte;
                current_state = RCV_ADDR;
            } 
            else if (byte != FLAG_DELIM) {
                *offset = FLAG_INDEX;
                current_state = RCV_NIL;
            }
            break;
        case RCV_ADDR:
            if (byte == CMND_SET || byte == CMND_DISC || byte == RESP_UA ||
                byte == (seq_number ? RESP_RR_S : RESP_RR) ||
                byte == (!seq_number ? RESP_REJ_S : RESP_REJ) ||
                byte == (seq_number ? CMND_INFO_S : CMND_INFO)) {
                frame[(*offset)++] = byte;
                current_state = RCV_CTRL;
            } else if (byte == (!seq_number ? CMND_INFO_S : CMND_INFO)) {
                current_state = REP_DATA;
            }
            else if (byte == FLAG_DELIM) {
                *offset = ADDR_INDEX;
                current_state = RCV_F;
            } 
            else {
                *offset = FLAG_INDEX;
                current_state = RCV_NIL;
            }
            break;
        case RCV_CTRL:
            if (byte == FLAG_DELIM) {
                *offset = ADDR_INDEX;
                current_state = RCV_F;
            } 
            else if (byte == generate_bcc(frame, ADDR_INDEX, BCC_INDEX)) {
                frame[(*offset)++] = byte;
                current_state = RCV_BCC;
            }
            else {
                *offset = FLAG_INDEX;
                current_state = RCV_NIL;
            }
            break;
        case RCV_BCC:
            if (byte == FLAG_DELIM) {
                frame[(*offset)++] = byte;
                current_state = RCV_FRAME;
            }
            else {
                frame[(*offset)++] = byte;
                current_state = RCV_DATA;
            }
            break;
        case RCV_DATA:
            if (byte == FLAG_DELIM) {

                frame[(*offset)++] = byte;

                if ((*offset = byte_destuffing(frame, *offset)) == -1) {
                    printf(" [sm_handler]: error byte destuffing.\n");
                    exit(-1);
                }

                int packet_size = *offset - 6;

                if (frame[DATA_INDEX + packet_size] != generate_bcc(frame, DATA_INDEX, DATA_INDEX + packet_size))
                    current_state = INV_DATA;
                else 
                    current_state = RCV_FRAME;
            }
            else {
                frame[(*offset)++] = byte;
            }
            break;
        case RCV_FRAME:
            break;
        default:
            printf(" [ll_sm_handler]: unexpected state.\n");
            exit(-1);
    }

    return current_state;
}