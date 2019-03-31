#include "iWatch.h"
using namespace std;
using namespace cv;

#include "GarbageDetection.h"
#include "StationaryObjectDetection.h"
void showResult(cv::Mat& image, Violation& violation);
void showResult(cv::Mat source_frame, cv::Mat& image, Violation& violation);
void test_stage2() {
	GarbageDetection GD;
	StationaryObjectDetection SOD;

	cv::VideoCapture cap;
	cap.open("2.mp4");
	cv::Mat frame;
	cap >> frame;
	GD.SetParam(2, cv::Size(20, 20));											//设置参数： 检测时间阈值（单位：秒），最小垃圾尺寸
	GD.Init(frame, cv::Rect(50, 200, 150, 150), 0);								//初始化垃圾检测区域：当前图像，检测区域位置大小，检测区域编号

	SOD.SetParam(5);															//设置参数： 初始化定置物监测事件 时间阈值（单位：秒）
	SOD.InitMonitorArea(0, cv::Rect(20, 20, frame.cols / 2, frame.rows / 2));	//初始化监测区域：检测区域编号，检测区域位置大小
	SOD.AddStationaryObject(frame.clone(), 0, 0, cv::Rect(200, 200, 150, 150));	//添加监测目标：当前图像，监测区域编号，监测目标编号，监测目标位置大小
	while (1) {
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		cv::Mat image = frame.clone();
		GD.Update(frame.clone());												//更新数据
		SOD.Update(frame);														//更新数据

		std::vector<Garbage> garbages = GD.GetResult();							//获取结果
		std::vector<DisappearedObject> result = SOD.GetResult();				//获取结果
																				//绘制结果
		cv::rectangle(image, cv::Rect(50, 200, 150, 150), cv::Scalar(0, 255, 0));
		for (int o = 0; o < garbages.size(); o++) {
			cv::rectangle(image, garbages.at(o).area, cv::Scalar(0, 255, 255));
		}
		for (int r_idx = 0; r_idx < result.size(); r_idx++) {
			cv::putText(image, "disappear ", cv::Point(result.at(r_idx).fixed_object_area.x, result.at(r_idx).fixed_object_area.y), 1, 1, cv::Scalar(0, 0, 255));
			cv::rectangle(image, result.at(r_idx).fixed_object_area, cv::Scalar(0, 0, 255));
		}

		cv::imshow("image", image);
		cv::imshow("frame", frame);
		cv::waitKey(10);
	}
}
cv::Mat GetImgMask(cv::Mat inputImage, cv::Size size) {
	int s;
	if (inputImage.rows > inputImage.cols) {
		s = inputImage.rows;
	}
	else {
		s = inputImage.cols;
	}
	cv::Mat image_temp_ep(s, s, CV_8UC3, cv::Scalar(0, 0, 0));
	if (inputImage.channels() == 1) {
		cv::cvtColor(image_temp_ep, image_temp_ep, CV_BGR2GRAY);
	}
	cv::Mat image_temp_ep_roi = image_temp_ep(cv::Rect((s - inputImage.cols) / 2, (s - inputImage.rows) / 2, inputImage.cols, inputImage.rows));
	cv::Mat dstNormImg;
	addWeighted(image_temp_ep_roi, 0., inputImage, 1.0, 0., image_temp_ep_roi);
	resize(image_temp_ep, dstNormImg, size, 0, 0, 1);    //大小归一化
	return dstNormImg;
}
int main() {
	//test_stage2();
	//return 0;
	VideoCapture cap;
	string video_dir = "medias/";
	//cap.open("C:/workspace/jietong_video/gathering.mp4");
	//cap.open("C:/workspace/jietong_video/running_2.mp4");
	cap.open(video_dir + "smoking.mp4");
	//cap.open("C:/workspace/jietong_video/running.mp4");
	//cap.open("C:/workspace/jietong_video/smoke_4.mp4");
	//cap.open("C:/workspace/jietong_video/smoking.mp4");
	//cap.open("C:/workspace/jietong_video/falling.mp4");
	//cap.open("C:/workspace/jietong_video/1.avi");
	//cap.open("C:/workspace/jietong_video/calling.mp4");
	//cap.open("C:/workspace/jietong_video/call_4.mp4");
	//cap.open(0);
	//cap.open("C:/Users/Yifeng Huang/Pictures/Camera Roll/fight.mp4");

	cv::VideoWriter cap_write;
	if (!cap.isOpened()) { cout << "Fail to open file " << endl; }
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cap_write.open(
		"medias/result.avi",
		CV_FOURCC('X', 'V', 'I', 'D'),
		29.0,
		cv::Size(RESOLUTION, int(RESOLUTION / dWidth*dHeight)));
	char key = '-1';
	std::vector<cv::Point> contours;
	std::vector<std::string> tasks;
	std::vector<Violation> violations;
	std::vector<P_Skeleton> skeletons;

	iWatch iwatch;
	iwatch.iWatch_Engine_Init(cv::Mat::zeros(cv::Size(RESOLUTION, int(RESOLUTION / dWidth*dHeight)), CV_8UC3));
	int frame_count = 0;
	while (key != 's')
	{
		cv::Mat source_frame, frame;
		//std::cout << frame_count << std::endl;
		cap >> source_frame;
		frame_count++;
		if (frame_count % 1 != 0) {
			continue;
		}
		if (source_frame.empty()) { break; }
		if (source_frame.channels() != 3) {
			continue;
		}
		resize(source_frame, frame, cv::Size(RESOLUTION, int(RESOLUTION / dWidth*dHeight)));
		cv::GaussianBlur(frame, frame, cv::Size(3, 3), 0);
		double start = static_cast<double>(cv::getTickCount());
		iwatch.iWatch_Engine_SenceAnalysis(source_frame);
		std::vector<cv::Point> contour = { cv::Point(0,0), cv::Point(frame.cols - 1,frame.rows - 1) };
		std::vector<cv::Point> contour_appe = { cv::Point(frame.cols / 5,frame.rows / 3), cv::Point(2 * frame.cols / 3,frame.rows) };
		cv::Mat result = iwatch.iWatch_Engine_GetResult();
		Violation violation;
		violation = iwatch.iWatch_Session_GetState(RUN, contour);		showResult(frame, result, violation);
		violation = iwatch.iWatch_Session_GetState(STAY, contour_appe);	showResult(frame, result, violation);
		violation = iwatch.iWatch_Session_GetState(SMOKE, contour);		showResult(frame, result, violation);
		violation = iwatch.iWatch_Session_GetState(FIGHT, contour);		showResult(frame, result, violation);
		violation = iwatch.iWatch_Session_GetState(GATHER, contour);	showResult(frame, result, violation);
		violation = iwatch.iWatch_Session_GetState(FALL, contour);		showResult(frame, result, violation);

		std::cout << "FPS:         " << 1 / (((double)(cv::getTickCount()) - start) / (cv::getTickFrequency())) << std::endl;
		imshow("result", result);
		waitKey(1);
		cap_write.write(result);
	}
	cap.release();
	cap_write.release();
	system("pause");
}
void showResult(cv::Mat source_frame, cv::Mat& image, Violation& violation) {
	for (int v_idx = 0; v_idx < violation.bbox.size(); v_idx++) {
		std::cout << "ID: " << violation.target_id.at(v_idx) << " Type: " << violation.task << " Rigion: (" << int(violation.bbox.at(v_idx).x) << "," << int(violation.bbox.at(v_idx).y) << "," << int(violation.bbox.at(v_idx).width) << "," << int(violation.bbox.at(v_idx).height) << " )" << std::endl;
		cv::rectangle(image, cv::Rect(int(violation.bbox.at(v_idx).x), int(violation.bbox.at(v_idx).y), int(violation.bbox.at(v_idx).width), int(violation.bbox.at(v_idx).height)), cv::Scalar(255, 255, 255));
		cv::putText(image, violation.task + ":" + std::to_string(violation.target_id.at(v_idx)), cv::Point(int(violation.bbox.at(v_idx).x), int(violation.bbox.at(v_idx).y)), 1, 1, cv::Scalar(0, 0, 255));
		cv::imshow("Detected", GetImgMask(source_frame(cv::Rect(int(violation.bbox.at(v_idx).x), int(violation.bbox.at(v_idx).y), int(violation.bbox.at(v_idx).width), int(violation.bbox.at(v_idx).height))), cv::Size(256, 256)));
	}
}