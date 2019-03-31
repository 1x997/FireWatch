//
// Created by Lian chenyu on 2018-12-04.
//

#ifndef ENEYES_IWATCH_API_IWATCH_CONDITION_H
#define ENEYES_IWATCH_API_IWATCH_CONDITION_H

#include "iwatch/entity/iwatch_abnormality.h"
#include "iwatch/iwatch_macros.h"


#define IWATCH_MAX_THING_ABNORMALITY_COUNT 100    //temporary and undetermined
#define IWATCH_MAX_STAFF_ABNORMALITY_COUNT 100    //temporary and undetermined
#define IWATCH_MAX_CUSTOMER_ABNORMALITY_COUNT 100    //temporary and undetermined

typedef struct iWatch_Thing_Abnormality_Condition {
    iWatch_Region region;
    iWatch_Thing_Abnormality_ID thing_abnormality_ids[IWATCH_MAX_THING_ABNORMALITY_COUNT];
    int thing_abnormality_id_count;
} iWatch_Thing_Abnormality_Condition;

typedef struct iWatch_Staff_Abnormality_Condition {
    iWatch_Region region;
    iWatch_Staff_Abnormality_ID staff_abnormality_ids[IWATCH_MAX_STAFF_ABNORMALITY_COUNT];
    int staff_abnormality_id_count;
} iWatch_Staff_Abnormality_Condition;

typedef struct iWatch_Customer_Abnormality_Condition {
    iWatch_Region region;
    iWatch_Customer_Abnormality_ID customer_abnormality_ids[IWATCH_MAX_CUSTOMER_ABNORMALITY_COUNT];
    int customer_abnormality_id_count;
} iWatch_Customer_Abnormality_Condition;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Release condition's memories and its memembers' memories
 * @param condition
 */
IWATCH_API void iWatch_Release_Customer_Abnormality_Condition(iWatch_Customer_Abnormality_Condition *condition);

/**
 * Release condition's memories and its memembers' memories
 * @param condition
 */
IWATCH_API void iWatch_Release_Staff_Abnormality_Condition(iWatch_Staff_Abnormality_Condition *condition);

#ifdef __cplusplus
}
#endif


#endif //ENEYES_IWATCH_API_IWATCH_CONDITION_H
