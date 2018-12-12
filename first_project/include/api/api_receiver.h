#ifndef API_RECEIVER_H
#define API_RECEIVER_H

#include "link_layer/ll_fwd.h"

/**
 * Connects receiver.
 * @param fd File descriptor.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int receiver_connect(int fd);

/**
 * Disconnects receiver.
 * @param fd File descriptor.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int receiver_disconnect(int fd);

/**
 * Receives data over serial port.
 * @param fd File descriptor.
 * @param frame Frame to be received.
 * @returns Returns bytes read, -2 if received an unexpected packet, -1 otherwise.
 */
int receive_data(int fd, ll_packet_t packet);

#endif