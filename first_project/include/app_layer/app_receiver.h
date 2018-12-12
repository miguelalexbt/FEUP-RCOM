#ifndef APP_RECEIVER_H
#define APP_RECEIVER_H

#include "app_layer/app_fwd.h"

/**
 * Receives a file over serial port.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int receive_file();

/**
 * Processes a START packet.
 * @param packet Packet to be processed.
 * @param packet_size Size of the packet.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int process_start_packet(const app_packet_t* packet, int packet_size);

/**
 * Processes an END packet.
 * @param start_packet Start packet.
 * @param end_packet End packet.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int process_end_packet(app_packet_t* start_packet, app_packet_t* end_packet);

/**
 * Processes a DATA packet.
 * @param packet Packet to be processed.
 * @param packet_size Size of the packet.
 * @param offset Offset of the data.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int process_data_packet(const app_packet_t* packet, int* offset);

#endif