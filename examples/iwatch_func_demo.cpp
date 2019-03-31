#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>

#include "iwatch/iwatch_engine.h"
#include "iwatch/iwatch_session.h"


using namespace cv;


bool test_customer_abnormalities(const char *file_name, iWatch_Customer_Abnormality_ID customer_abnormality_id);
bool test_staff_abnormalities(const char *file_name, iWatch_Staff_Abnormality_ID staff_abnormality_id);

int main() {
    bool succeeded;

    //customer
    //succeeded = test_customer_abnormalities("medias/customer-falling.mp4", iWatch_CAI_A_Falling);
    //succeeded = test_customer_abnormalities("medias/customer-gathering.mp4", iWatch_CAI_A_Gathering);
    //succeeded = test_customer_abnormalities("medias/customer-running.mp4", iWatch_CAI_A_Running);
    //succeeded = test_customer_abnormalities("medias/customer-standing-fighting.mp4", iWatch_CAI_A_Fighting);
    //succeeded = test_customer_abnormalities("medias/customer-standing-fighting.mp4", iWatch_CAI_A_Standing);
    //succeeded = test_customer_abnormalities("medias/customer-wandering.mp4", iWatch_CAI_A_Wandering);

    //staff
    //succeeded = test_staff_abnormalities("medias/staff-answering-phone.mp4", iWatch_SAI_V_Answering_Phone);
    //succeeded = test_staff_abnormalities("medias/staff-eating.mp4", iWatch_SAI_V_Eating);
    //succeeded = test_staff_abnormalities("medias/staff-sleeping.mp4", iWatch_SAI_V_Sleeping);
    succeeded = test_staff_abnormalities("medias/staff-sleeping-02.mp4", iWatch_SAI_V_Sleeping);
    //succeeded = test_staff_abnormalities("medias/staff-smoking.mp4", iWatch_SAI_V_Smoking);
    //succeeded = test_staff_abnormalities("medias/staff-using-mobile.mp4", iWatch_SAI_V_Using_Mobile);

    return succeeded ? EXIT_SUCCESS : EXIT_FAILURE;
}


bool test_customer_abnormalities(const char *file_name, iWatch_Customer_Abnormality_ID customer_abnormality_id) {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return false;
    }

    VideoCapture capture;
    capture.open(file_name);

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return false;
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
        return false;
    }

    Mat frame;
    int frame_index = 0;
    const char *title = "Abnormalities: press esc to quit!";

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

        imshow(title, frame);

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

        for (int i = 0; i < abnormality_count; ++i) {
            // Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Customer_Abnormality_ID actual_abnormality_id = (*abnormality).customer_abnormality.id;

            if (iWatch_ATT_Customer != (*abnormality).target_type || customer_abnormality_id != actual_abnormality_id) {
                continue;
            }

            iWatch_Rect *region = &((*abnormality).region);
            printf("abnormality_id(customer): %d. region: (%d,%d,%d,%d).\n", actual_abnormality_id, (*region).x,
                    (*region).y, (*region).width, (*region).height);

            // Show the frame with abnormalities
            rectangle(frame, Rect((*region).x, (*region).y, (*region).width, (*region).height), Scalar(255, 255, 255));

            for (int skeleton_index = 0; skeleton_index < (*abnormality).skeleton_count; skeleton_index++) {
                iWatch_Skeleton *skeleton = &((*abnormality).skeletons[skeleton_index]);
                for (int key_point_index = 0; key_point_index < skeleton->key_point_count; key_point_index++) {
                    Point point(skeleton->key_points[key_point_index].x, skeleton->key_points[key_point_index].y);
                    circle(frame, point, 2, Scalar(255, 0, 0), 1);
                }
            }

            imshow(title, frame);
        }


        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);

    return true;
}

bool test_staff_abnormalities(const char *file_name, iWatch_Staff_Abnormality_ID staff_abnormality_id) {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return false;
    }

    VideoCapture capture;
    capture.open(file_name);

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return false;
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
        return false;
    }

    Mat frame;
    int frame_index = 0;
    const char *title = "Abnormalities: press esc to quit!";

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

        imshow(title, frame);

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

        for (int i = 0; i < abnormality_count; ++i) {
            iWatch_Abnormality *abnormality = abnormalities[i];
            iWatch_Staff_Abnormality_ID actual_abnormality_id = (*abnormality).staff_abnormality.id;
            if (iWatch_ATT_Staff != (*abnormality).target_type || staff_abnormality_id != actual_abnormality_id) {
                continue;
            }

            iWatch_Rect *region = &((*abnormality).region);
            printf("abnormality_id(staff): %d. region: (%d,%d,%d,%d).\n", actual_abnormality_id, (*region).x,
                   (*region).y, (*region).width, (*region).height);

            // Show the frame with abnormalities
            rectangle(frame, Rect((*region).x, (*region).y, (*region).width, (*region).height), Scalar(255, 255, 255));

            for (int skeleton_index = 0; skeleton_index < (*abnormality).skeleton_count; skeleton_index++) {
                iWatch_Skeleton *skeleton = &((*abnormality).skeletons[skeleton_index]);
                for (int key_point_index = 0; key_point_index < skeleton->key_point_count; key_point_index++) {
                    Point point(skeleton->key_points[key_point_index].x, skeleton->key_points[key_point_index].y);
                    circle(frame, point, 2, Scalar(255, 0, 0), 1);
                }
            }

            imshow(title, frame);
        }


        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);

    return true;
}
