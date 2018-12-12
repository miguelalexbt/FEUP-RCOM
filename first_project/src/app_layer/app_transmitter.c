#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>

#include "app_layer/app_transmitter.h"
#include "app_layer/app_debug.h"
#include "app_layer/app_layer.h"
#include "app_layer/app_packet.h"
#include "app_layer/app_io.h"
#include "api/api.h"

extern app_layer_t app_layer;

int transmit_file(char* file_path) {
    
    printf(" [Application]: Transmitting file...\n\n");

    if (file_path == NULL) {
        printf(" [transmit_file]: error in file.\n");
        exit(-1);
    }

    if (memmove(app_layer.file_path, file_path, strlen(file_path)) == NULL) {
        printf(" [set_app_layer_file]: error setting app layer file.\n");
        exit(-1);
    }

    if (read_file_data() == -1) {
        printf(" [transmit_file]: error reading data from file.\n");
        exit(-1);
    }

    app_packet_t control_packet[PACKET_SIZE];
    int packet_size;

    if ((packet_size = create_control_packet(control_packet, app_layer.file_size, basename(app_layer.file_path))) == -1) {
        printf(" [transmit_file]: error creating control packet.\n");
        exit(-1);
    }

    if (llwrite(app_layer.fd, (char*)control_packet, packet_size) != packet_size) {
        printf(" [transmit_file]: error writing START control packet.\n");
        exit(-1);
    }

    int offset = 0, end_transmission = 0;

    do {
        printf("\33[2K\r");

        int data_size;

        if (offset + MAX_DATA_FIELDS > app_layer.file_size) {
            data_size = app_layer.file_size - offset;
            end_transmission = 1;
        } 
        else {
            data_size = MAX_DATA_FIELDS;
        }

        if (transmit_data_packet(offset, data_size) == -1) {
            printf(" [transmit_file]: error transmitting data packet.\n");
            exit(-1);
        }

        offset += MAX_DATA_FIELDS;

        update_progress_bar(MAX_DATA_FIELDS);

    } while (!end_transmission);

    set_packet_header(control_packet, PACKET_END);

    if (llwrite(app_layer.fd, (char*)control_packet, packet_size) != packet_size) {
        printf(" [transmit_file]: error writing END control packet.\n");
        exit(-1);
    }

    printf("\n\n [Application]: File transmitted.\n");
    printf(" File path: %s\n", app_layer.file_path);
    printf(" File size: %d\n\n", app_layer.file_size);

    return 0;
}

int transmit_data_packet(int offset, int data_size) {

    if (offset < 0 || data_size < 0) {
        printf(" [transmit_data_packet]: error in arguments.\n");
        exit(-1);
    }

    app_packet_t packet[4 + MAX_DATA_FIELDS];
    int packet_size;

    if ((packet_size = create_data_packet(packet, app_layer.file_data + offset, data_size)) == -1) {
        printf(" [transmit_data_packet]: error creating data packet.\n");
        exit(-1);
    }

    if (llwrite(app_layer.fd, (char*)packet, packet_size) == -1) {
        printf(" [transmit_data_packet]: error writing data packet.\n");
        exit(-1);
    }

#ifdef APP_DEBUG_TRANSMITTER
    printf("\n    Packet info:\n");
    printf("    Sequence number: %d\n", (int)packet[SEQUENCE_INDEX]);
    printf("    Size: %d\n", data_size);
#endif

    return 0;
} 