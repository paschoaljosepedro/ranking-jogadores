#include "header/time.h"

TIMERETURN calcTimer(struct timeval start, struct timeval end) {
    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    return (TIMERETURN){seconds, microseconds, elapsed};
}