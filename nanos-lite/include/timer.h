typedef struct
{
    uint32_t tv_sec;     /* seconds */
    uint32_t tv_usec;    /* microseconds */
} timeval;

typedef struct
{
    int tz_minuteswest;     /* minutes west of Greenwich */
    int tz_dsttime;         /* type of DST correction */
} timezone;
#include <sys/time.h>