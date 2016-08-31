#include <iostream>
using namespace std;

#define _MAXSIZE_ 5096
#include "server.h"

int udp_server::add_user(struct sockaddr_in &client)
{
    string _ip_key = inet_ntoa(client.sin_addr);
    map<string, struct sockaddr_in>::iterator _iter = online_user.find(_ip_key);

    if(_iter == online_user.end())
    {
        online_user.insert(pair<string, struct sockaddr_in>(_ip_key, client));
    }
    return 0;
}

udp_server::udp_server(const string& _ip = _IP_,const int& _port = _PORT_)
    :ip(_ip)
    ,port(_port)
    ,sock(-1)
	,pool(_MAXSIZE_)
{}

void udp_server::init()
{
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        print_log(strerror(errno), __FUNCTION__, __LINE__);
        exit(1);
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(ip.c_str());

    if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        print_log(strerror(errno), __FUNCTION__, __LINE__);
        exit(1);
    }
}

ssize_t udp_server::udp_send(const string& in,const struct sockaddr_in &cli, socklen_t len)
{
    ssize_t _ret = sendto(sock, in.c_str(), in.size(), 0, (struct sockaddr*)&cli, len);
    if(_ret < 0)
    {
        print_log(strerror(errno), __FUNCTION__, __LINE__);
    }
    return _ret;
}

ssize_t udp_server::udp_recv(string& out)
{
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    char buf[_SIZE_];
    memset(buf, '\0', sizeof(buf));
    ssize_t _ret = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&client, &len);
    cout<<buf<<endl;
    cout<<_ret<<endl		pool.data_put(out);
        add_user(client);
    }
    else
    {
        print_log(strerror(errno), __FUNCTION__, __LINE__);
    }
    return _ret;
}

void udp_server::broadcast_msg(string& out)
{
    string msg;
    pool.data_get(msg);

    out = msg;
    map<string, struct sockaddr_in>::iterator iter = online_user.begin();
   
    for(; iter != online_user.end(); iter++)
    {
         udp_send(msg, iter->second, sizeof(iter->second));
    }
}

udp_server::~udp_server()
{
    if(sock != -1)
    {
        close(sock);
    }
    printf("server destroy\n");
}
