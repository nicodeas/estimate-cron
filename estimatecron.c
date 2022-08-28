#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
// How do I pass a mutable 3D array 
int analyse_line(char *line, char *storage_arr[50]){
    if (line[0]!='#'){
        char *split;
        int ind = 0; 
        split = strtok(line," ");
        while (split!=NULL){
            printf("Arr %d: %s\n",ind++,split);
            // storage_arr[*line_num][ind] = *split; 
            split = strtok(NULL," "); 
        }
    } else {
        printf("Comment\n");
    } 
}
int get_day(char cont[], char *days[]){
    int ret = -1;
    for (int i = 0;i<7;i++){
        printf("day " +i+ " is " + days[i]);
        if (strcmp(cont,days[i])){
            ret=i;
        }
    }
    return ret; 
}
int main(int argc, char *argv[]){
    if (argc != 4){
        printf("Requires 3 arguments: month, crontab file and estimates file.");
        exit(4);
    }
    int MAX_LINES = 60; 
    char est_data[MAX_LINES][2][50];
    int est_ind = 0; 
    char cron_data[MAX_LINES][6][50];
    int cron_ind = 0; 
    int defaults[] = {60,24,-1,12,7};
    char *days[] = {{"sun"},{"mon"},{"tue"},{"wed"},{"thu"},{"fri"},{"sat"}};
    int mon_len[] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
    int month = atoi(argv[1]);
    if ( month > 12 || month < 0){
        printf("Month must be a valid integer between 0 and 11");
        exit(1);
    }
    FILE *est;
    if (est = fopen(argv[3],"r")){
        printf("\nOpening %s:\n",argv[3]);
        char cur[60]; 
        while (fgets(cur,60,est)!=NULL){
            // printf("%s",cur);
            // analyse_line(cur,est_data[est_ind++]);
            if (cur[0]!='#'){
                char *split;
                int ind = 0; 
                split = strtok(cur," ");
                while (split!=NULL){
                    printf("Arr %d: %s\n",ind,split);
                    strcpy(est_data[est_ind][ind++],split);
                    split = strtok(NULL," "); 
                }
                est_ind++;
            } else {
                printf("Comment\n");
            } 
        }
        fclose(est); 
    } else { 
        printf("Specified estimates file does not exist.\n");
        exit(2);
    }
    
    FILE *cron; 
    if (cron = fopen(argv[2],"r")){
        printf("\nOpening %s:\n",argv[2]);
        char cur[60]; 
        while (fgets(cur,60,est)!=NULL){
            if (cur[0]!='#'){
                char *split;
                int ind = 0; 
                split = strtok(cur," ");
                while (split!=NULL && ind<6){
                    strcpy(cron_data[cron_ind][ind++],split);
                    printf("Arr %d: %s\n",ind-1,cron_data[cron_ind][ind-1]);
                    split = strtok(NULL," "); 
                }                   
                cron_ind++; 
            } else {
                printf("Comment\n");
            } 
        }
        fclose(cron);
    } else { 
        printf("Specified crontab file does not exist.");
        exit(3);
    }
    printf("PRINTING EST\n");
    for (int i = 0;i<est_ind;i++){
        for (int j = 0;j<2;j++){
            printf("n%d %s : ",j , est_data[i][j]);
        }
        printf("\n");
    }
    printf("PRINTING CRON\n");
    for (int i = 0;i<cron_ind;i++){
        for (int j = 0;j<6;j++){
            printf("%s : ",cron_data[i][j]);
        }
        printf("\n");
    }
    //FIND MOST COMMON
    int num = 0;
    int max = -1; 
    char *task;  
    for (int i = 0;i<cron_ind;i++){
        int n=1; 
        if (strcmp(cron_data[i][3],"*")!=0 && atoi(cron_data[i][3]) != month){
            printf("%d is not the right month for month %d!",atoi(cron_data[i][3])+1,month);
            n=0;
        }
        for (int j = 0;j<4;j++){
            int cur = 1; 
            if (strcmp(cron_data[i][j],"*")){
                if (j == 2){
                    cur = mon_len[month];
                } else{ 
                    cur = defaults[j]; 
                }
            } 
            n *= cur; 
        }
        num += n; 
        if (n > max){
            task = cron_data[i][5];   
            max = n;   
        }

    }
    for (int i=0;i<7;i++){
        printf("%s is index %d!\n",days[i],get_day(days[i],days));
    }
    printf("%s occurs the most frequently, for a total of %d times!\n",task,max);
    printf("Overall, there a total of %d processes.\n",num);


    return 0; 
}