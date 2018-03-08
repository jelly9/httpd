
#include "sql_api.h"
using namespace std;

void selectFromDb(const char *arg)
{
    sqlApi mydb("127.0.0.1", \
                3306, \
                "root", \
                "m9", \
                "httpd");
    mydb.connect();
    mydb.select();
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

    selectFromDb(string_arg);

#endif
    return 0;
}



