#include "iwatch/impl/eventloop.h"
#include <thread>         // std::thread, std::thread::id, std::this_thread::get_id
#include <chrono>         // std::chrono::seconds
#include <iostream>
using namespace std;
//EventLoop EventLoop::m_eventLoop;
void EventLoop::show()
{
    inforData data;
    while (!m_isItOver)
    {
        if(EventLoop::getMessage(data) ==-1)
        {
//            cout<<"EventLoop::show--queue is no data"<<endl;
        }
        else
        {
            if(EventLoop::doFun(data.command,data.data))
            {
//                cout<<"EventLoop::show the call back fun is null"<<endl;
            }
            else
            {
//                cout<<"EventLoop::show the command is"<<data.command<<endl;
            }
        }
        this_thread::sleep_for(chrono::microseconds(1000));
    }
}


EventLoop::EventLoop()
{
    m_isItOver = false;
    thread t1(&EventLoop::show,this);
    t1.detach();
}

EventLoop::~EventLoop()
{
    m_isItOver = true;
}
//EventLoop *EventLoop::getEventLoop()
//{
//    return &m_eventLoop;
//}
int EventLoop::setMessage(int command,void* data)
{
    inforData infor_data_tmp;
    infor_data_tmp.command =command;
    infor_data_tmp.data = data;
    m_messageInQueue_mutex.lock();
    m_messageInQueue.push(infor_data_tmp);
    m_messageInQueue_mutex.unlock();
    return 0;
}
int EventLoop::getMessage(inforData &data)
{
    if(m_messageInQueue.empty())
        return -1;
    data = m_messageInQueue.front();
    m_messageInQueue_mutex.lock();
    m_messageInQueue.pop();
    m_messageInQueue_mutex.unlock();
    return 0;
}
int EventLoop::setFun(FunDoAll fun)
{
    m_fun_mutex.lock();
    m_fun = fun;
    m_fun_mutex.unlock();
    return 0;
}
int EventLoop::doFun(int command,void* data)
{
    if(m_fun == nullptr)
        return -1;
    m_fun(command,data);
    return 0;
}
