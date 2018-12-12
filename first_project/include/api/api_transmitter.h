#ifndef API_TRANSMITTER_H
#define API_TRANSMITTER_H

#include "link_layer/ll_fwd.h"

/**
 * Connects transmitter.
 * @param fd File descriptor.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int transmitter_connect(int fd);

/**
 * Disconnects transmitter.
 * @param fd File descriptor.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int transmitter_disconnect(int fd);

/**
 * Transmits data over serial port.
 * @param fd File descriptor.
 * @param packet Packet to transmit.
 * @param packet_size Size of the packet.
 * @returns Returns bytes written, -2 if received an unexpected response, -1 otherwise.
 */
int transmit_data(int fd, const ll_packet_t packet, int packet_size);


#endif