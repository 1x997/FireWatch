#include "AutonomousServiceTerminalAbnormalDetection.h"



AutonomousServiceTerminalAbnormalDetection::AutonomousServiceTerminalAbnormalDetection()
{
}


AutonomousServiceTerminalAbnormalDetection::~AutonomousServiceTerminalAbnormalDetection()
{
}
int AutonomousServiceTerminalAbnormalDetection::Init(cv::Mat frame, cv::Rect area, int id) {
	if (findIndex(id) == -1) {
		AST ast;
		ast.id = id;
		ast.area = area;
		ast_manager.push_back(ast);
		return 1;
	}
	else {
		return -1;
	}
}

int AutonomousServiceTerminalAbnormalDetection::SetParam(int timer) {
	if (timer < 1) {
		return -1;
	}
	else {
		this->time_threshold = timer;
	}
}

int AutonomousServiceTerminalAbnormalDetection::Update(cv::Mat frame) {
	if (frame.empty()) return -1;
	//满足条件才更新：time_threshold时间内，只保留sequence_length帧图像，间隔一定时间保留。
	if (frame_count % (this->time_threshold * 10 / this->sequence_length) != 0) {
		this->frame_count++;
		return 0;
	}
	//更新每一个监控区域
	for (int i = 0; i < ast_manager.size(); i++) {
		AST ast = ast_manager.at(i);
		cv::Mat ast_image = frame(ast.area);
		int result = this->analysis(ast_image);
		if (ast.states.size() == sequence_length) {
			ast.states.erase(ast.states.begin());
		}
		ast.states.push_back(result);
		ast_manager.at(i) = ast;
	}
	this->frame_count++;
	//std::cout << this->frame_count << std::endl;
}

std::vector<std::pair<int, cv::Rect>> AutonomousServiceTerminalAbnormalDetection::GetResult() {
	std::vector<std::pair<int, cv::Rect>> abnormalities;
	for (int i = 0; i < ast_manager.size(); i++) {
		float mean = std::accumulate(std::begin(ast_manager.at(i).states), std::end(ast_manager.at(i).states), 0.0) / ast_manager.at(i).states.size();
		if (mean < 0.3 && ast_manager.at(i).states.size()>=this->sequence_length) {
			abnormalities.push_back(std::make_pair(ast_manager.at(i).id,ast_manager.at(i).area));
		}
	}
	return abnormalities;
}
int AutonomousServiceTerminalAbnormalDetection::findIndex(int id) {
	int index = -1;
	for (int i = 0; i < this->ast_manager.size(); i++) {
		if (this->ast_manager.at(i).id == id) {
			index = i;
			break;
		}
	}
	return index;
}
int AutonomousServiceTerminalAbnormalDetection::analysis(cv::Mat image) {
	cv::Scalar rgb  = cv::mean(image);
	float b, g, r;
	b = rgb.val[0];
	g = rgb.val[1];
	r = rgb.val[2];
	if (b>150 && b > (g + r)) {
		return 0;
	}
	if (r < 60 && g < 60 && b < 60) {
		return 0;
	}
	return 1;
}