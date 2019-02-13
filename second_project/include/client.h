#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>

/** Buffer size */
#define BUF_SIZE    1024

/** Mark */
#define MRK     1

/** Acknowledgment */
#define ACK     2

/** Requires */
#define REQ     3

/** Error */
#define ERR     4

/** Negative acknowledgment */
#define NACK    5

/** Command port */
#define CMD_PORT 21

/** Client structure */
typedef struct {

    /** Command socket */
    int ctrl_socket;

    /** Data socket */
    int data_socket;

    /* File */
    FILE* file;

    /** User */
    char user[BUF_SIZE];

    /** Password */
    char pass[BUF_SIZE];

    /** Host */
    char host[BUF_SIZE];

    /** File */
    char url[BUF_SIZE];
} client_t;

/**
 * Starts the client.
 * @param args Arguments for the client.
 */
void ftp_start(char* args);

/**
 * Creates the client.
 * @param args Arguments for the client.
 */
void ftp_create(char* args);

/**
 * Creates a control connection to the server.
 */
void ftp_connect_control();

/**
 * Logs the client to the server.
 */
void ftp_log();

/**
 * Creates a data connection to the server.
 */
void ftp_connect_data();

/**
 * Transfers a file from the server.
 */
void ftp_transfer();

/**
 * Disconnects client from the server.
 */
void ftp_disconnect();

/**
 * Creates the client.
 * @param args Arguments for the client.
 */
int ftp_write(char* verb, char* arg);

/**
 * Creates the client.
 * @param args Arguments for the client.
 */
int ftp_read();

/**
 * Returns IP address of host.
 * @param host Host.
 * @return Returns IP address of the host given.
 */
char* ftp_get_ip(char* host);

/**
 * Get path for new file.
 * @return Returns path for new file.
 */
char* ftp_get_path();

/**
 * Progress bar.
 */
void ftp_progress(int curr, int max);

#endif