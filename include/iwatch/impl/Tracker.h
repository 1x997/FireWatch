#pragma once
#include "common.h"
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/dir_nav.h>
#include <dlib/opencv.h>
class Util
{
public:

	/* --------------------------------------------
	Function : cvtRectToRect
	Convert cv::Rect to dlib::drectangle
	----------------------------------------------- */
	static dlib::drectangle cvtRectToDrect(cv::Rect _rect)
	{
		return dlib::drectangle(_rect.tl().x, _rect.tl().y, _rect.br().x - 1, _rect.br().y - 1);
	}


	/* -------------------------------------------------
	Function : cvtMatToArray2d
	convert cv::Mat to dlib::array2d<unsigned char>
	------------------------------------------------- */
	static dlib::array2d<unsigned char> cvtMatToArray2d(cv::Mat _mat) // cv::Mat, not cv::Mat&. Make sure use copy of image, not the original one when converting to grayscale
	{

		//Don't need to use color image in HOG-feature-based tracker
		//Convert color image to grayscale
		if (_mat.channels() == 3)
			cv::cvtColor(_mat, _mat, cv::COLOR_RGB2GRAY);

		//Convert opencv 'MAT' to dlib 'array2d<unsigned char>'
		dlib::array2d<unsigned char> dlib_img;
		dlib::assign_image(dlib_img, dlib::cv_image<unsigned char>(_mat));

		return dlib_img;
	}
	/* -----------------------------------------------------------------
	Function : setRectToImage
	Put all tracking results(new rectangle) on the frame image
	Parameter _rects is stl container(such as vector..) filled with
	cv::Rect
	----------------------------------------------------------------- */
	template <typename Container>
	static void setRectToImage(cv::Mat& _mat_img, Container _rects)
	{
		std::for_each(_rects.begin(), _rects.end(), [&_mat_img](cv::Rect rect) {
			cv::rectangle(_mat_img, rect, cv::Scalar(0, 0, 255));
		});
	}
};
class SingleTracker
{
private:
	int			target_id;			// Unique Number for target
	double		confidence;			// Confidence of tracker
	cv::Rect	rect;				// Initial Rectangle for target
	cv::Point	center;				// Current center point of target
	bool		is_tracking_started;// Is tracking started or not? (Is initializing done or not?)
	cv::Scalar	color;				// Box color
	int			frame_count = 0;    //total number of frames
	P_Skeleton	skeleton;
	
	std::vector<P_Skeleton> seq_skeletons;
	std::vector<cv::Rect> seq_bbox;

	std::vector<P_Skeleton> short_seq_skeletons;
	std::vector<cv::Rect> short_seq_bbox;

	std::vector<P_Skeleton> medium_seq_skeletons;
	std::vector<cv::Rect> medium_seq_bbox;
	
	std::vector<P_Skeleton> long_seq_skeletons;
	std::vector<cv::Rect> long_seq_bbox;

public:
	dlib::correlation_tracker tracker{4, 4, 15};  // Correlation tracker
										/* Member Initializer & Constructor*/
	SingleTracker(int _target_id, cv::Rect _init_rect, cv::Scalar _color)
		: target_id(_target_id), confidence(0), is_tracking_started(false)
	{
		// Exception
		if (_init_rect.area() == 0)
		{
			std::cout << "======================= Error Occured! ======================" << std::endl;
			std::cout << "Function : Constructor of SingleTracker" << std::endl;
			std::cout << "Parameter cv::Rect _init_rect's area is 0" << std::endl;
			std::cout << "=============================================================" << std::endl;
		}
		else
		{
			// Initialize rect and center using _init_rect
			this->setRect(_init_rect);
			this->setCenter(_init_rect);
			this->setColor(_color);
		}
	}
	/* Get Function */
	int			getTargetID() { return this->target_id; }
	cv::Rect	getRect() { return this->rect; }
	cv::Point	getCenter() { return this->center; }
	double		getConfidence() { return this->confidence; }
	bool		getIsTrackingStarted() { return this->is_tracking_started; }
	cv::Scalar	getColor() { return this->color; }
	P_Skeleton	getSkeleton() { return this->skeleton; };
	//reset
	void resetSeqSkeleton();
	void resetSeqBBox();
	//获取一定时间内的骨架或者检测框信息
	std::vector<P_Skeleton> getSeqSkeleton();
	std::vector<cv::Rect> getSeqBBox();
	std::vector<P_Skeleton> getShortSeqSkeleton();
	std::vector<cv::Rect> getShortSeqBBox();
	std::vector<P_Skeleton> getMediumSeqSkeleton();
	std::vector<cv::Rect> getMediumSeqBBox();
	std::vector<P_Skeleton> getLongSeqSkeleton();
	std::vector<cv::Rect> getLongSeqBBox();

