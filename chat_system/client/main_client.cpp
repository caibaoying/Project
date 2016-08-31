#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "client.h"
#include "udp_data.h"
#include "chat_window.h"

using namespace std;

chat_window* win_p = NULL;
string _nick_name;
string _school;
string _cmd = "None";

static void usage(const char* proc)
{
    cout<<"usage: "<<proc<<" [remote_ip] " " [remote_port] "<<endl;
}

void* draw_header(void* arg)
{
    udp_client *cli_p = (udp_client*)arg;
    win_p->create_header();
    win_p->flush_window(win_p->header);

    int max_y, max_x;
    getmaxyx(win_p->header, max_y, max_x);

    string head_line = "Welcome bit-tech";
    int index = 1;

    while(1)
    {
        win_p->put_str(win_p->header, max_y/2, index++, head_line);
        if(index >= (max_x*3)/4)
        {
            index = 1;
        }

        win_p->flush_window(win_p->header);
        usleep(500000);
        win_p->clear_win_line(win_p->header, max_y/2, 1);
                
        
    }
    return NULL;
}

void* draw_output(void *arg)
{
    udp_client* cli_p = (udp_client*)arg;
    win_p->create_output();
    win_p->create_flist();
    win_p->flush_window(win_p->output);
    win_p->flush_window(win_p->flist);
    
    string _recv_str;
    udp_data data;
    string _nn;
    string _s;
    string _c;
    string _m;

    int max_y, max_x;
    getmaxyx(win_p->output, max_y, max_x);

    int max_fy, max_fx;
    getmaxyx(win_p->flist, max_fy, max_fx);
    string flist_str;

    int index = 1;
    while(1)
    {
        cli_p->udp_recv(_recv_str);

        data.str_to_val(_recv_str);

        data.get_nick_name(_nn);
        data.get_school(_s);
        data.get_cmd(_c);
        data.get_msg(_m);

        string _win_str = _nn;
        _win_str += "-";
        _win_str += _s;

        flist_str = _win_str;
        _win_str += "# ";
        _win_str += _m;

        flist_str += _m;
        cli_p->add_flist(flist_str);

        win_p->put_str(win_p->output, index++, 1, _win_str);
        if(index >= max_y-1)
        {
            win_p->flush_window(win_p->output);
            sleep(1);
            index = 1;
            win_p->clear_win_line(win_p->output, 1, max_y);
        }

        int i = 0;
        for(; i < cli_p->flist.size(); i++)
        {
            win_p->put_str(win_p->flist, i+1, 1, cli_p->flist[i]);
        }
        win_p->flush_window(win_p->flist);
        win_p->flush_window(win_p->output);

    }

    return NULL;
}

void* draw_flist(void* arg)
{
    sleep(3);
    udp_client* cli_p = (udp_client*)arg;
    win_p->create_flist();
    win_p->flush_window(win_p->flist);
    while(1)
    {
        sleep(1);
    }

    return NULL;
}

void* draw_input(void* arg)
{
    udp_client* cli_p = (udp_client*)arg;

    string cmd_line = "Please Enter# ";
    win_p->create_input();
    string _win_str;
    string send_str;
    udp_data data;

    while(1)
    {
        win_p->put_str(win_p->input, 1, 1, cmd_line);
        win_p->flush_window(win_p->input);
        win_p->get_str(win_p->input, _win_str);
        win_p->clear_win_line(win_p->input, 1, 1);

        data.set_nick_name(_nick_name);
        data.set_school(_school);
        data.set_cmd(_cmd);
        data.set_msg(_win_str);

        data.val_to_str(send_str);

        cli_p->udp_send(send_str);

        usleep(100000);
    }
    return NULL;
}

int main(int argc, char*argv[])
{
    if(argc != 3)
    {
        usage(argv[0]);
        exit(1);
    }

    string _ip = argv[1];
    int _port = atoi(argv[2]);

    string _sstr;

    cout<<"Please enter your nick_name# ";
    fflush(stdout);
    cin>>_nick_name;
	cout<<"Please enter your school# ";
    fflush(stdout);
    cin>>_school;

    udp_client _cli(_ip, _port);
    _cli.init();
    chat_window win;
    win_p = &win;
    
    pthread_t header, output, flist, input;
    pthread_create(&header, NULL, draw_header, (void*)&_cli);
    pthread_create(&output, NULL, draw_output, (void*)&_cli);
    pthread_create(&flist, NULL, draw_flist, (void*)&_cli);
    pthread_create(&input, NULL, draw_input, (void*)&_cli);

    pthread_join(header, NULL);
    pthread_join(output, NULL);
    pthread_join(flist, NULL);
    pthread_join(input, NULL);

    return 0;
}
