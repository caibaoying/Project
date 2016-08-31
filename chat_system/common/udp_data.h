#pragma once 

#include <iostream>
using namespace std;


class udp_data
{
    public:
        udp_data();
        void set_nick_name(const string& _n)
        {
            nick_name  =  _n;
        }

        void set_school(const string& _s)
        {
            school = _s;
        }

        void set_msg(const string& _m)
        {
            msg = _m;
        }

        void set_cmd(const string& _c)
        {
            cmd = _c;
        }

        void get_nick_name(string& _n)
        {
            _n = nick_name;
        }

        void get_school(string& _s)
        {
            _s = school;
        }
        void get_msg(string& _m)
        {
            _m = msg;
        }
        void get_cmd(string& _c)
        {
            _c = cmd;
        }
        
        bool val_to_str(string& _out_msg);
        bool str_to_val(string& _in_msg);

        ~udp_data();
    private:
        string nick_name;
        string school;
        string cmd;
        string msg;
};

