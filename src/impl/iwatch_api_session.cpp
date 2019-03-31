//
// Created by Lian chenyu on 2018-12-04.
//

#include "iwatch/impl/iwatch_api_session.h"

#include <iostream>

#include "iwatch/impl/iwatch_manager.h"

using namespace cv;
using namespace std;

using namespace eneyes::iwatch::impl;
int doAll(int command,void* data)
{
    switch(command){
        case SC_Init:
        {
			ST_Init* st_init = (ST_Init*)data;
			int64_t *watch_id = (int64_t *)(*(st_init->session_handle));
            set_iWatchSessionStatus(*watch_id,
                                    iWatch_Session_API_Init(st_init->session_handle,st_init->video_frame_size,st_init->engine_handle));
            break;
        }
        case SC_Analyze_Frame:
        {
            ST_Analyze_Frame* st_analyze_frame = (ST_Analyze_Frame*)data;

            set_iWatchSessionStatus(*(int64_t *)st_analyze_frame->session_handle,
                                    iWatch_Session_API_Analyze_Frame(st_analyze_frame->session_handle,st_analyze_frame->video_frame,st_analyze_frame->video_frame_count,st_analyze_frame->video_frame_size));
            break;
        }
        case SC_Detect_Abnormalities:
        {
            ST_Detect_Abnormalities* st_detect_abnormalities = (ST_Detect_Abnormalities*)data;

            set_iWatchSessionStatus(*(int64_t *)st_detect_abnormalities->session_handle,
                                    iWatch_Session_API_Detect_Abnormalities(st_detect_abnormalities->session_handle,st_detect_abnormalities->abnormalities,st_detect_abnormalities->abnormality_count));
            break;
        }

    }
    return 0;
}
typedef struct iWatch_AbnormalityArray {
    iWatch_Abnormality *abnormalities = nullptr;
    int abnormality_count = 0;
    size_t abnormality_size = 0;
} iWatch_AbnormalityArray;

static bool detect_abnormalities(iWatch *watch, int violation_type, iWatch_Abnormality **abnormalities,
                                 int *abnormality_count);

static void convert_violation_type_to_abnormality(int violation_type, iWatch_Abnormality &abnormality);

static void convert_violation_to_abnormality(
        int violation_type,
        double scale,
        const cv::Rect &violation_bbx,
        const std::vector<P_Skeleton > &violation_skeletons,
        iWatch_Abnormality &abnormality);

iWatch_Session_Status iWatch_Session_API_Init(iWatch_Sesssion_Handle *session_handle,
                                          const iWatch_Size *video_frame_size,
                                          iWatch_Engine_Handle engine_handle) {

    if(video_frame_size->height == 0 || video_frame_size->width == 0)
    {
        return iWatch_SS_Failed;
    }
    int64_t *watch_id = (int64_t *)(*session_handle);
    iWatch *watch = get_watch(*watch_id);
    Size original_video_size(video_frame_size->width, video_frame_size->height);
    watch->setOrginal_video_frame_size(original_video_size);

    int height = (double) RESOLUTION / video_frame_size->width * video_frame_size->height;
    Size video_size(RESOLUTION, height);
    Mat frame(video_size, CV_8UC3);
    int64_t *engine_id = (int64_t*)engine_handle;
    string model_root = get_engine_modelRoot_map(*engine_id);
    if(model_root.empty())
    {
        watch->iWatch_Engine_Init(frame);
    }
    else
    {
        watch->iWatch_Engine_Init(frame,model_root);
    }
    watch->setVideo_frame_size(video_size);

    std::vector<cv::Point> contours;
    std::vector<std::string> tasks;
    watch->iWatch_Session_Init(contours, tasks, 1);

    return iWatch_SS_Succeeded;
}

iWatch_Session_Status iWatch_Session_API_Init_With_Thing_Abnormality_Cond(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Thing_Abnormality_Condition *condition,
        iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Failed;
}

iWatch_Session_Status iWatch_Session_API_Init_With_Thing_Abnormality_Cond_And_Targets(
        iWatch_Sesssion_Handle *session_handle,
        const iWatch_Size *video_frame_size,
        const iWatch_Thing_Abnormality_Condition *condition,
        const iWatch_Target targets[IWATCH_MAX_TARGET_COUNT],
        int target_count,
        iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Failed;
}

