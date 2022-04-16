#include <stdio.h>
#include "thread_work.h"
#include "httppub.h"

int main(int argc, char* argv[]){
    if(argc<2){
        printf("Need argc!\n");
        return 0;
    }
    int port = atoi(argv[1]);
    if(port < 0 ){
        printf("port need integer!\n");
        return 0;
    }
    
    int st = socket_create(port);
    if(st == 0){
        return 0;
    }
    setDaemon();
    printf("myserver begin\n");
    socket_accept(st);
    return 0;
}