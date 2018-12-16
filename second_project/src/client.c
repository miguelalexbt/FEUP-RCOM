#include "client.h"

#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <linux/limits.h>

/** Client */
static client_t client;

static char msg_buf[BUF_SIZE];

void ftp_start(char* args) {

    // Create client
    ftp_create(args);

    // Create control connection to server
    ftp_connect_control();

    // Log
    ftp_log();

    // Create data connection to server
    ftp_connect_data();

    // Transfer file
    ftp_transfer();

    // Disconnect from server
    ftp_disconnect();
}

void ftp_create(char* args) {

    printf("[Client]: Starting client.\n");

    int matches = 0;

    if (strstr(args, "@") != NULL) {
        matches = sscanf(args, "ftp://%[^:]:%[^@]@%[^/]/%[^\n]", client.user, client.pass, client.host, client.url);
    } else {
        matches = sscanf(args, "ftp://%[^/]/%[^\n]", client.host, client.url);
        matches += 2;
        snprintf(client.user, sizeof(client.user), "anonymous");
        snprintf(client.pass, sizeof(client.pass), "anonymous");
    }

    if (matches != 4) {
        printf("[Client]: Error in argument.\n");
        exit(EXIT_FAILURE);
    }
}

void ftp_connect_control() {

    printf("[Client]: Establishing control connection.\n");

    if ((client.ctrl_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ftp_get_ip(client.host));
    server_addr.sin_port = htons(CMD_PORT);
    
    if (connect(client.ctrl_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    if (ftp_read(client.ctrl_socket) != ACK) {
        printf("[Client]: Server rejected request.\n");
        exit(EXIT_FAILURE);
    }

    printf("[Client]: Control connection established.\n");
}

void ftp_log() {

    printf("[Client]: Logging in.\n");

    ftp_write("USER", client.user);

    switch (ftp_read()) {
        case ACK:
            break;
        case REQ:
            ftp_write("PASS", client.pass);

            switch (ftp_read()) {
                case ACK:
                    break;
                case REQ:
                    printf("[Client]: Client does not support ACCT.\n");
                    exit(EXIT_FAILURE);
                case NACK:
                    printf("[Client]: User and password combination is not valid.\n");
                    exit(EXIT_FAILURE);
            }
            break;
        case NACK:
            printf("[Client]: User is not valid.\n");
            exit(EXIT_FAILURE);
    }

    printf("[Client]: Logged in as %s.\n", client.user);
}

void ftp_connect_data() {

    printf("[Client]: Establishing data connection.\n");

    if ((client.data_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    ftp_write("PASV", "");

    if (ftp_read() != ACK) {
        printf("[Client]: Server rejected request.\n");
        exit(EXIT_FAILURE);
    }

    int msb, lsb;
    sscanf(msg_buf, "%*[^,],%*d,%*d,%*d,%d,%d%*[^\012]", &msb, &lsb);

    int port = msb * 256 + lsb;

    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ftp_get_ip(client.host));
    server_addr.sin_port = htons(port);
    
    if (connect(client.data_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    printf("[Client]: Data connection established.\n");
}

void ftp_transfer() {
    
    printf("[Client]: Requesting file transfer.\n");

    // Request binary flag on
    ftp_write("TYPE", "I");

    if (ftp_read() != ACK) {
        printf(" Server rejected request.\n");
        exit(EXIT_FAILURE);
    }

    // Request file size
    ftp_write("SIZE", client.url);

    if (ftp_read() != ACK) {
        printf(" Server rejected request.\n");
        exit(EXIT_FAILURE);
    };

    int file_size;
    sscanf(msg_buf, "%*d %d", &file_size);

    // Request file transfer
    ftp_write("RETR", client.url);

    if (ftp_read() != MRK) {
        printf("[Client]: Server rejected request.\n");
        exit(EXIT_FAILURE);
    }

    char* file_path = ftp_get_path();

    if ((client.file = fopen(file_path, "w")) == NULL) {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    free(file_path);

    char buf[BUF_SIZE];
    int num_bytes = 0;

    // Clear buffer
    if (memset(buf, 0, sizeof(buf)) == NULL) {
        perror("memset()");
        exit(EXIT_FAILURE);
    }

    do {
        int bytes_read = 0;
        if ((bytes_read = read(client.data_socket, buf, sizeof(buf))) < 0) {
            perror("read()");
            exit(EXIT_FAILURE);
        }

        if (fwrite(buf, bytes_read, 1, client.file) != 1) {
            perror("fwrite()");
            exit(EXIT_FAILURE);
        }

        num_bytes += bytes_read;

        ftp_progress(num_bytes, file_size);

    } while (num_bytes != file_size);

    if (fclose(client.file) < 0) {
        perror("flclose()");
        exit(EXIT_FAILURE);
    }

    if (close(client.data_socket) < 0) {
        perror("close()");
        exit(EXIT_FAILURE);
    }

    if ((ftp_read() != ACK) || (num_bytes != file_size)) {
        printf("[Client]: Error receiving file.\n");
        exit(EXIT_FAILURE);
    }

    printf("[Client]: Received file.\n");
}

void ftp_disconnect() {
    
    printf("[Client]: Closing client.\n");

    if (close(client.ctrl_socket) < 0) {
        perror("close()");
        exit(-1);
    }
}

int ftp_write(char* verb, char* param) {

    // Clear message buffer
    if (memset(msg_buf, 0, sizeof(msg_buf)) == NULL) {
        perror("memset()");
        exit(-1);
    }

    if (strcmp(param, "") == 0)
        snprintf(msg_buf, sizeof(msg_buf), "%s\015\012", verb);
    else 
        snprintf(msg_buf, sizeof(msg_buf), "%s %s\015\012", verb, param);

    if (write(client.ctrl_socket, msg_buf, strlen(msg_buf)) < 0) {
        perror("write()");
        exit(-1);
    }

    printf("> %s", msg_buf);

    return 0;
}

int ftp_read() {

    // Clear message buffer
    if (memset(msg_buf, 0, sizeof(msg_buf)) == NULL) {
        perror("memset()");
        exit(-1);
    }

    int offset = 0;

    do {
        // Clear buffer
        char buf[BUF_SIZE];
        int bytes_read = 0;

        if (memset(buf, 0, sizeof(buf)) == NULL) {
            perror("memset()");
            exit(-1);
        }

        // Read line(s)
        do {
            if ((bytes_read += read(client.ctrl_socket, buf + bytes_read, sizeof(buf) - bytes_read)) < 0) {
                perror("read()");
                exit(-1);
            }
        } while(buf[strlen(buf) - 1] != '\012');

        // Copy contents to message buffer (strtok will destroy buf)
        if (offset + bytes_read < (int) sizeof(msg_buf)) {
            memcpy(msg_buf + offset, buf, bytes_read);
            offset += bytes_read;
        } else {
            printf(" [Client]: Buffer is out of memory.");
            exit(EXIT_FAILURE);
        }

        // Check for end line (ex: "XXX message here") and return it
        char* pch = strtok(buf, "\012");
        while (pch != NULL) {
            if (pch[0] != ' ' && pch[3] != '-') {
                printf("< %s", msg_buf);
                return pch[0] - '0';
            }
            else
                pch = strtok(NULL, "\012");
        }
    } while (1);
}

char* ftp_get_ip(char* host) {

    struct hostent* h;

    if ((h = gethostbyname(host)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }

    return inet_ntoa(*((struct in_addr *)h->h_addr_list[0]));
}

char* ftp_get_path() {

    char cwd[BUF_SIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd()");
        exit(EXIT_FAILURE);
    }

    char* file_name;
    if ((file_name = basename(client.url)) == NULL) {
        perror("basename()");
        exit(EXIT_FAILURE);
    }

    char* file_path = (char*) malloc(PATH_MAX * sizeof(char));
    snprintf(file_path, PATH_MAX, "%s/%s", cwd, file_name);

    return file_path;
}

void ftp_progress(int curr, int max) {

    printf("\33[2K\r");

    double percentage = (double) curr / (double) max;

    int num = (int) (percentage * 100);

    printf("[Client]: %d%% [", num);

    int i = 0;
    do {
        if (i <= num / 2)
            printf("#");
        else
            printf(" ");
    } while (i++ < 50);

    if (num == 100)
        printf("] %d/%d bytes\n", curr, max);
    else
        printf("] %d/%d bytes", curr, max);

    fflush(stdout);
}