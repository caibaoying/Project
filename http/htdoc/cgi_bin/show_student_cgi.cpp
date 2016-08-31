#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
    cout<<"<head>show friends</head>"<<endl;
    cout<<"<body>"<<endl;

    const string _host = _remote_ip;
    const string _user = _remote_user;
    const string _passwd = _remote_passwd;
    const string _db = _remote_db;

    string _sql_data[1024][5];
    string header[5];
    int curr_row = -1;

    sql_connecter conn(_host, _user, _passwd, _db);
    conn.begin_connect();
    
    conn.select_sql(header, _sql_data, curr_row);
   
    cout<<"<table border=\"1\">"<<endl;
    cout<<"<tr>"<<endl;

    for(int i= 0; i < 5; i++)
    {
        cout<<"<th>"<<header[i]<<"</th>"<<endl;
    }

    
    for(int i = 0; i < curr_row; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            cout<<_sql_data[i][j]<<"\t";
        }
        cout<<"<br/>";
    }

    cout<<"</tr>"<<endl;
    cout<<"</table>"<<endl;


    cout<<"</body>"<<endl;
    cout<<"</html>"<<endl;

}
