#include "httppub.h"
#include "thread_work.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <pthread.h>
#include <errno.h>

// log buffer
char LOGBUFFER[1024] = {0};

// log file
void save_log(char *buf)
{
    FILE *fp = fopen("log.txt","a+");  
    fputs(buf,fp);  
    fclose(fp);  
}

// create socket of server   return the socket file description
int socket_create(int port){
    int st = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    if(-1 == st){
        memset(LOGBUFFER, 0 , sizeof(LOGBUFFER));
        sprintf(LOGBUFFER, "%s, %d line: socket error: %s", __FILE__, __LINE__, strerror(errno));
        save_log(LOGBUFFER);
        return 0;
    }
    if(setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))==-1){
        memset(LOGBUFFER, 0, sizeof(LOGBUFFER));
        sprintf(LOGBUFFER, "%s, %d line: socket error: %s", __FILE__, __LINE__, strerror(errno));
        save_log(LOGBUFFER);
        return 0;
    }
    struct sockaddr_in sockaddr = {0};
    sockaddr.sin_port = htons(port);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(st, (struct sockaddr*)&sockaddr, sizeof(sockaddr))==-1){
        memset(LOGBUFFER, 0, sizeof(LOGBUFFER));
        sprintf(LOGBUFFER, "%s, %d line: socket error: %s", __FILE__, __LINE__, strerror(errno));
        save_log(LOGBUFFER);
        return 0;
    }

    if(listen(st, 10)==-1){
        memset(LOGBUFFER, 0, sizeof(LOGBUFFER));
        sprintf(LOGBUFFER, "%s, %d line: socket error: %s", __FILE__, __LINE__, strerror(errno));
        save_log(LOGBUFFER);
        return 0;
    }

    printf("start server success!\n");
	return st;
}

int socket_accept(int st){
    int cliectFd;   
    struct sockaddr_in cliect_add = {0};
    socklen_t len = sizeof(cliect_add);

    pthread_t thrd_t;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED); 

    while(1){
        cliectFd = accept(st, (struct sockaddr*)&cliect_add, &len);
        if(-1==cliectFd){
            memset(LOGBUFFER, 0, sizeof(LOGBUFFER));
            sprintf(LOGBUFFER, "%s, %d line: socket error: %s", __FILE__, __LINE__, strerror(errno));
            save_log(LOGBUFFER);
            return 0;
        }
        else
        {
            int *tmp = (int *)malloc(sizeof(int));
            *tmp = cliectFd;
            pthread_create(&thrd_t, &attr, http_thread, tmp);
            
        }
    }
    pthread_attr_destroy(&attr);
}

// set daemon
int setDaemon(){
    pid_t pid, sid;
    pid = fork();
    
    if(pid < 0){
        memset(LOGBUFFER, 0, sizeof(LOGBUFFER));
        sprintf(LOGBUFFER, "%s, %d line: socket error: %s", __FILE__, __LINE__, strerror(errno));
        save_log(LOGBUFFER);
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
	{
		exit (EXIT_SUCCESS);
	}
	if ((sid = setsid()) < 0)
	{
		printf("setsid failed %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}
    memset(LOGBUFFER, 0, sizeof(LOGBUFFER));
    sprintf(LOGBUFFER, "pid: %d\n", getpid());
    save_log(LOGBUFFER);
    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}