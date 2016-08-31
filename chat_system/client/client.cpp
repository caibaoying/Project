#include "client.h"
#include "udp_data.h"
#include "chat_window.h"

using namespace std;

udp_client::udp_client(const string& _ip = _IP_, const int& _port = _PORT_)
    :sock(-1)
    ,ip(_ip)
    ,port(_port)
{
    printf("client create\n");
}

void udp_client::init()
{
	sock = socket(AF_INET,SOCK_DGRAM,0);
	if(sock<0)
	{
		print_log(strerror(errno),__FUNCTION__,__LINE__);
		exit(1);
	}
}
ssize_t udp_client::udp_send(string& in)
{
	struct sockaddr_in remote;
	ssize_t len = sizeof(remote);
	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	remote.sin_addr.s_addr = inet_addr(ip.c_str());

    ssize_t _ret = sendto(sock, in.c_str(), in.size(), 0, (struct sockaddr*)&remote, len);
    if(_ret < 0)
    {
        print_log(strerror(errno), __FUNCTION__, __LINE__);
    }
    return _ret;
}
ssize_t udp_client::udp_recv(string& out)
{
    struct sockaddr_in remote;
    socklen_t len = sizeof(remote);
    char buf[_SIZE_];
    memset(buf, '\0', sizeof(buf));
    ssize_t _ret = recvfrom(sock, buf, sizeof(buf)-1, 0, (struct sockaddr*)&remote, &len);
    if( _ret > 0 )
    {
        out = buf;
    }
    else
    {
        print_log(strerror(errno), __FUNCTION__, __LINE__);
    }
    return _ret;
}
bool udp_client::IsFriendExit(const string& key)
{
	vector<string>::iterator iter = flist.begin();
	for(;iter != flist.end();iter++)
	{
		if(*iter == key)
			return true;
	}
	return false;
}
void udp_client::add_flist(string& user)
{
	if(!IsFriendExit(user))
	{
		flist.push_back(user);
	}
	else
	{
		return;
	}
}
void udp_client::del_flist(string& old_user)
{
	if(IsFriendExit(old_user))
	{
		vector<string>::iterator iter = flist.begin();
		for(;iter != flist.end();iter++)
		{
			if(*iter == old_user)
			{
				flist.erase(iter);
				return;
			}
		}
	}
	else
	{
		return;
	}
}
udp_client::~udp_client()
{
    if(sock != -1)
    {
        close(sock);
    }
    printf("client destroy\n");
}
