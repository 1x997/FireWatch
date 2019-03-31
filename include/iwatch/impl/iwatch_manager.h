//
// Created by Lian chenyu on 2018-12-18.
//

#ifndef ENEYES_IWATCH_IWATCH_MANAGER_H
#define ENEYES_IWATCH_IWATCH_MANAGER_H

#include "iWatch.h"
#include "iwatch/iwatch_session.h"
class EventLoop;
namespace eneyes {
    namespace iwatch {
        namespace impl {
            int allocate_engine(int64_t &engine_id,const char* model_root);
            void release_engine(int64_t engine_id);
            std::string get_engine_modelRoot_map(int64_t engine_id);
            iWatch *allocate_watch(int64_t &watch_id);
            void release_watch(int64_t watch_id);
            iWatch *get_watch(int64_t watch_id);
            EventLoop *get_eventLoop(int64_t watch_id);
            //EventLoop *get_engineSharedEventLoop();
            iWatch_Session_Status get_iWatchSessionStatus(int64_t watch_id);
            int set_iWatchSessionStatus(int64_t watch_id,int status);
            void clear_all_watch();

        }
    }
}


#endif //IWATCH_IWATCH_MANAGER_H
