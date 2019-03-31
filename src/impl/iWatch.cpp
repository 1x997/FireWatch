#include "iWatch.h"
iWatch::iWatch(){}
iWatch::~iWatch(){}

bool iWatch::iWatch_Engine_Init(cv::Mat frame, std::vector<std::string> models , std::vector<int> batch_size) {
	PE.Init("models/", "-1x" + std::to_string(RESOLUTION), "-1x-1");
	TS = * new TrackingSystem (frame);
	return false;
}
bool iWatch::iWatch_Engine_Init(cv::Mat frame, std::string model_dir) {
	if (model_dir.size() == 0) {
		std::cout << "model_dir is empty! \n";
		return false;
	}
	PE.Init(model_dir, "-1x"+std::to_string(RESOLUTION), "-1x-1");
	TS = * new TrackingSystem (frame);
	return false;
}
bool iWatch::iWatch_Engine_SenceAnalysis(cv::Mat frame) {
	cv::RNG rng(cv::getTickCount());
	cv::Mat pose_estimated_result;

    //cv::GaussianBlur(frame,frame,cv::Size(3,3),1.5,1.5);
	
	// 开始估计行人姿态
	pose_estimated_result = PE.Estimate(frame);
	std::vector<cv::Rect> bbox = PE.GetBBox();
	std::vector<P_Skeleton> skeletons = PE.GetSkeletons();

	std::vector<std::shared_ptr<SingleTracker>> old_tracker_vec = TS.getTrackerManager().getTrackerVec();

	//跟踪目标不包含于已检测目标，便删除该跟踪目标
	for (int t = 0; t < old_tracker_vec.size(); t++) {
		bool needDel = true;
		for (int r = 0; r < bbox.size(); r++) {
			float iou = calcIOU(bbox.at(r), old_tracker_vec.at(t)->getRect());
			//std::cout<<iou<<std::endl;
			if (iou > 0.6) {
				needDel = false;
				TS.updateTacker(old_tracker_vec.at(t)->getTargetID(), bbox.at(r), skeletons.at(r));
				bbox.erase(bbox.begin() + r);
				r--;
				break;
			}
		}
		if (needDel) {
		    //std::cout<<"delete "<<old_tracker_vec.at(t)->getTargetID()<<std::endl;
			TS.delTracker(old_tracker_vec.at(t)->getTargetID());
		}
	}
	//统计待跟踪目标
	std::vector<std::pair<cv::Rect, cv::Scalar>> new_target;
	for (int b_i = 0; b_i < bbox.size(); b_i++) {
		new_target.push_back(std::make_pair(bbox.at(b_i), cv::Scalar(255, 255, 255)));
	}
	//更新跟踪目标
	for (int t = 0; t < new_target.size(); t++) {
		//获取当前所有的跟踪目标
		std::vector<std::shared_ptr<SingleTracker>> old_tracker_vec = TS.getTrackerManager().getTrackerVec();
		cv::Rect rect = new_target.at(t).first;
		//寻找一个新的跟踪目标编号
		int id = 0;
		for (; id < TRACKER_MAX_NUM; id++) {
			bool exist = false;
			for (int reg_t = 0; reg_t < old_tracker_vec.size(); reg_t++) {
				if (old_tracker_vec.at(reg_t)->getTargetID() == id) { exist = true; }
			}
			if (!exist) { break; }
		}
		//添加跟踪目标
		TS.initTrackingSystem(id, rect, new_target.at(t).second);
	}
	// 开始跟踪
	TS.startTracking(frame);

    //绘制跟踪结果
	if (TS.getTrackerManager().getTrackerVec().size() != 0)
		TS.drawTrackingResult(pose_estimated_result);


	//cv::imshow("result",pose_estimated_result);

	//获取当前所有的跟踪目标，并进行事件识别
	std::vector<std::shared_ptr<SingleTracker>> tracker_vec = TS.getTrackerManager().getTrackerVec();
	evt_stay = Violation{};		evt_stay.task = "stay";
	evt_wander = Violation{};	evt_wander.task = "wander";
	evt_run = Violation{};		evt_run.task = "run";
	evt_fall = Violation{};		evt_fall.task = "fall";
	evt_fight = Violation{};	evt_fight.task = "fight";
	evt_gather = Violation{};	evt_gather.task = "gather";

	evt_smoke = Violation{};	evt_smoke.task = "smoke";
	evt_sleep = Violation{};	evt_sleep.task = "sleep";
	evt_eat = Violation{};		evt_eat.task = "eat";
	evt_call = Violation{};		evt_call.task = "call";

	std::vector<cv::Rect>	cur_bbox;
	std::vector<int>		cur_target_id;
	std::vector<P_Skeleton> cur_skeletons;
	//判断个体事件
	for (int i = 0; i < tracker_vec.size(); i++) {
		cur_target_id.push_back(tracker_vec.at(i)->getTargetID());
		cur_bbox.push_back(tracker_vec.at(i)->getRect());
		cur_skeletons.push_back(tracker_vec.at(i)->getSkeleton());
		
		JD.setSeqBBox(tracker_vec.at(i)->getSeqBBox());
		JD.setSeqSkeleton(tracker_vec.at(i)->getSeqSkeleton());
		JD.setCurFrame(frame);

		std::future<void> f_stay = async(std::launch::async, [&]() {
			if (JD.isStanding()) {
				evt_stay.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_stay.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_stay.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});
		std::future<void> f_wander = async(std::launch::async, [&]() {
			if (JD.isWandering()) {
				evt_wander.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_wander.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_wander.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});
		std::future<void> f_run = async(std::launch::async, [&]() {
			if (JD.isRunning()) {
				evt_run.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_run.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_run.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});
		std::future<void> f_fall = async(std::launch::async, [&]() {
			if (JD.isFalling()) {
				evt_fall.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_fall.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_fall.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});

		std::future<void> f_smoke = async(std::launch::async, [&]() {
			if (JD.isSmoking()) {
				evt_smoke.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_smoke.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_smoke.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});
		std::future<void> f_call = async(std::launch::async, [&]() {
			if (JD.isCalling()) {
				evt_call.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_call.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_call.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});
		std::future<void> f_eat = async(std::launch::async, [&]() {
			if (JD.isEating()) {
				evt_eat.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_eat.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_eat.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});
		std::future<void> f_sleep = async(std::launch::async, [&]() {
			if (JD.isSleeping()) {
				evt_sleep.target_id.push_back(tracker_vec.at(i)->getTargetID());
				evt_sleep.skeleton.push_back(tracker_vec.at(i)->getSkeleton());
				evt_sleep.bbox.push_back(tracker_vec.at(i)->getRect());
			}
		});

		f_stay.wait();
		f_wander.wait();
		f_run.wait();
		f_fall.wait();

		f_smoke.wait();
		f_call.wait();
		f_eat.wait();
		f_sleep.wait();

	}
	//判断群体事件
	JD.setCurFrame(frame);
	JD.setCurTargetID(cur_target_id);
	JD.setCurBBox(cur_bbox);
	JD.setCurSkeleton(cur_skeletons);
	std::future<void> f_fight = async(std::launch::async, [&]() {
		evt_fight = JD.isFighting();
	});
	std::future<void> f_gather = async(std::launch::async, [&]() {
		evt_gather = JD.isGathering();
	});
	f_fight.wait();
	f_gather.wait();
	
	

	processed_image = pose_estimated_result;
	return true;
}

Violation iWatch::iWatch_Session_GetState(int violation_type, std::vector<cv::Point> contour) {
	cv::Rect roi = CalcArea(contour);
	if (roi.width == 0 || roi.height == 0) {
		return Violation();
	}
	// customer FALL, FIGHT, GATHER, RUN, STAY, WANDER
	// staff CALL, EATING, SLEEPING, SMOKE
	switch (violation_type) {
		// staff
		case CALL:
			return SelectViolation(roi, evt_call);
		case EATING:
			return SelectViolation(roi, evt_eat);
		case SLEEPING:
			return SelectViolation(roi, evt_sleep);
		case SMOKE:
			return SelectViolation(roi, evt_smoke);


		// customer
		case FALL:
			return SelectViolation(roi, evt_fall);
		case FIGHT:
			return SelectViolation(roi, evt_fight);
		case GATHER:
			return SelectViolation(roi, evt_gather);
		case RUN:
			return SelectViolation(roi, evt_run);
		case STAY:
			return SelectViolation(roi, evt_stay);
		case WANDER:
			return SelectViolation(roi, evt_wander);
		default:
			break;
	}
}

cv::Rect iWatch::CalcArea(std::vector<cv::Point> contour) {
	cv::Rect roi(0,0,0,0);
	if (contour.size() > 0) {
		roi = cv::boundingRect(contour);
	}
	return roi;
}
Violation iWatch::SelectViolation(cv::Rect roi, Violation input_violation) {
	for (int i = 0; i < input_violation.bbox.size(); i++) {
		if (calcIOU(input_violation.bbox.at(i), roi)<0.5) {
			input_violation.target_id.erase(input_violation.target_id.begin() + i);
			input_violation.bbox.erase(input_violation.bbox.begin() + i);
			input_violation.skeleton.erase(input_violation.skeleton.begin() + i);
			i--;
		}
	}
	return input_violation;
}
cv::Mat iWatch::iWatch_Engine_GetResult() {
	return processed_image;
}
int iWatch::FindSession(int session_id) {
	for (int i = 0; i < session_list.size(); i++) {
		if (session_list.at(i).second == session_id) {
			return i;
		}
	}
	return -1;
}
void iWatch::iWatch_Session_Init(std::vector<cv::Point> roi, std::vector<std::string> tasks, int session_id) {

}
std::vector<Violation> iWatch::iWatch_Session_GetState(int session_id) {

	std::vector<Violation> violation;
	return violation;
}
std::vector<P_Skeleton> iWatch::iWatch_Session_GetSkeleton(int session_id) {
	std::vector<P_Skeleton> skeletons;
	return skeletons;
}
std::vector< std::string> iWatch::split(std::string str, std::string pattern)
{
	std::vector<std::string> ret;
	if (pattern.empty()) return ret;
	size_t start = 0, index = str.find_first_of(pattern, 0);
	while (index != str.npos)
	{
		if (start != index)
			ret.push_back(str.substr(start, index - start));
		start = index + 1;
		index = str.find_first_of(pattern, start);
	}
	if (!str.substr(start).empty())
		ret.push_back(str.substr(start));
	return ret;
}
int iWatch::Str2Int(std::string str) {
	std::stringstream ss;
	int num;
	ss << str;
	ss >> num;
	return num;
}
float iWatch::calcIOU(cv::Rect new_bbox, cv::Rect old_bbox)
{
	if (new_bbox.x < old_bbox.x) {
		if ((old_bbox.x - new_bbox.x) > new_bbox.width) {
			return -1;
		}
	}
	else {
		if ((new_bbox.x - old_bbox.x) > old_bbox.width) {
			return -1;
		}
	}
	if (new_bbox.y < old_bbox.y) {
		if ((old_bbox.y - new_bbox.y) > new_bbox.height) {
			return -1;
		}
	}
	else {
		if ((new_bbox.y - old_bbox.y) > old_bbox.height) {
			return -1;
		}
	}
	int x1 = std::max(new_bbox.x, old_bbox.x);
	int y1 = std::max(new_bbox.y, old_bbox.y);
	int x2 = std::min((new_bbox.x + new_bbox.width), (old_bbox.x + old_bbox.width));
	int y2 = std::min((new_bbox.y + new_bbox.height), (old_bbox.y + old_bbox.height));
	float over_area = (x2 - x1) * (y2 - y1);
	//float iou = over_area / (new_bbox.width * new_bbox.height + old_bbox.width * old_bbox.height - over_area);
	float iou = over_area / (new_bbox.width * new_bbox.height);
	return iou;
}
const cv::Size &iWatch::getVideo_frame_size() const {
    return video_frame_size;
}

void iWatch::setVideo_frame_size(const cv::Size &video_frame_size) {
    iWatch::video_frame_size = video_frame_size;
}

const cv::Size &iWatch::getOrginal_video_frame_size() const {
    return orginal_video_frame_size;
}

void iWatch::setOrginal_video_frame_size(const cv::Size &orginal_video_frame_size) {
    iWatch::orginal_video_frame_size = orginal_video_frame_size;
}
