#include <time.h>
#include <stdlib.h>

char *printDatetime()
{
    char *formattedDatetime = malloc(128 * sizeof(char));
    time_t datetime;
    time(&datetime);
    struct tm *timeInfo = localtime(&datetime);
    strftime(formattedDatetime, 128, "%Y%m%d-%H%M%S.txt", timeInfo);
    return formattedDatetime;
}