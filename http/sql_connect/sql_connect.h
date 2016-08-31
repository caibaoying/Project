#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include "mysql.h"

using namespace std;

class sql_connecter
{
    public:
        sql_connecter(const string& _host, const string& _user, const string& _passwd, const string& _db);
        
        //connect remote mysql
        bool begin_connect();
        bool insert_sql(const string& data);
        bool select_sql(string field_name[], string _out_str[][5], int& _out_row);

        bool updata_sql();
        bool delete_sql();
        bool delete_table();
        bool create_table();

        //close remote link
        bool close_connect();
        ~sql_connecter();
        void show_info();
    private:
        MYSQL_RES* res;
        MYSQL* mysql_base;
        string host;
        string user;
        string passwd;
        string db;
};

