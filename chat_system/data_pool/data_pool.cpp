#include <iostream>
using namespace std;

#include "data_pool.h"
#include <pthread.h>


data_pool::data_pool(int _size = 128)
    :pool(_size)
     ,capacity(_size)
     ,index_pro(0)
     ,index_com(0)
{
    sem_init(&blank_sem, 0, _size);
    sem_init(&data_sem, 0, 0);
}

void data_pool::data_get(string& _msg_out)
{
    sem_wait(&data_sem);
    _msg_out = pool[index_com];
    cout<<_msg_out<<endl;
    index_com++;
    index_com %= capacity;
    sem_post(&blank_sem);
}

void data_pool::data_put(const string& _msg)
{
    sem_wait(&blank_sem);
    pool[index_pro] = _msg;
    index_pro++;
    index_pro %= capacity;
    sem_post(&data_sem);
}

data_pool::~data_pool()
{

}
