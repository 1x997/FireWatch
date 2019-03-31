#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>

#include "iwatch/iwatch_engine.h"
#include "iwatch/iwatch_hyper_parameter.h"
#include "iwatch/iwatch_session.h"


using namespace cv;

int main() {

    return EXIT_SUCCESS;
}

void detect_fixed_thing_violative_placement() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return;
    }

    VideoCapture capture;
    capture.open("medias/fixed_thing_violative_placement.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    // 2 Initialize the session.
    iWatch_Sesssion_Handle session_handle;
    const iWatch_Size video_size{(int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT)};
    printf("video size: %d x %d\n", video_size.width, video_size.height);

    // Initialize targets
    iWatch_Target targets[IWATCH_MAX_TARGET_COUNT];
    // ...
    int target_count = 1;
    iWatch_Thing_Abnormality_Condition session_condition;
    // initialize session_condition.region
    session_condition.thing_abnormality_id_count = 1;
    session_condition.thing_abnormality_ids[0] = iWatch_TAI_FTVP_DISAPPEARED;

    iWatch_Session_Status status = iWatch_Session_Init_With_Thing_Abnormality_Cond_And_Targets(&session_handle,
                                                                                               &video_size,
                                                                                               &session_condition,
                                                                                               targets,
                                                                                               target_count,
                                                                                               engine_handle);
    if (iWatch_SS_Succeeded != status) {
        printf("The session was initialized unsuccessfully!");
        capture.release();
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    iWatch_Session_Set_Param_I(session_handle, IWATCH_TA_FTVP_MAX_DISAPPEARED_TIME, 2 * 60);

    Mat frame;
    int frame_index = 0;

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        printf("Frame index %d\n", frame_index++);
        if (frame.empty() || frame.cols <= 0 || frame.rows <= 0) {
            printf("The video has been read completely or the frame is broken!\n");
            break;
        }

        double start_ticks = getTickCount();
        const uint8_t *video_frame = frame.data;
        status = iWatch_Session_Analyze_Frame(session_handle, &video_frame, 1, &video_size);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        // 4 Get abnormalities
        // 4.1 Get abnormalities
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT];
        int abnormality_count = 0;
        status = iWatch_Session_Detect_Thing_Abnormalities(session_handle, abnormalities, &abnormality_count,
                                                           &session_condition);
        printf("fps: %f\n", getTickFrequency() / (getTickCount() - start_ticks));
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        if (1 == abnormality_count) {
            printf("Only one abnormality is detected:\n");
        } else {
            printf("%d abnormalities are detected:\n", abnormality_count);
        }

        for (int i = 0; i < abnormality_count; ++i) {
            // Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Region *region = &((*abnormality).region1);
            printf("region: id: %d; vertices' count: %d. ", (*region).id, (*region).region.contour_roi_point_count);

            if (iWatch_ATT_Thing == (*abnormality).target_type) {
                printf("Abnormality_ID(Thing): %d.\n", (*abnormality).thing_abnormality.id);
            }
        }

        iWatch_Release_Abnormalities(abnormalities, abnormality_count);

        // 4.2 Confirm wheter the abnormality is disappeared or not
        uint64_t abnormality_id = 0;    // Initialize it from other abnormality that have been detected.
        iWatch_Abnormality *abnormality = NULL;
        status = iWatch_Session_Detect_Abnormality(session_handle, abnormality_id, &abnormality);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        if (NULL == abnormality) {
            printf("The abnormality(id: %d) is disappeared!\n");
        } else {
            printf("The abnormality(id: %d) is still existing!\n");
        }
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);
}


