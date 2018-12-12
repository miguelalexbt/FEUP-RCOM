#ifndef LL_FWD_H
#define LL_FWD_H

#include <stdint.h>

/** Delimitation flag */
static const uint8_t FLAG_DELIM = 0x7e;

/** Escape flag */
static const uint8_t FLAG_ESC   = 0x7d;

/** XOR octet */
static const uint8_t XOR_OCT    = 0x20;
 
/** Frame type */
typedef uint8_t ll_frame_t;

/** Packet type */
typedef uint8_t* ll_packet_t;

/** Address type */
typedef enum {

    /** Emissor owner/target */
    ADDR_EMI = 0x03,

    /** Receptor owner/target */
    ADDR_REC = 0x01
} ll_addr_t;

/** Control type */
typedef enum {

    /** Information command */
    CMND_INFO   = 0x00,

    /** Information command shifted */
    CMND_INFO_S = 0x40,

    /** Set command */
    CMND_SET    = 0x03,

    /** Disc command */
    CMND_DISC   = 0x0b,

    /** Unnumbered acknowledgment response */
    RESP_UA     = 0x07,

    /** Receive response */
    RESP_RR     = 0x05,

    /** Receive response shifted */
    RESP_RR_S   = 0x85, 

    /** Reject response */
    RESP_REJ    = 0x01,

    /** Reject response shifted */
    RESP_REJ_S  = 0x81
} ll_ctrl_t;

/** Frame indexes */
enum frame_index {

    /** Flag index */
    FLAG_INDEX  = 0,

    /** Address index */
    ADDR_INDEX  = 1,

    /** Control index */
    CTRL_INDEX  = 2,

    /** BCC index */
    BCC_INDEX   = 3,

    /** Data index */
    DATA_INDEX  = 4
};

#endif
