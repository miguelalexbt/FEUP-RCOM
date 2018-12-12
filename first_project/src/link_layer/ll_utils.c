#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "link_layer/ll_utils.h"
#include "link_layer/ll_debug.h"

unsigned char generate_bcc(const ll_frame_t* frame, int start, int end) {

    unsigned char bcc = frame[start];

    // Calculate XOR of elements between [start, end[
    int i;
    for (i = start + 1; i < end; ++i)
        bcc ^= frame[i];
        
#ifdef LL_DEBUG_BCC
    printf("\n    Protected octets (size: %d):\n", end - start);
    int d;
    for (d = start; d < end; ++d) printf("    [%.2d] -> 0x%.2x\n", d, frame[d]);
    printf("\n    Generated BBC:\n [%.2d] -> 0x%.2x\n", end, bcc);
#endif

    return bcc;
}

int byte_stuffing(ll_frame_t* frame, int size) {

    if (frame == NULL || size < 0) {
        printf(" [byte_stuffing]: error in frame.\n");
        exit(-1);
    }

#ifdef LL_DEBUG_BS
    printf("\n    Frame before byte stuffing (size: %d):\n", size);
    int d;
    for (d = 0; d < size; ++d) printf("    [%.2d] -> 0x%.2x\n", d, frame[d]);
#endif

    int i, current_size = size;

    // Start at first data byte and end at data's BCC
    for (i = DATA_INDEX; i < current_size - 1; ++i) {

        if (frame[i] == FLAG_DELIM || frame[i] == FLAG_ESC) {

            if (memmove(frame + i + 1, frame + i, current_size - i) == NULL) {
                perror(" [byte_stuffing]");
                exit(-1);
            }

            frame[i + 1] = frame[i] ^ XOR_OCT;
            frame[i] = FLAG_ESC;

            ++current_size;
        }
    }

#ifdef LL_DEBUG_BS
    printf("\n    Frame after byte stuffing (size: %d):\n", current_size);
    for (d = 0; d < current_size; ++d) printf("    [%.2d] -> 0x%.2x\n", d, frame[d]);
#endif

    return current_size;
}

int byte_destuffing(ll_frame_t* frame, int size) {

    if (frame == NULL || size < 0) {
        printf(" [byte_destuffing]: error in frame.\n");
        exit(-1);
    }
    
#ifdef LL_DEBUG_BS
    printf("\n    Frame before byte destuffing (size: %d):\n", size);
    int d;
    for (d = 0; d < size; ++d) printf("    [%.2d] -> 0x%.2x\n", d, frame[d]);
#endif

    int i, current_size = size;

    // Start at first data byte and end at data's BCC
    for (i = DATA_INDEX; i < current_size - 1; ++i) {

        if (frame[i] == FLAG_ESC) {

            if (memmove(frame + i, frame + i + 1, current_size - i) == NULL) {
                perror(" [byte_destuffing]");
                exit(-1);
            }

            frame[i] = frame[i] ^ XOR_OCT;

            --current_size;
        }
    }

#ifdef LL_DEBUG_BS
    printf("\n    Frame after byte destuffing (size: %d):\n", current_size);
    for (d = 0; d < current_size; ++d) printf("    [%.2d] -> 0x%.2x\n", d, frame[d]);
#endif

    return current_size;
}
