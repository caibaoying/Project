#pragma once
#include <iostream>
#include "comm.h"
#include "data_pool.h"
#include <map>
#include "udp_data.h"
#include <pthread.h>
#include <netinet/in.h>

using namespace std;

class udp_server
{
    int add_user(struct sockaddr_in &client);

    public:
        udp_server(){}
        udp_server(const string& _ip, const int& _port);
        void init();
        ssize_t udp_send(const string& in,const struct sockaddr_in &cli, socklen_t len);
        ssize_t udp_recv(string& out);

        void broadcast_msg(string& out);
   
        ~udp_server();
		
       
    private:
   // public:
        int sock;
        string ip;
        int port;

        data_pool pool;
        map<string, struct sockaddr_in> online_user;
};
