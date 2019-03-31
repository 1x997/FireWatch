#include "Judgement.h"
Judgement::Judgement()
{
//	IR.Init("models/incident/resnet.prototxt","models/incident/resnet.caffemodel","models/incident/ref_label.txt");
}
Judgement::~Judgement()
{}
int Judgement::setSeqSkeleton(std::vector<P_Skeleton> seq_skeleton) {
	if (seq_skeleton.size() > 0) {
		this->seq_skeleton = seq_skeleton;
		return 1;
	}
	else {
		return 0;
	}
}
int Judgement::setSeqBBox(std::vector<cv::Rect> seq_bbox) {
	if (seq_bbox.size() > 0) {
		this->seq_bbox = seq_bbox;
		return 1;
	}
	else {
		return 0;
	}
}
int Judgement::setCurFrame(cv::Mat frame) {
	if (!frame.empty()) {
		this->cur_frame = frame;
		return 1;
	}
	else {
		return 0;
	}
}
int Judgement::setCurTargetID(std::vector<int> cur_target_id) {
	this->cur_target_id = cur_target_id;
	return 0;
}
int Judgement::setCurSkeleton(std::vector<P_Skeleton> cur_skeleton) {
	if (cur_skeleton.size() > 0) {
		this->cur_skeleton = cur_skeleton;
		return 1;
	}
	else {
		return 0;
	}	
}
int Judgement::setCurBBox(std::vector<cv::Rect> cur_bbox) {
	if (cur_bbox.size() > 0) {
		this->cur_bbox = cur_bbox;
		return 1;
	}
	else {
		return 0;
	}
}

bool Judgement::isCalling() {
	int calling_pose_frame_count_threshold = 5 * 25;		//frame rate: 25fps
	calling_pose_frame_count_threshold = 1;

	if (seq_skeleton.size() < calling_pose_frame_count_threshold) {
		return false;
	}

	std::vector<int> joints = { 0,1,2,5,15,16,17,18 };

	int calling_pose_frame_count = 0;
	P_Skeleton current_skeleton;

	for (int s = 0; s < seq_skeleton.size(); s++) {
		current_skeleton = seq_skeleton.at(s);

		float distance_1_to_2 = CalcEuclidean(current_skeleton.at(1).x, current_skeleton.at(1).y, current_skeleton.at(2).x, current_skeleton.at(2).y);
		float distance_4_to_0 = CalcEuclidean(current_skeleton.at(4).x, current_skeleton.at(4).y, current_skeleton.at(0).x, current_skeleton.at(0).y);
		float distance_7_to_0 = CalcEuclidean(current_skeleton.at(7).x, current_skeleton.at(7).y, current_skeleton.at(0).x, current_skeleton.at(0).y);

		//TODO: Fixed abnormal frame(757th frame) in Judgement::isCalling method, distance_4_to_0: 263.619476, distance_7_to_0: 284.730927
		//TODO: Fixed abnormal frames(<756th frame) in Judgement::isCalling method, calling_pose_frame_count is always 500
		if (distance_4_to_0 < distance_1_to_2 * 1.5 || distance_7_to_0 < distance_1_to_2 * 1.5) {
			calling_pose_frame_count++;
		} else {
			calling_pose_frame_count = 0;
		}
	}

	if (calling_pose_frame_count >= calling_pose_frame_count_threshold) {
		//cv::Rect head_rect = this->GetHeadArea(current_skeleton, joints);
		//cv::Mat roi = cur_frame(head_rect);
		//std::vector<std::pair<std::string, float>> result = IR.Predict(GetImgMask(roi, cv::Size(128, 128)));
		//if (result.at(0).second > result.at(6).second) {
		//	is_calling = true;
		//}
		return true;
	}
	return false;
}

bool Judgement::isEating() {
	P_Skeleton current_skeleton;
	if (seq_skeleton.size() > 15) {
		int eat_state = 0;
		for (int s = 0; s < 10; s++) {
			current_skeleton = seq_skeleton.at(seq_skeleton.size() - s - 1);
			if (current_skeleton.size() == 25) {
				float distance = CalcEuclidean(current_skeleton.at(7).x, current_skeleton.at(7).y, current_skeleton.at(0).x, current_skeleton.at(0).y);
				if (distance < 130 && current_skeleton.at(0).p > 0.5) {
					eat_state++;
				}
			}
		}
		if (eat_state > 5) {
			return true;
		}
	}
	return false;
}
bool Judgement::isPlayingPhone() {
	return false;
}

