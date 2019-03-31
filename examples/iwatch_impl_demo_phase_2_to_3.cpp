#include "GarbageDetection.h"
#include "StationaryObjectDetection.h"
#include "ServicePeopleAppearanceDetection.h"
#include "AutonomousServiceTerminalAbnormalDetection.h"
#include "VideoEquipmentAbnormalDetection.h"
void test_spad();
void test_sod();
void test_gd();
void test_astad();
void test_vead();

int main() {

	//test_astad();	//测试自助服务终端设备异常检测算法
	//test_spad();	//测试服务人员形象着装算法：工牌,发色
	//test_gd();	//测试杂物/垃圾物品检测算法
	//test_sod();	//测试定置物移动检测算法
	//test_vead();	//测试视频设备故障检测算法

	system("pause");
	return 0;
}

/*
void test_opticalflow() {
	// 初始化载入图片
	vector<Mat> imgs, grayImgs;

	Mat img = imread("11.jpg"); imgs.push_back(img);
	img = imread("22.jpg");     imgs.push_back(img);

	for (size_t i = 0; i<imgs.size(); i++)
	{
		Mat grayImg; grayImg.create(imgs[i].rows, imgs[i].cols, CV_8UC1);
		cvtColor(imgs[i], grayImg, CV_BGR2GRAY);
		grayImgs.push_back(grayImg);
	}

	// 角点检测
	vector<Point2f> vecCorners[2];
	double qualityLevel = 0.01;//or 0.01  
	double minDistance = 10;
	goodFeaturesToTrack(grayImgs[0], vecCorners[0], MAX_CORNERS, qualityLevel, minDistance);

	//// 角点显示测试
	//for(size_t i=0; i<vecCorners[0].size(); i++)
	//	circle(imgs[0], cvPoint((int)(vecCorners[0][i].x), (int)(vecCorners[0][i].y)), 3, cvScalar(255), 1, CV_AA, 0); 

	//imshow("corners",imgs[0]);
	//cvWaitKey(1);

	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);
	vector<uchar> status;
	vector<float> err;

	// 稀疏光流
	calcOpticalFlowPyrLK(imgs[0], imgs[1], vecCorners[0], vecCorners[1], status, err, cvSize(21, 21), 3, termcrit, 0, 0.001);
	for (size_t i = 0; i<vecCorners[0].size() && i<vecCorners[1].size(); i++)
	{
		Point p0 = Point(cvRound(vecCorners[0][i].x), cvRound(vecCorners[0][i].y));
		Point p1 = Point(cvRound(vecCorners[1][i].x), cvRound(vecCorners[1][i].y));
		line(imgs[1], p0, p1, CV_RGB(0, 50, 200), 1);
	}
	imshow("LK", imgs[1]);

	// 稠密光流
	Mat flow;
	calcOpticalFlowFarneback(grayImgs[0], grayImgs[1], flow, 0.5, 3, 15, 3, 5, 1.2, 0);
	for (int y = 0; y<imgs[0].rows; y += 10)
	{
		for (int x = 0; x<imgs[0].cols; x += 10)
		{
			Point2f fxy = flow.at<Point2f>(y, x);
			line(imgs[0], Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), CV_RGB(0, 255, 0));
			//circle(imgs[0], Point(x,y), 2, CV_RGB(255, 0, 0), -1);
		}
	}
	imshow("Farneback", imgs[0]);
}
*/
void test_vead() {
	cv::VideoCapture cap;
	cap.open(0);
	VideoEquipmentAbnormalDetection VEAD;
	cv::Mat background ;
	cap >> background;
	cv::Rect roi(100,100,500,500);
	if (VEAD.Init(background)) {
		std::cout << "sucessfully initialize VEAD !\n";
	}
	
	cv::Mat frame;
	while (1) {
		cap >> frame;
		if (frame.empty()) { break; }
		std::cout << VEAD.detectOcclusion(frame) << std::endl;
		//VEAD.detectRotation(frame(roi));
		cv::waitKey(33);
	}
}
void test_spad() {
	ServicePeopleAppearanceDetection SPAD;
	for (int i = 0; i < 25; i++) {
		cv::Mat frame = cv::imread("D:/Monitor-Alg/iWatch/medias/work_card/" + std::to_string(i) + ".jpg");
		std::cout << "work card : "<<SPAD.detectWorkCard(frame, false) << std::endl;
		std::cout << "hair color: "<<SPAD.detectHairColor(frame, false) << std::endl;
		cv::waitKey(33);
	}
}
void test_sod() {
	StationaryObjectDetection SOD;
	cv::VideoCapture cap;
	cap.open("2.mp4");
	cv::Mat frame;
	cap >> frame;
	SOD.SetParam(2);
	SOD.InitMonitorArea(0, cv::Rect(20, 20, frame.cols / 2, frame.rows / 2));
	SOD.AddStationaryObject(frame.clone(), 0, 0, cv::Rect(200, 200, 150, 150));
	while (1) {
		cap >> frame;
		if (frame.empty()) { break; }
		cv::Mat image = frame.clone();
		SOD.Update(frame);
		std::vector<DisappearedObject> result = SOD.GetResult();
		for (int r_idx = 0; r_idx < result.size(); r_idx++) {
			cv::putText(image, "disappear ", cv::Point(result.at(r_idx).fixed_object_area.x, result.at(r_idx).fixed_object_area.y), 1, 1, cv::Scalar(0, 0, 255));
			cv::rectangle(image, result.at(r_idx).fixed_object_area, cv::Scalar(0, 0, 255));
		}
		cv::imshow("image", image);
		cv::waitKey(33);
	}
}
void test_gd() {
	GarbageDetection GD;
	cv::VideoCapture cap;
	cap.open("2.mp4");
	cv::Mat frame;
	cap >> frame;
	GD.Init(frame, cv::Rect(50, 200, 150, 150), 0);
	GD.SetParam(5, cv::Size(20, 20));
	while (1) {
		cap >> frame;
		if (frame.empty()) { break; }
		cv::Mat image = frame.clone();
		GD.Update(frame.clone());

		std::vector<Garbage> garbages = GD.GetResult();
		cv::rectangle(image, cv::Rect(50, 200, 150, 150), cv::Scalar(0, 255, 0));
		for (int o = 0; o < garbages.size(); o++) {
			cv::rectangle(image, garbages.at(o).area, cv::Scalar(0, 255, 255));
		}
		cv::imshow("image", image);
		cv::waitKey(33);
	}
}
void test_astad() {
	AutonomousServiceTerminalAbnormalDetection ASTAD;
	cv::Mat image = cv::imread("D:/Monitor-Alg/iWatch/medias/image/black.jpg");
	ASTAD.Init(image, cv::Rect(0, 0, 10, 10), 0);
	ASTAD.Init(image, cv::Rect(1, 1, 10, 10), 1);

	ASTAD.SetParam(1);
	for (int i = 0; i < 100; i++) {
		ASTAD.Update(image);
		std::vector<std::pair<int, cv::Rect>> result = ASTAD.GetResult();
		for (int j = 0; j < result.size(); j++) {
			std::cout << i << " " << result.at(j).first << " " << result.at(j).second << std::endl;
		}
	}
}