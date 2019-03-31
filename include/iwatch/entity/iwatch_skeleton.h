//
// Created by Lian chenyu on 2018-12-04.
//

#ifndef ENEYES_IWATCH_API_IWATCH_SKELETON_H
#define ENEYES_IWATCH_API_IWATCH_SKELETON_H

#include "iwatch/entity/iwatch_common.h"
#include "iwatch/iwatch_macros.h"

#define IWATCH_MAX_SKELETON_KEY_POINT_COUNT 25    //temporary and undetermined

typedef struct iWatch_Skeleton {
    iWatch_Point key_points[IWATCH_MAX_SKELETON_KEY_POINT_COUNT];
    int key_point_count;
} iWatch_Skeleton;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Release skeleton's memories and its memembers' memories
 * @param skeleton
 */
IWATCH_API void iWatch_Release_Skeleton(iWatch_Skeleton *skeleton);

/**
 * Release skeletons' memories and its memembers' memories
 * @param skeletons
 * @param skeleton_count
 */
IWATCH_API void iWatch_Release_Skeletons(iWatch_Skeleton **skeletons, int skeleton_count);

#ifdef __cplusplus
}
#endif


#endif //ENEYES_IWATCH_API_IWATCH_SKELETON_H
