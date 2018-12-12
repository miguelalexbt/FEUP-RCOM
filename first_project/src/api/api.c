#include <stdio.h>
#include <stdlib.h>

#include "api/api.h"
#include "api/api_receiver.h"
#include "api/api_transmitter.h"
#include "link_layer/ll_fwd.h"
#include "link_layer/ll_io.h"

/** Host's status */
static int host_status;

int llopen(int port, int status) {

    // Save host's status
    host_status = status;

    // Check port range
    if (port < 0 || port > 3) {
        printf(" [llopen]: port number must be between 0 and 3.\n");
        exit(-1);
    }

    // Check status
    if (host_status != 0 && host_status != 1) {
        printf(" [llopen]: device must be either a transmitter or a receiver.\n");
        exit(-1);
    }

    char serial_port[11];
    sprintf(serial_port, "%s%d", "/dev/ttyS", port);

    // Start IO operations
    int fd;
    if ((fd = open_port(serial_port)) == -1) {
        printf(" [llopen]: starting io.\n");
        exit(-1);
    }
    
    // Connect host to serial port
    if (host_status == 0) {
        if (transmitter_connect(fd) == -1) {
            printf(" [llopen]: error connecting transmitter.\n");
            exit(-1);
        }
    }
    else if (host_status == 1) {
        if (receiver_connect(fd) == -1) {
            printf(" [llopen]: error connecting receiver.\n");
            exit(-1);
        }
    }

    return fd;
}

int llwrite(int fd, char* buffer, int length) {

    if (fd < 0) {
        printf(" [llwrite]: error in file descriptor.\n");
        exit(-1);
    }
    else if (buffer == NULL || length < 0) {
        printf(" [llwrite]: error in buffer.\n");
        exit(-1);
    }

    int bytes_written;
    if ((bytes_written = transmit_data(fd, (ll_packet_t)buffer, length)) == -1) {
        printf(" [llwrite]: error writing to serial port.\n");
        exit(-1);
    }

    return bytes_written;
}

int llread(int fd, char* buffer) {

    if (fd < 0) {
        printf(" [llread]: error in file descriptor.\n");
        exit(-1);
    }
    else if (buffer == NULL) {
        printf(" [llread]: error in buffer.\n");
        exit(-1);
    }

    int bytes_read;
    if ((bytes_read = receive_data(fd, (ll_packet_t)buffer)) == -1) {
        printf(" [llread]: error reading from serial port.\n");
        exit(-1);
    }
    
    return bytes_read;
}

int llclose(int fd) {

    if (fd < 0) {
        printf(" [llclose]: error in file descriptor.\n");
        exit(-1);
    }
    
    if (host_status == 0) {
        if (transmitter_disconnect(fd) == -1) {
            printf(" [llclose]: error disconnecting transmitter.\n");
            exit(-1);
        }
    } 
    else if (host_status == 1) {
        if (receiver_disconnect(fd) == -1) {
            printf(" [llclose]: error disconnecting receiver.\n");
            exit(-1);
        }
    }

    // Stop IO operations
    if (close_port(fd) == -1) {
        printf(" [llclose]: stopping io.\n");
        exit(-1);
    }

    return 0;
}