bool Judgement::isSleeping() {
	P_Skeleton current_skeleton;
	if (seq_skeleton.size() > 15) {
		current_skeleton = seq_skeleton.at(seq_skeleton.size() - 1);
		float distance_3_to_4 = CalcEuclidean(current_skeleton.at(3).x, current_skeleton.at(3).y, current_skeleton.at(4).x, current_skeleton.at(4).y);
		float distance_3_to_7 = CalcEuclidean(current_skeleton.at(3).x, current_skeleton.at(3).y, current_skeleton.at(7).x, current_skeleton.at(7).y);
		float distance_6_to_7 = CalcEuclidean(current_skeleton.at(6).x, current_skeleton.at(6).y, current_skeleton.at(7).x, current_skeleton.at(7).y);
		float distance_6_to_4 = CalcEuclidean(current_skeleton.at(6).x, current_skeleton.at(6).y, current_skeleton.at(4).x, current_skeleton.at(4).y);

		if (distance_3_to_4 > distance_3_to_7 || distance_6_to_7 > distance_6_to_4) {
			return true;
		}
	}

	return false;
}

bool Judgement::isSmoking() {
	bool is_smoking = false;
	if (seq_skeleton.size() < SHORT_SEQ_NUM / 2) {
		return is_smoking;
	}
	////持续警告smoke_detect_stride次
	//if (smoke_detect_stride > 0) {
	//	smoke_detect_stride--;
	//	return true;
	//}

	std::vector<int> joints = { 0,1,2,5,15,16,17,18 };

	std::vector<uint> smoke_state;
	int smoke_count = 0;
	float lh_dst2_le = 0;
	float rh_dst2_re = 0;
	float lh_dst = 0;
	float rh_dst = 0;
	float ls_dst = 0;
	float rs_dst = 0;
	for (int s = 0; s < seq_skeleton.size();s++) {
		P_Skeleton current_skeleton;
		current_skeleton = seq_skeleton.at(s);
		if (current_skeleton.at(0).p < 0.5) {
			continue;
		}
		lh_dst2_le = CalcEuclidean(current_skeleton.at(4).x, current_skeleton.at(4).y, current_skeleton.at(17).x, current_skeleton.at(17).y);
		rh_dst2_re = CalcEuclidean(current_skeleton.at(7).x, current_skeleton.at(7).y, current_skeleton.at(17).x, current_skeleton.at(17).y);
		lh_dst = CalcEuclidean(current_skeleton.at(4).x, current_skeleton.at(4).y, current_skeleton.at(0).x, current_skeleton.at(0).y);
		rh_dst = CalcEuclidean(current_skeleton.at(7).x, current_skeleton.at(7).y, current_skeleton.at(0).x, current_skeleton.at(0).y);
		ls_dst = CalcEuclidean(current_skeleton.at(2).x, current_skeleton.at(2).y, current_skeleton.at(0).x, current_skeleton.at(0).y);
		rs_dst = CalcEuclidean(current_skeleton.at(5).x, current_skeleton.at(5).y, current_skeleton.at(0).x, current_skeleton.at(0).y);
		
		float ll_angle = CalcAngle(current_skeleton.at(9).x, current_skeleton.at(9).y, current_skeleton.at(11).x, current_skeleton.at(11).y);
		float rl_angle = CalcAngle(current_skeleton.at(12).x, current_skeleton.at(12).y, current_skeleton.at(11).x, current_skeleton.at(11).y);
				
		if (current_skeleton.at(4).p < 0.5) {
			lh_dst = 999;
		}
		if (current_skeleton.at(7).p < 0.5) {
			rh_dst = 999;
		}
		if (current_skeleton.at(2).p < 0.5) {
			ls_dst = 999;
		}
		if (current_skeleton.at(5).p < 0.5) {
			rs_dst = 999;
		}

		if ((lh_dst < ls_dst && lh_dst<lh_dst2_le)) {
			joints.push_back(4);
			smoke_state.push_back(1);
			smoke_count++;
		}
		else if (rh_dst < rs_dst && rh_dst < rh_dst2_re) {
			joints.push_back(7);
			smoke_state.push_back(1);
			smoke_count++;
		}
		else {
			smoke_state.push_back(0);
		}
	}

	P_Skeleton current_skeleton = seq_skeleton.at(seq_skeleton.size() - 1);
	if (smoke_count / float(seq_skeleton.size()) > 0.3 && ((lh_dst < ls_dst && lh_dst < lh_dst2_le) || (rh_dst < rs_dst && rh_dst < rh_dst2_re))) {
		//裁剪人脸区域
		cv::Rect head_rect = this->GetHeadArea(current_skeleton, joints);
		//std::vector<std::pair<std::string, float>> result = IR.Predict(GetImgMask(cur_frame(head_rect), cv::Size(128, 128)));

		//if (result.at(6).second > result.at(0).second) {
			is_smoking = true;
			//smoke_detect_stride = 10;
		//}
	}
	return is_smoking;
}
double Judgement::angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	double angle_line = (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
	return acos(angle_line) * 180 / 3.141592653;
}
bool Judgement::isFalling() {
	P_Skeleton current_skeleton;
	if (seq_skeleton.size() > 3) {
		current_skeleton = seq_skeleton.at(seq_skeleton.size() - 1);
	}
	else {
		return false;
	}

	int joints = 0;
	for (int s = 0; s < current_skeleton.size(); s++) {
		if (current_skeleton.at(s).p > 0.05 && current_skeleton.at(s).x > 0 && current_skeleton.at(s).y > 0) {
			joints++;
		}
	}
	if (joints < 20) {
		return false;
	}
	//compute joint angles
	//计算人体的折叠角度
	float jiajiao_1 = 180;
	if (current_skeleton.at(1).p > 0.05 && 
		current_skeleton.at(10).p > 0.05 && 
		current_skeleton.at(8).p > 0.05 &&
		current_skeleton.at(1).x >1 &&
		current_skeleton.at(1).y >1 &&
		current_skeleton.at(10).x >1 &&
		current_skeleton.at(10).y >1 &&
		current_skeleton.at(8).x>1 &&
		current_skeleton.at(8).y>1) {
		jiajiao_1=angle(
			cv::Point(current_skeleton.at(1).x, current_skeleton.at(1).y),
			cv::Point(current_skeleton.at(10).x, current_skeleton.at(10).y),
			cv::Point(current_skeleton.at(8).x, current_skeleton.at(8).y));
	}
	float jiajiao_2 = 180;
	if (current_skeleton.at(1).p > 0.05 &&
		current_skeleton.at(13).p > 0.05 &&
		current_skeleton.at(8).p > 0.05 &&
		current_skeleton.at(1).x >1 &&
		current_skeleton.at(1).y >1 &&
		current_skeleton.at(13).x >1 &&
		current_skeleton.at(13).y>1 &&
		current_skeleton.at(8).x>1 &&
		current_skeleton.at(8).y>1) {
		jiajiao_2=angle(
			cv::Point(current_skeleton.at(1).x, current_skeleton.at(1).y),
			cv::Point(current_skeleton.at(13).x, current_skeleton.at(13).y),
			cv::Point(current_skeleton.at(8).x, current_skeleton.at(8).y));
	}
	if ((jiajiao_1 < 90 && jiajiao_2 < 120)  || (jiajiao_1 < 120 && jiajiao_2 < 90)) {
		/*std::cout << jiajiao_1 << "," << jiajiao_2 << std::endl;
		std::cout << "(" << current_skeleton.at(1).x << "," << current_skeleton.at(1).y << ")"\
			<< "(" << current_skeleton.at(8).x << "," << current_skeleton.at(8).y << ")"\
			<< "(" << current_skeleton.at(10).x << "," << current_skeleton.at(10).y << ")"\
			<< "(" << current_skeleton.at(13).x << "," << current_skeleton.at(13).y << ")" << std::endl;*/
		return true;
	}
	return false;
}
bool Judgement::isRunning() {
	int used_frames = seq_bbox.size();
	int contral_frame_size=5;
	if (used_frames<contral_frame_size) {
		return false;
	}
    //judge the direction of motion is same in short-time
    bool same_direction=false;
	std::vector<bool> l1,l2,l3,l4;
	for(int i=0;i<contral_frame_size-1;i++){
        if(seq_bbox.at(seq_bbox.size()-1-i).x<seq_bbox.at(seq_bbox.size()-1-i-1).x){
            l1.push_back(true);
        }
        if(seq_bbox.at(seq_bbox.size()-1-i).x>seq_bbox.at(seq_bbox.size()-1-i-1).x){
            l2.push_back(true);
        }
        if(seq_bbox.at(seq_bbox.size()-1-i).y<seq_bbox.at(seq_bbox.size()-1-i-1).y){
            l3.push_back(true);
        }
        if(seq_bbox.at(seq_bbox.size()-1-i).y>seq_bbox.at(seq_bbox.size()-1-i-1).y){
            l4.push_back(true);
        }
    }
    if(l1.size()+1==contral_frame_size || l2.size()+1==contral_frame_size || l3.size()+1==contral_frame_size ||l4.size()+1==contral_frame_size){
        same_direction=true;
    }
	if(!same_direction) return false;
//    float moving_distance = CalcEuclidean(seq_bbox.at(seq_bbox.size() - 1).x, seq_bbox.at(seq_bbox.size() - 1).y,seq_bbox.at(seq_bbox.size() -used_frames).x, seq_bbox.at(seq_bbox.size() - used_frames).y);
    float mean_move=0.0;
    for(int i=1;i<used_frames;i++){
        float distance=CalcEuclidean(seq_bbox.at(seq_bbox.size()-used_frames+i).x,seq_bbox.at(seq_bbox.size()-used_frames+i).y,seq_bbox.at(seq_bbox.size()-used_frames+i-1).x,seq_bbox.at(seq_bbox.size()-used_frames+i-1).y);
        float move_ratio=distance/(seq_bbox.at(seq_bbox.size()-used_frames+i).width);
        mean_move+=move_ratio;
    }
    mean_move/=used_frames;
//	float person_width = seq_bbox.at(seq_bbox.size()  - used_frames).width;
//	std::cout << "running:---------------------------------->" << moving_distance /float(person_width) << std::endl;
//    std::cout << "mean_move:---------------------------------->" << mean_move << std::endl;
//	if (moving_distance /float(person_width) >0.3) {
//		return true;
//	}
    if (mean_move >0.07) {
        return true;
    }
	return false;
}
bool Judgement::isStanding() {
	int min_x = 999;
	int min_y = 999;
	int max_x = 0;
	int max_y = 0;
	int used_frames = SHORT_SEQ_NUM / 2;
	if (seq_bbox.size() < used_frames) {
		return false;
	}
	//for (int k = 0; k < seq_bbox.size(); k++) {
	//	if (seq_bbox.at(k).x > max_x || seq_bbox.at(k).y > max_y) {
	//		max_x = seq_bbox.at(k).x;
	//		max_y = seq_bbox.at(k).y;
	//	}
	//	if (seq_bbox.at(k).x < min_x || seq_bbox.at(k).y < min_y) {
	//		min_x = seq_bbox.at(k).x;
	//		min_y = seq_bbox.at(k).y;
	//	}
	//}
	float moving_distance = CalcEuclidean(seq_bbox.at(seq_bbox.size() - used_frames).x, seq_bbox.at(seq_bbox.size() - used_frames).y, seq_bbox.at(seq_bbox.size() - 1).x, seq_bbox.at(seq_bbox.size() - 1).y);
	float person_width = seq_bbox.at(seq_bbox.size()-1).width;

	if (moving_distance < 0.1*person_width ){
		//std::cout << "stay:"<< moving_distance <<  "," << person_width << std::endl;
		return true;
	}
	return false;
}
bool Judgement::isStopping() {
	if (seq_skeleton.size() < SHORT_SEQ_NUM / 2) {
		return false;
	}
	std::vector<uint> stand_state;
	int smoke_count = 0;
	for (int s = 0; s < seq_skeleton.size(); s++) {
		P_Skeleton current_skeleton;
		current_skeleton = seq_skeleton.at(s);
	}
}
bool Judgement::isWandering() {
	int min_x = 999;
	int min_y = 999;
	int max_x = 0;
	int max_y = 0;
	if (seq_bbox.size() < SHORT_SEQ_NUM / 3) {
		return false;
	}
	for (int k = 0; k < seq_bbox.size(); k++) {
		if (seq_bbox.at(k).x > max_x || seq_bbox.at(k).y > max_y) {
			max_x = seq_bbox.at(k).x;
			max_y = seq_bbox.at(k).y;
		}
		if (seq_bbox.at(k).x < min_x || seq_bbox.at(k).y < min_y) {
			min_x = seq_bbox.at(k).x;
			min_y = seq_bbox.at(k).y;
		}
	}
	float moving_distance_x = max_x - min_x;
	float moving_distance_y = max_y - min_y;
	float person_width = seq_bbox.at(seq_bbox.size() - 1).width;

	//std::cout << "wander:" << moving_distance_x << "," << moving_distance_y << "," << 0.2*person_width << std::endl;
	if (((moving_distance_x >0.5*person_width) && (moving_distance_y < 0.2*person_width)) || ((moving_distance_x <0.2*person_width) && (moving_distance_y > 0.5*person_width))) {
		return true;
	}
	return false;
}
Violation Judgement::isFighting() {
	Violation violation;
	violation.task = "fight";
	for (int t = 0; t < cur_bbox.size(); t++) {
		for (int r = t + 1; r < cur_bbox.size(); r++) {
			float iou = CalcIOU(cur_bbox.at(t), cur_bbox.at(r));
			//std::cout << "fighting iou:" << iou << std::endl;
			if (iou > 0.5) {
				int min_x = cv::min (cur_bbox.at(t).x,cur_bbox.at(r).x);
				int min_y = cv::min(cur_bbox.at(t).y, cur_bbox.at(r).y);
				int max_x = cv::max(cur_bbox.at(t).x+ cur_bbox.at(t).width, cur_bbox.at(r).x+ cur_bbox.at(r).width);
				int max_y = cv::max(cur_bbox.at(t).y + cur_bbox.at(t).height, cur_bbox.at(r).y + cur_bbox.at(r).height);
				cv::Rect rect = cv::Rect(min_x,min_y,max_x-min_x,max_y-min_y);
//				cv::Rect cut_rect;
//				int edge_size = cv::max(rect.height,rect.width);
//
//				cut_rect.x = (min_x + max_x) / 2 - edge_size / 2 ;
//				cut_rect.y = (min_y + max_y) / 2 - edge_size / 2;
//				cut_rect.height = edge_size;
//				cut_rect.width = edge_size;
//
//				if (cut_rect.x <= 0) {
//					cut_rect.x = 0;
//				}
//				if (cut_rect.x + cut_rect.width > cur_frame.cols) {
//					cut_rect.width = cur_frame.cols - cut_rect.x - 1;
//				}
//				if (cut_rect.y <= 0) {
//					cut_rect.y = 0;
//				}
//				if (cut_rect.y + cut_rect.height > cur_frame.rows) {
//					cut_rect.height = cur_frame.rows - cut_rect.y - 1;
//				}
                //judge through the location of hand
                P_Skeleton skt_r=cur_skeleton.at(r);
                P_Skeleton skt_t=cur_skeleton.at(t);
                float distance_1=999;
                float distance_2=999;
                float distance_3=999;
                float distance_4=999;
                if(skt_r.at(4).p>0.1 && skt_t.at(1).p>0.1) {
                    distance_1 = CalcEuclidean(skt_r.at(4).x, skt_r.at(4).y, skt_t.at(1).x, skt_t.at(1).y);
                }
                if(skt_r.at(7).p>0.1 && skt_t.at(1).p>0.1) {
                    distance_2 = CalcEuclidean(skt_r.at(7).x, skt_r.at(7).y, skt_t.at(1).x, skt_t.at(1).y);
                }
                if(skt_r.at(1).p>0.1 && skt_t.at(4).p>0.1) {
                    distance_3 = CalcEuclidean(skt_r.at(1).x, skt_r.at(1).y, skt_t.at(4).x, skt_t.at(4).y);
                }
                if(skt_r.at(1).p>0.1 && skt_t.at(7).p>0.1) {
                    distance_4 = CalcEuclidean(skt_r.at(1).x, skt_r.at(1).y, skt_t.at(7).x, skt_t.at(7).y);
                }
                if( distance_1<cur_bbox.at(t).width/2 ||
                    distance_2<cur_bbox.at(t).width/2 ||
                    distance_3<cur_bbox.at(r).width/2 ||
                    distance_4<cur_bbox.at(r).width/2  ){
                    violation.bbox.push_back(rect);
                    violation.skeleton.push_back(cur_skeleton.at(t));
                    violation.skeleton.push_back(cur_skeleton.at(r));
                    violation.target_id.push_back(cur_target_id.at(t));
                    violation.target_id.push_back(cur_target_id.at(r));
                }
			}
		}
	}
	return violation;
}
Violation Judgement::isGathering() {
	Violation violation;
	violation.task = "gather";
	std::vector<std::pair<int, int>> near_person;
	std::vector<std::vector<int>> close_person_list;
	std::vector<std::vector<cv::Point>> close_person_corner;
	float radio = 1.2;
	for (int t = 0; t < cur_bbox.size(); t++) {
		for (int r = t + 1; r < cur_bbox.size(); r++) {
			//统计两人的距离小于一个身位的样本对，记录编号
			//身位计算方法：bbox左上角的欧式距离，左侧人的宽度
			//视频中，人体宽度不会受到摄像头角度的影响，而高度会受到影响
			float dst = CalcEuclidean(cur_bbox.at(t).x,cur_bbox.at(t).y,cur_bbox.at(r).x,cur_bbox.at(r).y);
			float thr = 0;
			if (cur_bbox.at(t).x <= cur_bbox.at(r).x) {
				thr = radio*cur_bbox.at(t).width;
			}
			else {
				thr = radio*cur_bbox.at(r).width;
			}
			if (dst < thr) {
				bool flag = true;
				for (int cp = 0; cp < close_person_list.size(); cp++) {
					int count_t = std::count(close_person_list.at(cp).begin(), close_person_list.at(cp).end(), t);
					int count_r = std::count(close_person_list.at(cp).begin(), close_person_list.at(cp).end(), r);
					if (count_t> 0 && count_r> 0) {
						flag = false;
						break;
					}
					else if(count_t>0 && count_r==0){
						flag = false;
						close_person_list.at(cp).push_back(r);
						close_person_corner.at(cp).push_back(cv::Point(cur_bbox.at(r).x, cur_bbox.at(r).y));
						close_person_corner.at(cp).push_back(cv::Point(cur_bbox.at(r).x+ cur_bbox.at(r).width, cur_bbox.at(r).y+ cur_bbox.at(r).height));
						break;
					}				
					else if (count_t==0 && count_r > 0) {
						flag = false;
						close_person_list.at(cp).push_back(t);
						close_person_corner.at(cp).push_back(cv::Point(cur_bbox.at(t).x, cur_bbox.at(t).y));
						close_person_corner.at(cp).push_back(cv::Point(cur_bbox.at(t).x + cur_bbox.at(t).width, cur_bbox.at(t).y + cur_bbox.at(t).height));
						break;
					}
				}
				if (flag) {
					close_person_list.push_back(std::vector<int>{t,r});
					close_person_corner.push_back(std::vector<cv::Point>{
						cv::Point(cur_bbox.at(t).x, cur_bbox.at(t).y), 
						cv::Point(cur_bbox.at(t).x + cur_bbox.at(t).width, cur_bbox.at(t).y + cur_bbox.at(t).height),
						cv::Point(cur_bbox.at(r).x, cur_bbox.at(r).y),
						cv::Point(cur_bbox.at(r).x + cur_bbox.at(r).width, cur_bbox.at(r).y + cur_bbox.at(r).height),
					});
				}
			}
		}
	}

	for (int i = 0; i < close_person_list.size(); i++) {
		//std::cout << close_person_list.at(i).size() << std::endl;
		if (close_person_list.at(i).size() >= 3) {
			//寻找最小保卫矩形
			cv::Rect rect = cv::boundingRect(close_person_corner.at(i));
			cv::Rect cut_rect;
			int edge_size = cv::max(rect.height, rect.width);
			cut_rect.x = (rect.x + rect.x+rect.width) / 2 - edge_size / 2;
			cut_rect.y = (rect.y + rect.y+rect.height) / 2 - edge_size / 2;
			cut_rect.height = edge_size;
			cut_rect.width = edge_size;
			if (cut_rect.x <= 0) {
				cut_rect.x = 0;
			}
			if (cut_rect.x + cut_rect.width > cur_frame.cols) {
				cut_rect.width = cur_frame.cols - cut_rect.x - 1;
			}
			if (cut_rect.y <= 0) {
				cut_rect.y = 0;
			}
			if (cut_rect.y + cut_rect.height > cur_frame.rows) {
				cut_rect.height = cur_frame.rows - cut_rect.y - 1;
			}
			cv::Mat roi = cur_frame(cut_rect);
			//std::vector<std::pair<std::string, float>> result = IR.Predict(roi);
			//if (result.at(4).second > 0.2) {
			violation.bbox.clear();
			violation.skeleton.clear();
			violation.bbox.push_back(rect);
			for (int cp = 0; cp < close_person_list.at(i).size(); cp++) {
				violation.skeleton.push_back(cur_skeleton.at(close_person_list.at(i).at(cp)));
				violation.target_id.push_back(cur_target_id.at(close_person_list.at(i).at(cp)));
			}
			//}
		}
	}
	return violation;
}
Violation Judgement::isGathering_v2() {
	//std::cout << "cur_bbox : " << cur_bbox.size() << std::endl;
	//std::cout << "cur_skt : " << cur_skeleton.size() << std::endl;

	Violation violation;
	violation.task = "gather";
	//step 0. 初始化数据存储变量
	float ratio = 1.2;
	std::vector<std::pair<int, int>> pair_node;
	std::vector<std::vector<int>> networks;
	//step 1. 统计所有的最近邻成对人体
	for (int i = 0; i < cur_bbox.size(); i++) {
		//计算其他个体距离i个体的距离
		std::vector<float> distances; 
		for (int j = 0; j < cur_bbox.size(); j++) {
			float distance = CalcEuclidean(cur_bbox.at(i).x, cur_bbox.at(i).y, cur_bbox.at(j).x, cur_bbox.at(j).y);
			if (i != j &&  distance<cur_bbox.at(i).width*ratio) {
				pair_node.push_back(std::make_pair(i, j));
			}
			//if(j==i)	distances.push_back(999);
			//else distances.push_back(distance);
		}
		//auto smallest = std::min_element(std::begin(distances), std::end(distances));
		//if (*smallest < cur_bbox.at(i).width*ratio) {
		//	pair_node.push_back(std::make_pair(i, std::distance(std::begin(distances), smallest)));
		//}
	}
	//step 2. 构建节点网络
	int gather_area_index = -1;
	int node_num = 0;
	for (int i = 0; i < pair_node.size(); i++) {
		std::vector<int> network = {pair_node.at(i).first,pair_node.at(i).second};
		for (int j = i+1; j < pair_node.size(); j++) {
			bool del_node = false;

			std::vector<int>::iterator index_first, index_second;
			index_first = std::find(network.begin(), network.end(), pair_node.at(j).first);
			index_second = std::find(network.begin(), network.end(), pair_node.at(j).second);

			if (index_first != network.end() && index_second == network.end()) {
				network.push_back(pair_node.at(j).second);
				del_node = true;
			}
			if (index_first == network.end() && index_second != network.end()) {
				network.push_back(pair_node.at(j).first);
				del_node = true;
			}
			
			if (del_node) {
				pair_node.erase(pair_node.begin()+j);
				j--;
			}
		}
		networks.push_back(network);
		if (network.size() > node_num) {
			node_num = network.size();
			gather_area_index = networks.size();
		}
	}
	std::cout << "--------------------------------> node num " << node_num << std::endl;

	//step 3. 依据网络中节点数量进行判别
	std::vector<cv::Point> gather_area_corner;
	if (node_num>=3) {
		for (int i = 0; i < networks.at(gather_area_index).size(); i++) {
			gather_area_corner.push_back(
				cv::Point( 
					cur_bbox.at(networks.at(gather_area_index).at(i)).x,
					cur_bbox.at(networks.at(gather_area_index).at(i)).y
				)
			);
			gather_area_corner.push_back(
				cv::Point(
					cur_bbox.at(networks.at(gather_area_index).at(i)).x+ cur_bbox.at(networks.at(gather_area_index).at(i)).width,
					cur_bbox.at(networks.at(gather_area_index).at(i)).y+ cur_bbox.at(networks.at(gather_area_index).at(i)).height
				)
			);
			violation.skeleton.push_back(cur_skeleton.at(networks.at(gather_area_index).at(i)));
			violation.target_id.push_back(cur_target_id.at(networks.at(gather_area_index).at(i)));
		}
		cv::Rect gather_area = cv::boundingRect(gather_area_corner);
		violation.bbox.push_back(gather_area);
	}
	//step 4. 输出信息
	return violation;
}
float  Judgement::CalcAngle(float x1, float y1, float x2, float y2) {
	float angle_temp;
	float xx, yy;
	xx = abs(x2 - x1);
	yy = abs(y2 - y1);
	return atan2(yy, xx) * 180 / PI;
}
float Judgement::CalcEuclidean(float x1, float y1, float x2, float y2) {
	float distance = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	return distance;
}
std::vector<std::pair<float, float>> Judgement::CalcFiveKeyPoints(P_Skeleton skeleton) {
	std::vector<std::vector<int>> joint_list = {
		{ 0,15,16,17,18 }, //head 
		{ 1,2,5 },         //shoulder 
		{ 8,9,12 },        //crotch 
		{ 11,22,23,24 },   //left foot
		{ 14,19,20,21 }    //right foot
	};
	std::vector<std::pair<float, float>> key_point;
	for (int i = 0; i < joint_list.size(); i++) {
		float center_x = 0.0;
		float center_y = 0.0;
		int num = 0;
		for (int j = 0; j < joint_list.at(i).size(); j++) {
			if (skeleton.at(joint_list.at(i).at(j)).p > 0.5) {
				center_x += skeleton.at(joint_list.at(i).at(j)).x;
				center_y += skeleton.at(joint_list.at(i).at(j)).y;
				num += 1;
			}
		}
		center_x /= num;
		center_y /= num;
		key_point.push_back(std::make_pair(center_x, center_y));
	}
	return key_point;
}
float Judgement::CalcIOU(cv::Rect new_bbox, cv::Rect old_bbox){
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
cv::Mat Judgement::GetImgMask(cv::Mat inputImage, cv::Size size) {
	int s;
	if (inputImage.rows > inputImage.cols) {
		s = inputImage.rows;
	}
	else {
		s = inputImage.cols;
	}
	cv::Mat image_temp_ep(s, s, CV_8UC3, cv::Scalar(0, 0, 0));
	if (inputImage.channels() == 1) {
		cv::cvtColor(image_temp_ep, image_temp_ep, CV_BGR2GRAY);
	}
	cv::Mat image_temp_ep_roi = image_temp_ep(cv::Rect((s - inputImage.cols) / 2, (s - inputImage.rows) / 2, inputImage.cols, inputImage.rows));
	cv::Mat dstNormImg;
	addWeighted(image_temp_ep_roi, 0., inputImage, 1.0, 0., image_temp_ep_roi);
	resize(image_temp_ep, dstNormImg, size, 0, 0, 1);    //大小归一化
	return dstNormImg;
}
int Judgement::RecIncident(cv::Mat image) {
	std::vector<std::pair<std::string, float>> result= IR.Predict(image);
	return 0;
}
cv::Rect Judgement::GetHeadArea(P_Skeleton skeleton, std::vector<int> joints) {
	std::vector<cv::Point> keypoints;
	for (int j = 0; j < joints.size(); j++) {
		if (skeleton.at(joints.at(j)).p > 0.5) {
			keypoints.push_back(cv::Point(skeleton.at(joints.at(j)).x, skeleton.at(joints.at(j)).y));
		}
	}
	cv::Rect rect = cv::boundingRect(keypoints);

	cv::Rect cut_rect;
	int edge_size = cv::max(rect.height, rect.width);

	cut_rect.x = (rect.x + rect.x + rect.width) / 2 - edge_size / 2;
	cut_rect.y = (rect.y + rect.y + rect.height) / 2 - edge_size / 2;
	cut_rect.height = edge_size;
	cut_rect.width = edge_size;

	if (cut_rect.x <= 0) {
		cut_rect.x = 0;
	}
	if (cut_rect.x + cut_rect.width > cur_frame.cols) {
		cut_rect.width = cur_frame.cols - cut_rect.x - 1;
	}
	if (cut_rect.y <= 0) {
		cut_rect.y = 0;
	}
	if (cut_rect.y + cut_rect.height > cur_frame.rows) {
		cut_rect.height = cur_frame.rows - cut_rect.y - 1;
	}
	return cut_rect;
}

float Judgement::getAngelOfTwoVector(cv::Point2f pt1, cv::Point2f pt2, cv::Point2f c) {
	float theta = atan2(pt1.x - c.x, pt1.y - c.y) - atan2(pt2.x - c.x, pt2.y - c.y);
	if (theta > CV_PI)
		theta -= 2 * CV_PI;
	if (theta < -CV_PI)
		theta += 2 * CV_PI;

	theta = theta * 180.0 / CV_PI;
	return theta;
}
void Judgement::LoadTemplate() {

}
bool Judgement::exist(P_Skeleton skt, std::vector<int> index, float thr) {
	bool state = true;
	for (int i = 0; i < index.size(); i++) {
		if (skt.at(index.at(i)).p < thr) {
			state = false;
			break;
		}
	}
	return state;
}
