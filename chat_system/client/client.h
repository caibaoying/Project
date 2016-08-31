#pragma once

#include <iostream>
using namespace std;

#include "comm.h"
#include <vector>

class udp_client
{
    public:
        udp_client(const string& _ip, const int& _port);
        
        void init();
        void add_flist(string& user);
		void del_flist(string& old_user);
		bool IsFriendExit(const string& key);

        ssize_t udp_send(string& in);
        ssize_t udp_recv(string& out);
        ~udp_client();

        vector<string> flist;

    private:
    int sock;
    string ip;
    int port;
};
