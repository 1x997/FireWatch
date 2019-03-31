#pragma once
#include <numeric>
#include<opencv2/opencv.hpp>

//通过监控视频对固定位置的自助终端设备屏幕进行监测。能够识别屏幕终端是否黑屏、蓝屏。
typedef struct AST {
	int id;
	cv::Rect area;
	std::vector<unsigned int> states;	//连续时间内的状态：0异常，1正常。数组长度最大不超过sequence_length
}AST;

class AutonomousServiceTerminalAbnormalDetection
{
public:
	AutonomousServiceTerminalAbnormalDetection();
	~AutonomousServiceTerminalAbnormalDetection();
	
	int Init(cv::Mat frame, cv::Rect area, int id=-1);	//id可以自动分配，如果重复则提醒id重复
	int SetParam(int timer=10);
	int Update(cv::Mat frame);
	std::vector<std::pair<int,cv::Rect>> GetResult();


private:
	std::vector<AST> ast_manager;	//管理监控的各台设备
	int time_threshold = 10;		//单位：秒
	int sequence_length = 10;		//AST中states数组长度限制,默认FPS为10。
	int frame_count = 0;

	int findIndex(int id);			//查找监控区域的索引
	int analysis(cv::Mat image);	//分析图像的颜色分布

};

