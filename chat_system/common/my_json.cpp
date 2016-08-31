#include "my_json.h"

my_json::my_json()
{}
my_json::~my_json()
{}
int my_json::serlialize(Json::Value& in_val, string& str_out)
{
#ifdef _FAST_
    Json::FastWriter _write;
#else
    Json::StytleWriter _write;
#endif
    str_out = _write.write(in_val);
}
int my_json::unserlialize(string& in_str, Json::Value& val_out)
{
    Json::Reader _reader;
    if(_reader.parse(in_str, val_out, false) > 0)
    {
        return 0;
    }
    return 1;
}

#ifdef _DEBUG_
int main()
{
    // serlialize
    Json::Value _val;
    _val["nick_name"] = "zhangke";
    _val["school"] = "sust";
    _val["msg"] = "hello";

    my_json::serlialize(_val, str_out);
    cout<< str_out <<endl;

    //unserlialize
    Json::Value _val_out;
    my_json::unserlialize(str_out, _val_out);

    cout<< _val_out["nick_name"].asString() <<endl;
    cout<< _val_out["school"].asString() <<endl;
    cout<< _val_out["msg"].asString() <<endl;

    return 0;
}
#endif
