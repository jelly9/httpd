
#include "sql_api.h"
using namespace std;

void insertToDb(const char *arg)
{
    char arg_buff[1024];

    //name="a"&sex="man"&age="20"&hobby="coding"&school="SUST"
    
    char *myargv[6];
    char *start = arg_buff;
    strcpy(arg_buff, arg);
    int i = 0;
    while(*start){
        if(*start == '='){
            myargv[i++] = start;
        }
        if(*start == '&'){
            *start = '\0';
        }
        start++;
    }


    myargv[i] = '\0';
    sqlApi mydb("127.0.0.1", \
                3306, \
                "root", \
                "m9", \
                "httpd");
    mydb.connect();
    mydb.insert(myargv[0], myargv[1], myargv[2], myargv[3], myargv[4]);
}

int main()
{
#ifdef _DEBUG_
    sqlApi mydb("127.0.0.1", \
                3306, \
                "root", \
                "m9", \
                "httpd");
    mydb.connect();
    mydb.select();
#else

    char *method = NULL;
    char *query_string = NULL;
    char *string_arg = NULL;
    int content_len = -1;
    char buf[1024];
    if((method = getenv("METHOD"))){
        if(strcasecmp(method, "GET") == 0){\
            if((query_string = getenv("QUERY_STRING"))){
                string_arg = query_string;
            }
        }
        else if(strcasecmp(method, "POST") == 0){
            if(getenv("CONTET_LEN")){
                content_len = atoi(getenv("CONTENT_LEN"));
                int i = 0;
                for(; i < content_len; i++){
                    read(0, &buf[i], 1);
                }

                buf[i] = '\0';
                string_arg = buf;
            }
        }
    }

    insertToDb(string_arg);

#endif
    return 0;
}



