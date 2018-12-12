#include <stdio.h>
#include <stdlib.h>

#include "app_layer/app_layer.h"
#include "app_layer/app_transmitter.h"
#include "app_layer/app_receiver.h"
#include "link_layer/ll_test.h"

extern app_layer_t app_layer;

int main(int argc, char* argv[]) {

#ifdef LL_TEST
    start_test();
#endif

    if (argc < 2 || argc > 3) {
        printf(" Usage: serial <num_port> [file-path]\n");
        exit(-1);
    }

    if (create_app_layer(&app_layer, atoi(argv[1]), argc == 3 ? TRANSMITTER : RECEIVER) == -1) {
        printf(" [main]: error creating app layer.\n");
        exit(-1);
    }

    switch (app_layer.status) {
        case TRANSMITTER:
            if(transmit_file(argv[2]) == -1) {
                printf(" [main]: error transmitting file.\n");
                exit(-1);
            }
            break;
        case RECEIVER:
            if(receive_file() == -1) {
                printf(" [main]: error receiving file.\n");
                exit(-1);
            }
            break;
    }

    if (destroy_app_layer(&app_layer) == -1) {
        printf(" [main]: error destroying app layer.\n");
        exit(-1);
    }

#ifdef LL_TEST
    end_test();
#endif

    return 0;
}