#ifndef API_H
#define API_H

/**
 * Opens a serial port for reading and writing operations.
 * @param port Port to open.
 * @param status Status of the host.
 * @returns Returns a file descriptor if successful, -1 otherwise.
 */
int llopen(int port, int status);

/**
 * Writes a buffer to serial port.
 * @param fd File descriptor.
 * @param buffer Buffer to be written.
 * @param length Length of the buffer.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int llwrite(int fd, char* buffer, int length);

/**
 * Reads a buffer from serial port.
 * @param fd File descriptor.
 * @param buffer Buffer to store bytes read.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int llread(int fd, char* buffer);

/**
 * Closes a serial port.
 * @param fd File descriptor to close.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int llclose(int fd);

#endif