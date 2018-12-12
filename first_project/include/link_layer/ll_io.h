#ifndef LL_IO_H
#define LL_IO_H

#include "ll_fwd.h"

/** IO return types */
enum ll_io_ret {
    IO_TO       = -2,
    IO_INV_DATA = -3,
    IO_REP_DATA = -4
};

/**
 * Sets off when an alarm finishes.
 */
void timeout();

/**
 * Starts IO operations.
 * @param port Serial port to open.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int open_port(char* port);

/**
 * Stops IO operations.
 * @param fd File descriptor to close.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int close_port(int fd);

/** 
 * Writes buffer to serial port.
 * @param fd File descriptor.
 * @returns Returns bytes writen if successful, -1 otherwise.
 */
int write_port(int fd);

/**
 * Reads buffer of serial port.
 * @param fd File descriptor.
 * @returns Returns bytes read if successful, -1 otherwise.
 */
int read_port(int fd);

/**
 * Puts an I frame in the link layer buffer.
 * @param packet Packet to put.
 * @param packet_size Packet size.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int puts_iframe(ll_packet_t packet, int packet_size);

/**
 * Puts a SU frame in the link layer buffer.
 * @param control Control byte of the frame.
 * @param address Address byte of the frame.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int puts_suframe(ll_ctrl_t control, ll_addr_t address);

#endif