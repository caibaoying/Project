#pragma once

#include <iostream>
using namespace std;

#include <string>
#include <vector>
#include <semaphore.h>

class data_pool
{
    public:
        data_pool();
        data_pool(int _size);
        void data_get(string& _msg_out);
        void data_put(const string& _msg);
        ~data_pool();
		
        
    private:
        vector<string> pool;
        int capacity;
        
        int index_pro;
        int index_com;

        sem_t blank_sem;
        sem_t data_sem;
};
