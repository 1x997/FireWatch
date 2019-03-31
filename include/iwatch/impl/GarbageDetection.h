#pragma once
#include<opencv2/opencv.hpp>

typedef struct Garbage {
	int id;
	cv::Rect area;
}Garbage;
typedef struct GarbageParam {
	int timer=1;						//杂物检测事件阈值,单位 秒
	int foreground_frames = 10;
	cv::Size min_size = cv::Size(10,10);				//最小尺寸
}SundryParam;
typedef struct MonitorArea {
	int id;							//监控区域编号
	cv::Rect area;					//监控区域，相对于整体监控场景的位置
	cv::Mat background;				//监控区域背景图像
	std::vector<cv::Mat> foregrounds;	//一段时间内的前景图像
	std::vector<cv::Rect> sundries;	//杂物存在的区域，相对于整体监控场景的位置
}MonitorArea;
class GarbageDetection
{
private:
	int frame_count = 0;
	GarbageParam garbage_param;
	std::vector<MonitorArea> monitor_areas;		//监控区域
	int findMonitorArea(int monitor_area_id);	//查找监控区域的索引
	cv::Mat extractForeground(cv::Mat background,cv::Mat current_frame);
	cv::Mat mergeSeqForegounds(std::vector<cv::Mat> foregrounds);
	std::vector<cv::Rect> detectObject(cv::Mat binary_image);
public:
	GarbageDetection();
	~GarbageDetection();
	//初始化监控区域
	int Init(cv::Mat frame,cv::Rect monitor_area,int monitor_area_id);
	int SetParam(int timer,cv::Size min_size);
	int Update(cv::Mat frame);
	std::vector<Garbage> GetResult();
};

