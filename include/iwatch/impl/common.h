#pragma once
#include <iostream>
#include <string>
#include <math.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <io.h>
#include <cstdlib>
#define PI 3.1415926
#define FAIL  -1
#define SUCCESS     1
#define FALSE  0
#define TRUE  1

#define OUT_OF_FRAME    2

#define ENTER  13
#define ESC   27
typedef struct Joint {
	int index;
	float x;
	float y;
	float p;
}Joint;
#define P_Skeleton std::vector<Joint>
//会话事件结果定义
// 任务名称，每人的骨架信息，包围矩形框信息，和事件的可能性。
typedef struct Violation {
	std::string task;
	std::vector<P_Skeleton> skeleton;
	std::vector<cv::Rect> bbox;
	std::vector<int> target_id;
};

enum {
    // customer
    FALL, FIGHT, GATHER, RUN, STAY, WANDER,
    // staff
    CALL, EATING, SLEEPING, SMOKE
};

#define TRACKER_MAX_NUM 65535
//the needed image resolution for openpose
//resize all of the frame to this size
#define RESOLUTION 400
/*
	30s 内的人体状态保留 FPS 8 
*/
#define SHORT_SEQ_NUM 240
#define SHORT_SEQ_STRIDE 1
//  90s
#define MEDIUM_SEQ_NUM 240
#define MEDIUM_SEQ_STRIDE 3
//  180s
#define LONG_SEQ_NUM 240
#define LONG_SEQ_STRIDE 6
