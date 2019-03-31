//
// Created by Lian chenyu on 2018-12-04.
//

#include "iwatch/iwatch_engine.h"

#include "iwatch/impl/iwatch_manager.h"
#include "iwatch/impl/iwatch_api_session.h"
#include "iwatch/impl/eventloop.h"

using namespace eneyes::iwatch::impl;

iWatch_Engine_Status iWatch_Engine_Init(iWatch_Engine_Handle *engine_handle, const char *model_root) {
    if(engine_handle == nullptr)
    {
        return iWatch_ES_FAILED;
    }
    int64_t *engine_id = new int64_t;
    *engine_handle = engine_id;
    allocate_engine(*engine_id,model_root);
    return iWatch_ES_SUCCEEDED;
}

void iWatch_Engine_Uninit(iWatch_Engine_Handle engine_handle) {
    if(engine_handle != nullptr)
    {
        int64_t *engine_id = (int64_t *)engine_handle;
        release_engine(*engine_id);
        delete engine_id;
    }
}
