#ifndef _HTTPHUB_H
#define _HTTPHUB_H
#include <stdlib.h>

// create socket of server 
int socket_create(int port);
// accpet socket
int socket_accept(int st);
// set daemon
int setDaemon();

#endif /* _HTTPHUB_H */