//
// Created by Lian chenyu on 2018-12-04.
//

#include "iwatch/iwatch_session.h"
#include "iwatch/impl/iwatch_api_session.h"
#include "iwatch/impl/iwatch_manager.h"
#include "iwatch/impl/eventloop.h"
#include <iostream>
using namespace std;
iWatch_Session_Status iWatch_Session_Init(iWatch_Sesssion_Handle *session_handle,
                                          const iWatch_Size *video_frame_size,
                                          iWatch_Engine_Handle engine_handle)
{

	int64_t *watch_id = new int64_t;
	*session_handle = watch_id;
	eneyes::iwatch::impl::allocate_watch(*watch_id);
    EventLoop *eventloop = eneyes::iwatch::impl::get_eventLoop(*watch_id);
    eventloop->setFun(doAll);
    ST_Init tmp;
    tmp.engine_handle = engine_handle;
    tmp.session_handle = session_handle;
    tmp.video_frame_size = video_frame_size;
    eventloop->setMessage(SC_Init,&tmp);
    iWatch_Session_Status haha = eneyes::iwatch::impl::get_iWatchSessionStatus(*watch_id);
    return haha;
    //return iWatch_Session_API_Init(session_handle,video_frame_size,engine_handle);
}

iWatch_Session_Status iWatch_Session_Init_With_Thing_Abnormality_Cond(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Thing_Abnormality_Condition *condition,
        iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Failed;
}

iWatch_Session_Status iWatch_Session_Init_With_Thing_Abnormality_Cond_And_Targets(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Thing_Abnormality_Condition *condition,
        const iWatch_Target targets[IWATCH_MAX_TARGET_COUNT],
        int target_count,
        iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Failed;
}

void iWatch_Session_Uninit(iWatch_Sesssion_Handle session_handle) {}


iWatch_Session_Status iWatch_Session_Init_With_Staff_Abnormality_ID(iWatch_Sesssion_Handle *session_handle,
                                                                    const iWatch_Size *video_frame_size,
                                                                    const iWatch_Staff_Abnormality_ID *abnormality_ids,
                                                                    int abnormality_id_count,
                                                                    iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_Init_With_Customer_Abnormality_ID(iWatch_Sesssion_Handle *session_handle,
                                                                       const iWatch_Size *video_frame_size,
                                                                       const iWatch_Customer_Abnormality_ID *abnormality_ids,
                                                                       int abnormality_id_count,
                                                                       iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Invalid_Session_Handle;
}


iWatch_Session_Status iWatch_Session_Get_Param_I(iWatch_Sesssion_Handle session_handle, const char *key, int *value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_Set_Param_I(iWatch_Sesssion_Handle session_handle, const char *key, int value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_Get_Param_F(
        iWatch_Sesssion_Handle session_handle, const char *key, float *value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_Set_Param_F(
        iWatch_Sesssion_Handle session_handle, const char *key, float value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status
iWatch_Session_Analyze_Frame(iWatch_Sesssion_Handle session_handle,
                             const uint8_t **video_frame,
                             int video_frame_count,
                             const iWatch_Size *video_frame_size)
{
	if (session_handle == nullptr)
	{
		return iWatch_SS_Invalid_Session_Handle;
	}
    int64_t watch_id = (*(int64_t *) session_handle);//这里应该用session_handle
    EventLoop *eventloop = eneyes::iwatch::impl::get_eventLoop(watch_id);
    ST_Analyze_Frame tmp;
    tmp.session_handle = session_handle;
    tmp.video_frame = video_frame;
    tmp.video_frame_count = video_frame_count;
    tmp.video_frame_size = video_frame_size;
    eventloop->setMessage(SC_Analyze_Frame,&tmp);
	if(video_frame_count!=1)
	{
		std::cout << "thread-id:" << std::this_thread::get_id() << "--video_frame_count" << video_frame_count << endl;
	}
    iWatch_Session_Status haha = eneyes::iwatch::impl::get_iWatchSessionStatus(watch_id);
    return haha;
    //return iWatch_Session_API_Analyze_Frame(session_handle,video_frame,video_frame_count,video_frame_size);
}

iWatch_Session_Status
iWatch_Session_Detect_Abnormalities(iWatch_Sesssion_Handle session_handle,
                                    iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
                                    int *abnormality_count)
{
    /*********************注释内容为将iWatch_Session_Detect_Abnormalities放入EventLoopz中
    int64_t watch_id = (*(int64_t *) session_handle);//这里应该用session_handle
    EventLoop *eventloop = eneyes::iwatch::impl::get_eventLoop(watch_id);
    ST_Detect_Abnormalities tmp;
    tmp.session_handle = session_handle;
    tmp.abnormalities = abnormalities;
    tmp.abnormality_count = abnormality_count;
    eventloop->setMessage(SC_Detect_Abnormalities,&tmp);
    iWatch_Session_Status haha = eneyes::iwatch::impl::get_iWatchSessionStatus((int64_t)watch_id);
    return iWatch_SS_Succeeded;
    */
    return iWatch_Session_API_Detect_Abnormalities(session_handle,abnormalities,abnormality_count);
}


iWatch_Session_Status
iWatch_Session_Detect_Customer_Abnormalities(iWatch_Sesssion_Handle session_handle, iWatch_Abnormality **abnormalities,
                                             int *abnormality_count,
                                             const iWatch_Customer_Abnormality_Condition *condition) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status
iWatch_Session_Detect_Staff_Abnormalities(iWatch_Sesssion_Handle session_handle, iWatch_Abnormality **abnormalities,
                                          int *abnormality_count, const iWatch_Staff_Abnormality_Condition *condition) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_Detect_Thing_Abnormalities(
        iWatch_Sesssion_Handle session_handle,
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
        int *abnormality_count,
        const iWatch_Thing_Abnormality_Condition *condition) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_Detect_Abnormality(
        iWatch_Sesssion_Handle session_handle,
        uint64_t abnormality_id,
        iWatch_Abnormality **abnormality) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status
iWatch_Session_Get_Skeletons(iWatch_Sesssion_Handle session_handle, iWatch_Skeleton **skeletons, int *skeleton_count) {
    return iWatch_SS_Invalid_Session_Handle;
}
