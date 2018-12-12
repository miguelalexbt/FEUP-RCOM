#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "api/api_transmitter.h"
#include "link_layer/ll_io.h"
#include "link_layer/link_layer.h"

extern link_layer_t ll_layer;

int transmitter_connect(int fd) {

    if (fd < 0) {
        printf(" [transmitter_connect]: error in file descriptor\n");
        exit(-1);
    }

    printf(" [Transmitter]: Connecting to receiver...\n");

    int try = 0;
    do {
        if (puts_suframe(CMND_SET, ADDR_EMI) == -1) {
            printf(" [transmitter_connect]: error putting SET frame in buffer.\n");
            exit(-1);
        }

        if (write_port(fd) == -1) {
            printf(" [transmitter_connect]: error writing buffer to serial port.\n");
            exit(-1);
        }

        alarm(ll_layer.time_out);

        int ret;
        if ((ret = read_port(fd)) == -1) {
            printf(" [transmitter_connect]: error reading buffer from serial port.\n");
            exit(-1);
        }

        alarm(0);

        if (ret == IO_TO) {
            printf(" [Transmitter]: Time-out.\n");
            continue;
        }

        if (ll_layer.buffer[CTRL_INDEX] != RESP_UA) {
            printf(" [Transmitter]: Unexpected response received.\n");
            continue;
        }

        break;

    } while (++try < ll_layer.max_tries);

    if (try == ll_layer.max_tries) {
        printf(" [Transmitter]: Exceeded maximum number of tries.\n");
        exit(-1);
    }

    printf(" [Transmitter]: Connected.\n");

    return 0;
}

int transmitter_disconnect(int fd) {

    if (fd < 0) {
        printf(" [transmitter_disconnect]: error in file descriptor\n");
        exit(-1);
    }

    printf(" [Transmitter]: Disconnecting from receiver...\n");

    int try = 0;

    do {
        if (puts_suframe(CMND_DISC, ADDR_EMI) == -1) {
            printf(" [transmitter_disconnect]: error putting DISC frame to buffer.\n");
            exit(-1);
        }

        if (write_port(fd) == -1) {
            printf(" [transmitter_disconnect]: error writing buffer to serial port.\n");
            exit(-1);
        }

        alarm(ll_layer.time_out);

        int ret;
        if ((ret = read_port(fd)) == -1) {
            printf(" [transmitter_disconnect]: error reading buffer from serial port.\n");
            exit(-1);
        }

        alarm(0);

        if (ret == IO_TO) {
            printf(" [Transmitter]: Time-out.\n");
            continue;   
        }

        if (ll_layer.buffer[CTRL_INDEX] != CMND_DISC) {
            printf(" [Transmitter]: Unexpected command received.\n");
            continue;
        }

        if (puts_suframe(RESP_UA, ADDR_REC) == -1) {
            printf(" [transmitter_disconnect]: putting UA frame to buffer.\n");
            exit(-1);
        }

        if (write_port(fd) == -1) {
            printf(" [transmitter_disconnect]: writing buffer to serial port.\n");
            exit(-1);
        }

        break;

    } while (++try < ll_layer.max_tries);

    // Check number of time-outs
    if (try == ll_layer.max_tries) {
        printf(" [Transmitter]: Exceeded maximum number of tries.\n");
        exit(-1);
    }

    printf(" [Transmitter]: Disconnected.\n");

    return 0;
}

int transmit_data(int fd, const ll_packet_t packet, int packet_size) {

    if (fd < 0) {
        printf(" [transmit_data]: error in file descriptor.\n");
        exit(-1);
    }
    else if (packet == NULL || packet_size < 0) {
        printf(" [transmit_data]: error in packet.\n");
        exit(-1);
    }
    
    int bytes_written;

    int try = 0;

    do {
        if (puts_iframe(packet, packet_size) == -1) {
            printf(" [transmit_data]: error putting I frame in buffer.\n");
            exit(-1);
        }

        if((bytes_written = write_port(fd)) == -1) {
            printf(" [transmit_data]: error writing buffer to serial port.\n");
            exit(-1);
        }

        ll_layer.sequence_number = !ll_layer.sequence_number;

        alarm(ll_layer.time_out);

        int ret;
        if ((ret = read_port(fd)) == -1) {
            printf(" [transmit_data]: error reading buffer from serial port.\n");
            exit(-1);
        }

        alarm(0);

        if (ret == IO_TO) {
            ll_layer.sequence_number = !ll_layer.sequence_number;
            printf(" [Transmitter]: Time-out.\n");
            continue;
        }

        if (ll_layer.buffer[CTRL_INDEX] != (ll_layer.sequence_number ? RESP_RR_S : RESP_RR)) {

            if (ll_layer.buffer[CTRL_INDEX] == (!ll_layer.sequence_number ? RESP_REJ_S : RESP_REJ)) {
                ll_layer.sequence_number = !ll_layer.sequence_number;
                printf(" [Transmitter]: Received REJ response.\n");                
                continue;
            }
            else {
                printf(" [Transmitter]: Unexpected response received.\n");
                exit(-1);
            }
        }

        break;

    } while (++try < ll_layer.max_tries);

    if (try == ll_layer.max_tries) {
        printf(" [Transmitter]: Exceeded maximum number of tries.\n");
        exit(-1);
    }

    return bytes_written - 6;
}