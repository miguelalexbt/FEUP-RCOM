#include "include/client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        printf(" Usage: ./download ftp://[<user>:<pass>@]<host>/<url>\n");
        return -1;
    }

    ftp_start(argv[1]);

    return 0;
}