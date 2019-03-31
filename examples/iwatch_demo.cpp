#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <time.h>

#include "iwatch/iwatch_engine.h"
#include "iwatch/iwatch_session.h"


using namespace cv;

void draw_polygon(Mat &frame, const iWatch_Contour *contour);

int main() {
    // 1 Initialize the engine.
    iWatch_Engine_Handle engine_handle;
    const char *model_root = NULL;
    //const char *model_root = "/home/lianchenyu/workspace/iwatch_JT/models";
    if (iWatch_ES_SUCCEEDED != iWatch_Engine_Init(&engine_handle, model_root)) {
        printf("The engine was initialized unsuccessfully!");
        return EXIT_FAILURE;
    }

    VideoCapture capture;

    //capture.open("medias/customer-falling.mp4");
    //capture.open("medias/customer-standing-fighting.mp4");
    //capture.open("medias/staff-smoking.mp4");
    capture.open("medias/customer-wandering.mp4");
    //capture.open(0);

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

        // 4 Detect abnormalities
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
            // 4.1 Print abnormality
            iWatch_Abnormality *abnormality = abnormalities[i];

            // abnormality info
            printf("target_type: %d. skeleton_count: %d. ",
                   (*abnormality).target_type,
                   (*abnormality).skeleton_count);
            if (iWatch_ATT_Customer == (*abnormality).target_type) {
                printf("Abnormality_ID(Customer): %d. ", (*abnormality).customer_abnormality.id);
            }
            if (iWatch_ATT_Staff == (*abnormality).target_type) {
                printf("Abnormality_ID(Staff): %d. ", (*abnormality).staff_abnormality.id);
            }

            // region1
            iWatch_Contour *region1 = &((*abnormality).region1.region);
            const int point_count = (*region1).contour_roi_point_count;
            printf("region1: point count: %d, points: [", point_count);
            for (int j = 0; j < point_count; ++j) {
                iWatch_Point *point = &((*region1).contour_roi_points[j]);
                if (point_count - 1 != j) {
                    printf("(%d, %d), ", (*point).x, (*point).y);
                } else {
                    printf("(%d, %d)", (*point).x, (*point).y);
                }
            }
            printf("]. ");

            // region(deprecated)
            iWatch_Rect *region = &((*abnormality).region);
            printf("region(deprecated): (%d, %d, %d, %d).\n",  (*region).x, (*region).y, (*region).width, (*region).height);


            // 4.2 Show the frame with the abnormality
            // Draw the region
            //rectangle(frame, Rect((*region).x, (*region).y, (*region).width, (*region).height), Scalar(255, 255, 255));
            draw_polygon(frame, region1);

            // Draw the skeletons
            for (int skeleton_index = 0; skeleton_index < (*abnormality).skeleton_count; skeleton_index++) {
                iWatch_Skeleton *skeleton = &((*abnormality).skeletons[skeleton_index]);
                for (int key_point_index = 0; key_point_index < skeleton->key_point_count; key_point_index++) {
                    Point point(skeleton->key_points[key_point_index].x, skeleton->key_points[key_point_index].y);
                    circle(frame, point, 2, Scalar(255, 0, 0), 1);
                }
            }

            // Show the frame.
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

// importing c++ vector standard library just for drawing polygon by opencv's api.
#include <vector>
using namespace std;

void draw_polygon(Mat &frame, const iWatch_Contour *contour) {
    if (NULL == contour || 0 >= (*contour).contour_roi_point_count) {
        return;
    }

    vector<Point> cv_contour;
    for (int i = 0; i < (*contour).contour_roi_point_count; ++i) {
        const iWatch_Point *point = &((*contour).contour_roi_points[i]);
        cv_contour.push_back(Point((*point).x, (*point).y));
    }

    polylines(frame, cv_contour, true, Scalar(255, 255, 255));
}
