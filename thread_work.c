#include "thread_work.h"
#include "httppub.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <errno.h>

// the http protrol header
#define HEAD    "HTTP/1.0 200 OK\n\
Content-Type: %s\n\
Transfer-Encoding: chunked\n\
Connection: Keep-Alive\n\
Accept-Ranges:bytes\n\
Content-Length:%d\n\n"

#define TAIL   "\n\n"

char LOG_BUFFER[1024] = {0};

// get httpcommand the behind of "GET" string
void getHttpGetCommand(char *httpMsg, char* command){
    char* p_start = httpMsg;
    char* p_end = httpMsg;
    while(*p_start){
        if(*p_start == '/'){
            break;
        }
        p_start ++;
    }
    p_start ++;
    p_end = strchr(httpMsg, '\n');
    while(p_end!=p_start){
        if(*p_end == ' '){
            break;
        }
        p_end--;
    }
    strncpy(command, p_start, p_end - p_start);
}

// get file contont by filepath return the length of file
int getFileContent(char* filepath, char** content){
    int file_length = 0;
    FILE* fp = NULL;
    if(filepath == NULL){
        return file_length;
    }
    fp = fopen(filepath, "rb");
    if(fp == NULL){
        memset(LOG_BUFFER,0,sizeof(LOG_BUFFER));
		sprintf(LOG_BUFFER,"file name: %s,%s,%d:open file failture %s \n",filepath, __FILE__, __LINE__,
				strerror(errno));
		save_log(LOG_BUFFER);
		return file_length;
    }
    fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);
	rewind(fp);
    *content = (char *) malloc(file_length);
    memset(*content, 0, file_length);
	if (*content == NULL)
	{
		memset(LOG_BUFFER,0,sizeof(LOG_BUFFER));
		sprintf(LOG_BUFFER,"%s,%d:malloc failture %s \n", __FILE__, __LINE__,
				strerror(errno));
		save_log(LOG_BUFFER);
		return 0;
	}
	fread(*content, file_length, 1, fp);
	fclose(fp);
    return file_length;
}

const char* getFileType(char* filepath){
    char sExt[32];
	const char *p_start=filepath;
	memset(sExt, 0, sizeof(sExt));
	while(*p_start)
	{
		if (*p_start == '.')
		{
			p_start++;
			strncpy(sExt, p_start, sizeof(sExt));
			break;
		}
		p_start++;
	}

	if (strncmp(sExt, "bmp", 3) == 0)
		return "image/bmp";

	if (strncmp(sExt, "gif", 3) == 0)
		return "image/gif";

	if (strncmp(sExt, "ico", 3) == 0)
		return "image/x-icon";

	if (strncmp(sExt, "jpg", 3) == 0)
		return "image/jpeg";

	if (strncmp(sExt, "avi", 3) == 0)
		return "video/avi";

	if (strncmp(sExt, "css", 3) == 0)
		return "text/css";

	if (strncmp(sExt, "dll", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "exe", 3) == 0)
		return "application/x-msdownload";

	if (strncmp(sExt, "dtd", 3) == 0)
		return "text/xml";

	if (strncmp(sExt, "mp3", 3) == 0)
		return "audio/mp3";

	if (strncmp(sExt, "mpg", 3) == 0)
		return "video/mpg";

	if (strncmp(sExt, "png", 3) == 0)
		return "image/png";

	if (strncmp(sExt, "ppt", 3) == 0)
		return "application/vnd.ms-powerpoint";

	if (strncmp(sExt, "xls", 3) == 0)
		return "application/vnd.ms-excel";

	if (strncmp(sExt, "doc", 3) == 0)
		return "application/msword";

	if (strncmp(sExt, "mp4", 3) == 0)
		return "video/mpeg4";

	if (strncmp(sExt, "ppt", 3) == 0)
		return "application/x-ppt";

	if (strncmp(sExt, "wma", 3) == 0)
		return "audio/x-ms-wma";

	if (strncmp(sExt, "wmv", 3) == 0)
		return "video/x-ms-wmv";

	return "text/html";
}

int MakeGetResponse(char* command, char** content){
    char *file_buf;
	int file_length;
	char headbuf[1024];
	if (command[0] == 0)
	{
		file_length = getFileContent("index.html", &file_buf);
	} else
	{
		file_length = getFileContent(command, &file_buf);
	}
	if (file_length == 0)
	{
		return 0;
	}
    memset(headbuf, 0, sizeof(headbuf));
    sprintf(headbuf, HEAD, getFileType(command), file_length);
    int iheadlen = strlen(headbuf);                     // get the length of header
	int itaillen = strlen(TAIL);                        // get the length of tail 
	int isumlen = iheadlen + file_length + itaillen;    // cal , and get the length of message detail
	*content = (char *) malloc(isumlen);                // depend in length of sum malloc
	if(*content==NULL)
	{
		memset(LOG_BUFFER,0,sizeof(LOG_BUFFER));
		sprintf(LOG_BUFFER,"malloc failed %s\n", strerror(errno));
		save_log(LOG_BUFFER);
	}
	char *tmp = *content;
	memcpy(tmp, headbuf, iheadlen);
	memcpy(&tmp[iheadlen], file_buf, file_length);
	memcpy(&tmp[iheadlen] + file_length, TAIL, itaillen);
	if (file_buf)
	{
		free(file_buf);
	}
	return isumlen;
}

// http thread
void * http_thread(void *arg){
    if(arg == NULL){
        return NULL;
    }
    int cliectFd = *(int *)arg;
    free((int *)arg);
    char buffer[1024] = {0};
    int r = recv(cliectFd, buffer, 1023, 0);
    if(r < 0){
        memset(LOG_BUFFER, 0, sizeof(LOG_BUFFER));
        sprintf(LOG_BUFFER, "%s, %d line: socket error: %s", __FILE__, __LINE__, strerror(errno));
        save_log(LOG_BUFFER);
    }else{
        printf("Request:\n %s\n\n", buffer);
        char commond[1024];
        memset(commond, 0, sizeof(commond));
        getHttpGetCommand(buffer, commond);
        printf("command: /%s\n", commond);
        char* content = NULL;
        int ilen = MakeGetResponse(commond, &content);
        printf("Response: \n%s \n", content);
        printf("Response length:%d\n", ilen);
        if(ilen>0){
            int rd = send(cliectFd, content, ilen, 0);
            printf("send result: %d\n", rd);
            free(content);
        }
    }
    close(cliectFd);
    return NULL;
}