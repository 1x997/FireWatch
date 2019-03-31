#include "StationaryObjectDetection.h"



StationaryObjectDetection::StationaryObjectDetection()
{
}


StationaryObjectDetection::~StationaryObjectDetection()
{
	fo_param.timer = 1;
	fo_param.foreground_frames = fo_param.timer * 5;

}

float StationaryObjectDetection::calcOcclusion(cv::Mat background, cv::Mat current_frame) {
	float occlusion_rate = 0;
	cv::Mat diff_image, binary_image;
	cv::Mat gray_probe,gray_gallery;
	cvtColor(current_frame, gray_probe, cv::COLOR_BGR2GRAY);
	cvtColor(background, gray_gallery, cv::COLOR_BGR2GRAY);
	cv::medianBlur(gray_probe, gray_probe, 3);
	subtract(gray_probe, gray_gallery, diff_image, cv::Mat(), CV_16SC1);
	diff_image = cv::abs(diff_image);
	diff_image.convertTo(diff_image, CV_8UC1);
	cv::threshold(diff_image, binary_image, 50, 1, cv::THRESH_BINARY);
	occlusion_rate = float(cv::sum(binary_image)[0]) / (binary_image.rows*binary_image.cols);
	return occlusion_rate;
}
int StationaryObjectDetection::findMonitorArea(int monitor_area_id) {
	int index = -1;
	for (int i = 0; i < foma_list.size(); i++) {
		if (foma_list.at(i).id == monitor_area_id) {
			index = i;
			break;
		}
	}
	return index;
}
int StationaryObjectDetection::InitMonitorArea(int monitor_id, cv::Rect monitor_area){
	if (findMonitorArea(monitor_id) == -1) {
		FixedObjectMonitorArea monitor;
		monitor.id = monitor_id;
		monitor.area = monitor_area;
		foma_list.push_back(monitor);
		return 1;
	}
	else {
		return -1;
	}
}
int StationaryObjectDetection::AddStationaryObject(cv::Mat cur_frame, int monitor_id, int object_id, cv::Rect area) {
	int index = findMonitorArea(monitor_id);
	if (index == -1) {
		return -1;
	}
	else {
		FixedObjectMonitorArea foma = foma_list.at(index);
		bool m_insert = true;
		for (int i = 0; i < foma.fixed_objects.size(); i++) {
			if (foma.fixed_objects.at(i).id == object_id) {
				m_insert = false;
				break;
			}
		}
		if (m_insert) {
			cv::Mat gallery_image = cur_frame(area);
			cv::Mat bil_image;
			cv::medianBlur(gallery_image, gallery_image, 3);
			foma.fixed_objects.push_back(FixedObject{object_id,area,gallery_image });
			foma.similarity.push_back({1.0});
			foma.occlusion_rate.push_back({0.0});
		}
		else {
			return -2;
		}
		foma_list.at(index) = foma;
	}
	return 0;
}
int StationaryObjectDetection::SetParam(int timer_threshold) {
	if(timer_threshold>=1) fo_param.timer = timer_threshold;
	fo_param.foreground_frames = fo_param.timer * 5;
	return 0;
}
int StationaryObjectDetection::Update(cv::Mat frame) {
	for (int m = 0; m < foma_list.size(); m++) {
		FixedObjectMonitorArea monitor = foma_list.at(m);
		cv::Mat monitor_area_image = frame(monitor.area);
		for (int n = 0; n < monitor.fixed_objects.size(); n++) {
			cv::Mat probe_image= frame(monitor.fixed_objects.at(n).area);
			cv::Mat gallery_image = monitor.fixed_objects.at(n).image;
			if (probe_image.size == gallery_image.size) {
				cv::medianBlur(probe_image, probe_image, 3);
				double sim = compare(gallery_image, probe_image);
				float occlusion_rate = calcOcclusion(gallery_image, probe_image);
				if (monitor.similarity.at(n).size() >= fo_param.foreground_frames) {
					monitor.similarity.at(n).erase(monitor.similarity.at(n).begin());
				}
				monitor.similarity.at(n).push_back(sim);
				if (monitor.occlusion_rate.at(n).size() >= fo_param.foreground_frames) {
					monitor.occlusion_rate.at(n).erase(monitor.occlusion_rate.at(n).begin());
				}
				monitor.occlusion_rate.at(n).push_back(occlusion_rate);
			}
			else {

			}
		}
		foma_list.at(m) = monitor;
	}
	return 0;
}
void StationaryObjectDetection::Show(cv::Mat frame) {
	for (int i = 0; i < foma_list.size(); i++) {
		FixedObjectMonitorArea foma = foma_list.at(i);
		cv::putText(frame,"foma_id = "+std::to_string(foma.id),cv::Point(foma.area.x,foma.area.y),1,1,cv::Scalar(0,255,0));
		cv::rectangle(frame,foma.area,cv::Scalar(0,255,0));
		for (int j = 0; j < foma.fixed_objects.size(); j++) {
			FixedObject fo = foma.fixed_objects.at(j);
			cv::putText(frame, "fo_id = " + std::to_string(fo.id)+", sm = "+std::to_string(foma.similarity.at(j).at(foma.similarity.at(j).size()-1))+",oc = "+ std::to_string(foma.occlusion_rate.at(j).at(foma.occlusion_rate.at(j).size() - 1)), cv::Point(fo.area.x, fo.area.y), 1, 1, cv::Scalar(0, 255, 255));
			cv::rectangle(frame, fo.area, cv::Scalar(0, 255, 255));
		}
	}
	cv::imshow("fixed monitor image",frame);
	cv::waitKey(33);
}
float StationaryObjectDetection::compare(cv::Mat probe_image, cv::Mat gallery_image) {
	return compareHist(probe_image,gallery_image);
}
float StationaryObjectDetection::compareHist(cv::Mat probe_image, cv::Mat gallery_image) {
	int Channels[] = { 0,1,2 };
	int nHistSize[] = { 256,256,256 };
	float range[] = { 0, 255 };
	const float* fHistRanges[] = { range,range,range };
	cv::MatND probe_hist, gallery_hist;
	cv::calcHist(&probe_image, 1, Channels, cv::Mat(), probe_hist, 1, nHistSize, fHistRanges, true, false);
	cv::normalize(probe_hist, probe_hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	cv::calcHist(&gallery_image, 1, Channels, cv::Mat(), gallery_hist, 1, nHistSize, fHistRanges, true, false);
	cv::normalize(gallery_hist, gallery_hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	float sim = cv::compareHist(gallery_hist, probe_hist, 0);
	return sim;
}
float StationaryObjectDetection::compareHOG(cv::Mat probe_image, cv::Mat gallery_image) {
	cv::cvtColor(probe_image,probe_image,CV_BGR2GRAY);
	cv::cvtColor(gallery_image, gallery_image, CV_BGR2GRAY);

	cv::HOGDescriptor *hog = new cv::HOGDescriptor(cv::Size(64,48),cv::Size(16,16),cv::Size(8,8),cv::Size(16,16),9);
	std::vector<float> des_probe, des_gallery;
	hog->compute(probe_image,des_probe,cv::Size(64,28),cv::Size(0,0));
	hog->compute(gallery_image, des_gallery, cv::Size(64, 28), cv::Size(0, 0));

	return 0;
}
std::vector<DisappearedObject> StationaryObjectDetection::GetResult() {
	std::vector<DisappearedObject> disappear_objects;
	for (int i = 0; i < foma_list.size(); i++) {
		FixedObjectMonitorArea foma = foma_list.at(i);
		for (int j = 0; j < foma.fixed_objects.size(); j++) {
			float mean_sim = std::accumulate(std::begin(foma.similarity.at(j)), std::end(foma.similarity.at(j)), 0.0)/ foma.similarity.at(j).size();
			float mean_oc = std::accumulate(std::begin(foma.occlusion_rate.at(j)),std::end(foma.occlusion_rate.at(j)),0.0)/ foma.occlusion_rate.at(j).size();
			if ((mean_sim<0.3 || mean_oc >0.3)) {
				disappear_objects.push_back(DisappearedObject{foma.id,foma.fixed_objects.at(j).id,foma.fixed_objects.at(j).area });
			}
		}
	}
	return disappear_objects;
}