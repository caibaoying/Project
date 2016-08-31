#pragma once
#include <iostream>

#include <string>
#include "json/json.h"

using namespace std;

class my_json
{
    public:
        my_json();
        ~my_json();

        static int serlialize(Json::Value& in_val, string& str_out);
        static int unserlialize(string& in_str, Json::Value& val_out); 
    private:
        Json::Value val;
        string str;
};
