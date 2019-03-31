#include "Incident_Recognition.h"



Incident_Recognition::Incident_Recognition()
{
}


Incident_Recognition::~Incident_Recognition()
{
}
void Incident_Recognition::Init(std::string net_proto, std::string net_weights, std::string label) {
	std::string modelTxt = net_proto;
	std::string modelBin = net_weights;
	try {
		net = readNetFromCaffe(modelTxt, modelBin);
	}
	catch (cv::Exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		if (net.empty())
		{
			std::cerr << "Can't load network by using the following files: " << std::endl;
			std::cerr << "prototxt:   " << modelTxt << std::endl;
			std::cerr << "caffemodel: " << modelBin << std::endl;
			exit(-1);
		}
	}
	labels = readClassNames(label.data());
}
std::vector<std::pair<std::string, float>> Incident_Recognition::Predict(cv::Mat image) {
	cv::Mat inputBlob = blobFromImage(image, 1.0f, cv::Size(128, 128),
		cv::Scalar(0, 0, 0), false);   //Convert Mat to batch of images
	net.setInput(inputBlob, "data");        //set the network input
	cv::Mat prob = net.forward("prob");         //compute output
											
	int classID;
	double classProb;
	getMaxClass(prob, &classID, &classProb);

	std::vector<float> v = convertMat2Vector<float>(prob);
	std::vector<std::pair<std::string, float>> rec_result;

	for (int i = 0; i < v.size(); i++) {
		rec_result.push_back(std::make_pair(labels.at(i), v.at(i)));
	}
	return rec_result;
}
void Incident_Recognition::getMaxClass(const cv::Mat &probBlob, int *classId, double *classProb) {
	cv::Mat probMat = probBlob.reshape(1, 1); //reshape the blob to  matrix
	cv::Point classNumber;
	minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
	//cout << *classProb << endl;
	//cout << classNumber.x << "\t" << classNumber.y << endl;
	*classId = classNumber.x;
}
std::vector<std::string> Incident_Recognition::readClassNames(const char *filename) {
	std::vector<std::string> classNames;

	std::ifstream fp(filename);
	if (!fp.is_open())
	{
		std::cerr << "File with classes labels not found: " << filename << std::endl;
		exit(-1);
	}

	std::string name;
	while (!fp.eof())
	{
		std::getline(fp, name);
		if (name.length())
			classNames.push_back(name.substr(name.find(' ') + 1));
	}

	fp.close();
	return classNames;
}