//  CITS2002 Project 1 2022
//  Student1:   23126543   CHENG   DANIEL   
//  Student2:   22719855   MICHLIN  NICHOLAS
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
struct proc{ 
    char name[40];
    char day[4]; 
    char wkday[4]; 
    char mon[4]; 
    char hr[4]; 
    char min[4]; 
    int len;
    // time_t end;
};
/* 
    Structure holding currently running processes. 
    If done = 0, then the process is still executing. 
    If done = 1, the process has completed and can be safely overwritten. 
*/

struct queued{ 
    char name[40]; 
    time_t end_time; 
    int done; 
};
void printProc(struct proc p){ 
    printf("NAME %s WKDAY %s MIN %s HR %s DAY %s MONTH %s LEN %d\n",p.name,p.wkday,p.min,p.hr,p.day,p.mon,p.len);
}
/*
    Checks if given time type from read-in process 
    matches either the required time (int time) or a *.
*/
int eq(char *one, int time, int bounds[]){
    
    if (strcmp(one,"*")==0){
        return 1; 
    } else if (atoi(one)==time){
        return 1;
    } else if (atoi(one)<bounds[0] && atoi(one)>bounds[1]){
        printf("Invalid time");
        exit(EXIT_FAILURE);
    }
    return 0;
}
/*
    Converts 3 letter inputs to their digit equivalents.
*/
void get_num(char *cont, char *arr[],int len){
    for (int i = 0;i<len;i++){
        if (strcmp(cont,arr[i])==0){
            sprintf(cont,"%d",i);
        }
    }
}
int main(int argc, char *argv[]){
    if (argc != 4){
        printf("Requires 3 arguments: month, crontab file and estimates file.");
        exit(EXIT_FAILURE);
    }
    int MAX_LINES = 20;
    int running = 0;  
    int max_proc = 0;
    int num_procs = 0; 
    struct proc cmds[MAX_LINES];
    struct queued active[MAX_LINES];
    int cmd_counts[MAX_LINES];
    int defaults[][2] = {{0,59},{0,23},{1,31},{0,11},{0,6}};
    char *days[] = {"sun","mon","tue","wed","thu","fri","sat"};
    char *mons[] = {"jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};
    int mon_len[] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
    char *char_mon = argv[1]; 
    get_num(char_mon,mons,12);
    int month = atoi(char_mon);
    if ( month > 12 || month < 0){
        printf("Month must be a valid integer between 0 and 11\n");
        exit(EXIT_FAILURE);
    }
    for (int i =0;i<MAX_LINES;i++){ 
        struct queued ph = {"null",(time_t) (-1),1};
        active[i] = ph;
    }
    FILE *cron; 
    if (cron = fopen(argv[2],"r")){
        // printf("\nOpening %s:\n",argv[2]);
        char cur[60]; 
        while (fgets(cur,60,cron)!=NULL){
            if (cur[0]!='#'){
                struct proc process;
                process.len = -1; 
                sscanf(cur,"%s %s %s %s %s %s",process.min,process.hr,process.day,process.mon,process.wkday,process.name);
                get_num(process.mon,mons,12);
                get_num(process.wkday,days,7);
                cmd_counts[num_procs] = 0; 
                cmds[num_procs++] = process;
                // printProc(cmds[num_procs-1]);
            } else {
                // printf("Comment\n");
            } 
        }
        fclose(cron);
    } else { 
        printf("Specified crontab file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    FILE *est;
    if (est = fopen(argv[3],"r")){
        // printf("\nOpening %s:\n",argv[3]);
        char cur[60]; 
        while (fgets(cur,60,est)!=NULL){
            if (cur[0]!='#'){
                char name[60];
                int len; 
                sscanf(cur,"%s %d",name,&len); 
                for (int i = 0; i < num_procs;i++){
                    if (strcmp(cmds[i].name,name)==0){
                        cmds[i].len = len;
                    }
                }
            } else {
                // printf("Comment\n");
            } 
        }
        fclose(est); 
    } else { 
        printf("Specified estimates file does not exist.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0;i<num_procs;i++){
        if (cmds[i].len == -1){
            printf("Invalid command input\n");
            exit(EXIT_FAILURE);
        }
    }
    int m = month;
    for (int d = 1;d<=mon_len[m];d++){
        for (int hr = 0; hr<24;hr++){
            for (int min = 0;min<60;min++){
                struct tm str_time;
                time_t cur_t;
                str_time.tm_year = 2022-1900;
                str_time.tm_mon = m;
                str_time.tm_mday = d;
                str_time.tm_hour = hr;
                str_time.tm_min = min;
                str_time.tm_sec = 0;
                str_time.tm_isdst = 0;
                cur_t = mktime(&str_time);
                char *wkday = days[str_time.tm_wday];
                int cur_time[] = {str_time.tm_min,str_time.tm_hour,str_time.tm_mday,str_time.tm_mon,str_time.tm_wday};
                for (int j = 0;j<MAX_LINES;j++){
                    if (difftime(active[j].end_time,cur_t)==0){
                        active[j].done = 1; 
                        // printf("\nPROCESS %s ENDED\n",active[j].name);
                        // printf(ctime(&cur_t));
                        // printf("\n");
                        running--;
                    }
                }
                for (int i = 0;i<num_procs;i++){
                    int count = 0;
                    struct proc p = cmds[i];
                    int *proc_match[] = {p.min,p.hr,p.day,p.mon,p.wkday};
                    /*
                        Checks if all time types match 
                        to see if command will execute 
                    */
                    for (int j = 0;j<5;j++){
                        if (eq(proc_match[j],cur_time[j],defaults[j])==1){
                            count++;
                        }
                    }
                    if (count == 5){ 
                        cmd_counts[i]++;
                        str_time.tm_min += cmds[i].len;
                        cur_t =  mktime(&str_time);
                        for (int j = 0;j<MAX_LINES;j++){ 
                            if (active[j].done){
                                // printf("Overwriting %d %s\n",j,active[j].name);
                                time_t a = cur_t;
                                strcpy(active[j].name,cmds[i].name);
                                active[j].done = 0;  
                                active[j].end_time = cur_t;
                                // printf("NAME OF CMD %s ",active[j].name);
                                // printf(ctime(&active[j].end_time));
                                break;
                            }
                        }
                        running++; 
                        if (running>max_proc){
                            max_proc = running;
                        }
                        // printProc(p);
                        // printf("Ends at ");
                        // printf(ctime(&cur_t));
                        // printf("\n");
                        str_time.tm_min -=cmds[i].len;
                    }
                }
            }
        }
    }
    int max = 0;
    int total = 0;
    char *name; 
    for (int i = 0;i<num_procs;i++){ 
        // printf("%s executed %d times.\n",cmds[i].name,cmd_counts[i]);
        total += cmd_counts[i];
        if (cmd_counts[i]>max){
            max = cmd_counts[i];
            name = cmds[i].name;
        }
    }
    // printf("%s executed the most times, with %d executions. Overall, there were %d processes run. There were at most %d concurrent processes.\n",name,max,total,max_proc);
    printf("%s %d %d\n",name,total,max_proc);

    return 0; 
}