#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// time.h starts counting from 0
#define CURRENT_YEAR (2022 - 1900 + 1)
#define NUM_MONTHS 12
#define NUM_WEEKDAYS 7

#define MAX_MINS 59
#define MAX_HOURS 23

#define MAX_LINES 20
#define MAX_CHARS 100

#define MAX_NUM_OF_COMMANDS 20
#define MAX_CONCURRENT_PROCESSES 20

struct command
{
    char name[40];
    int minutes_to_complete;
    int minute;
    int hour;
    int day;
    int month;
    int week_day;
    int times_invoked;
};

int total_commands = 0;
struct command commands[MAX_NUM_OF_COMMANDS];

int process_month(char monthStr[])
{
    // returns -1 if invalid month
    int month, length;
    bool numeric_month = true;
    char months[NUM_MONTHS][4] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
    month = -1;
    length = strlen(monthStr);
    if (length > 3)
    {
        return -1;
    }
    for (int i = 0; i < length; i++)
    {
        if (!isdigit(monthStr[i]))
        {
            numeric_month = false;
            break;
        }
    }
    if (numeric_month)
    {
        month = atoi(monthStr);
        if (month < 0 || month > 11)
        {
            month = -1;
        }
    }
    else
    {
        for (int i = 0; i < NUM_MONTHS; i++)
        {
            if (strcmp(monthStr, months[i]) == 0)
            {
                month = i;
            }
        }
    }
    return month;
}
int parse_weekdays(char *weekday)
{
    if (isdigit(weekday[0]))
    {
        return atoi(&weekday[0]);
    }
    char weekdays[NUM_WEEKDAYS][4] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
    for (int i = 0; i < NUM_WEEKDAYS; i++)
    {
        if (strcmp(weekday, weekdays[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}
void process_estimates(char *filename)
{
    printf("Processing estimates\n");
    FILE *fp = fopen(filename, "r");
    char buffer[MAX_CHARS];
    if (fp == NULL)
    {
        printf("invalid %s\n", filename);
        exit(EXIT_FAILURE);
    }
    while (fgets(buffer, MAX_CHARS, fp) != NULL)
    {
        if (buffer[0] == '#')
        {
            continue;
        }
        sscanf(buffer, "%s%i", commands[total_commands].name, &commands[total_commands].minutes_to_complete);
        printf("%s:\t%i minutes\n", commands[total_commands].name, commands[total_commands].minutes_to_complete);
        total_commands++;
    }
    printf("Processing complete!\n\n");
}
void process_crontab(char *filename)
{
    printf("Processing crontab\n");
    FILE *fp = fopen(filename, "r");
    char buffer[MAX_CHARS];
    if (fp == NULL)
    {
        printf("invalid %s\n", filename);
        exit(EXIT_FAILURE);
    }
    while (fgets(buffer, MAX_CHARS, fp) != NULL)
    {
        // struct command *curr;

        char minute[5], hour[5], day[5], month[5], weekday[5], name[40];
        if (buffer[0] == '#')
        {
            continue;
        }
        sscanf(buffer, "%s %s %s %s %s %s", minute, hour, day, month, weekday, name);
        // in case crontab is not in the same order as estimates
        for (int i = 0; i < total_commands; i++)

        {
            if (strcmp(name, commands[i].name) == 0)
            {
                printf("%s\t", minute);
                printf("%s\t", hour);
                printf("%s\t", day);
                printf("%s\t", month);
                printf("%s\t", weekday);
                printf("%s found!\n", name);

                if (minute[0] != '*')
                {
                    commands[i].minute = atoi(minute);
                }
                if (hour[0] != '*')
                {
                    commands[i].hour = atoi(hour);
                }
                if (day[0] != '*')
                {
                    commands[i].day = atoi(day);
                }
                if (month[0] != '*')
                {
                    commands[i].month = atoi(month);
                }
                if (weekday[i] != '*')
                {
                    commands[i].week_day = parse_weekdays(weekday);
                }
                break;
            }
        }
    }
    printf("Processing complete!\n\n");
}
void init_commands()
{
    for (int i = 0; i < total_commands; i++)
    {
        commands[i].minute = -1;
        commands[i].hour = -1;
        commands[i].day = -1;
        commands[i].month = -1;
        commands[i].week_day = -1;
        commands[i].times_invoked = 0;
    }
}

void sim_processes(int month)
{

    struct tm tm;
    mktime(&tm);

    tm.tm_year = CURRENT_YEAR;
    tm.tm_mon = month;

    tm.tm_min = 0;
    tm.tm_hour = 0;
    tm.tm_mday = 1;
    bool simulation_complete = false;
    printf("month = %i\n", tm.tm_mon);
    printf("hour before = %i\n", tm.tm_hour);
    int total = 0;
    while (!simulation_complete)
    {
        tm.tm_min++;
        mktime(&tm);
        simulation_complete = (month != tm.tm_mon);
    }
    printf("%i", total);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s takes in exactly 3 arguments: month, crontabfile, estimatesfile\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int month = process_month(argv[1]);
    if (month < 0)
    {
        printf("%s: Invalid month: %s\n", argv[0], argv[1]);
        exit(EXIT_FAILURE);
    }
    printf("The month argument ingested is %i\n\n", month);

    process_estimates(argv[3]);
    init_commands(); // not sure what we will use to represent * in the schedules
    process_crontab(argv[2]);

    for (int i = 0; i < total_commands; i++)
    {
        printf("%s runs for %i\t\t mins: %i\t  hours: %i\t days: %i \t month: %i \t weekdays: %i\n", commands[i].name, commands[i].minutes_to_complete, commands[i].minute, commands[i].hour, commands[i].day, commands[i].month, commands[i].week_day);
    }
    printf("\n\n");
    sim_processes(month);
    for (int i = 0; i < total_commands; i++)
    {
        printf("%s ran %i times\n", commands[i].name, commands[i].times_invoked);
    }

    exit(EXIT_SUCCESS);
}