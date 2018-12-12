#include <sys/ioctl.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>

#include "link_layer/ll_io.h"
#include "link_layer/ll_debug.h"
#include "link_layer/ll_test.h"
#include "link_layer/link_layer.h"
#include "link_layer/ll_frame.h"
#include "link_layer/ll_sm.h"

/** Link layer structure */
link_layer_t ll_layer;

/** Time-out variable */
static int time_exceeded = 0;

void timeout() {
    time_exceeded = 1;
}

int open_port(char* port) {

    if (port == NULL) {
        printf(" [open_port]: error in port.\n");
        exit(-1);
    }

    int fd;
    if ((fd = open(port, O_RDWR | O_NOCTTY)) < 0) {
        perror(" [start_connection]");
        exit(-1);
    }

    struct termios oldtio, newtio;

    if (tcgetattr(fd, &oldtio) == -1) {
        perror(" [start_connection]");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    
    newtio.c_cflag = LL_BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 5;

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror(" [start_connection]");
        exit(-1);
    }

    if (create_link_layer(&ll_layer, oldtio) == -1) {
        printf(" [start_connection]: error creating link layer.\n");
        exit(-1);
    }
    
    (void) signal(SIGALRM, timeout);

    return fd;
}

int close_port(int fd) {

    if (fd < 0) {
        printf(" [close_port]: error in file descriptor.\n");
        exit(-1);
    }

    if (tcsetattr(fd, TCSANOW, &ll_layer.oldtio) == -1) {
        perror(" [close_port]");
        exit(-1);
    }

    if (close(fd) == -1) {
        perror(" [close_port]");
        exit(-1);
    }

    return 0;
}

int write_port(int fd) {

    if (fd < 0) {
        printf(" [write_port]: error in file descriptor.\n");
        exit(-1);
    }

    tcflush(fd, TCIOFLUSH);

    int bytes_written;
    if ((bytes_written = write(fd, ll_layer.buffer, ll_layer.buffer_size)) == -1) {
        perror(" [write_port]");
        exit(-1);
    }

    tcdrain(fd);

#ifdef LL_DEBUG_IO
    printf("    Wrote frame to serial port.\n");
#endif

    return bytes_written;
}

int read_port(int fd) {

    if (fd < 0) {
        printf(" [read_port]: error in file descriptor.\n");
        exit(-1);
    }

    ll_initialize_sm(ll_layer.sequence_number);

    ll_frame_t frame[FRAME_SIZE];
    int frame_size = 0;

    do {
        // Check available bytes to read
        int bytes_available;
        if (ioctl(fd, FIONREAD, &bytes_available) == -1) {
            perror(" [read_port]");
            exit(-1);
        }

        if (bytes_available > 0) {

            uint8_t bytes[bytes_available];

            if (read(fd, bytes, bytes_available) == -1) {
                perror(" [read_port]");
                exit(-1);
            };

            int frame_received = 0;

#ifdef LL_TEST
            usleep(T_PROP * 1000);
#endif
            int i;
            for (i = 0; i < bytes_available && !frame_received; ++i) {

                int ret;
                if ((ret = ll_sm_handler(bytes[i], frame, &frame_size)) == -1) {
                    printf(" [read_port]: error in state machine.\n");
                    exit(-1);
                }

                if (frame_size < 0 || frame_size > FRAME_SIZE) {
                    printf(" [read_port]: error in frame size.\n");
                    exit(-1);
                }

                if (ret == INV_DATA) {
                    tcflush(fd, TCIFLUSH);
                    return IO_INV_DATA;
                } 
                else if (ret == REP_DATA) {
                    tcflush(fd, TCIFLUSH);
                    return IO_REP_DATA;
                } 
                else if (ret == RCV_FRAME) {
                    tcflush(fd, TCIFLUSH);
                    frame_received = 1;
                }
            }

            if (frame_received) {
                frame_received = 0;
                break;
            }
        }

    } while (!time_exceeded);

    if (time_exceeded) {
        tcflush(fd, TCIFLUSH);
        time_exceeded = 0;
        return IO_TO;
    }

    if (set_link_layer_buffer(&ll_layer, frame, frame_size) == -1) {
        printf(" [read_port]: error setting buffer.\n");
        exit(-1);
    }

#ifdef LL_DEBUG_IO
    printf("    Read frame from serial port.\n");
#endif

    return ll_layer.buffer_size;
}

int puts_iframe(ll_packet_t packet, int packet_size) {

    if (packet == NULL || packet_size < 0) {
        printf(" [puts_iframe]: error in packet.\n");
        exit(-1);
    }

    ll_frame_t frame[FRAME_SIZE];
    int frame_size;
    
    if ((frame_size = create_iframe(frame, ll_layer.sequence_number, ADDR_EMI, packet, packet_size)) == -1) {
        printf(" [puts_iframe]: error creating I frame.\n");
        exit(-1);
    }

    if (set_link_layer_buffer(&ll_layer, frame, frame_size) == -1) {
        printf(" [puts_iframe]: error setting buffer.\n");
        exit(-1);
    }

#ifdef LL_DEBUG_IO
    printf("    Put I frame buffer.\n");
#endif

    return 0;
}

int puts_suframe(ll_ctrl_t control, ll_addr_t address) {

    ll_frame_t frame[FRAME_SIZE];
    int frame_size;

    if ((frame_size = create_suframe(frame, ll_layer.sequence_number, address, control)) == -1) {
        printf(" [puts_suframe]: error creating S/U frame.\n");
        exit(-1);
    }

    if (set_link_layer_buffer(&ll_layer, frame, frame_size) == -1) {
        printf(" [puts_suframe]: error setting buffer.\n");
        exit(-1);
    }

#ifdef LL_DEBUG_IO
    printf("    Put S/U frame in buffer.\n");
#endif

    return 0;
}