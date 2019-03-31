//
// Created by Lian chenyu on 2018-12-04.
//

#include <iwatch/entity/iwatch_skeleton.h>

#include <malloc.h>

#include "iwatch/entity/iwatch_skeleton.h"

void iWatch_Release_Skeleton(iWatch_Skeleton *skeleton) {
    free(skeleton);
}

void iWatch_Release_Skeletons(iWatch_Skeleton **skeletons, int skeleton_count) {

}