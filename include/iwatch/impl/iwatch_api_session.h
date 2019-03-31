#ifndef ENEYES_IWATCH_API_SESSION_H
#define ENEYES_IWATCH_API_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "iwatch/iwatch_session.h"
#include "iwatch/entity/iwatch_condition.h"
#include "iwatch/iwatch_engine.h"
#include "iwatch/iwatch_macros.h"
/*******************struct to command***********/
typedef struct ST_Init
{
    iWatch_Sesssion_Handle *session_handle;
    const iWatch_Size *video_frame_size;
    iWatch_Engine_Handle engine_handle;
}ST_Init;
typedef struct ST_Analyze_Frame
{
    iWatch_Sesssion_Handle session_handle;
    const uint8_t **video_frame;
    int video_frame_count;
    const iWatch_Size *video_frame_size;
}ST_Analyze_Frame;
typedef struct ST_Detect_Abnormalities
{
    iWatch_Sesssion_Handle session_handle;
    iWatch_Abnormality **abnormalities;
    int *abnormality_count;
}ST_Detect_Abnormalities;
/*******************struct to command end***********/


/*******************command enum ******************/
typedef enum SessionCommand {
    SC_Init = 0,
    SC_Analyze_Frame,
    SC_Detect_Abnormalities,
} SessionCommand;
/*******************command enum end******************/


int doAll(int command,void* data);

iWatch_Session_Status iWatch_Session_API_Init(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        iWatch_Engine_Handle engine_handle);

iWatch_Session_Status iWatch_Session_API_Init_With_Thing_Abnormality_Cond(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Thing_Abnormality_Condition *condition,
        iWatch_Engine_Handle engine_handle);

iWatch_Session_Status iWatch_Session_API_Init_With_Thing_Abnormality_Cond_And_Targets(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Thing_Abnormality_Condition *condition,
        const iWatch_Target targets[IWATCH_MAX_TARGET_COUNT],
        int target_count,
        iWatch_Engine_Handle engine_handle);


iWatch_Session_Status iWatch_Session_API_Init_With_Staff_Abnormality_ID(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Staff_Abnormality_ID *abnormality_ids,
        int abnormality_id_count,
        iWatch_Engine_Handle engine_handle);

iWatch_Session_Status iWatch_Session_API_Init_With_Customer_Abnormality_ID(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Customer_Abnormality_ID *abnormality_ids,
        int abnormality_id_count,
        iWatch_Engine_Handle engine_handle);

void iWatch_Session_API_Uninit(iWatch_Sesssion_Handle session_handle);


iWatch_Session_Status iWatch_Session_API_Get_Param_I(
        iWatch_Sesssion_Handle session_handle, const char *key, int *value);

iWatch_Session_Status iWatch_Session_API_Set_Param_I(
        iWatch_Sesssion_Handle session_handle, const char *key, int value);

iWatch_Session_Status iWatch_Session_API_Get_Param_F(
        iWatch_Sesssion_Handle session_handle, const char *key, float *value);

iWatch_Session_Status iWatch_Session_API_Set_Param_F(
        iWatch_Sesssion_Handle session_handle, const char *key, float value);


/**
 *
 * @param session_handle
 * @param video_frame
 * @param video_frame_count only one frame is supported.
 * @param video_frame_size
 * @return
 */
iWatch_Session_Status iWatch_Session_API_Analyze_Frame(
        iWatch_Sesssion_Handle session_handle,
        const uint8_t *video_frame[IWATCH_MAX_FRAME_SIZE],
        int video_frame_count,
        const iWatch_Size *video_frame_size);

/**
 * Detect the abnormalities.
 *  Need release abnormalities mannually by iWatch_Release_Abnormalities function after calling this function successfully.
 * @param session_id
 * @param abnormalities
 * @param abnormality_count
 * @return calling status
 */
iWatch_Session_Status iWatch_Session_API_Detect_Abnormalities(
        iWatch_Sesssion_Handle session_handle,
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
        int *abnormality_count);

/**
 * Detect customer's abnormalities.
 *  Need release abnormalities mannually by iWatch_Release_Abnormalities function after calling this function successfully.
 * @param session_id
 * @param abnormalities
 * @param abnormality_count
 * @param condition
 * @return calling status
 */
iWatch_Session_Status iWatch_Session_API_Detect_Customer_Abnormalities(
        iWatch_Sesssion_Handle session_handle,
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
        int *abnormality_count,
        const iWatch_Customer_Abnormality_Condition *condition);

/**
 * Detect staff's abnormalities.
 *  Need release abnormalities mannually by iWatch_Release_Abnormalities function after calling this function successfully.
 * @param session_id
 * @param abnormalities
 * @param abnormality_count
 * @param condition
 * @return calling status
 */
iWatch_Session_Status iWatch_Session_API_Detect_Staff_Abnormalities(
        iWatch_Sesssion_Handle session_handle,
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
        int *abnormality_count,
        const iWatch_Staff_Abnormality_Condition *condition);

/**
 * Detect thing's abnormalities.
 *  Need release abnormalities mannually by iWatch_Release_Abnormalities function after calling this function successfully.
 * @param session_id
 * @param abnormalities
 * @param abnormality_count
 * @param condition
 * @return calling status
 */
iWatch_Session_Status iWatch_Session_API_Detect_Thing_Abnormalities(
        iWatch_Sesssion_Handle session_handle,
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
        int *abnormality_count,
        const iWatch_Thing_Abnormality_Condition *condition);

/**
 * Detect the abnormality, if *abnormality is null, it means that the abnormality is disappeared.
 * @param session_handle
 * @param abnormality_id
 * @param abnormality
 * @return
 */
iWatch_Session_Status iWatch_Session_API_Detect_Abnormality(
        iWatch_Sesssion_Handle session_handle,
        uint64_t abnormality_id,
        iWatch_Abnormality **abnormality);

/**
 * Get the skeletons.
 *  Need release memory mannually by iWatch_Release_Skeletons function after calling this function successfully.
 * @param session_id
 * @param skeletons
 * @param skeleton_count
 * @return call status
 */
iWatch_Session_Status iWatch_Session_API_Get_Skeletons(
        iWatch_Sesssion_Handle session_handle,
        iWatch_Skeleton *skeletons[IWATCH_MAX_SKELETON_COUNT],
        int *skeleton_count);


#endif //ENEYES_IWATCH_API_IWATCH_SESSION_H