void detect_unqualified_enviroment() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return;
    }

    VideoCapture capture;
    capture.open("medias/unqualified_enviroment.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    // 2 Initialize the session.
    iWatch_Sesssion_Handle session_handle;
    const iWatch_Size video_size{(int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT)};
    printf("video size: %d x %d\n", video_size.width, video_size.height);

    iWatch_Thing_Abnormality_Condition session_condition;
    // initialize session_condition.region
    session_condition.thing_abnormality_id_count = 1;
    session_condition.thing_abnormality_ids[0] = iWatch_TAI_UE_Sundries_Existing;

    iWatch_Session_Status status = iWatch_Session_Init_With_Thing_Abnormality_Cond(&session_handle, &video_size,
            &session_condition, engine_handle);
    if (iWatch_SS_Succeeded != status) {
        printf("The session was initialized unsuccessfully!");
        capture.release();
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    iWatch_Session_Set_Param_I(session_handle, IWATCH_TA_UE_MAX_SUNDRY_APPEARED_TIME, 2 * 60);

    Mat frame;
    int frame_index = 0;

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        printf("Frame index %d\n", frame_index++);
        if (frame.empty() || frame.cols <= 0 || frame.rows <= 0) {
            printf("The video has been read completely or the frame is broken!\n");
            break;
        }

        double start_ticks = getTickCount();
        const uint8_t *video_frame = frame.data;
        status = iWatch_Session_Analyze_Frame(session_handle, &video_frame, 1, &video_size);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        // 4 Get abnormalities
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT];
        int abnormality_count = 0;
        iWatch_Thing_Abnormality_Condition detecting_condition;
        // initialize detecting_condition.region
        detecting_condition.thing_abnormality_id_count = 1;
        detecting_condition.thing_abnormality_ids[0] = iWatch_TAI_UE_Sundries_Existing;

        status = iWatch_Session_Detect_Thing_Abnormalities(session_handle, abnormalities, &abnormality_count, &detecting_condition);
        printf("fps: %f\n", getTickFrequency() / (getTickCount() - start_ticks));
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        if (1 == abnormality_count) {
            printf("Only one abnormality is detected:\n");
        } else {
            printf("%d abnormalities are detected:\n", abnormality_count);
        }

        for (int i = 0; i < abnormality_count; ++i) {
            // Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Region *region = &((*abnormality).region1);
            printf("region: id: %d; vertices' count: %d. ", (*region).id, (*region).region.contour_roi_point_count);

            if (iWatch_ATT_Thing == (*abnormality).target_type) {
                printf("Abnormality_ID(Thing): %d.\n", (*abnormality).thing_abnormality.id);
            }
        }

        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);
}


void detect_violative_grooming() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return;
    }

    VideoCapture capture;
    capture.open("medias/violative_grooming.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    // 2 Initialize the session.
    iWatch_Sesssion_Handle session_handle;
    const iWatch_Size video_size{(int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT)};
    printf("video size: %d x %d\n", video_size.width, video_size.height);

    iWatch_Session_Status status = iWatch_Session_Init(&session_handle, &video_size, engine_handle);
    if (iWatch_SS_Succeeded != status) {
        printf("The session was initialized unsuccessfully!");
        capture.release();
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    Mat frame;
    int frame_index = 0;

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        printf("Frame index %d\n", frame_index++);
        if (frame.empty() || frame.cols <= 0 || frame.rows <= 0) {
            printf("The video has been read completely or the frame is broken!\n");
            break;
        }

        double start_ticks = getTickCount();
        const uint8_t *video_frame = frame.data;
        status = iWatch_Session_Analyze_Frame(session_handle, &video_frame, 1, &video_size);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        // 4 Get abnormalities
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT];
        int abnormality_count = 0;

        iWatch_Staff_Abnormality_Condition detecting_condition;
        // initialize detecting_condition.region
        detecting_condition.staff_abnormality_id_count = 2;
        detecting_condition.staff_abnormality_ids[0] = iWatch_SAI_VG_Unwearing_Work_Card;
        detecting_condition.staff_abnormality_ids[1] = iWatch_SAI_VG_Unwearing_Work_Clothes;

        status = iWatch_Session_Detect_Staff_Abnormalities(session_handle, abnormalities, &abnormality_count, &detecting_condition);
        printf("fps: %f\n", getTickFrequency() / (getTickCount() - start_ticks));
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        if (1 == abnormality_count) {
            printf("Only one abnormality is detected:\n");
        } else {
            printf("%d abnormalities are detected:\n", abnormality_count);
        }

        for (int i = 0; i < abnormality_count; ++i) {
            // Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Region *region = &((*abnormality).region1);
            printf("region: id: %d; vertices' count: %d. ", (*region).id, (*region).region.contour_roi_point_count);

            if (iWatch_ATT_Staff == (*abnormality).target_type) {
                printf("Abnormality_ID(Staff): %d.\n", (*abnormality).thing_abnormality.id);
            }
        }

        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);
}


