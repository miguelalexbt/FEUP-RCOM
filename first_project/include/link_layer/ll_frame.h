#ifndef LL_FRAME_H
#define LL_FRAME_H

#include "ll_fwd.h"

/** Frame settings */
enum frame_settings {
    FRAME_SIZE = 2048,
};

/**
 * Sets the header for the frame.
 * @param frame Frame to be set.
 * @param address Address of the frame.
 * @param control Control flag of the header.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int set_frame_header(ll_frame_t* frame, ll_addr_t address, ll_ctrl_t control);

/**
 * Sets the data for the frame.
 * @param frame Frame to be set.
 * @param packet Packet that will be set.
 * @param packet_size Size of the packet.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int set_frame_data(ll_frame_t* frame, const ll_packet_t packet, int packet_size);

/**
 * Sets the trailer for the frame.
 * @param frame Frame to be set.
 * @param offset Offset for the trailer.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int set_frame_trailer(ll_frame_t* frame, int offset);

/**
 * Creates an information frame.
 * @param dest Frame pointer that will hold the frame.
 * @param s Number s of the frame.
 * @param address Address of the frame.
 * @param packet Packet of the frame.
 * @param packet_size Size of the packet.
 * @returns Returns frame size if successful, -1 otherwise.
 */
int create_iframe(ll_frame_t* frame, unsigned int s, ll_addr_t address, const ll_packet_t packed, int packet_size);

/**
 * Creates an supervision/unnumbered frames.
 * @param dest Frame pointer that will hold the frame.
 * @param n Number n of the frame.
 * @param address Address of the frame.
 * @param control Control flag of the frame
 * @returns Returns frame size if successful, -1 otherwise.
 */
int create_suframe(ll_frame_t* frame, unsigned int n, ll_addr_t address, ll_ctrl_t control);

#endif