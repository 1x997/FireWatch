#pragma once
#include<opencv2/opencv.hpp>
#define M_PI 3.14159265358979323846

//三、服务人员仪容仪表异常
//（1）通过检测服务人员胸前佩戴物情况判定是否佩戴工牌，如检测到胸前无佩戴物，判定为未戴工牌；
//（2）检测服务人员发色大范围异常，如出现大面积非正常发色，则判定为疑似发色异常；
//（3）对位于柜台服务人员侧的服务人员仪容仪表进行检测，检测到服务人员未着工服，判定为异常，记录服务人员信息、时间信息等并抓拍；

class ServicePeopleAppearanceDetection
{
public:
	ServicePeopleAppearanceDetection();
	~ServicePeopleAppearanceDetection();
	bool detectWorkCard(cv::Mat image,bool vis=false);
	bool detectHairColor(cv::Mat image,bool vis=false);
private:
	void ProcessWithSobel(cv::Mat &image);
};

