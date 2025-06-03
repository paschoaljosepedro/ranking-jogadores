#include <sys/time.h>
#include <time.h>

typedef struct {
    long seconds;
    long microseconds;
    double elapsed;
} TIMERETURN;

TIMERETURN calcTimer(struct timeval start, struct timeval end);
