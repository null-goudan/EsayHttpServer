#ifndef _THREAD_WORK_H
#define _THREAD_WORK_H
#include <stdlib.h>

// get httpcommand the behind of "GET" string
void getHttpGetCommand(char *httpMsg, char* command); 

// According to the command in the GET request, Get the file which user want get and response the request
// Return the msg's length
int MakeGetResponse(char* command, char** content);
int getFileContent(char* filepath, char** content);  // get file contont by filepath return the length of file
const char* getFileType(char* filepath);                     // get type of file  in order to make the header of reponse

// http thread
void * http_thread(void *arg);

#endif /* _THREAD_WORK_H */