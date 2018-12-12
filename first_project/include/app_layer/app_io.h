#ifndef APP_IO_H
#define APP_IO_H

/**
 * Opens file.
 * @param file_path Path of the file.
 * @param flags Flags for opening the file.
 * @returns Return 0 if successful, -1 otherwise.
 */
int open_file(char* file_path, char* flags);

/**
 * Closes file.
 * @returns Return 0 if successful, -1 otherwise.
 */
int close_file();

/**
 * Writes the data to a file.
 * @returns Returns 0 if successful, -1 otherwise.
 */
int write_file_data();

/**
 * Reads the data from a file
 * @returns Returns 0 if successful, -1 otherwise.
 */
int read_file_data();

/**
 * Updates progress bar.
 * @param bytes_per_packet Bytes per packet.
 */
void update_progress_bar(int bytes_per_packet);

#endif