	/* Set Function */
	void setTargetId(int _target_id) { this->target_id = _target_id; }
	void setRect(cv::Rect _rect) { this->rect = _rect; }
	void setRect(dlib::drectangle _drect) { this->rect = cv::Rect(_drect.tl_corner().x(), _drect.tl_corner().y(), _drect.width(), _drect.height()); }
	void setCenter(cv::Point _center) { this->center = _center; }
	void setCenter(cv::Rect _rect) { this->center = cv::Point(_rect.x + (_rect.width) / 2, _rect.y + (_rect.height) / 2); }
	void setCenter(dlib::drectangle _drect) { this->center = cv::Point(_drect.tl_corner().x() + (_drect.width() / 2), _drect.tl_corner().y() + (_drect.height() / 2)); }
	void setConfidence(double _confidence) { this->confidence = _confidence; }
	void setIsTrackingStarted(bool _b) { this->is_tracking_started = _b; }
	void setColor(cv::Scalar _color) { this->color = _color; }
	void setSkeleton(P_Skeleton skeleton) ;
	void setBBox(cv::Rect bbox);
	/* Core Function */
	// Initialize
	int startSingleTracking(cv::Mat _mat_img);

	// Do tracking
	int doSingleTracking(cv::Mat _mat_img);
	//update the bbox and skeleton of the specific person
	int update(cv::Rect bbox,P_Skeleton skeleton);
	// Check the target is inside of the frame
	int isTargetInsideFrame(int _frame_width, int _frame_height);
};
/* ==========================================================================

Class : TrackerManager
TrackerManagerÖ¼ÔÚ¹ÜÀívector <std :: shared_ptr <SingleTracker >>
ÓÃÓÚ¶àÄ¿±ê¸ú×Ù¡£
£¨ÎªÁË·½±ãÆð¼û£¬ËüÓëvector <SigleTracker *>¼¸ºõÏàÍ¬£©
Õâ¸öÀàÌá¹©ÁËinsert£¬find£¬delete¹¦ÄÜ¡£
========================================================================== */
class TrackerManager
{
private:
	std::vector<std::shared_ptr<SingleTracker>> tracker_vec; // Vector filled with SingleTracker shared pointer. It is the most important container in this program.
	
public:
	/* Get Function */
	std::vector<std::shared_ptr<SingleTracker>>& getTrackerVec() { return this->tracker_vec; } // Return reference! not value!

																							   /* Core Function */
																							   // Insert new SingleTracker shared pointer into the TrackerManager::tracker_vec
	int insertTracker(cv::Rect _init_rect, cv::Scalar _color, int _target_id);
	int insertTracker(std::shared_ptr<SingleTracker> new_single_tracker);
	int updateTracker(int _target_id,cv::Rect new_rect,P_Skeleton skeeton);
	// Find SingleTracker in the TrackerManager::tracker_vec using SingleTracker::target_id
	int findTracker(int _target_id);

	// Deleter SingleTracker which has ID : _target_id from TrackerManager::tracker_vec
	int deleteTracker(int _target_id);
};
class TrackingSystem
{
private:
	std::string		frame_path;		// Path to the frame image
	int				frame_width;	// Frame image width
	int				frame_height;	// Frame image height
	cv::Mat			current_frame;	// Current frame
	std::vector<std::pair<cv::Rect, cv::Scalar>> init_target;
	std::string Transform2String(int number);
	//TargetRectDrawer	drawer;		// TargetRectDrawer
	TrackerManager		manager;	// TrackerManager

public:
	/* Constructor */
	TrackingSystem(cv::Mat image)
	{
		this->setFrameWidth(image.cols);
		this->setFrameHeight(image.rows);
		//×¢²áÐèÒª¸üÐÂµÄbbox
		//this->init_target = this->drawer.drawInitRect(image);
	};
	TrackingSystem()
	{
	};
	/* Get Function */
	std::string  getFramePath() { return this->frame_path; }
	int    getFrameWidth() { return this->frame_width; }
	int    getFrameHeight() { return this->frame_height; }
	cv::Mat   getCurrentFrame() { return this->current_frame; }
	TrackerManager getTrackerManager() { return this->manager; }

	/* Set Function */
	void   setFramePath(std::string _frame_path) { this->frame_path.assign(_frame_path); }
	void   setFrameWidth(int _frame_width) { this->frame_width = _frame_width; }
	void   setFrameHeight(int _frame_height) { this->frame_height = _frame_height; }
	void   setCurrentFrame(cv::Mat _current_frame) { this->current_frame = _current_frame; }

	/* Core Function */
	// Initialize TrackingSystem.
	int initTrackingSystem(int _target_id, cv::Rect _rect, cv::Scalar _color);
	int initTrackingSystem();

	// Start tracking
	int startTracking(cv::Mat& _mat_img);
	int startTracking(int _target_id, cv::Mat& _mat_img);
	int delTracker(int target_ID);
	int updateTacker(int target_ID,cv::Rect new_rect,P_Skeleton skeleton);
	// Draw tracking result
	int drawTrackingResult(cv::Mat& _mat_img);

	// Terminate program
	void terminateSystem();
};
