#pragma once

#include <iostream>
#include <ncurses.h>
#include <string>

using namespace std;

class chat_window
{
    public:
        chat_window();

        void init();

        void flush_window(WINDOW* _win);
        void put_str(WINDOW* _win, int _y, int _x, string& str);
        void get_str(WINDOW* _win, string& out_str);
        void clear_win_line(WINDOW* _win, int begin_y, int nums);

        void create_header();
        void create_output();
        void create_flist();
        void create_input();

        ~chat_window();
    //private:
    public:
        WINDOW* header;
        WINDOW* output;
        WINDOW* flist;
        WINDOW* input;
};
