#include "GarbageDetection.h"
GarbageDetection::GarbageDetection()
{
}
GarbageDetection::~GarbageDetection()
{
}
int GarbageDetection::SetParam(int timer, cv::Size min_size) {
	if(timer>=1)	garbage_param.timer = timer;
	if(min_size.width>=0 && min_size.height>=0) garbage_param.min_size = min_size;
	garbage_param.foreground_frames = garbage_param.timer * 10;
	return 0;
}
int GarbageDetection::findMonitorArea(int monitor_area_id) {
	int index = -1;
	for (int i = 0; i < monitor_areas.size(); i++) {
		if (monitor_areas.at(i).id == monitor_area_id) {
			index = i;
			break;
		}
	}
	return index;
}
cv::Mat GarbageDetection::extractForeground(cv::Mat background, cv::Mat current_frame) {
	cv::Mat diff_image,binary_image;
	cv::Mat gray;
	cvtColor(current_frame, gray, cv::COLOR_BGR2GRAY);
	cv::medianBlur(gray, gray, 3);
	subtract(gray, background, diff_image, cv::Mat(), CV_16SC1);
	diff_image = cv::abs(diff_image);
	diff_image.convertTo(diff_image,CV_8UC1);
	cv::threshold(diff_image, binary_image, 50, 255, cv::THRESH_BINARY);
	return binary_image;
}
cv::Mat GarbageDetection::mergeSeqForegounds(std::vector<cv::Mat> foregrounds) {
	cv::Mat mask = cv::Mat::zeros(cv::Size(foregrounds.at(0).cols,foregrounds.at(0).rows),CV_8UC1);
	int width = foregrounds.at(0).cols;
	int height = foregrounds.at(0).rows;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			bool SAME_PIXEL = true;
			for (int f = 1; f < foregrounds.size(); f++) {
				if (foregrounds.at(f).ptr<uchar>(i, j)[0] != foregrounds.at(0).ptr<uchar>(i, j)[0]) {
					SAME_PIXEL = false;
					break;
				}
			}
			if (SAME_PIXEL) {
				mask.ptr<uchar>(i, j)[0] = foregrounds.at(0).ptr<uchar>(i, j)[0];
			}
		}
	}
	return mask;
}
std::vector<cv::Rect> GarbageDetection::detectObject(cv::Mat binary_image) {
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	findContours(binary_image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));//CV_RETR_TREE
	std::vector<cv::Rect> boundRect;
	for (int index = 0; index < contours.size(); index++)
	{
		cv::Rect rect = cv::boundingRect(cv::Mat(contours[index]));
		boundRect.push_back(rect);
	}
	return boundRect;
}
int GarbageDetection::Init(cv::Mat frame, cv::Rect monitor_area, int monitor_area_id) {
	if (findMonitorArea(monitor_area_id) == -1) {
		MonitorArea monitor;
		monitor.id = monitor_area_id;
		monitor.area = monitor_area;
		cv::Mat gray;
		cvtColor(frame(monitor_area), gray, cv::COLOR_BGR2GRAY);
		cv::medianBlur(gray,gray,3);
		monitor.background = gray;
		monitor_areas.push_back(monitor);
		return 1;
	}
	else {
		return -1;
	}
}
int GarbageDetection::Update(cv::Mat frame) {
	this->frame_count++;
	//if(this->frame_count%(this->garbage_param.timer*10/10)!=0){
	//	return 0;
	//}
	for (int m = 0; m < monitor_areas.size(); m++) {
		MonitorArea monitor = monitor_areas.at(m);
		cv::Mat cur_image = frame(monitor.area);
		cv::Mat foreground= extractForeground(monitor.background,cur_image);
		if (monitor.foregrounds.size() < garbage_param.foreground_frames) {
			monitor.foregrounds.push_back(foreground);
		}
		else {
			monitor.foregrounds.erase(monitor.foregrounds.begin());
			monitor.foregrounds.push_back(foreground);
		}
		monitor_areas.at(m) = monitor;
	}
	return 0;
}
std::vector<Garbage> GarbageDetection::GetResult() {
	std::vector<Garbage> garbages;
	for (int m = 0; m < monitor_areas.size(); m++) {
		MonitorArea monitor = monitor_areas.at(m);
		if (monitor.foregrounds.size()< garbage_param.foreground_frames) {
			continue;
		}
		cv::Mat static_foreground_object = mergeSeqForegounds(monitor.foregrounds);
		std::vector<cv::Rect> objects = detectObject(static_foreground_object);
		for (int r = 0; r < objects.size(); r++) {
			if (objects.at(r).width >= this->garbage_param.min_size.width && objects.at(r).height >= this->garbage_param.min_size.height) {
				garbages.push_back(Garbage{ monitor.id,cv::Rect(objects.at(r).x+ monitor.area.x,objects.at(r).y + monitor.area.y,objects.at(r).width,objects.at(r).height) });
			}
		}
	}
	return garbages;
}