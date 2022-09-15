#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define CURRENT_YEAR (2022 - 1900)
#define NUM_MONTHS 12
#define NUM_WEEKDAYS 7

#define MAX_LINES 20
#define MAX_CHARS 100
#define MAX_NAME_LEN 40

#define MAX_NUM_OF_COMMANDS 20
#define MAX_CONCURRENT_PROCESSES 20

struct command
{
    char name[MAX_NAME_LEN];
    int minutes_to_complete;
    int minute;
    int hour;
    int day;
    int month;
    int week_day;
    int times_invoked;
};

struct process
{
    int pid;
    char name[MAX_NAME_LEN];
    time_t end_time;
    bool ended;
};

int total_commands = 0;
int max_concurrent_processes = 0;
int pid = 0;
struct command commands[MAX_NUM_OF_COMMANDS];
struct process processes[MAX_CONCURRENT_PROCESSES];

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
    fclose(fp);
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

        char minute[5], hour[5], day[5], month[5], weekday[5], name[MAX_NAME_LEN];
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
    fclose(fp);
    printf("Crontab processed!\n\n");
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
void init_processes()
{
    for (int i = 0; i < MAX_CONCURRENT_PROCESSES; i++)
    {
        processes[i].pid = -1;
        processes[i].ended = true;
    }
}
void end_processes(time_t current_time)
{
    for (int i = 0; i < MAX_CONCURRENT_PROCESSES; i++)
    {

        if (difftime(processes[i].end_time, current_time) == 0 && !processes[i].ended)
        {
            processes[i].ended = true;
            printf("%s%s (PID %i) ended!\n\n", ctime(&current_time), processes[i].name, processes[i].pid);
            processes[i].pid = -1;
            memset(&processes[i].end_time, 0, sizeof processes[i].end_time);
        }
    }
}
int get_num_concurrent_processes()
{
    int num_processes = 0;
    for (int i = 0; i < MAX_CONCURRENT_PROCESSES; i++)
    {
        if (!processes[i].ended && processes[i].pid > 0)
        {
            num_processes++;
        }
    }
    return num_processes;
}

void simultate(int month)
{

    struct tm tm;
    memset(&tm, 0, sizeof tm);
    tm.tm_year = CURRENT_YEAR;
    tm.tm_mon = month;

    tm.tm_min = 0;
    tm.tm_hour = 0;
    tm.tm_mday = 1;
    bool simulation_complete = false;
    time_t current_time = mktime(&tm);
    int total = 0;

    while (!simulation_complete)
    {
        int weekday, day, hour, minute;
        weekday = tm.tm_wday;
        day = tm.tm_mday;
        hour = tm.tm_hour;
        minute = tm.tm_min;

        for (int i = 0; i < total_commands; i++)
        {
            int running_processes = get_num_concurrent_processes();
            if (running_processes > max_concurrent_processes)
            {
                printf("New Max Concurrent Processes:%i\n\n", running_processes);
                max_concurrent_processes = running_processes;
            }
            if (running_processes == MAX_CONCURRENT_PROCESSES)
            {
                printf("Max number of processes reached: %i\n", MAX_CONCURRENT_PROCESSES);
                break;
            }
            if ((commands[i].week_day == -1 || commands[i].week_day == weekday) && (commands[i].day == -1 || commands[i].day == day) && (commands[i].hour == -1 || commands[i].hour == hour) && (commands[i].minute == -1 || commands[i].minute == minute) && (commands[i].month == -1 || commands[i].month == month))
            {

                for (int j = 0; j < MAX_CONCURRENT_PROCESSES; ++j)
                {
                    if (processes[j].ended)
                    {
                        strcpy(processes[j].name, commands[i].name);
                        struct tm end_time;
                        memset(&end_time, 0, sizeof end_time);
                        end_time.tm_min = tm.tm_min + commands[i].minutes_to_complete;
                        end_time.tm_mday = tm.tm_mday;
                        end_time.tm_hour = tm.tm_hour;
                        end_time.tm_mon = tm.tm_mon;
                        end_time.tm_year = tm.tm_year;
                        processes[j].end_time = mktime(&end_time);
                        pid++;
                        processes[j].pid = pid;
                        processes[j].ended = false;
                        printf("%s%s (PID %i) started!\n\n", ctime(&current_time), processes[j].name, processes[j].pid);
                        break;
                    }
                }
                commands[i].times_invoked++;
            }
        }

        tm.tm_min++;
        total++;
        current_time = mktime(&tm);
        end_processes(current_time);
        simulation_complete = (tm.tm_mon != month);
    }
    printf("Total minutes simulated:%i\n", total);
    printf("Simulation Complete!\n\n");
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
    init_commands();
    init_processes();
    process_crontab(argv[2]);

    for (int i = 0; i < total_commands; i++)
    {
        printf("%s runs for %i\t\t mins: %i\t  hours: %i\t days: %i \t month: %i \t weekdays: %i\n", commands[i].name, commands[i].minutes_to_complete, commands[i].minute, commands[i].hour, commands[i].day, commands[i].month, commands[i].week_day);
    }
    printf("\n\n");
    simultate(month);
    struct command most_invoked_command;
    for (int i = 0; i < total_commands; i++)
    {
        printf("%s ran %i times\n", commands[i].name, commands[i].times_invoked);
        if (commands[i].times_invoked > most_invoked_command.times_invoked)
        {
            most_invoked_command = commands[i];
        }
    }
    printf("\n\n");
    printf("The command %s ran the most, with %d invocations\n", most_invoked_command.name, most_invoked_command.times_invoked);
    printf("The most number of concurrent processes is %d\n", max_concurrent_processes);
    exit(EXIT_SUCCESS);
}