
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
}

int sqlApi::connect()
{
    if(mysql_real_connect(m_conn, m_host.c_str(), \
                          m_user.c_str(), m_passwd.c_str(),\
                          m_db.c_str(), m_port, NULL, 0)){
        cout << "connect seccess!" << endl;

    }else{
        cout << "connect faild!" << endl;
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
        cout << "insert sucess!" << endl;
    }
    else{
        cout << "insert faild!" << endl;
    }

}

int sqlApi::select()
{
    string sql = "SELECT * FROM http_test1";
    if(mysql_query(m_conn, sql.c_str()) == 0){
        m_res = mysql_store_result(m_conn);
        cout << "m_res: " << m_res << endl;


        if(m_res){
            int rows = mysql_num_rows(m_res);
            int col =  mysql_num_fields(m_res);
            cout << "rows: " << rows << "col: " << col << endl;

        //    free(m_res);
        }
    }
 
    //mysql_store_result(mconn);
}


sqlApi::~sqlApi()
{
    mysql_close(m_conn);
}
