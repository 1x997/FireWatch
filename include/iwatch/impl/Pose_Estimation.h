#pragma once
#include <openpose/core/headers.hpp>
#include <openpose/filestream/headers.hpp>
#include <openpose/gui/headers.hpp>
#include <openpose/pose/headers.hpp>
#include <openpose/utilities/headers.hpp>
#include "common.h"
class Pose_Estimation
{
public:
	Pose_Estimation();
	~Pose_Estimation();
	//  初始化。预加载caffe模型和各项参数。
	//	model_folder：指向openpose 存放模型参数的文件夹，默认 “. / models / ” 。如果模型文件路径发生变化，需修改此处的路径。
	//	net_resolution：网络要求的输入图像数据的尺寸，默认是"-1x368"。要求是16的倍数。如果增加，准确度可能会增加。 如果减少，速度会增加。 为了获得最大的速度 - 精度平衡，应该保持与要处理的图像或视频最接近的宽高比。默认的 - 1x368相当于16：9视频中的656x368。
	//	output_resolution：图像分辨率（显示和输出）大小，默认” - 1x - 1”, 表示强制程序使用输入图像分辨率。减小该数值，会略微减少延迟并增加FPS，但是显示图像的质量会下降。
	//	Return : 0 表示成功初始化模型，并正确加载。
	//	- 1 表示遇到一些意料之外的问题，加载失败。可能原因有网络路径不对，GPU设备出现故障（例如显存大小不够，无法加载完模型）。
	int Init(std::string model_folder, std::string net_resolution, std::string output_resolution);
	//  估计人体姿态，共25个关节点。关节点分布如图所示
	//	image : 输入彩色图像 ，3通道RGB格式。
	//	return： 描绘了人体姿态掩模的彩色图，3通道RGB格式。
	cv::Mat Estimate(cv::Mat image);
	std::vector<P_Skeleton> GetSkeletons();
	std::vector<cv::Rect> GetBBox();
private:
	double scale_gap = 0.3;
	int scale_number = 1;

	op::Point<int> outputSize;
	op::Point<int> netInputSize;
	op::PoseModel  poseModel;
	op::PoseExtractorCaffe* poseExtractorCaffe;
	op::PoseCpuRenderer* poseRenderer;
	op::CvMatToOpInput* cvMatToOpInput;
	op::CvMatToOpOutput cvMatToOpOutput;
	op::OpOutputToCvMat opOutputToCvMat;
	op::ScaleAndSizeExtractor* scaleAndSizeExtractor;
	std::vector<P_Skeleton> m_skeletons;
	std::vector<cv::Rect> person_bbox;
	//  判断当前人体姿态所呈现的状态。输入数据为每一个人的25个关节点的数据。
	//	skeleton ：包含25个Joint 数据的向量，对应一个人体所检测到的所有关节点的信息。
	//	return :  0 ，表示站立正常姿态；
	//	1 ，表示摔倒状态。
	float CalcAngle(float x1, float y1, float x2, float y2);
	std::string Transform_Double2String(double number);
};