void detect_abnormal_video_equipment() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return;
    }

    VideoCapture capture;
    capture.open("medias/abnormal_video_equipment.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    // 2 Initialize the session.
    iWatch_Sesssion_Handle session_handle;
    const iWatch_Size video_size{(int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT)};
    printf("video size: %d x %d\n", video_size.width, video_size.height);

    iWatch_Thing_Abnormality_Condition session_condition;
    // initialize session_condition.region
    session_condition.thing_abnormality_id_count = 2;
    session_condition.thing_abnormality_ids[0] = iWatch_TAI_AVE_In_Misdirection;
    session_condition.thing_abnormality_ids[1] = iWatch_TAI_AVE_Occluded;

    iWatch_Session_Status status = iWatch_Session_Init_With_Thing_Abnormality_Cond(&session_handle, &video_size,
                                                                                   &session_condition, engine_handle);
    if (iWatch_SS_Succeeded != status) {
        printf("The session was initialized unsuccessfully!");
        capture.release();
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    iWatch_Session_Set_Param_F(session_handle, IWATCH_TA_AVE_MAX_IMAGE_OFFSET_RATIO, 0.1f);

    Mat frame;
    int frame_index = 0;

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        printf("Frame index %d\n", frame_index++);
        if (frame.empty() || frame.cols <= 0 || frame.rows <= 0) {
            printf("The video has been read completely or the frame is broken!\n");
            break;
        }

        double start_ticks = getTickCount();
        const uint8_t *video_frame = frame.data;
        status = iWatch_Session_Analyze_Frame(session_handle, &video_frame, 1, &video_size);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        // 4 Get abnormalities
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT];
        int abnormality_count = 0;
        iWatch_Thing_Abnormality_Condition detecting_condition;
        // initialize detecting_condition.region
        detecting_condition.thing_abnormality_id_count = 2;
        detecting_condition.thing_abnormality_ids[0] = iWatch_TAI_AVE_In_Misdirection;
        detecting_condition.thing_abnormality_ids[1] = iWatch_TAI_AVE_Occluded;

        status = iWatch_Session_Detect_Thing_Abnormalities(session_handle, abnormalities, &abnormality_count, &detecting_condition);
        printf("fps: %f\n", getTickFrequency() / (getTickCount() - start_ticks));
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        if (1 == abnormality_count) {
            printf("Only one abnormality is detected:\n");
        } else {
            printf("%d abnormalities are detected:\n", abnormality_count);
        }

        for (int i = 0; i < abnormality_count; ++i) {
            // Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Region *region = &((*abnormality).region1);
            printf("region: id: %d; vertices' count: %d. ", (*region).id, (*region).region.contour_roi_point_count);

            if (iWatch_ATT_Thing == (*abnormality).target_type) {
                printf("Abnormality_ID(Thing): %d.\n", (*abnormality).thing_abnormality.id);
            }
        }

        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);
}

void detect_abnormal_self_service_equipment() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return;
    }

    VideoCapture capture;
    capture.open("medias/abnormal_self_service_equipment.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    // 2 Initialize the session.
    iWatch_Sesssion_Handle session_handle;
    const iWatch_Size video_size{(int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT)};
    printf("video size: %d x %d\n", video_size.width, video_size.height);

    iWatch_Thing_Abnormality_Condition session_condition;
    // initialize session_condition.region
    session_condition.thing_abnormality_id_count = 1;
    session_condition.thing_abnormality_ids[0] = iWatch_TAI_ASSE_Screen_Abnormal;

    iWatch_Session_Status status = iWatch_Session_Init_With_Thing_Abnormality_Cond(&session_handle, &video_size,
                                                                                   &session_condition, engine_handle);
    if (iWatch_SS_Succeeded != status) {
        printf("The session was initialized unsuccessfully!");
        capture.release();
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    Mat frame;
    int frame_index = 0;

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        printf("Frame index %d\n", frame_index++);
        if (frame.empty() || frame.cols <= 0 || frame.rows <= 0) {
            printf("The video has been read completely or the frame is broken!\n");
            break;
        }

        double start_ticks = getTickCount();
        const uint8_t *video_frame = frame.data;
        status = iWatch_Session_Analyze_Frame(session_handle, &video_frame, 1, &video_size);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        // 4 Get abnormalities
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT];
        int abnormality_count = 0;
        iWatch_Thing_Abnormality_Condition detecting_condition;
        // initialize detecting_condition.region
        detecting_condition.thing_abnormality_id_count = 1;
        detecting_condition.thing_abnormality_ids[0] = iWatch_TAI_ASSE_Screen_Abnormal;

        status = iWatch_Session_Detect_Thing_Abnormalities(session_handle, abnormalities, &abnormality_count, &detecting_condition);
        printf("fps: %f\n", getTickFrequency() / (getTickCount() - start_ticks));
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        if (1 == abnormality_count) {
            printf("Only one abnormality is detected:\n");
        } else {
            printf("%d abnormalities are detected:\n", abnormality_count);
        }

        for (int i = 0; i < abnormality_count; ++i) {
            // Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Region *region = &((*abnormality).region1);
            printf("region: id: %d; vertices' count: %d. ", (*region).id, (*region).region.contour_roi_point_count);

            if (iWatch_ATT_Thing == (*abnormality).target_type) {
                printf("Abnormality_ID(Thing): %d.\n", (*abnormality).thing_abnormality.id);
            }
        }

        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);
}

