#ifndef APP_TRANSMITTER_H
#define APP_TRANSMITTER_H

/**
 * Transmits a file over serial port.
 * @param file_path Path of the file.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int transmit_file(char* file_path);

/**
 * Transmits a data packet over serial port.
 * @param offset Offset of the data.
 * @param data_size Size of the data.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int transmit_data_packet(int offset, int data_size);

#endif