
#include "sql_api.h"
using namespace std;


int main()
{
    cout << mysql_get_client_info() << endl;

    sqlApi mydb("127.0.0.1", \
                3306, \
                "root", \
                "m9", \
                "httpd");
    mydb.connect();
    mydb.insert("LMM", "nv", "19", "study", "SUST");
    mydb.select();

    return 0;
}



