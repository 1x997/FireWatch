#pragma once
#include <opencv2/opencv.hpp>
#include <numeric>
typedef struct DisappearedObject {
	int monitor_area_id;
	int fixed_oject_id;
	cv::Rect fixed_object_area;
}DisappearedObject;
typedef struct FixedObject {
	int id;
	cv::Rect area;
	cv::Mat image;
}FixedObject;
typedef struct FixedObjectParam {
	int timer=1;						//杂物检测事件阈值,单位 秒
	int foreground_frames = 10;
}FixedObjectParam;
typedef struct FixedObjectMonitorArea {
	int id;							//监控区域编号
	cv::Rect area;					//监控区域，相对于整体监控场景的位置
	std::vector<FixedObject> fixed_objects; //  Rect存在的位置，Mat物体图像
	std::vector<std::vector<float>> similarity; //连续时间内相似度
	std::vector<std::vector<float>> occlusion_rate; //遮挡比例
}FixedObjectMonitorArea;

class StationaryObjectDetection
{
private:
	FixedObjectParam fo_param;
	std::vector<FixedObjectMonitorArea> foma_list;		//监控区域
	int findMonitorArea(int monitor_area_id);	//查找监控区域的索引
	float calcOcclusion(cv::Mat background, cv::Mat current_frame);
	float compareHist(cv::Mat probe_image, cv::Mat gallery_image);
	float compareHOG(cv::Mat probe_image,cv::Mat gallery_image);
	float compare(cv::Mat probe_image,cv::Mat gallery_image);
public:
	StationaryObjectDetection();
	~StationaryObjectDetection();
	int InitMonitorArea(int monitor_id,cv::Rect monitor_area);
	int AddStationaryObject(cv::Mat cur_frame, int monitor_id,int object_id,cv::Rect area);
	int SetParam(int timer_threshold);
	int Update(cv::Mat frame);
	std::vector<DisappearedObject> GetResult();
	void Show(cv::Mat frame);
};
