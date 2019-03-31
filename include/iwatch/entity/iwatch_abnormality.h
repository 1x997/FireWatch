//
// Created by Lian chenyu on 2018-12-04.
//

#ifndef ENEYES_IWATCH_API_IWATCH_ABNORMALITY_H
#define ENEYES_IWATCH_API_IWATCH_ABNORMALITY_H

#include <stdint.h>

#include "iwatch/iwatch_macros.h"
#include "iwatch/entity/iwatch_skeleton.h"
#include "iwatch/entity/iwatch_target.h"

#define IWATCH_SKELETON_COUNT 25   //temporary and undetermined

typedef enum iWatch_Abnormal_Target_Type {
    iWatch_ATT_Unknown = 0,
    iWatch_ATT_Staff,
    iWatch_ATT_Customer,
    iWatch_ATT_Thing
} iWatch_Abnormal_Target_Type;

typedef enum iWatch_Thing_Abnormality_ID {
    iWatch_TAI_Unknown = 0,
    iWatch_TAI_FTVP_DISAPPEARED,
    iWatch_TAI_UE_Sundries_Existing,
    iWatch_TAI_AVE_In_Misdirection,
    iWatch_TAI_AVE_Occluded,
    iWatch_TAI_AVE_Abnormal_Color,
    iWatch_TAI_ASSE_Screen_Abnormal     // only support black screen detection temporarily
} iWatch_Thing_Abnormality_ID;

typedef enum iWatch_Thing_Abnormablity_Type {
    iWatch_TAT_Unknown = 0,
    iWatch_TAT_Fixed_Thing_Violative_Placement,
    iWatch_TAT_Unqualified_Enviroment,
    iWatch_TAT_Abnormal_Video_Equipment,
    iWatch_TAT_Abnormal_Self_Service_Equipment
} iWatch_Thing_Abnormablity_Type;

typedef struct iWatch_Thing_Abnormality {
    iWatch_Thing_Abnormality_ID id;
    iWatch_Thing_Abnormablity_Type type;
    iWatch_Target targets[IWATCH_MAX_TARGET_COUNT];
    int target_count;
} iWatch_Thing_Abnormality;

typedef enum iWatch_Staff_Abnormality_ID {
    iWatch_SAI_Unknown = 0,
    iWatch_SAI_V_Using_Mobile,  //SAI_V means Staff_Abnormality_ID and Violation
    iWatch_SAI_V_Eating,
    iWatch_SAI_V_Smoking,
    iWatch_SAI_V_Sleeping,
    iWatch_SAI_V_Answering_Phone,
    iWatch_SAI_VG_Unwearing_Work_Card,
    iWatch_SAI_VG_Violative_Hair_Color,
    iWatch_SAI_VG_Unwearing_Work_Clothes,
} iWatch_Staff_Abnormality_ID;

typedef enum iWatch_Staff_Abnormablity_Type {
    iWatch_SAT_Unknown = 0,
    iWatch_SAT_Violation,
    iWatch_SAT_Violative_Grooming
} iWatch_Staff_Abnormablity_Type;

typedef struct iWatch_Staff_Abnormality {
    iWatch_Staff_Abnormality_ID id;
    iWatch_Staff_Abnormablity_Type type;

} iWatch_Staff_Abnormality;

typedef enum iWatch_Customer_Abnormality_ID {
    iWatch_CAI_Unknown = 0,
    iWatch_CAI_A_Standing,      // CAI_A means Customer_Abnormality_ID and Abnormablity
    iWatch_CAI_A_Wandering,
    iWatch_CAI_A_Gathering,
    iWatch_CAI_A_Running,
    iWatch_CAI_A_Fighting,
    iWatch_CAI_A_Falling
} iWatch_Customer_Abnormality_ID;

typedef enum iWatch_Customer_Abnormablity_Type {
    iWatch_CAT_Unknown = 0,
    iWatch_CAT_Abnormablity
} iWatch_Customer_Abnormablity_Type;

typedef struct iWatch_Customer_Abnormality {
    iWatch_Customer_Abnormality_ID id;
    iWatch_Customer_Abnormablity_Type type;
} iWatch_Customer_Abnormality;

typedef struct iWatch_Abnormality {
    uint64_t id;
    iWatch_Abnormal_Target_Type target_type;
    iWatch_Staff_Abnormality staff_abnormality;
    iWatch_Customer_Abnormality customer_abnormality;
    iWatch_Thing_Abnormality thing_abnormality;
    iWatch_Rect region;     //deprecated, use region1
    iWatch_Region region1;
    iWatch_Skeleton skeletons[IWATCH_SKELETON_COUNT];
    int skeleton_count;
} iWatch_Abnormality;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Release abnormality's memories and its memembers' memories
 * @param abnormality
 */
IWATCH_API void iWatch_Release_Abnormality(iWatch_Abnormality *abnormality);

/**
 * Release abnormalities' memories and its memembers' memories
 * @param abnormalities
 * @param abnormality_count
 */
IWATCH_API void iWatch_Release_Abnormalities(iWatch_Abnormality **abnormalities,  int abnormality_count);

#ifdef __cplusplus
}
#endif



#endif //ENEYES_IWATCH_API_IWATCH_ABNORMALITY_H
