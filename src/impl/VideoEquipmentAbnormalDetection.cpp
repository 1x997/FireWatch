#include "VideoEquipmentAbnormalDetection.h"



VideoEquipmentAbnormalDetection::VideoEquipmentAbnormalDetection()
{
}


VideoEquipmentAbnormalDetection::~VideoEquipmentAbnormalDetection()
{
}
bool VideoEquipmentAbnormalDetection::Init(cv::Mat image) {
	if (image.empty()) {
		return false;
	}
	else {
		this->background = image;
		return true;
	}
}
bool VideoEquipmentAbnormalDetection::Update(cv::Mat image) {
	if (image.empty()) return false;
	float occlusion_rate = this->detectOcclusion(image);
	float rotation_rate = this->detectRotation(image);
	if (this->occlusion_state.size() >= this->sequence_number) {
		this->occlusion_state.erase(this->occlusion_state.begin());
	}
	if (this->rotation_state.size() >= this->sequence_number) {
		this->occlusion_state.erase(this->occlusion_state.begin());
	}

	this->occlusion_state.push_back(occlusion_rate);
	this->rotation_state.push_back(rotation_rate);
	return true;
}
float VideoEquipmentAbnormalDetection::detectOcclusion(cv::Mat frame) {
	float occlusion_rate = 0;
	cv::Mat gray, binary_image;
	cv::cvtColor(frame,gray,CV_BGR2GRAY);
	gray = cv::abs(gray -255);
	cv::threshold(gray, binary_image, 250, 1, cv::THRESH_BINARY);
	occlusion_rate = float(cv::sum(binary_image)[0]) / (binary_image.rows*binary_image.cols);
	cv::imshow("gray",gray);
	cv::imshow("binary", binary_image * 255);
	//std::cout << occlusion_rate << std::endl;
	return occlusion_rate;
	/*
	cv::Mat diff_image, binary_image;
	cv::Mat gray_probe, gray_gallery;
	cvtColor(frame, gray_probe, cv::COLOR_BGR2GRAY);
	cvtColor(this->background, gray_gallery, cv::COLOR_BGR2GRAY);
	cv::medianBlur(gray_probe, gray_probe, 3);
	subtract(gray_probe, gray_gallery, diff_image, cv::Mat(), CV_16SC1);
	diff_image = cv::abs(diff_image);
	diff_image.convertTo(diff_image, CV_8UC1);
	cv::threshold(diff_image, binary_image, 50,1, cv::THRESH_BINARY);
	occlusion_rate = float(cv::sum(binary_image)[0]) / (binary_image.rows*binary_image.cols);
	*/
	//cv::imshow("binary", binary_image*255);
	//return occlusion_rate;
}
float VideoEquipmentAbnormalDetection::detectRotation(cv::Mat frame) {
	cv::Mat gray_object, gray_scene;
	if (frame.channels() == 3) { cv::cvtColor(frame, gray_object, CV_RGB2GRAY); }
	else if (frame.channels() == 1) { gray_object = frame; }
	if (background.channels() == 3) { cv::cvtColor(background, gray_scene, CV_RGB2GRAY); }

	//提取特征点    
	cv::Ptr<cv::ORB> OrbDetector = cv::ORB::create();
	std::vector<cv::KeyPoint> keyPoint_obj, keyPoint_scene;
	cv::Mat des_obj, des_scene;
	OrbDetector->detectAndCompute(gray_object, cv::Mat(), keyPoint_obj, des_obj);
	OrbDetector->detectAndCompute(gray_scene, cv::Mat(), keyPoint_scene, des_scene);
	des_obj.convertTo(des_obj, CV_32F);
	des_scene.convertTo(des_scene, CV_32F);

	cv::FlannBasedMatcher matcher;
	std::vector< cv::DMatch > matches;

	matcher.match(des_obj, des_scene, matches);

	//计算匹配点之间最大和最小距离
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < des_obj.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	std::vector< cv::DMatch > good_matches;
	for (int i = 0; i < des_obj.rows; i++)
	{
		if (matches[i].distance <= cv::max(2 * min_dist, 0.02))
		{
			good_matches.push_back(matches[i]);
		}
	}

	cv::Mat first_match;
	drawMatches(gray_object, keyPoint_obj, gray_scene, keyPoint_scene, good_matches, first_match);
	if (good_matches.size() <= 4) return false;
	//定位“好”的匹配点
	std::vector<cv::Point2f> obj;
	std::vector<cv::Point2f> scene;
	for (int i = 0; i < good_matches.size(); i++)
	{
		//DMathch类型中queryIdx是指match中第一个数组的索引,keyPoint类型中pt指的是当前点坐标
		obj.push_back(keyPoint_obj[good_matches[i].queryIdx].pt);
		scene.push_back(keyPoint_scene[good_matches[i].trainIdx].pt);
	}

	cv::Mat H = findHomography(obj, scene, CV_RANSAC);
	std::vector<cv::Point2f> obj_corners(4), scene_corners(4);
	obj_corners[0] = cv::Point2f(0, 0);
	obj_corners[1] = cv::Point2f(gray_object.cols, 0);
	obj_corners[2] = cv::Point2f(gray_object.cols, gray_object.rows);
	obj_corners[3] = cv::Point2f(0, gray_object.rows);

	perspectiveTransform(obj_corners, scene_corners, H);

	cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(scene_corners));
	std::cout << minRect.angle << std::endl;
	cv::Rect rect(0, 0, 0, 0);
	rect = cv::boundingRect(scene_corners);
	//绘制角点之间的直线
	//cv::line(basic_img, scene_corners[0] ,
	//	scene_corners[1] , cv::Scalar(0, 0, 255), 2);
	//cv::line(basic_img, scene_corners[1] ,
	//	scene_corners[2] , cv::Scalar(0, 0, 255), 2);
	//line(basic_img, scene_corners[2] ,
	//	scene_corners[3] , cv::Scalar(0, 0, 255), 2);
	//line(basic_img, scene_corners[3] ,
	//	scene_corners[0] , cv::Scalar(0, 0, 255), 2);
	cv::imshow("object ", first_match);
}
void VideoEquipmentAbnormalDetection::SetParam_Occlusion(float occlusion_rate ) {
	this->occlusion_threshold = occlusion_rate;
}
void VideoEquipmentAbnormalDetection::SetParam_Rotation(float rotation_rate) {
	this->rotation_threshold = rotation_rate;
}
int VideoEquipmentAbnormalDetection::GetResult(cv::Mat image) {
	unsigned int result = 0;

	int occlusion_count = 0;
	if (this->occlusion_state.size() >= this->sequence_number) {
		for (int i = 0; i < this->occlusion_state.size(); i++) {
			if (occlusion_state.at(i) > this->occlusion_threshold) {
				occlusion_count++;
			}
		}
	}


	int rotation_count = 0;
	if (this->rotation_state.size() >= this->sequence_number) {
		for (int i = 0; i < this->rotation_state.size(); i++) {
			if (rotation_state.at(i) > this->rotation_threshold) {
				rotation_count++;
			}
		}
	}

	int discoloration = 0;
    return 0;
}
