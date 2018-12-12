#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_layer/app_packet.h"
#include "app_layer/app_debug.h"

int set_packet_header(app_packet_t* packet, app_control_t control) {

    if (packet == NULL) {
        printf(" [set_packet_header]: error in packet.\n");
        exit(-1);
    }

    packet[CONTROL_INDEX] = (uint8_t)control;

    return 0;
}

int set_packet_data(app_packet_t* packet, const app_data_t data, int data_size) {

    if (packet == NULL) {
        printf(" [set_packet_data]: error in packet.\n");
        exit(-1);
    }
    else if (data == NULL || data_size < 0) {
        printf(" [set_packet_data]: error in data.\n");
        exit(-1);
    }
    
    static uint8_t sequence_number = 0;

    // Sequence number
    packet[SEQUENCE_INDEX] = sequence_number++;

    // L2
    packet[L2_INDEX] = (uint8_t) (data_size / 256);
    
    // L1
    packet[L1_INDEX] = (uint8_t) (data_size % 256);

    if (memmove(packet + DATA_INDEX, data, data_size) == NULL) {
        perror(" [set_packet_data]");
        exit(-1);
    }

    return 0;
}

int create_control_packet(app_packet_t* packet, int file_size, char* file_name) {

    if (packet == NULL) {
        printf(" [create_control_packet]: error in packet.\n");
        exit(-1);
    }
    else if (file_size < 0 || file_name == NULL) {
        printf(" [create_control_packet]: error in file.\n");
        exit(-1);
    }

    int file_size_oct = 4;
    int file_name_oct = strlen(file_name);

    int packet_size = 1 + 2 * 2 + file_size_oct + file_name_oct;

    if (packet_size > PACKET_SIZE) {
        printf(" [create_control_packet]: packet size is too big.\n");
        exit(-1);
    }

    if (set_packet_header(packet, PACKET_START) == -1) {
        printf(" [create_control_packet]: error setting packet's header.\n");
        exit(-1);
    }

    int offset = 1;

    // File size
    packet[offset++] = PARAM_FILE_SIZE;
    packet[offset++] = file_size_oct;

    if (memmove(packet + offset, &file_size, file_size_oct) == NULL) {
        perror(" [create_control_packet]");
        exit(-1);
    }

    offset += file_size_oct;

    // File name
    packet[offset++] = PARAM_FILE_NAME;
    packet[offset++] = file_name_oct;

    if (memmove(packet + offset, file_name, file_name_oct) == NULL) {
        perror(" [create_control_packet]");
        exit(-1);
    }

#ifdef APP_DEBUG_PACKET
    printf("\n Created control packet (size: %d):\n", packet_size);
    int i;
    for (i = 0; i < packet_size; ++i) 
        printf(" [%.2d] -> 0x%.2x\n", i, packet[i]);
    printf(" -----\n");
#endif

    return packet_size;
}

int create_data_packet(app_packet_t* packet, const app_data_t data, int data_size) {

    if (packet == NULL) {
        printf(" [create_data_packet]: error in packet.\n");
        exit(-1);
    }
    else if (data == NULL || data_size < 0) {
        printf(" [create_data_packet]: error in data.\n");
        exit(-1);
    }

    int packet_size = 4 + data_size;

    if (packet_size > PACKET_SIZE) {
        printf(" [create_data_packet]: packet size is too big.\n");
        exit(-1);
    }

    if (set_packet_header(packet, PACKET_DATA) == -1) {
        printf(" [create_data_packet]: error setting packet's header.\n");
        exit(-1);
    }

    if (set_packet_data(packet, data, data_size) == -1) {
        printf(" [create_data_packet]: error setting packet data.\n");
        exit(-1);
    }

#ifdef APP_DEBUG_PACKET
    printf("\n Created data packet (size: %d):\n", packet_size);
    int i;
    for (i = 0; i < packet_size; ++i) 
        printf(" [%.2d] -> 0x%.2x\n", i, packet[i]);
    printf(" -----\n");
#endif

    return packet_size;
}