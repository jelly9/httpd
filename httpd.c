#include "httpd.h"

void print_log(const char *msg, int level)
{
#ifndef _STDOUT_
    const char *level_msg[] = {
        "SUCCESS",
        "NOTICE",
        "WARNING",
        "ERROR",
        "FATAL"
    };
    printf("[%s][%s]\n", msg, level_msg[level%5]);
#endif
}

int startup(const char *ip, int port)
{
    //创建sock
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        print_log(strerror(errno), FATAL);
        exit(2);
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    //绑定
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(ip);
    if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0){
        print_log(strerror(errno), FATAL);
        exit(3);
    }

    if(listen(sock, 10) < 0){
        print_log(strerror(errno), FATAL);
        exit(4);
    }

    return sock;
}

//ret > 1, line != '\0', ret=1&line=='\n', ret=0&line=='\0'
static int get_line(int sock, char line[], int size)
{
    // read 1 char, only by one
    char c = '\0';
    int len = 0;
    //while(read(sock, &c, 1) > 0 && len < size-1){
    while(c != '\n' && len < size-1){
        int ret = recv(sock, &c, 1, 0);
        if(ret > 0){
            if(c == '\r'){// 
                //窥探
                int r = recv(sock, &c, 1, MSG_PEEK);
                if(r > 0){
                    if(c == '\n')
                        recv(sock, &c, 1, 0);
                    else
                        c = '\n';
                }

            }// \r -> \n \r\n -> \n
            // c== '\n'
            line[len++] = c;
        }else{
            c = '\n';
        }
    }
    line[len] = '\0';
    return len;
}

void *handler_request(void *arg)
{
    int sock = (int)arg;
#ifndef _DEBUG_
    char line[1024];
    int ret;
    do{
        ret = get_line(sock, line, sizeof(line));
        if(ret > 0){//success
            printf("%s", line);
        }else{
            printf("request ... done!\n");
            break;
        }

    }while(1);

#endif

}

