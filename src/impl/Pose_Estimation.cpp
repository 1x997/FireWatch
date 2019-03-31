#include "Pose_Estimation.h"

Pose_Estimation::Pose_Estimation()
{
}
Pose_Estimation::~Pose_Estimation()
{
}
int Pose_Estimation::Init(std::string model_folder, std::string net_resolution, std::string output_resolution) {
	try
	{	
		std::string model_pose = "BODY_25";
		int num_gpu_start = 0;
		bool disable_blending = false;
		double render_threshold = 0.05;
		double alpha_pose = 0.6;
		double scale_gap = 0.3;
		int scale_number = 1;
		int logging_level =255;
		op::log("Starting...", op::Priority::High);
		netInputSize = op::flagsToPoint(net_resolution);
		outputSize = op::flagsToPoint(output_resolution);

		poseModel = op::flagsToPoseModel(model_pose);
		poseExtractorCaffe= new op::PoseExtractorCaffe{ poseModel, model_folder, num_gpu_start };
		poseRenderer = new op::PoseCpuRenderer { poseModel, (float)render_threshold, !disable_blending,(float)alpha_pose };
		cvMatToOpInput = new op::CvMatToOpInput{ poseModel };
		scaleAndSizeExtractor = new op::ScaleAndSizeExtractor(netInputSize, outputSize, scale_number, scale_gap);

		// Step 4 - Initialize resources on desired thread (in this case single thread, i.e. we init resources here)
		poseExtractorCaffe->initializationOnThread();
		poseRenderer->initializationOnThread();
		return 0;
	}
	catch (const std::exception& e)
	{
		op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
		return -1;
	}
}
cv::Mat Pose_Estimation::Estimate(cv::Mat inputImage) {
	cv::Mat outputImage = inputImage;
	m_skeletons.clear();
	person_bbox.clear();

	if (inputImage.empty())		op::error("Could not open or find the image ", __LINE__, __FUNCTION__, __FILE__);
	
	const op::Point<int> imageSize{ inputImage.cols, inputImage.rows };
	// Step 2 - Get desired scale sizes
	std::vector<double> scaleInputToNetInputs;
	std::vector<op::Point<int>> netInputSizes;
	double scaleInputToOutput;
	op::Point<int> outputResolution;
	std::tie(scaleInputToNetInputs, netInputSizes, scaleInputToOutput, outputResolution) = scaleAndSizeExtractor->extract(imageSize);
	// Step 3 - Format input image to OpenPose input and output formats
	std::vector<op::Array<float>>  netInputArray = cvMatToOpInput->createArray(inputImage, scaleInputToNetInputs, netInputSizes);
	op::Array<float> outputArray = cvMatToOpOutput.createArray(inputImage, scaleInputToOutput, outputResolution);
	// Step 4 - Estimate poseKeypoints
	poseExtractorCaffe->forwardPass(netInputArray, imageSize, scaleInputToNetInputs);
	op::Array<float> poseKeypoints = poseExtractorCaffe->getPoseKeypoints();
	// Common parameters needed
	int numberPeopleDetected = poseKeypoints.getSize(0);
	int numberBodyParts = poseKeypoints.getSize(1);

	// Step 5 - Render poseKeypoints
	poseRenderer->renderPose(outputArray, poseKeypoints, scaleInputToOutput);
	// Step 6 - OpenPose output format to cv::Mat
	outputImage = opOutputToCvMat.formatToCvMat(outputArray);

	for (int person = 0; person < numberPeopleDetected; person++) {
		std::vector<cv::Point> points;
		P_Skeleton skeleton;
		for (int part = 0; part < numberBodyParts; part++) {
			int baseIndex = poseKeypoints.getSize(2)*(person*numberBodyParts + part);
			float x = poseKeypoints[baseIndex];
			float y = poseKeypoints[baseIndex + 1];
			float score = poseKeypoints[baseIndex + 2];
			if (score > 0.1) {
				points.push_back(cv::Point(x,y));
			}
			skeleton.push_back(Joint{ baseIndex/3,x,y,score });
		}		
		cv::Point2f fourPoint2f[4];
		if (points.size() > 5) {
			cv::Rect rect=cv::boundingRect(points);
			person_bbox.push_back(rect);
			m_skeletons.push_back(skeleton);
		}
	}
	//cv::imshow("PE",outputImage);
	return outputImage;
}
std::vector<P_Skeleton> Pose_Estimation::GetSkeletons() {
	return m_skeletons;
}
std::vector<cv::Rect> Pose_Estimation::GetBBox() {
	return person_bbox;
}
float  Pose_Estimation::CalcAngle(float x1, float y1, float x2, float y2) {
	float angle_temp;
	float xx, yy;
	xx = x2 - x1;
	yy = y2 - y1;
	return abs(atan(yy / xx)) / PI * 180;
}
std::string Pose_Estimation::Transform_Double2String(double number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}
