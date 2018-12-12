#ifndef LL_SM_H
#define LL_SM_H

#include "ll_fwd.h"

/** State machine states */
typedef enum {

    /** Received nothing */
    RCV_NIL,

    /** Received delimitation flag */
    RCV_F,

    /** Received address */
    RCV_ADDR,

    /** Received control */
    RCV_CTRL,

    /** Received header's BCC */
    RCV_BCC,

    /** Received data */
    RCV_DATA,

    /** Received frame */
    RCV_FRAME,

    /** Invalid data */
    INV_DATA,

    /** Repeated data */
    REP_DATA
} ll_sm_state_t;

/**
 * Initializes state machine.
 */
void ll_initialize_sm(unsigned int number);

/**
 * Handles events.
 * @param byte Byte received.
 * @param frame Frame to be filled.
 * @param frame_size Size of the frame.
 * @param offset Offset of the frame.
 * @returns Returns state machine's current state.
 */
int ll_sm_handler(uint8_t byte, ll_frame_t* frame, int* offset);

#endif