#include "sql_connect.h"

sql_connecter::sql_connecter(const string& _host, const string& _user, const string& _passwd, const string& _db)
{
    mysql_base = mysql_init(NULL);
    res = NULL;
    host = _host;
    user = _user;
    passwd = _passwd;
    db = _db;
}
//connect remote mysql
bool sql_connecter::begin_connect()
{
    if(mysql_real_connect(mysql_base, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 3306, NULL, 0) == NULL)
    {
#ifdef _DEBUG_
        cout<<"connect failed\n"<<endl;
#endif
        return false; 
    }
    else
    {
#ifdef _DEBUG_
        cout<<"connect done..."<<endl;
#endif
    }
    return true;
}

bool sql_connecter::insert_sql(const string& data)
{
    string sql = "INSERT INTO friends (name, age, school, hobby) values ";
    sql += "(";
    sql += data;
    sql += ");";
    
    if(mysql_query(mysql_base, sql.c_str()) == 0)
    {
#ifdef _DEBUG_
        cout<<"query success!"<<endl;
#endif
        return true;
    }
    else
    {
#ifdef _DEBUG_
        cout<<"query failed"<<endl;
#endif
        return false;
    }
}
bool sql_connecter::select_sql(string field_name[], string _out_str[][5], int& _out_row)
{
    string sql = "SELECT * FROM friends";
    if(mysql_query(mysql_base, sql.c_str()) == 0)
    {
#ifdef _DEBUG_
        cout<<"query success"<<endl;
#endif
    }
    else
    {
#ifdef _DEBUG_
        cout<<"query failed"<<endl;
#endif
    }
    res = mysql_store_result(mysql_base);
    int row_num = mysql_num_rows(res);
    int field_num = mysql_num_fields(res);
    _out_row = row_num;

#ifdef _DEBUG_
    cout<<mysql_num_rows(res)<<endl;
    cout<<mysql_num_fields(res);
#endif
    // fields name
    MYSQL_FIELD* fd = NULL;
    int i = 0;
    for(; fd = mysql_fetch_field(res); )
    {
#ifdef _DEBUG_
        cout<<fd->name<<"\t";
#endif
        field_name[i++] = fd->name;
    }
#ifdef _DEBUG_
    cout<<endl;
#endif
    for(int index = 0; index < row_num; index++)
    {
        MYSQL_ROW _row = mysql_fetch_row(res);
        if(_row)
        {
            int start = 0;
            for(; start < field_num; start++ )
            {
                _out_str[index][start] = _row[start];
#ifdef _DEBUG_
                cout<<_row[start]<<"\t";
#endif
            }
#ifdef _DEBUG_
            cout<<endl;
#endif
        }
    }
    return true;
}

bool sql_connecter::updata_sql()
{

}
bool sql_connecter::delete_sql()
{

}
bool sql_connecter::delete_table()
{

}
bool sql_connecter::create_table()
{

}

//close remote link
bool sql_connecter::close_connect()
{
    mysql_close(mysql_base);
#ifdef _DEBUG_
    cout<<"connect close..."<<endl;
#endif

}
sql_connecter::~sql_connecter()
{
    close_connect();
    if(res != NULL)
    {
        free(res);
    }
}
void sql_connecter::show_info()
{
    cout<<"mysql_get_client_info: "<<mysql_get_client_info()<<endl;
}


#ifdef _DEBUG_
int main()
{
    string _sql_data[1024][5];
    string header[5];
    int curr_row = -1;
    const string _host = "10.110.196.51";
    const string _user = "pig";
    const string _passwd = "";
    const string _db = "fuck";
    const string data = "'窑子', 24, 'xgd', 'sleep'";
    sql_connecter conn(_host, _user, _passwd, _db);
    
    conn.begin_connect();
    conn.insert_sql(data);
    conn.select_sql(header, _sql_data, curr_row);
    sleep(1);
    
    for(int i = 0; i < 5; i++)
    {
        cout<<header[i]<<"\t";
    }
    cout<<endl;

    for(int i = 0; i < curr_row; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            cout<<_sql_data[i][j]<<"\t";
        }
        cout<<endl;
    }

    return 0;
}
#endif