// It may have a low frame rate that detecting all abnormalities in the whole region.
void detect_all_abnormalities_except_for_violative_placement() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return;
    }

    VideoCapture capture;

    capture.open("medias/all_abnormalities_except_for_violative_placement.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    // 2 Initialize the session.
    iWatch_Sesssion_Handle session_handle;
    const iWatch_Size video_size{(int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT)};
    printf("video size: %d x %d\n", video_size.width, video_size.height);

    iWatch_Session_Status status = iWatch_Session_Init(&session_handle, &video_size, engine_handle);
    if (iWatch_SS_Succeeded != status) {
        printf("The session was initialized unsuccessfully!");
        capture.release();
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    Mat frame;
    int frame_index = 0;

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        printf("Frame index %d\n", frame_index++);
        if (frame.empty() || frame.cols <= 0 || frame.rows <= 0) {
            printf("The video has been read completely or the frame is broken!\n");
            break;
        }

        double start_ticks = getTickCount();
        const uint8_t *video_frame = frame.data;
        status = iWatch_Session_Analyze_Frame(session_handle, &video_frame, 1, &video_size);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        // 4 Get abnormalities
        iWatch_Abnormality *abnormalities[IWATCH_MAX_ABNORMALITIY_COUNT];
        int abnormality_count = 0;

        status = iWatch_Session_Detect_Abnormalities(session_handle, abnormalities, &abnormality_count);
        printf("fps: %f\n", getTickFrequency() / (getTickCount() - start_ticks));
        if (iWatch_SS_Succeeded !=  status) {
            continue;
        }

        if (1 == abnormality_count) {
            printf("Only one abnormality is detected:\n");
        } else {
            printf("%d abnormalities are detected:\n", abnormality_count);
        }

        for (int i = 0; i < abnormality_count; ++i) {
            // Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Rect *region = &((*abnormality).region);
            printf("target_type: %d. region: (%d,%d,%d,%d). skeleton_count: %d. ",
                   (*abnormality).target_type, (*region).x, (*region).y, (*region).width, (*region).height,
                   (*abnormality).skeleton_count);

            if (iWatch_ATT_Customer == (*abnormality).target_type) {
                printf("Abnormality_ID(Customer): %d\n", (*abnormality).customer_abnormality.id);
            } else if (iWatch_ATT_Staff == (*abnormality).target_type) {
                printf("Abnormality_ID(Staff): %d\n", (*abnormality).staff_abnormality.id);
            } else if (iWatch_ATT_Thing == (*abnormality).target_type) {
                printf("Abnormality_ID(Thing): %d\n", (*abnormality).thing_abnormality.id);
            }
        }

        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);
}

// It may have a low frame rate that detecting all abnormalities in the whole region.
void detect_skeletons() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return;
    }

    VideoCapture capture;

    capture.open("medias/skeletons.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    // 2 Initialize the session.
    iWatch_Sesssion_Handle session_handle;
    const iWatch_Size video_size{(int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                                 (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT)};
    printf("video size: %d x %d\n", video_size.width, video_size.height);

    iWatch_Session_Status status = iWatch_Session_Init(&session_handle, &video_size, engine_handle);
    if (iWatch_SS_Succeeded != status) {
        printf("The session was initialized unsuccessfully!");
        capture.release();
        iWatch_Engine_Uninit(engine_handle);
        return;
    }

    Mat frame;
    int frame_index = 0;

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        printf("Frame index %d\n", frame_index++);
        if (frame.empty() || frame.cols <= 0 || frame.rows <= 0) {
            printf("The video has been read completely or the frame is broken!\n");
            break;
        }

        double start_ticks = getTickCount();
        const uint8_t *video_frame = frame.data;
        status = iWatch_Session_Analyze_Frame(session_handle, &video_frame, 1, &video_size);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }

        // 4 Get skeletons
        iWatch_Skeleton *skeletons[IWATCH_MAX_SKELETON_COUNT];
        int skeleton_count = 0;
        status = iWatch_Session_Get_Skeletons(session_handle, skeletons, &skeleton_count);
        if (iWatch_SS_Succeeded != status) {
            continue;
        }
        printf("Got %d skeletons\n", skeleton_count);
        iWatch_Release_Skeletons(skeletons, skeleton_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);
}
