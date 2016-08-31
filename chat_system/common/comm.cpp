#include <iostream>

using namespace std;

#include "comm.h"

void print_log(const char* str, const string& func,const int& line)
{
    cout<<*str<<func<<line<<endl;  
}
