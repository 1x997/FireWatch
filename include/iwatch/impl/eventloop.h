/**
* @projectName   iwatch
* @brief         "eventloop是一个消息循环，现在实现是单例模式，将所有消息放到里面循环，处理。升级方案打破单例，每个eventloop对象完成自己的任务"
* @author        haichi
* @date          2019-01-28
*/
#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include <queue>
#include <mutex>
//enum functionCommand
//{

//};
typedef int (*FunDoAll)(int command,void *data);
typedef struct inforData
{
    int command;
    void *data;
}inforData;


class EventLoop
{
private:
    void show();
    int doFun(int command,void* data);
    int getMessage(inforData &data);

//    static EventLoop m_eventLoop;
    std::queue<inforData> m_messageInQueue;//注意这里是无锁队列，非常危险
    FunDoAll m_fun;
    std::mutex m_fun_mutex;
    std::mutex m_messageInQueue_mutex;
    bool m_isItOver;
public:
    EventLoop();
    /**
     * @brief getEventLoop 得到一个EventLoop对象
     * @return 返回一个EventLoop对象（暂时是单例模式，返回的是同一个对象）
     */
//    static EventLoop* getEventLoop();

    /**
     * @brief setMessage 将命令放入EventLoop,EventLoop回自动调用setFun绑定的回调
     * @param command setFun中绑定回调的第一参数
     * @param data setFun中绑定回调的第二参数
     * @return 成功为0，失败为-1
     */
    int setMessage(int command,void* data);

    /**
     * @brief setFun 为EventLoop设置回调（一个EventLoop只可设置一个回调，第二次设置回调时第一次的回调失效）
     * @param fun 要设置的回调函数
     * @return 成功为0，失败为-1
     */
    int setFun(FunDoAll fun);
    ~EventLoop();
};

#endif // EVENTLOOP_H
