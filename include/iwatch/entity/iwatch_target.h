//
// Created by Lian chenyu on 2018-12-28.
//

#ifndef ENEYES_IWATCH_API_IWATCH_TARGET_H
#define ENEYES_IWATCH_API_IWATCH_TARGET_H

#include "iwatch/entity/iwatch_common.h"

#define IWATCH_MAX_TARGET_COUNT 10   //temporary and undetermined

typedef struct iWatch_Target {
    uint64_t target_id;
    iWatch_Point position;
    iWatch_Region region;
} iWatch_Target;


#endif //ENEYES_IWATCH_API_IWATCH_TARGET_H
