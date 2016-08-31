#include  <iostream>
using namespace std;

#include "chat_window.h"
#include "comm.h"

chat_window::chat_window()
{
	initscr();
}


void chat_window::flush_window(WINDOW* _win)
{
    box(_win, 0, 0);   
    wrefresh(_win);
}

void chat_window::put_str(WINDOW* _win, int _y, int _x, string& str)
{
    mvwaddstr(_win, _y, _x, str.c_str());
}
        
void chat_window::get_str(WINDOW* _win, string& out_str)
{
    char buf[_SIZE_];
    memset(buf, '\0', sizeof(buf));
    wgetnstr(_win, buf, sizeof(buf));
}

void chat_window::clear_win_line(WINDOW* _win, int begin_y, int nums)
{
    while(nums-- > 0)
    {
        wmove(_win, begin_y++, 0);
        wclrtoeol(_win);
    }
}

void chat_window::create_header()
{
    int height = LINES / 5;
    int width = COLS;
    int y = 0;
    int x = 0;

    header = newwin(height, width, y, x);
}
void chat_window::create_output()
{
    int height = LINES*3 / 5;
    int width = 3*COLS / 4;
    int y = LINES/5;
    int x = 0;

    output = newwin(height, width, y, x);
}
void chat_window::create_flist()
{
    int height = LINES*3 / 5;
    int width = COLS / 4;
    int y = LINES / 5;
    int x = 3*COLS / 4;

    flist = newwin(height, width, y, x);
}
void chat_window::create_input()
{
    int height = LINES / 5;
    int width = COLS;
    int y = 4*LINES / 5;
    int x = 0;

    input = newwin(height, width, y, x);
}

chat_window::~chat_window()
{
    delwin(header);
    delwin(output);
    delwin(flist);
    delwin(input);
    endwin();
}
