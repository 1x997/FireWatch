#include <time.h>

#include "iWatch.h"

using namespace std;
using namespace cv;

int main() {
	VideoCapture cap;

	cap.open("medias/customer-wandering.mp4");
	cv::VideoWriter cap_write;
	if (!cap.isOpened()) {		cout << "Fail to open file!" << endl;	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the videos
	cout << "device size: " << dWidth << " x " << dHeight << endl;

    cv::Size video_size(RESOLUTION, int(RESOLUTION / dWidth*dHeight));
    cout << "video_size: " << video_size << endl;

	cap_write.open("/home/linux/Documents/iWatch2/result/result.avi",
		CV_FOURCC('X', 'V', 'I', 'D'), // MPEG-4 format
		29.0, video_size
	);

	iWatch iwatch;
    Mat frame(video_size, CV_8UC3);
	iwatch.iWatch_Engine_Init(frame);

    std::vector<cv::Point> contours;
    std::vector<std::string> tasks;
    std::vector<Violation> violations;
    std::vector<P_Skeleton> skeletons;

	/*
	contours = {cv::Point(20,20),cv::Point(20,100),cv::Point(100,20),cv::Point(100,100)};
	tasks = {"fight"};*/

	iwatch.iWatch_Session_Init(contours, tasks, 1);

	while (true) {
	    if (27 == waitKey(100)) {
            break;
	    }
		cap >> frame;

		resize(frame, frame, video_size);
		//blur(frame, frame, Size(3, 3));

        double start_ticks = getTickCount();
		iwatch.iWatch_Engine_SenceAnalysis(frame);
		
		//muguodong,20181217
		std::vector<cv::Point> contour = { cv::Point(frame.cols/2,frame.rows/2), cv::Point(frame.cols -1,frame.rows -1) };
		Violation violation = iwatch.iWatch_Session_GetState(STAY,contour);

        printf("fps: %f\n", getTickFrequency() / (getTickCount() - start_ticks));

		for (int v_idx = 0; v_idx < violation.bbox.size(); v_idx++) {
			std::cout << "ID:" << violation.target_id.at(v_idx) << std::endl;
			std::cout << "Rigion:" << violation.bbox.at(v_idx).x << "," << violation.bbox.at(v_idx).y << endl;
			std::cout << "Skeleton:" << violation.skeleton.at(v_idx).at(0).x << "," << violation.skeleton.at(v_idx).at(0).y << endl;
		}		
		cap_write.write(iwatch.iWatch_Engine_GetResult());
	}

	cap.release();
	cap_write.release();
}
