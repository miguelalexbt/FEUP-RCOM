#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_layer/app_receiver.h"
#include "app_layer/app_debug.h"
#include "app_layer/app_layer.h"
#include "app_layer/app_packet.h"
#include "app_layer/app_io.h"
#include "api/api.h"

extern app_layer_t app_layer;

int receive_file() {

    printf(" [Application]: Receiving file...\n\n");

    app_packet_t start_packet[PACKET_SIZE];
    int spacket_size;

    if ((spacket_size = llread(app_layer.fd, (char*) start_packet)) == -1) {
        printf(" [receive_file]: error reading control packet.\n");
        exit(-1);
    }

    if (process_start_packet(start_packet, spacket_size) == -1) {
        printf(" [receive_file]: error processing START control packet.\n");
        exit(-1);
    }

    app_packet_t data_packet[4 + MAX_DATA_FIELDS];
    int dpacket_size;

    int offset = 0;
    do {
        printf("\33[2K\r");

        if ((dpacket_size = llread(app_layer.fd, (char*) data_packet)) == -1) {
            printf(" [receive_file]: error reading data packet.\n");
            exit(-1);
        }

        if (dpacket_size == 0)
            continue;

        if (process_data_packet(data_packet, &offset) == -1) {
            printf(" [receive_file]: error processing data packet.\n");
            exit(-1);
        }

        update_progress_bar(MAX_DATA_FIELDS);

    } while (offset != app_layer.file_size);

    app_packet_t end_packet[PACKET_SIZE];
    int epacket_size;
    
    if ((epacket_size = llread(app_layer.fd, (char*) end_packet)) == -1) {
        printf(" [receive_file]: error reading control packet.\n");
        exit(-1);
    }

    if (process_end_packet(start_packet, end_packet) == -1) {
        printf(" [receive_file]: error processing END control packet.\n");
        exit(-1);
    }

    if (write_file_data() == -1) {
        printf(" [receive_file]: error writing data to file.\n");
        exit(-1);
    }

    printf("\n\n [Application]: File received.\n");
    printf(" File name: %s\n", app_layer.file_path);
    printf(" File size: %d\n\n", app_layer.file_size);

    return 0;
}

int process_start_packet(const app_packet_t* packet, int packet_size) {

    if (packet == NULL || packet_size < 0) {
        printf(" [process_start_packet]: error in packet.\n");
        exit(-1);
    }

    if (packet[CONTROL_INDEX] != PACKET_START) {
        printf(" [process_start_packet]: expected a START control packet.\n");
        exit(-1);
    }

    int offset = 1;

    do {
        const int T = offset, L = offset + 1, V = offset + 2;

        switch (packet[T]) {
            case PARAM_FILE_SIZE:
                if (memmove(&app_layer.file_size, packet + V, packet[L]) == NULL) {
                    perror(" [process_start_packet]");
                    exit(-1);
                }

                if ((app_layer.file_data = (app_data_t) malloc(app_layer.file_size * sizeof(uint8_t))) == NULL) {
                    perror(" [process_start_packet]");
                    exit(-1);
                }
                break;
            case PARAM_FILE_NAME:
                if (memmove(app_layer.file_path, packet + V, packet[L]) == NULL) {
                    perror(" [process_start_packet]");
                    exit(-1);
                }
                break;
        }

        offset += 2 + packet[L];

    } while (offset != packet_size);

    return 0;
}

int process_end_packet(app_packet_t* start_packet, app_packet_t* end_packet) {

    if (start_packet == NULL || end_packet == NULL) {
        printf(" [process_end_packet]: error in packets.\n");
        exit(-1);
    }

    if (end_packet[CONTROL_INDEX] != PACKET_END) {
        printf(" [process_end_packet]: expected an END control packet.\n");
        exit(-1);
    }

    if (set_packet_header(start_packet, PACKET_END) == -1) {
        printf(" [process_end_packet]: error setting START packet's header.\n");
        exit(-1);
    }

    if (strcmp((char*)start_packet, (char*)end_packet) != 0) {
        printf(" [process_end_packet]: START and END packet are not equal.\n");
        exit(-1);
    }

    return 0;
}

int process_data_packet(const app_packet_t* packet, int* offset) {

    static uint8_t seq = 0x00;

    if (packet == NULL) {
        printf(" [process_data_packet]: error in packet.\n");
        exit(-1);
    }
    else if (offset == NULL || *offset < 0) {
        printf(" [process_data_packet]: error in offset.\n");
        exit(-1);
    }

    if (packet[CONTROL_INDEX] != (uint8_t)PACKET_DATA) {
        printf(" [process_data_packet]: expected a data packet.\n");
        exit(-1);
    }

    if (packet[SEQUENCE_INDEX] != seq++) {
        printf(" [process_data_packet]: received wrong sequence number.\n");
        exit(-1);
    }

    int data_size = packet[L2_INDEX] * 256 + packet[L1_INDEX];

    if (memmove(app_layer.file_data + *offset, packet + DATA_INDEX, data_size) == NULL) {
        perror(" [process_data_packet]");
        exit(-1);
    }

    *offset += data_size;

#ifdef APP_DEBUG_RECEIVER
    printf("\n    Packet info:\n");
    printf("    Sequence number: %d\n", (int)packet[SEQUENCE_INDEX]);
    printf("    Size: %d\n", data_size);
#endif

    return 0;
}