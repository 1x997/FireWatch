#pragma once
#include "common.h"
#include "Incident_Recognition.h"
#include <math.h>
#include <algorithm>
class Judgement
{
private:
	Incident_Recognition IR;
	cv::Mat cur_frame;
	std::vector<int> cur_target_id;
	std::vector<P_Skeleton> cur_skeleton;
	std::vector<cv::Rect> cur_bbox;

	std::vector<P_Skeleton> seq_skeleton;
	std::vector<cv::Rect> seq_bbox;
	float getAngelOfTwoVector(cv::Point2f pt1, cv:: Point2f pt2, cv::Point2f c);
	double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
	float  CalcAngle(float x1, float y1, float x2, float y2);
	float  CalcEuclidean(float x1, float y1, float x2, float y2);
	float CalcIOU(cv::Rect new_bbox, cv::Rect old_bbox);
	std::vector<std::pair<float, float>> CalcFiveKeyPoints(P_Skeleton skeleton);
	bool USE_CNN = false;
	cv::Mat GetImgMask(cv::Mat inputImage, cv::Size size);
	int RecIncident(cv::Mat image);

	cv::Rect GetHeadArea(P_Skeleton skeleton, std::vector<int> joints);
	//判断index中的骨骼点是否全部存在。
	bool exist(P_Skeleton skt,std::vector<int> index, float thr = 0.05);
	int buffer_count = 0;
	int smoke_detect_stride = 0;
	void LoadTemplate();
public:
	Judgement();
	~Judgement();
	int setCurFrame(cv::Mat frame);
	int setCurTargetID(std::vector<int> cur_target_id);
	int setCurSkeleton(std::vector<P_Skeleton> cur_skeleton);
	int setCurBBox(std::vector<cv::Rect> cur_bbox);
	int setSeqSkeleton(std::vector<P_Skeleton> seq_skeleton);
	int setSeqBBox(std::vector<cv::Rect> seq_bbox);
	//个体事件
	  //服务人员
	bool isCalling();
	bool isEating();
	bool isPlayingPhone();
	bool isSleeping();
	bool isSmoking();
	
	  //客户人员
	bool isFalling();
	bool isRunning();
	bool isStanding();
	bool isStopping();
	bool isWandering();
	//群体事件
	Violation isFighting();
	Violation isGathering();
	Violation isGathering_v2();

};


