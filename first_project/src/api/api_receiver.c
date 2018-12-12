#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "api/api_receiver.h"
#include "link_layer/ll_io.h"
#include "link_layer/link_layer.h"

extern link_layer_t ll_layer;

int receiver_connect(int fd) {

    if (fd < 0) {
        printf(" [receiver_connect]: error in file descriptor\n");
        exit(-1);
    }

    printf(" [Receiver]: Connecting to transmitter...\n");

    while (1) {

        alarm(ll_layer.max_tries * ll_layer.time_out + 1);

        int ret;
        if ((ret = read_port(fd)) == -1) {
            printf(" [receiver_connect]: error reading buffer from serial port.\n");
            exit(-1);
        }

        alarm(0);

        if (ret == IO_TO) {
            printf(" [Receiver]: Time-out.\n");
            exit(-1);
        }

        if (ll_layer.buffer[CTRL_INDEX] != CMND_SET) {
            printf(" [Receiver]: Unexpected command received.\n");
            continue;
        }

        if (puts_suframe(RESP_UA, ADDR_EMI) == -1) {
            printf(" [receiver_connect]: error putting UA frame to buffer.\n");
            exit(-1);
        }

        if (write_port(fd) == -1) {
            printf(" [receiver_connect]: error writing buffer to serial port.\n");
            exit(-1);
        }

        break;
    }

    printf(" [Receiver]: Connected.\n");

    return 0;
}

int receiver_disconnect(int fd) {

    if (fd < 0) {
        printf(" [receiver_disconnect]: error in file descriptor.\n");
        exit(-1);
    }

    printf(" [Receiver]: Disconnecting from transmitter...\n");

    while (1) {

        alarm(ll_layer.max_tries * ll_layer.time_out + 1);

        int ret;
        if ((ret = read_port(fd)) == -1) {
            printf(" [receiver_disconnect]: error reading buffer from serial port.\n");
            exit(-1);
        }

        alarm(0);

        if (ret == IO_TO) {
            printf(" [Receiver]: Time-out.\n");
            exit(-1);
        }

        if (ll_layer.buffer[CTRL_INDEX] != CMND_DISC) {
            printf(" [Receiver]: Unexpected command received.\n");            
            continue;
        }

        if (puts_suframe(CMND_DISC, ADDR_REC) == -1) {
            printf(" [receiver_disconnect]: error putting DISC frame to buffer.\n");
            exit(-1);
        }

        if (write_port(fd) == -1) {
            printf(" [receiver_disconnect]: error writing buffer to serial port.\n");
            exit(-1);
        }

        if ((ret = read_port(fd)) == -1) {
            printf(" [receiver_disconnect]: error reading buffer from serial port.\n");
            exit(-1);
        }

        if (ll_layer.buffer[CTRL_INDEX] != RESP_UA) {
            printf(" [Receiver]: Unexpected response received.\n");
            continue;
        }

        break;
    }

    printf(" [Receiver]: Disconnected.\n");

    return 0;
}

int receive_data(int fd, ll_packet_t packet) {

    if (fd < 0) {
        printf(" [receive_data]: error in file descriptor.\n");
        exit(-1);
    }
    else if (packet == NULL) {
        printf(" [receive_data]: error in packet.\n");
        exit(-1);
    }

    int packet_size = 0;

    while (1) {

        alarm(ll_layer.max_tries * ll_layer.time_out + 1);

        int ret;
        if ((ret = read_port(fd)) == -1) {
            printf(" [receive_data]: error reading buffer from serial port.\n");
            exit(-1);
        }

        alarm(0);

        if (ret == IO_TO) {
            printf(" [Receiver]: Time-out.\n");
            exit(-1);
        }
        else if (ret == IO_INV_DATA) {

            printf(" [Receiver]: Received invalid data.\n");

            if (puts_suframe(RESP_REJ, ADDR_EMI) == -1) {
                printf(" [receive_data]: error putting REJ frame in buffer");
                exit(-1);
            }

            if (write_port(fd) == -1) {
                printf(" [receive_data]: error writing buffer to serial port.\n");
                exit(-1);
            }

            continue;
        }
        else if (ret == IO_REP_DATA) {

            printf(" [Receiver]: Received repeated data.\n");

            if (puts_suframe(RESP_RR, ADDR_EMI) == -1) {
                printf(" [receive]: error putting RR frame in buffer.\n");
                exit(-1);
            }

            if (write_port(fd) == -1) {
                printf(" [receive_data]: error writing buffer to serial port.\n");
                exit(-1);
            }
            
            return 0;
        }

        if (ll_layer.buffer[CTRL_INDEX] != (ll_layer.sequence_number ? CMND_INFO_S : CMND_INFO)) {
            printf(" [Receiver]: Unexpected command received.\n");
            exit(-1);
        }

        packet_size = ll_layer.buffer_size - 6;

        if (memmove(packet, ll_layer.buffer + DATA_INDEX, packet_size) == NULL) {
            perror(" [receive_data]");
            exit(-1);
        }

        ll_layer.sequence_number = !ll_layer.sequence_number;

        if (puts_suframe(RESP_RR, ADDR_EMI) == -1) {
            printf(" [receive]: error putting RR frame in buffer.\n");
            exit(-1);
        }

        if (write_port(fd) == -1) {
            printf(" [receive]: error writing buffer to serial port.\n");
            exit(-1);
        }

        break;
    }

    return packet_size;
}