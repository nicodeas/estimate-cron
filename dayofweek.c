#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        exit(EXIT_FAILURE);
    }
    struct tm tm;
    tm.tm_year = atoi(argv[1]) - 1900 + 1;
    tm.tm_mon = atoi(argv[2]);
    tm.tm_mday = atoi(argv[3]);
    mktime(&tm);
    printf("%d\n", tm.tm_wday);
    exit(EXIT_SUCCESS);
}
