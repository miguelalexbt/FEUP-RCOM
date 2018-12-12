
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "link_layer/ll_test.h"
#include "app_layer/app_packet.h"

#ifdef LL_TEST

    static time_t start, end;

    void start_test() {
        printf(" [Test]: Starting program measurements.\n\n");
        time(&start);
    }

    void end_test() {

        time(&end);

        double diff = difftime(end, start);

        printf("\n [Test]: Results:\n");
        printf(" Time spent: %d minutes and %d seconds.\n", (int)diff / 60, (int)diff % 60);
        printf(" Header's BER: %f%%\n", (double)HEADER_BER * 100);
        printf(" Data's BER: %f%%\n", (double)DATA_BER * 100);
        printf(" FER: %f%%\n", (double)(1 - pow(1 - HEADER_BER, 3 * 8) * pow(1 - DATA_BER, MAX_DATA_FIELDS * 8)) * 100);
    }

#endif