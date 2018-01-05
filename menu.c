#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static int executeCommand(char *arga[10]);

//handler to catch ctrl+c signal.
void sigintHandler(int sig_num){
        //printf("got ctrl+c");
}

void main()
{
        int exitShell=0;
        int multiCommand=0;
        int c_pid=0;
        int j = 0;
        int listJobs[100];
        char listJobsName[100][100];
        int exec_rtrn_value;
        pid_t return_pid;
        signal(SIGINT,sigintHandler);
        while(exitShell!=1)
        {
                char arg[500];
                char buffer[100];
                char *arga[10];
                int i = 0;
                pid_t terminated_child_pid=0;

                //This is how the shell will start with
                printf("\nminish>");
                fgets(arg,100,stdin);

                if (strcmp(arg,"\n") == 0)
                        continue;
                else
                        arg[strcspn(arg,"\n")]='\0';

                //exiting the shell and also killing all the processes before exit.
                if(strcmp(arg,"exit") == 0)
                {
                        for(int k = 0 ; k < j ; k++) {
                                kill(listJobs[k],9);
                        }
                        //waitpid(-1,0,0);
                        exitShell=1;
                        continue;
                }

                //generating tokens from the input provided by the user.
                strcpy(buffer, arg);
                char *token = strtok(buffer," ");
                while(token)
                {
                        if(strcmp(token,"&")==0)
                        {
                                multiCommand++;
                                break;
                        }
                        else {
                                if(multiCommand>0) {
                                        multiCommand=0;
                                }
                        }
                        arga[i]=token;
                        i++;
                        token = strtok(NULL, " ");
                }
                arga[i]=NULL;

                //implementing cd command.
                int chdir_rtrn;
                char path[500];
                if(strcmp(arga[0],"cd")==0){
                        if(strncmp(arga[1],"/",1)!=0) {
                                getcwd(path, sizeof(path));
                                strcpy(path, "/");
                                strcpy(path, arga[1]);
                                chdir_rtrn = chdir(path);
                        }
                        /*else if (strcmp(arga[1],"..")==0) {
                                getcwd(path, sizeof(path));
                                char *path1 = strstr(path,"/");
                                int s = strlen(path1);
                                strcpy(path,path1);
                                path[s]=NULL;
                                chdir_rtrn = chdir(path);
                        }*/
                        else if (strncmp(arga[1],"/root/",strlen("/root/"))==0) {
                                chdir_rtrn = chdir(arga[1]);
                        }
                        else {
                                printf("incorrect directory");
                        }

                        //error handling
                        if(chdir_rtrn<0)
                                printf("Error while changing the directory, error is : %s",strerror(errno));
                }
                //implementation for pwd command.
                else if(strcmp(arga[0],"pwd")==0) {
                        getcwd(path, sizeof(path));
                        printf("%s",path);
                }
                //implementing listjobs.
                else if (strcmp(arga[0],"listjobs")==0){
                        //char name_prc[200];
                        char command[100];
                        char proc_status_str[50];
                        int proc_status;
                        pid_t wait_status;
                        for(int k = 0 ; k < j ; k++) {
                                waitpid(listJobs[k],&wait_status,WNOHANG);
                                proc_status = kill(listJobs[k],0);
                                if(proc_status==0)
                                        strcpy(proc_status_str, "RUNNING");
                                else if(proc_status<0)
                                        strcpy(proc_status_str, "FINISHED");
                                /*sprintf(command,"/proc/%d/cmdline",listJobs[k]);
                                FILE *fp = popen(command, "r");
                                fscanf(fp, "%s", name_prc);
                                pclose(fp);
                                name_prc[strlen(name_prc)]=NULL;*/
                                /*proc_status = waitpid(listJobs[k],&wait_status,WNOHANG);
                                if(WIFCONTINUED(wait_status))
                                         strcpy(proc_status_str, "RUNNING");
                                if(WIFEXITED(wait_status))
                                        strcpy(proc_status_str, "FINISHED");*/

                                /*if(proc_status==0){
                                        strcpy(proc_status_str, "RUNNING");
                                }
                                else if(proc_status>0) {
                                        strcpy(proc_status_str, "FINISHED");
                                }
                                else {
                                        strcpy(proc_status_str, "error");
                                }*/
                                printf("%s\twith\tPID\t%d\tstatus:%s\n",listJobsName[k],listJobs[k],proc_status_str);
                        }
                }
                //implementing fg command to bring the background process to foreground.
                else if (strcmp(arga[0],"fg") == 0) {
                        int fg_pid = atoi(arga[1]);
                        int fg_wait_status;
                        pid_t fg_wait_rtrn = waitpid(fg_pid,&fg_wait_status,0);
                        int fg_rtrn = kill(fg_wait_rtrn, SIGCONT);
                        if(fg_rtrn<0 || fg_wait_rtrn<0)
                                printf("Snap error, cannot bring process to foreground");
                }
                //implementation for anyother single or multi command.
                else {
                        return_pid = fork();
                        //printf("fork return is %d \n", return_pid);
                        if (return_pid == 0) {
                                //inside child
                                exec_rtrn_value = executeCommand(arga);
                                //int setpgid_rtrn = setpgid(0,0);
                                //printf("setpgid return %d \t %d \n", setpgid_rtrn, errno);
                                //exec_rtrn_value = executeCommand(arga);
                                if(exec_rtrn_value<0){
                                        printf("You entered inproper command.");
                                }
                                exit(EXIT_SUCCESS);
                        }
                        else if (return_pid > 0) {
                                //inside parent and return_pid contains the pid of child created.
                                int wait_status;
                                //if it is multi command then the command will run in background.
                                if (multiCommand>0) {
                                        terminated_child_pid = waitpid(return_pid,&wait_status,WNOHANG);
                                        listJobs[j] = return_pid;
                                        strcpy(listJobsName[j],arga[0]);
                                        j++;
                                }
                                if(multiCommand == 0) {
                                        //while (wait(NULL) > 0)
                                        //waiting for proccess to get over.
                                        while(waitpid(return_pid,&wait_status,WUNTRACED)==0)
                                        {
                                                printf("%d child completed\n", j++);
                                        }
                                }
                        }
                        else {
                                printf("fork failure");
                        }
                }
        }
                exit(EXIT_SUCCESS);
}

//method that will call exec to run the command.
int executeCommand(char *arga[10]){
        char arg1[100];
        strcpy(arg1, "/bin/");
        strcat(arg1, arga[0]);
        int exec_rtrn_value = execvp(arg1,arga);
        return exec_rtrn_value;
}
