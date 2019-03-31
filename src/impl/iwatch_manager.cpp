//
// Created by Lian chenyu on 2018-12-18.
//

#include "iwatch/impl/iwatch_manager.h"
#include "iwatch/iwatch_session.h"
#include "iwatch/impl/eventloop.h"
#include <chrono>
#include <map>
#include <thread>

using namespace std;
using namespace std::chrono;

//engine map
static map<int64_t, string> engine_modelRoot_map;

//iwatch map
static map<int64_t, iWatch *> watch_map;
static map<int64_t, int> iwatch_ret_map;
static map<int64_t, EventLoop *> eventLoop_map;

int eneyes::iwatch::impl::allocate_engine(int64_t &engine_id,const char* model_root) {
    microseconds now_in_ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
    engine_id = now_in_ms.count();
    string root;
    if(model_root != nullptr)
    {
       root = model_root;
       if(root[root.length()-1] != '/')
       {
           root+='/';
       }
    }
    engine_modelRoot_map[engine_id] = root;
    return 0;
}
void eneyes::iwatch::impl::release_engine(int64_t engine_id) {
    engine_modelRoot_map.erase(engine_id);
}
string eneyes::iwatch::impl::get_engine_modelRoot_map(int64_t engine_id) {
    return engine_modelRoot_map[engine_id];
}
iWatch *eneyes::iwatch::impl::allocate_watch(int64_t &watch_id) {
    microseconds now_in_ms = duration_cast<microseconds>(system_clock::now().time_since_epoch());
    watch_id = now_in_ms.count();

    iWatch *watch = new iWatch();
    watch_map[watch_id] = watch;
    iwatch_ret_map[watch_id] = -1;
    eventLoop_map[watch_id] = new EventLoop;


    return watch;
}

void eneyes::iwatch::impl::release_watch(int64_t watch_id) {
    iWatch *watch = watch_map[watch_id];
    if (nullptr != watch) {
        delete watch;
        watch_map.erase(watch_id);
    }

}

iWatch *eneyes::iwatch::impl::get_watch(int64_t watch_id) {
    return watch_map[watch_id];
}
EventLoop *eneyes::iwatch::impl::get_eventLoop(int64_t watch_id) {
    return eventLoop_map[watch_id];
}
void eneyes::iwatch::impl::clear_all_watch() {
    map<int64_t, iWatch *>::iterator itr;
    itr = watch_map.begin();
    while (itr != watch_map.end()) {
        delete itr->second;
        watch_map.erase(itr->first);
    }
}
//EventLoop *eneyes::iwatch::impl::get_engineSharedEventLoop()
//{
//    return engineSharedEventLoop;
//}
/**
 * @brief eneyes::iwatch::impl::get_iWatchSessionStatus 该方法实现方式很危险，待重构
 * @param watch_id
 * @return
 */
iWatch_Session_Status eneyes::iwatch::impl::get_iWatchSessionStatus(int64_t watch_id)
{
    auto  iter = iwatch_ret_map.find(watch_id);
    if(iter == iwatch_ret_map.end())
    {
        return iWatch_SS_UNKNOWN;//错误类型待修订
    }
    while(iwatch_ret_map[watch_id] == -1)
    {
        this_thread::sleep_for(chrono::microseconds(1000));
    }
    int ret =  iwatch_ret_map[watch_id];
    iwatch_ret_map[watch_id] = -1;
    return  (iWatch_Session_Status)ret;

}
int eneyes::iwatch::impl::set_iWatchSessionStatus(int64_t watch_id,int status)
{
    iwatch_ret_map[watch_id] = status;
    return 0;
}
