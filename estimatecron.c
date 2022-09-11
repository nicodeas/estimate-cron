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
    time_t end;
};
void printProc(struct proc p){ 
    printf("NAME %s WKDAY %s MIN %s HR %s DAY %s MONTH %s LEN %d\n",p.name,p.wkday,p.min,p.hr,p.day,p.mon,p.len);
}
int eq(char *one, int time){
    if (strcmp(one,"*")==0){
        return 1; 
    } else if (atoi(one)==time){
        return 1;
    }
    return 0;
}
void get_num(char *cont, char *arr[],int len){
    printf("NUM OF %s\n" ,cont);
    for (int i = 0;i<len;i++){
        if (strcmp(cont,arr[i])==0){
            printf("convert %s to %d\n",cont,i);
            sprintf(cont,"%d",i);
        }
    }
}
int main(int argc, char *argv[]){
    if (argc != 4){
        printf("Requires 3 arguments: month, crontab file and estimates file.");
        exit(4);
    }
    int MAX_LINES = 20;
    int running = 0;  
    int max_proc = 0;
    struct proc cmds[MAX_LINES];
    int cron_ind = 0; 
    //Adjust below for days of the week - find 2* and 5mon
    int cmd_counts[MAX_LINES];
    char *days[] = {"sun","mon","tue","wed","thu","fri","sat"};
    char *mons[] = {"jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};
    //replace with num weeks 
    int mon_len[] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
    int month = atoi(argv[1]);
    if ( month > 12 || month < 0){
        printf("Month must be a valid integer between 0 and 11");
        exit(1);
    }
    FILE *cron; 
    if (cron = fopen(argv[2],"r")){
        printf("\nOpening %s:\n",argv[2]);
        char cur[60]; 
        while (fgets(cur,60,cron)!=NULL){
            if (cur[0]!='#'){
                struct proc process;
                sscanf(cur,"%s %s %s %s %s %s",process.min,process.hr,process.day,process.mon,process.wkday,process.name);
                get_num(process.mon,mons,12);
                get_num(process.wkday,days,7);
                cmd_counts[cron_ind] = 0; 
                cmds[cron_ind++] = process;
                printProc(cmds[cron_ind-1]);
            } else {
                printf("Comment\n");
            } 
        }
        fclose(cron);
    } else { 
        printf("Specified crontab file does not exist.");
        exit(3);
    }
    FILE *est;
    if (est = fopen(argv[3],"r")){
        printf("\nOpening %s:\n",argv[3]);
        char cur[60]; 
        while (fgets(cur,60,est)!=NULL){
            if (cur[0]!='#'){
                char name[60];
                int len; 
                sscanf(cur,"%s %d",name,&len); 
                for (int i = 0; i < cron_ind;i++){
                    if (strcmp(cmds[i].name,name)==0){
                        cmds[i].len = len;
                    }
                }
            } else {
                printf("Comment\n");
            } 
        }
        fclose(est); 
    } else { 
        printf("Specified estimates file does not exist.\n");
        exit(2);
    }
    int m = month;
    // for (int m = 0;m<12;m++){
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
                    for (int i = 0;i<cron_ind;i++){
                        if (difftime(cmds[i].end,cur_t)==0){
                            printf("\nPROCESS %s ENDED\n",cmds[i].name);
                            printf(ctime(&cur_t));
                            printf("\n");
                            running--;
                        }
                        int count = 0;
                        struct proc p = cmds[i];
                        // printProc(p);
                        int *proc_match[] = {p.min,p.hr,p.day,p.mon,p.wkday};
                        for (int j = 0;j<5;j++){
                            if (eq(proc_match[j],cur_time[j])==1){
                                count++;
                            }
                        }
                        // printf("\n");
                        if (count == 5){ 
                            // printf(ctime(&cur_t));
                            cmd_counts[i]++;
                            str_time.tm_min += cmds[i].len;
                            cur_t =  mktime(&str_time);
                            cmds[i].end = cur_t;
                            running++; 
                            if (running>max_proc){
                                max_proc = running;
                            }
                            printProc(p);
                            printf("Ends at");
                            printf(ctime(&cur_t));
                            str_time.tm_min -=cmds[i].len;
                        }
                        

                    }
                }
            }
        }
    //}
    int max = 0;
    int total = 0;
    char *name; 
    for (int i = 0;i<cron_ind;i++){ 
        printf("%s executed %d times.\n",cmds[i].name,cmd_counts[i]);
        total += cmd_counts[i];
        if (cmd_counts[i]>max){
            max = cmd_counts[i];
            name = &cmds[i].name;
        }
    }
    printf("%s executed the most times, with %d executions. Overall, there were %d processes run. There were at most %d concurrent processes.\n",name,max,total,max_proc);
    return 0; 
}