#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "sql_connect.h"

using namespace std;

const string _remote_ip = "10.110.192.33";
const string _remote_user = "pig";
const string _remote_passwd = "";
const string _remote_db = "fuck";

int main()
{
    int content_length = -1;
    char method[1024];
    char query_string[1024];
    char post_data[4096];
    memset(method, '\0', sizeof(method));
    memset(query_string, '\0', sizeof(query_string));
    memset(post_data, '\0', sizeof(post_data));

    cout<<"<html>"<<endl;
    cout<<"<head>INSERT FRIENDS</head>"<<endl;
    cout<<"<body>"<<endl;

    char name[64];
    char age[64];
    char school[64];
    char hobby[64];
    memset(name, '\0', sizeof(name));
    memset(age, '\0', sizeof(age));
    memset(school, '\0', sizeof(school));
    memset(hobby, '\0', sizeof(hobby));

    string insert_data = "'yaozi', '23', 'sust', 'fuck'";
    const string _host = _remote_ip;
    const string _user = _remote_user;
    const string _passwd = _remote_passwd;
    const string _db = _remote_db;


    sql_connecter conn(_host, _user, _passwd, _db);
    conn.insert_sql(insert_data);

    cout<<"</body>"<<endl;
    cout<<"</html>"<<endl;

}

