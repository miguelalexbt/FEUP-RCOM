#ifndef LL_UTILS_H
#define LL_UTILS_H

#include "ll_fwd.h"

/**
 * Generates a BBC (Block Check Character) for a section of the frame.
 * @param frame Frame to generate BCC from.
 * @param start Start of the frame (inclusive).
 * @param end End of the frame (exclusive).
 * @returns Returns generated BCC.
 */
unsigned char generate_bcc(const ll_frame_t* frame, int start, int end);

/**
 * Performs byte stuffing on a frame.
 * @param frame Frame to perform byte stuffing.
 * @param size Size of the frame.
 * @returns Returns increased size.
 */
int byte_stuffing(ll_frame_t* frame, int size);

/**
 * Performs byte destuffing on a frame.
 * @param frame Frame to perform byte destuffing.
 * @param size Size of the frame.
 * @returns Returns decreased size.
 */
int byte_destuffing(ll_frame_t* frame, int size);

#endif