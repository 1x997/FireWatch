#pragma once
#include <iostream>
#include <fstream>
#include<opencv2/dnn.hpp>
using namespace cv::dnn;
template<typename _Tp>
std::vector<_Tp> convertMat2Vector(const cv::Mat &mat)
{
	return (std::vector<_Tp>)(mat.reshape(1, 1));//通道数不变，按行转为一行
}
class Incident_Recognition
{
private:

	Net net;
	int class_id = 0;
	double class_prob = 0.0;
	std::vector<std::string> labels;
	std::vector<std::string> readClassNames(const char *filename);
	void getMaxClass(const cv::Mat &probBlob, int *classId, double *classProb);

public:
	Incident_Recognition();
	~Incident_Recognition();
	void Init(std::string net_proto, std::string net_weights, std::string label);
	std::vector<std::pair<std::string, float>> Predict(cv::Mat image);
};
//ref_label顺序
/*/
1 cellphone
2 eating
3 falling
4 fighting
5 gathering
6 vsleeping
7 smoking
8 snacks
*/