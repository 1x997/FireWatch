#pragma once
#include "Pose_Estimation.h"
#include "Tracker.h"
#include "Judgement.h"
#include "common.h"
#include<fstream>
#include <future>
#include <chrono>
typedef struct iWatch_Session {
	Pose_Estimation pe;
	TrackingSystem tracker;
	//cv::VideoCapture cap;
}iWatch_Session;

//会话事件定义
// 会话关注区域，会话关注任务类型，会话编号。
typedef struct Session {
	int id;
	cv::Rect roi;
	std::vector<std::string> task;
};



class iWatch
{
private:
	//
	Judgement JD;
	Pose_Estimation PE;
	TrackingSystem TS;
	//事件记录
	Violation evt_stay;
	Violation evt_wander;
	Violation evt_run;
	Violation evt_eat;
	Violation evt_sleep;
	Violation evt_smoke;
	Violation evt_call;
	Violation evt_fall;
	Violation evt_fight;
	Violation evt_gather;


	cv::Mat processed_image;
	std::vector<std::pair<iWatch_Session, int>> session_list;
	
	cv::Size video_frame_size;      //For integration temporarily
	cv::Size orginal_video_frame_size;      //For integration temporarily

	int FindSession(int session_id);
	float calcIOU(cv::Rect new_bbox, cv::Rect old_bbox);
	std::vector< std::string> split(std::string str, std::string pattern);
	int Str2Int(std::string str);
	cv::Rect CalcArea(std::vector<cv::Point> contour);
	Violation SelectViolation(cv::Rect roi,Violation input_violation);
public:
	iWatch();
	~iWatch();
	/*
	frame用以初始化跟踪系统，限制场景的长高。
	models 用以初始化加载模型的类型，按照任务类型给定<>
	*/
	bool iWatch_Engine_Init(cv::Mat frame, std::vector<std::string> models = { "pose","violation" }, std::vector<int> batch_size = {1,1});
	/*
	frame 用以初始化跟踪系统，限制场景的长高。
	model_dir 模型文件存放文件夹位置
	*/
	bool iWatch_Engine_Init(cv::Mat frame,std::string model_dir);
	/*
	roi 为会话工作的区域，由离散点组成的一个闭合包围区域
	taskes 为会话的执行的任务类型，例如判断"eating","smoking","running","fighting"等
	session_id 为会话的编号，对应于一个计算引擎而言是唯一的
	*/
	void iWatch_Session_Init(std::vector<cv::Point> roi, std::vector<std::string> tasks, int session_id);
	/*
	frame为要进行场景分析的监控摄像头全景图像
	*/
	bool iWatch_Engine_SenceAnalysis(cv::Mat frame);
	cv::Mat iWatch_Engine_GetResult();
	/*
	通过session_id获取会话中发生的事件
	*/
	std::vector<Violation> iWatch_Session_GetState(int session_id);
	std::vector<P_Skeleton> iWatch_Session_GetSkeleton(int session_id);

	bool iWatch_Engine_End();
	//获取某一个区域内的某个事件的发生情况
	Violation iWatch_Session_GetState(int violation_type,std::vector<cv::Point> contour);

	const cv::Size &getVideo_frame_size() const;
    void setVideo_frame_size(const cv::Size &video_frame_size);

    const cv::Size &getOrginal_video_frame_size() const;

    void setOrginal_video_frame_size(const cv::Size &orginal_video_frame_size);
};

