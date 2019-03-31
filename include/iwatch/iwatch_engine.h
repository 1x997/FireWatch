//
// Created by Lian chenyu on 2018-12-04.
//

#ifndef ENEYES_IWATCH_API_IWATCH_ENGINE_H
#define ENEYES_IWATCH_API_IWATCH_ENGINE_H

#include "iwatch/iwatch_macros.h"

typedef void * iWatch_Engine_Handle;

typedef enum iWatch_Engine_Status {
    iWatch_ES_UNKNOWN = 0, iWatch_ES_SUCCEEDED, iWatch_ES_FAILED
} iWatch_Engine_Status;

#ifdef __cplusplus
extern "C" {
#endif

IWATCH_API iWatch_Engine_Status iWatch_Engine_Init(iWatch_Engine_Handle *engine_handle, const char *model_root);
IWATCH_API void iWatch_Engine_Uninit(iWatch_Engine_Handle engine_handle);

#ifdef __cplusplus
}
#endif

#endif //ENEYES_IWATCH_API_IWATCH_ENGINE_H
