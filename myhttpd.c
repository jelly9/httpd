
//my httpd server:wq
#include "myhttpd.h"

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

static void echo_msg(int sock)
{
    const char *echo_line = "error\n";
    send(sock, echo_line, strlen(echo_line), 0);
}
static int echo_www(int sock, char *path, off_t size)
{
    printf("enter www\n");
    int fd = open(path, O_RDONLY);
    if(fd < 0){
        echo_msg(sock);
        print_log(strerror(errno), FATAL);
        return 8;
    }

    const char *echo_line = "HTTP/1.0 200 OK\r\n";
    send(sock, echo_line, strlen(echo_line), 0);
    const char *null_line = "\r\n";
    send(sock, null_line, strlen(null_line), 0);

    if(sendfile(sock, fd, NULL, size) < 0){
        echo_msg(sock);
        print_log(strerror(errno), FATAL);
        return 9;
    }

    return 0;
}

static void drop_handler(int sock)
{
    char line[1024];
    int ret;
    do{
        ret = get_line(sock, line, sizeof(line));
    }while(ret > 0 && strcmp(line, "\n"));
}

static int exec_cgi(int sock, char *method, char *path, char *query_string)
{

    int content_len = -1;
    char method_env[SIZE/10];
    char query_string_env[SIZE];
    char content_len_env[SIZE/10];
    char line[1024];
    if(strcasecmp(method, "GET") == 0){
        drop_handler(sock);

    }else{//POST
        int ret;
        do{
            ret = get_line(sock, line, sizeof(line));
            printf("ret: %d line: %s", ret, line);
            if(ret > 0 && \
               strncmp(line, "Content-Lenght:", 15) == 0){
                printf("line: %s", line);

                content_len = atoi(&line[16]);
                break;
            }
        }while(ret > 0 && strcmp(line, "\n"));       
        printf("content_len:%d\n", content_len);

        if(content_len == -1){
            echo_msg(sock);
            return 10;
        }
    }

    const char *echo_line = "HTTP/1.0 200 OK\r\n";
    send(sock, echo_line, strlen(echo_line), 0);
//    const char *type="Content-Type:text/html;charset=ISO-8859-1";
//    send(sock, type, strlen(type), 0);
    const char *null_line = "\r\n";
    send(sock, null_line, strlen(null_line), 0);

    //path -> exe
    //请求的资源已被受理
    //头部信息已经全被丢弃，剩下的全是正文信息
    //已经获得资源长度信息
    
    int input[2];//子进程从input读，往output写,即output->1，写到标准输出
    int output[2];//父进程从output读，往input写
    if(pipe(input) < 0 || pipe(output) < 0){
        echo_msg(sock);
        return 11;
    }

    pid_t id = fork();
    if(id < 0){
        echo_msg(sock);
        return 12;
    }else if(id == 0){//child
        close(input[1]);
        close(output[0]);
        
        sprintf(method_env, "METHOD=%s", method);
        putenv(method_env);
        

        if(strcasecmp(method, "GET") == 0){
            sprintf(query_string_env, "QUERY_STRING=%s", query_string);
            putenv(query_string_env);
        }else{//POST
            sprintf(content_len_env, "CONTENT_LEN=%s",&line[16]);
            printf("content_len:%d\n", content_len);
            putenv(content_len_env);
        }

        dup2(input[0], 0);
        dup2(output[1], 1);
        execl(path, path, NULL);
        echo_msg(sock);
        exit(1);
    }else{//father
        close(input[0]);
        close(output[1]);
        
        int i = 0;
        char c = '\0';

        if(strcasecmp(method, "POST") == 0){
            for(; i < content_len; i++){
                recv(sock, &c, 1, 0);
                write(input[1], &c, 1);
            }
        }

        c = '\0';
        while(read(output[0], &c, 1)){
            send(sock, &c, 1, 0);
        }

        waitpid(id, NULL, 0);
        close(input[1]);
        close(output[0]);
    }

    return 0;
}

//thread
void *handler_request(void *arg)
{
    int sock = *((int*)arg);
#ifdef _DEBUG_
    //获取HTTP头部信息，并显示出来
    char line[1024];
    int r;
//    FILE* fd = fopen("./log/log0.txt", "wb");
    do{
        r = get_line(sock, line, sizeof(line));
        if(r > 0){//success
            printf("%s", line);
//            fwrite(line, 1, ret, fd);
        }else{
            printf("request ... done!\n");
            break;
        }

    }while(1);
//    fclose(fd);
#else
    int ret = 0;
    char buf[SIZE];
    char method[SIZE/10];
    char url[SIZE];
    char path[SIZE];
    int i, j;
    int cgi = 0;
    char *query_string = NULL;

    //获取第一行: 
    //GET/POST url... HTTP/1.0
    if(get_line(sock, buf, sizeof(buf)) <= 0){
        echo_msg(sock);
        ret = 5;
        goto end;
    }
    
    i = 0;  //method -> index
    j = 0;  //buf -> index

    //提取方法和url
    while(!isspace(buf[j]) && j <sizeof(buf) && buf[j] != '\0' && i < sizeof(method) - 1){
        method[i] = buf[j];
        i++, j++;
    }

    //既不是GET也不是POST则出错了
    method[i] = '\0';
    if(strcasecmp(method, "GET") != 0 && strcasecmp(method, "POST") != 0){
        echo_msg(sock);
        ret = 6;
        goto end;
    }

    //如果是POST方法则以走CGI部分
    if(strcasecmp(method, "POST") == 0){
        cgi = 1;
    }

    //buf -> "GET / http/1.0"
    while(isspace(buf[j]) && j < sizeof(buf)){
        j++;
    }
    i = 0;

    while(!isspace(buf[j]) && j < sizeof(buf) && i < sizeof(url) - 1){
        url[i] = buf[j];
        i++, j++;
    }
    url[i] = '\0';

    printf("method:%s url:%s\n", method, url);

    query_string = url; //命令行参数

    while(*query_string != '\0'){
        if(*query_string == '?'){
            *query_string = '\0';
            query_string++;
            cgi = 1;
            break;
        }
        query_string++;
    }
    //处理url

    sprintf(path, "wwwroot%s", url);

    //method, url, query_string, cgi
    if(path[strlen(path)-1] == '/'){
        strcat(path, "index.html");
    }

    //最后一个字符绝对不是'/'
    //判断资源合法性,即判断path目录下的资源是否存在
    
    printf("path:%s cgi:%d query_string:%s\n", path, cgi, query_string);

    struct stat st;
    if(stat(path, &st) != 0){//目录不合法//??????????????
        printf("ERROR DIR\n");
        echo_msg(sock);
        ret = 7;
        goto end;
    }else{
        if(S_ISDIR(st.st_mode)){
            strcat(path, "/index.html");
        }else if((st.st_mode & S_IXUSR) || \
                 (st.st_mode & S_IXGRP) || \
                 (st.st_mode & S_IXOTH)){//说明是可执行文件
            cgi = 1;
        }else{
            //
        }

        //path绝对不是目录
        if(cgi){//可执行文件
            //exec
            exec_cgi(sock, method, path, query_string);
        }else{//普通文件,直接返回数据
            drop_handler(sock);
            echo_www(sock, path, st.st_size);
        }
    }
    

end:
    printf("quit client...\n");
    close(sock);
    //return (void*)ret;
    return NULL;
#endif
}

