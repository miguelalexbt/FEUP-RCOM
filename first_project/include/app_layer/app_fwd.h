#ifndef APP_FWD_H
#define APP_FWD_H

#include <stdint.h>

/** Packet type */
typedef uint8_t app_packet_t;

/** Data type */
typedef uint8_t* app_data_t;

/** Control flags */
typedef enum {

    /** Data packet */
    PACKET_DATA    = 1,

    /** Start control packet */
    PACKET_START   = 2,

    /** End control packet */
    PACKET_END     = 3
} app_control_t;

/** Parameter flags */
typedef enum {

    /** File size parameter */
    PARAM_FILE_SIZE = 0,

    /** File name parameter */
    PARAM_FILE_NAME = 1
} app_param_t;

/** Packet indexes */
enum packet_index {

    /** Control index */
    CONTROL_INDEX   = 0,

    /** Sequence index */
    SEQUENCE_INDEX  = 1,

    /** L2 index */
    L2_INDEX        = 2,

    /** L1 index */
    L1_INDEX        = 3,

    /** Data index */
    DATA_INDEX      = 4
};

#endif