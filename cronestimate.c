#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define NUM_MONTHS  12

int process_month(char monthStr[]){
    // returns -1 if invalid month
    int month,length;
    bool numeric_month = true;
    char months[NUM_MONTHS][4]={"jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};
    month = -1;
    length = strlen(monthStr);
    if(length>3){
        return -1;
    }
    for (int  i = 0; i < length; i++)
    {
        if (!isdigit(monthStr[i]))
        {
            numeric_month = false;
            break;
        }
    }
    if(numeric_month){
        month = atoi(monthStr);
        if(month<0|| month>11){
            month=-1;
        }
    }else{
        for(int i = 0; i<length; i++){
            monthStr[i] = tolower(monthStr[i]);
        }
        for (int i = 0; i < NUM_MONTHS; i++)
        {
            if(strcmp(monthStr,months[i])==0){
                month = i;
            }
        }
    }
    return month;
}


int main(int argc, char *argv[]){
    if (argc != 4){
        printf("Usage: %s takes in exactly 3 arguments: month, crontabfile, estimatesfile\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    int month = process_month(argv[1]);
}