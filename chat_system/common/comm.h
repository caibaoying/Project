#pragma once

#include <iostream>
using namespace std;

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define _SIZE_ 1024
#define _IP_ "127.0.0.1"
#define _PORT_ 8080

void print_log(const char* str, const string& func,const int& line);

