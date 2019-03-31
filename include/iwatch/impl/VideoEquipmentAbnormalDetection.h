#pragma once
#include<opencv2/opencv.hpp>
//（1）视频设备异常情况分类：摄像头转向、摄像头被遮挡、图像变色。
//
//细化需求：
//a.针对摄像头转向判断，当视频图像偏移超过一定阈值（阈值可调），即反馈“摄像头转向告警”；
//b.针对摄像头被遮挡判断，当视频图像出现一定面积的遮挡（范围阈值可调），即反馈“摄像头被遮挡告警”；
//c.针对图像变色判断，当视频图像显示颜色与正常颜色有较大差异时，即反馈“图像变色告警”；

class VideoEquipmentAbnormalDetection
{
private:
	int sequence_number = 30;				//序列长度
	
	float occlusion_threshold = 1.0;
	float rotation_threshold = 0.0;
	float discoloration_threshold = 0.3;
	std::vector<float> occlusion_state;		//数组长度不超过sequence_number
	std::vector<float> rotation_state;		//数组长度不超过sequence_number
	cv::Mat background;
	cv::Mat previous_image;
	bool registerBackground(cv::Mat image);

public:
	VideoEquipmentAbnormalDetection();
	~VideoEquipmentAbnormalDetection();

	//初始化，注册背景图像
	bool Init(cv::Mat image);

	//更新图像，上传当前图像
	bool Update(cv::Mat image);

	//获取结果
	int GetResult(cv::Mat image);

	//设置允许遮挡范围阈值：default:1.0, (0,1]
	void SetParam_Occlusion(float occlusion_rate=1.0);

	//设置允许转向幅度阈值：default:0.0, [0,1]
	void SetParam_Rotation(float rotation_rate=0.0);

	//计算遮挡范围、转向比例
	float detectOcclusion(cv::Mat frame);
	float detectRotation(cv::Mat frame);

};