void iWatch_Session_API_Uninit(iWatch_Sesssion_Handle session_handle) {}


iWatch_Session_Status iWatch_Session_API_Init_With_Staff_Abnormality_ID(iWatch_Sesssion_Handle *session_handle,
                                                                    const iWatch_Size *video_frame_size,
                                                                    const iWatch_Staff_Abnormality_ID *abnormality_ids,
                                                                    int abnormality_id_count,
                                                                    iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_API_Init_With_Customer_Abnormality_ID(iWatch_Sesssion_Handle *session_handle,
                                                                       const iWatch_Size *video_frame_size,
                                                                       const iWatch_Customer_Abnormality_ID *abnormality_ids,
                                                                       int abnormality_id_count,
                                                                       iWatch_Engine_Handle engine_handle) {
    return iWatch_SS_Invalid_Session_Handle;
}


iWatch_Session_Status iWatch_Session_API_Get_Param_I(iWatch_Sesssion_Handle session_handle, const char *key, int *value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_API_Set_Param_I(iWatch_Sesssion_Handle session_handle, const char *key, int value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_API_Get_Param_F(
        iWatch_Sesssion_Handle session_handle, const char *key, float *value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_API_Set_Param_F(
        iWatch_Sesssion_Handle session_handle, const char *key, float value) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status
iWatch_Session_API_Analyze_Frame(iWatch_Sesssion_Handle session_handle,
                             const uint8_t **video_frame,
                             int video_frame_count,
                             const iWatch_Size *video_frame_size) {
    if (1 != video_frame_count) {
        printf("Unsupported frame count(must be 1)!\n");
        return iWatch_SS_Failed;
    }

    if (video_frame_size->height <= 0 || video_frame_size->width <= 0) {
        printf("video_frame's height and width must be greater than zero!\n");
        return iWatch_SS_Failed;
    }
    if(session_handle == nullptr)
    {
        return iWatch_SS_Invalid_Session_Handle;
    }
    int64_t watch_id = (*(int64_t *) session_handle);
    iWatch *watch = get_watch(watch_id);
    if(watch == nullptr)
    {
        return iWatch_SS_Invalid_Session_Handle;
    }
    Mat frame(video_frame_size->height, video_frame_size->width, CV_8UC3, (uint8_t *) *video_frame);
    resize(frame, frame, watch->getVideo_frame_size());

    watch->iWatch_Engine_SenceAnalysis(frame);

    return iWatch_SS_Succeeded;
}

iWatch_Session_Status
iWatch_Session_API_Detect_Abnormalities(iWatch_Sesssion_Handle session_handle,
                                    iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
                                    int *abnormality_count) {
    if(abnormalities == nullptr || abnormality_count == nullptr)
    {
        return iWatch_SS_Failed;
    }
    if(session_handle == nullptr)
    {
        return iWatch_SS_Failed;
    }
    int64_t watch_id = (*(int64_t *) session_handle);
    iWatch *watch = get_watch(watch_id);

    int abnormality_count_ret = 0;

    const int violation_type_count = 10;
    int violation_types[violation_type_count] = {
            // customer
            FALL, FIGHT, GATHER, RUN, STAY, WANDER,
            // staff
            CALL, EATING, SLEEPING, SMOKE
    };
    iWatch_AbnormalityArray abnormality_arrays[violation_type_count] = {};

    // detect abnormalities
    for (int i = 0; i < violation_type_count; ++i) {
        iWatch_Abnormality *new_abnormalities = nullptr;
        int new_abnormality_count = 0;
        iWatch_AbnormalityArray &abnormality_array = abnormality_arrays[i];
        bool succeeded = detect_abnormalities(watch, violation_types[i], &new_abnormalities, &new_abnormality_count);
        if (succeeded) {
            abnormality_array.abnormalities = new_abnormalities;
            abnormality_array.abnormality_count = new_abnormality_count;
            abnormality_array.abnormality_size = new_abnormality_count * sizeof(iWatch_Abnormality);
        } else {
            abnormality_array.abnormalities = nullptr;
            abnormality_array.abnormality_count = 0;
            abnormality_array.abnormality_size = 0;
        }
    }

    // copy abnormalities and release the detected.
    for (int i = 0; i < violation_type_count; ++i) {
        iWatch_AbnormalityArray &abnormality_array = abnormality_arrays[i];
        if (abnormality_array.abnormality_count <= 0) {
            continue;
        }

        int rest_abnormality_count = IWATCH_MAX_ABNORMALITIY_COUNT - abnormality_count_ret;
        rest_abnormality_count = (rest_abnormality_count <= abnormality_array.abnormality_count)
                                 ? rest_abnormality_count : abnormality_array.abnormality_count;
        for (int j = 0; j < rest_abnormality_count; ++j) {
            iWatch_Abnormality *abnormality = new iWatch_Abnormality();
            memcpy(abnormality, &(abnormality_array.abnormalities[j]), sizeof(iWatch_Abnormality));
            abnormalities[abnormality_count_ret++] = abnormality;
        }

        delete[](abnormality_array.abnormalities);
    }

    if (0 >= abnormality_count_ret) {
        //cout << "Warning: No abnormalities are detected!" << endl;
        return iWatch_SS_Failed;
    }

    *abnormality_count = abnormality_count_ret;

    return iWatch_SS_Succeeded;
}


iWatch_Session_Status
iWatch_Session_API_Detect_Customer_Abnormalities(iWatch_Sesssion_Handle session_handle, iWatch_Abnormality **abnormalities,
                                             int *abnormality_count,
                                             const iWatch_Customer_Abnormality_Condition *condition) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status
iWatch_Session_API_Detect_Staff_Abnormalities(iWatch_Sesssion_Handle session_handle, iWatch_Abnormality **abnormalities,
                                          int *abnormality_count, const iWatch_Staff_Abnormality_Condition *condition) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_API_Detect_Thing_Abnormalities(
        iWatch_Sesssion_Handle session_handle,
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT],
        int *abnormality_count,
        const iWatch_Thing_Abnormality_Condition *condition) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status iWatch_Session_API_Detect_Abnormality(
        iWatch_Sesssion_Handle session_handle,
        uint64_t abnormality_id,
        iWatch_Abnormality **abnormality) {
    return iWatch_SS_Invalid_Session_Handle;
}

iWatch_Session_Status
iWatch_Session_API_Get_Skeletons(iWatch_Sesssion_Handle session_handle, iWatch_Skeleton **skeletons, int *skeleton_count) {
    return iWatch_SS_Invalid_Session_Handle;
}

static bool detect_abnormalities(iWatch *watch, int violation_type, iWatch_Abnormality **abnormalities,
                                 int *abnormality_count) {
    // get violations
    *abnormality_count = 0;
    Size video_frame_size = watch->getVideo_frame_size();
    vector<Point> contour{Point(0, 0), Point(video_frame_size.width, video_frame_size.height)};
    Violation violation = watch->iWatch_Session_GetState(violation_type, contour);

    if (violation.bbox.size() < 1) {
        *abnormality_count = 0;
        return false;
    }

    // convert them to iWatch_Abnormality
    *abnormality_count = violation.bbox.size();
    *abnormalities = new iWatch_Abnormality[(*abnormality_count)];
    double scale = (double) watch->getVideo_frame_size().width / watch->getOrginal_video_frame_size().width;

    for (int i = 0; i < (*abnormality_count); ++i) {
        iWatch_Abnormality &abnormality = (*abnormalities)[i];
        if (FIGHT == violation_type || GATHER == violation_type) {
            convert_violation_to_abnormality(violation_type, scale, violation.bbox[i], violation.skeleton, abnormality);
        } else {
            vector<P_Skeleton > skeleton{violation.skeleton[i]};
            convert_violation_to_abnormality(violation_type, scale, violation.bbox[i], skeleton, abnormality);
        }
    }

    return true;
}

static void convert_violation_type_to_abnormality(int violation_type, iWatch_Abnormality &abnormality) {
    if (CALL == violation_type || EATING == violation_type
        || SLEEPING == violation_type || SMOKE == violation_type) {    //staff
        abnormality.target_type = iWatch_ATT_Staff;
        abnormality.staff_abnormality.type = iWatch_SAT_Violation;

        if (CALL == violation_type) {
            abnormality.staff_abnormality.id = iWatch_SAI_V_Answering_Phone;
        } else if (EATING == violation_type) {
            abnormality.staff_abnormality.id = iWatch_SAI_V_Eating;
        } else if (SLEEPING == violation_type) {
            abnormality.staff_abnormality.id = iWatch_SAI_V_Sleeping;
        } else if (SMOKE == violation_type) {
            abnormality.staff_abnormality.id = iWatch_SAI_V_Smoking;
        }
    } else if (STAY == violation_type || WANDER == violation_type || RUN == violation_type
               || FIGHT == violation_type || GATHER == violation_type || FALL == violation_type) {   //customer
        abnormality.target_type = iWatch_ATT_Customer;
        abnormality.customer_abnormality.type = iWatch_CAT_Abnormablity;

        if (STAY == violation_type) {
            abnormality.customer_abnormality.id = iWatch_CAI_A_Standing;
        } else if (WANDER == violation_type) {
            abnormality.customer_abnormality.id = iWatch_CAI_A_Wandering;
        } else if (RUN == violation_type) {
            abnormality.customer_abnormality.id = iWatch_CAI_A_Running;
        } else if (FIGHT == violation_type) {
            abnormality.customer_abnormality.id = iWatch_CAI_A_Fighting;
        } else if (GATHER == violation_type) {
            abnormality.customer_abnormality.id = iWatch_CAI_A_Gathering;
        } else if (FALL == violation_type) {
            abnormality.customer_abnormality.id = iWatch_CAI_A_Falling;
        }
    } else {
        abnormality.target_type = iWatch_ATT_Unknown;
    }
}

static void convert_violation_to_abnormality(
        int violation_type,
        double scale,
        const cv::Rect &violation_bbx,
        const std::vector<P_Skeleton > &violation_skeletons,
        iWatch_Abnormality &abnormality) {

    convert_violation_type_to_abnormality(violation_type, abnormality);

    int x = violation_bbx.x / scale;
    int y = violation_bbx.y / scale;
    int width = violation_bbx.width / scale;
    int height = violation_bbx.height / scale;

    abnormality.region.x = x;
    abnormality.region.y = y;
    abnormality.region.width = width;
    abnormality.region.height = height;

    iWatch_Contour &contour = abnormality.region1.region;

    contour.contour_roi_point_count = 4;
    contour.contour_roi_points[0] = {x, y};
    contour.contour_roi_points[1] = {x + width, y};
    contour.contour_roi_points[2] = {x + width, y + height};
    contour.contour_roi_points[3] = {x, y + height};


    abnormality.skeleton_count = (violation_skeletons.size() < IWATCH_SKELETON_COUNT) ? (int) violation_skeletons.size()
                                                                                      : IWATCH_SKELETON_COUNT;
	std::vector<cv::Point> temp_points;
    for (int i = 0; i < abnormality.skeleton_count; i++) {
        abnormality.skeletons[i].key_point_count = 25;
        for (int j = 0; j < violation_skeletons.at(i).size(); j++) {
            if (violation_skeletons.at(i).at(j).p > 0.05) {
                abnormality.skeletons[i].key_points[j].x = violation_skeletons.at(i).at(j).x / scale;
                abnormality.skeletons[i].key_points[j].y = violation_skeletons.at(i).at(j).y / scale;

				temp_points.push_back(cv::Point(abnormality.skeletons[i].key_points[j].x, abnormality.skeletons[i].key_points[j].y));
            } else {
                abnormality.skeletons[i].key_points[j].x = 0;
                abnormality.skeletons[i].key_points[j].y = 0;
            }
        }
    }
	if (abnormality.skeleton_count == 1) {
		cv::Rect area(0,0,0,0);
		area = cv::boundingRect(temp_points);
		abnormality.region.x = area.x ;
		abnormality.region.y = area.y ;
		abnormality.region.width = area.width ;
		abnormality.region.height = area.height;

        iWatch_Contour &contour = abnormality.region1.region;

        contour.contour_roi_point_count = 4;
        contour.contour_roi_points[0] = {area.x, area.y};
        contour.contour_roi_points[1] = {area.x + area.width, area.y};
        contour.contour_roi_points[2] = {area.x + area.width, area.y + area.height};
        contour.contour_roi_points[3] = {area.x, area.y + area.height};
	}
}
