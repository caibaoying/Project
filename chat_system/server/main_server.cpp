#include <iostream>
using namespace std;

#include "server.h"
#include <vector>

void* get_data(void* arg)
{
    udp_server* server = (udp_server*)arg;
	bool done = false;
	string msg;
    while(!done)
	{
		server->udp_recv(msg);
        //cout<<msg<<endl;
    }
}

void* send_data(void* arg)
{
    udp_server* s = (udp_server*)arg;
	bool done = false;
    string _out;
    while(!done)
    {
		s->broadcast_msg(_out);
        //cout<<_out<<endl;
    }
}

static void usage(const char* proc)
{
    cout<<"usage: "<<proc<<" [ip] "<<" [port] "<<endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        usage(argv[0]);
        exit(1);
    }


	string ip = argv[1];
    int port = atoi(argv[2]);
    udp_server server(ip,port);
    server.init();

    pthread_t id1,id2;
    pthread_create(&id1, NULL, get_data, (void*)&server);
    pthread_create(&id2, NULL, send_data, (void*)&server);

    pthread_join(id1, NULL);
    pthread_join(id2, NULL);

    return 0;
}
