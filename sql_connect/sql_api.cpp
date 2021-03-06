
#include "sql_api.h"

sqlApi::sqlApi(const string &h, const int port, const string &u, const string &p, const string &db)
{
    m_host = h;
    m_res = NULL;
    m_user = u;
    m_passwd = p;
    m_db = db;
    m_port = port;
    m_conn = mysql_init(NULL);//初始化
    cout << "init sql_api success" << endl;
}

int sqlApi::connect()
{
    if(mysql_real_connect(m_conn, m_host.c_str(), \
                          m_user.c_str(), m_passwd.c_str(),\
                          m_db.c_str(), m_port, NULL, 0)){
        cout << "connect seccess" << endl;

    }else{
        cout << "connect faild" << endl;
    }
}

//name sex age hobby schoo:wq
int sqlApi::insert(const string& name, const string& sex, const string& age, const string& hobby, const string& school) 
{
    string sql = "INSERT INTO http_test1 (name, sex, age, hobby, school) VALUES ('";
    sql += name;
    sql += "','";
    sql += sex;
    sql += "','";
    sql += age;
    sql += "','";
    sql += hobby;
    sql += "','";
    sql += school;
    sql += "')";


    int ret = mysql_query(m_conn, sql.c_str());//像数据库插入函数
    if(ret == 0){
        cout << "insert sucess" << endl;
    }
    else{
        cout << "insert faild" << endl;
    }

}

int sqlApi::select()
{
    string sql = "SELECT * FROM http_test1";
    if(mysql_query(m_conn, sql.c_str()) == 0){
        m_res = mysql_store_result(m_conn);

        if(m_res){
            cout << "select success" << endl;
            int rows = mysql_num_rows(m_res);
            int col =  mysql_num_fields(m_res);
            cout << "rows: " << rows << " col: " << col << endl;

            MYSQL_FIELD *fd;
            while(fd = mysql_fetch_field(m_res)){
                cout << fd->name << " ";
            }
            cout << endl;

            int i = 0;
            int j = 0; 
            for(; i < rows; i++){
                MYSQL_ROW row_res = mysql_fetch_row(m_res);
                j = 0;
                for(; j < col; j++){
                    cout << row_res[j] << ',';
                }
                cout << endl;
            }
            cout << endl;
        }
        else{
            cout << "select faild" << endl;
        }
    }
    //mysql_store_result(mconn);
}

sqlApi::~sqlApi()
{
    mysql_close(m_conn);
}
