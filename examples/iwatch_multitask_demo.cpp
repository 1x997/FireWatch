#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>

#include <thread>

#include "iwatch/iwatch_engine.h"
#include "iwatch/iwatch_session.h"


using namespace cv;
using namespace std;

int test_complete_process(int index, const char *media_file_name);

int test_complete_process2();

void test_engine_level_multitask();

void test_engine_level_multitask2();

int main() {
    test_engine_level_multitask();

    return EXIT_SUCCESS;
}

void test_engine_level_multitask() {
    thread thread1(test_complete_process, 1, "medias/customer-falling.mp4");
    thread thread2(test_complete_process, 2, "medias/customer-standing-fighting.mp4");

    thread1.join();
    thread2.join();
}

void test_engine_level_multitask2() {
    thread thread1(test_complete_process2);
    thread thread2(test_complete_process2);

    thread1.join();
    thread2.join();
}

int test_complete_process(int index, const char *media_file_name) {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return EXIT_FAILURE;
    }

    VideoCapture capture;
    capture.open(media_file_name);

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
    }

    Mat frame;
    int frame_index = 0;

    char title[128] = {0};
    snprintf(title, 128, "%d Abnormalities: press esc to quit! - %s", index, media_file_name);

    while (true) {
        if (27 == waitKey(1)) {
            printf("The process is aborted by user!\n");
            break;
        }

        // 3 Read the frame and analyze it.
        capture >> frame;
        //imshow(title, frame);
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
            iWatch_Rect *region = &((*abnormality).region);
            printf("target_type: %d. region: (%d,%d,%d,%d). skeleton_count: %d. ",
                   (*abnormality).target_type, (*region).x, (*region).y, (*region).width, (*region).height,
                   (*abnormality).skeleton_count);

            if (iWatch_ATT_Customer == (*abnormality).target_type) {
                printf("Abnormality_ID(Customer): %d\n", (*abnormality).customer_abnormality.id);
            }
            if (iWatch_ATT_Staff == (*abnormality).target_type) {
                printf("Abnormality_ID(Staff): %d\n", (*abnormality).staff_abnormality.id);
            }


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

    return EXIT_SUCCESS;
}

int test_complete_process2() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, NULL)) {
        printf("The engine was initialized unsuccessfully!");
        return EXIT_FAILURE;
    }

    VideoCapture capture;
    capture.open("medias/customer-wandering.mp4");

    if (!capture.isOpened()) {
        printf("Failed to open the video!\n");
        iWatch_Engine_Uninit(engine_handle);
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
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
            iWatch_Rect *region = &((*abnormality).region);
            printf("target_type: %d. region: (%d,%d,%d,%d). skeleton_count: %d. ",
                   (*abnormality).target_type, (*region).x, (*region).y, (*region).width, (*region).height,
                   (*abnormality).skeleton_count);

            if (iWatch_ATT_Customer == (*abnormality).target_type) {
                printf("Abnormality_ID(Customer): %d\n", (*abnormality).customer_abnormality.id);
            }
            if (iWatch_ATT_Staff == (*abnormality).target_type) {
                printf("Abnormality_ID(Staff): %d\n", (*abnormality).staff_abnormality.id);
            }


            // Show the frame with abnormalities
            rectangle(frame, Rect((*region).x, (*region).y, (*region).width, (*region).height), Scalar(255, 255, 255));

            for (int skeleton_index = 0; skeleton_index < (*abnormality).skeleton_count; skeleton_index++) {
                iWatch_Skeleton *skeleton = &((*abnormality).skeletons[skeleton_index]);
                for (int key_point_index = 0; key_point_index < skeleton->key_point_count; key_point_index++) {
                    Point point(skeleton->key_points[key_point_index].x, skeleton->key_points[key_point_index].y);
                    circle(frame, point, 2, Scalar(255, 0, 0), 1);
                }
            }

            imshow("Abnormalities: press esc to quit!", frame);
        }


        iWatch_Release_Abnormalities(abnormalities, abnormality_count);
    }

    // 5 Uninitialize the session.
    iWatch_Session_Uninit(session_handle);
    capture.release();

    // 6 Uninitialize the engine.
    iWatch_Engine_Uninit(engine_handle);

    return EXIT_SUCCESS;
}
