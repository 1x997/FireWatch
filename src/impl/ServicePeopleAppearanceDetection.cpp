#include "ServicePeopleAppearanceDetection.h"



ServicePeopleAppearanceDetection::ServicePeopleAppearanceDetection()
{
}
ServicePeopleAppearanceDetection::~ServicePeopleAppearanceDetection()
{
}
void ServicePeopleAppearanceDetection::ProcessWithSobel(cv::Mat &image) {
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y, dst;

	cv::Sobel(image, grad_x, CV_16S, 1, 0, 3, 1, 1, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(grad_x, abs_grad_x);
	cv::Sobel(image, grad_y, CV_16S, 0, 1, 3, 1, 1, cv::BORDER_DEFAULT);
	cv::convertScaleAbs(grad_y, abs_grad_y);
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
	image = dst;
}
bool ServicePeopleAppearanceDetection::detectWorkCard(cv::Mat image, bool vis) {
	std::vector<cv::Rect> work_card_location;
	cv::Mat result= image.clone();
	ProcessWithSobel(image);
	cv::Mat gray, dst;
	cv::cvtColor(image, gray, CV_BGR2GRAY);
	cv::threshold(gray, dst, 0, 255, cv::THRESH_OTSU);
	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));
	dilate(dst, dst, element);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat mask = cv::Mat::zeros(cv::Size(image.cols, image.rows), CV_8UC1);
	cv::findContours(dst, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::drawContours(mask, contours, -1, cv::Scalar(255, 255, 255), cv::FILLED);
	cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	for (int ct_idx = 0; ct_idx < contours.size(); ct_idx++) {
		cv::Rect rect = cv::boundingRect(contours.at(ct_idx));
		if (cv::contourArea(contours.at(ct_idx)) / (rect.height*rect.width) < 0.6) continue;
		float ratio = rect.width / float(rect.height);
		if (ratio > 1.5) {
			work_card_location.push_back(rect);
			if (vis) {
				cv::rectangle(result, rect, cv::Scalar(0, 255, 0), 3);
			}
		}
	}
	if (vis) {
		cv::imshow("work card",result);
	}
	if (work_card_location.size() == 1) return true;
	else return false;
}
bool ServicePeopleAppearanceDetection::detectHairColor(cv::Mat image, bool vis) {
	cv::Scalar rgb = cv::mean(image);
	float b, g, r;
	b = rgb.val[0];
	g = rgb.val[1];
	r = rgb.val[2];
	if (r < 100 && g < 100 && b < 100) {
		return true;
	}
	else {
		return false;
	}
}