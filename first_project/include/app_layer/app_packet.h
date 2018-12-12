#ifndef APP_PACKET_H
#define APP_PACKET_H

#include "app_fwd.h"

/** Packet settings */
enum packet_settings {
    PACKET_SIZE     = 1000,
    MAX_DATA_FIELDS = 100
};

/**
 * Sets the header for the packet.
 * @param packet Packet to be set.
 * @param control Control flag of the header.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int set_packet_header(app_packet_t* packet, app_control_t control);

/**
 * Sets the data for the packet.
 * @param packet Packet to be set.
 * @param data Data that will be set.
 * @param data_size Size of the data.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int set_packet_data(app_packet_t* packet, const app_data_t data, int data_size);

/**
 * Creates a control packet.
 * @param packet Packet pointer that will hold the packet.
 * @param data_size Size of the data.
 * @returns Returns packet size if successful, -1 otherwise.
 */
int create_control_packet(app_packet_t* packet, int file_size, char* file_name);

/**
 * Creates a data packet.
 * @param packet Packet pointer that will hold the packet.
 * @param data Data of the packet.
 * @param data_size Size of the data.
 * @returns Returns packet size if successful, -1 otherwise.
 */
int create_data_packet(app_packet_t* packet, const app_data_t data, int data_size);


#endif