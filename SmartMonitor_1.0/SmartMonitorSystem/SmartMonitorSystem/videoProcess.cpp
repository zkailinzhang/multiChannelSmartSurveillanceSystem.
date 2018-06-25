#include "videoProcess.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/video/video.hpp"
#include "Scene.h"
#include "opencv2/ocl/ocl.hpp"

using namespace cv;
using namespace std;
using namespace cv::ocl;

extern string para_T1 ;
extern string para_T2;
extern string para_T3;
extern string para_T4;

extern string para_M1;
extern string para_M2;
extern string para_M3;
extern string para_M4;

string noPicture = "F://MonitorVideo//hello.jpg";

QImage Mat2QImage(cv::Mat& mat)
{
	cv::Mat rgb;
	cv::cvtColor(mat, rgb, CV_BGR2RGB);

	return QImage((const unsigned char*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888); 
}

Mat QImage2Mat(QImage& qimage)
{
	int nChannel=0;
	if(qimage.format()==QImage::Format_RGB888)nChannel=3;
	if(qimage.format()==QImage::Format_ARGB32)nChannel=4;
	cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC3); 
	mat.data = (uchar*)qimage.bits(); 
	Mat mat2;
	cvtColor(mat,mat2,CV_RGB2BGR);
	return mat2; 
}

CVideo::CVideo()
{

	videoHaveOpened = false; 
	needToDraw = false;     
	typeDetection = UNKNOWN_VIDEO;    
	pauseVideoFlag = false;       
	closeVideoFlag = false; 
	DoEventDetect = false;

	needProcessId = 0;
	no_picture = imread(noPicture);//测试视频时，若视频文件播放完则显示该画面

	InitializeSRWLock(&rw_lock);//初始化读写锁
}

CVideo::~CVideo()
{
	
}


bool CVideo::startPlay()
{
	
	h_showThread = (HANDLE)_beginthreadex(0,0,&VideoPlayThread,this,0,0);
	return true;
}

bool CVideo::closePlay()
{
	AcquireSRWLockExclusive(&rw_lock);
	videoHaveOpened = false;
	needToDraw = false;
	pauseVideoFlag = false;
	
	ReleaseSRWLockExclusive(&rw_lock);

	//WaitForSingleObject(h_showThread,INFINITE);
	CloseHandle(h_showThread);
	return true;
}


void CVideo::pausePlay()
{
	AcquireSRWLockExclusive(&rw_lock);
	needToDraw = !needToDraw;
	pauseVideoFlag = !pauseVideoFlag;
	ReleaseSRWLockExclusive(&rw_lock);
}

unsigned int WINAPI CVideo::VideoPlayThread(void* param)
{
	CVideo* p_CVideo = (CVideo*)param;
	Mat img,rgb;
	QImage qimg;
	DrawInfo df;
	bool drawFlag = false;
	bool pauseFlag = false;
	bool stopShow = false;
	if (!p_CVideo->cap.isOpened())
		return 1;
	while (1)
	{

		AcquireSRWLockShared(&p_CVideo->rw_lock);
		pauseFlag = p_CVideo->pauseVideoFlag;
		drawFlag = p_CVideo->needToDraw;	
		stopShow = p_CVideo->DoEventDetect || (!p_CVideo->videoHaveOpened);
		ReleaseSRWLockShared(&p_CVideo->rw_lock);

		if (stopShow)
			break;

		if (!pauseFlag)
		{
			p_CVideo->cap >> img;
		}

		if (!img.empty())
		{
			cvtColor(img,rgb,CV_BGR2RGB);
		}

		qimg =  QImage((uchar*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888);
		df.id = p_CVideo->needProcessId - 1;
		df.qimage = qimg;
		if (drawFlag) 
		{
			emit p_CVideo->sendDrawMess(df);
		}

		if (p_CVideo->video_mode == LOCAL_VIDEO)//测试视频，可以调整播放速度
		{
			int ms = 1000/p_CVideo->playRate;
			Sleep(ms);
		}
		else
		{
			Sleep(33);
		}
	}

	_endthreadex(0);
	return 0;
}


void CVideo::testEventDetect(int dType)
{
	AcquireSRWLockExclusive(&rw_lock);
	DoEventDetect = true;
	ReleaseSRWLockExclusive(&rw_lock);

	if (dType == RETROGRADE_DETECT)
	{
		h_retrograde =(HANDLE)_beginthreadex(0,0,&retrogradeThread,this,0,0);
	}
	else if (dType == CROWD_DENSITY_DETECT)
	{
		h_crowdestimate = (HANDLE)_beginthreadex(0,0,&crowdestimateThread,this,0,0);
	}
	else if(dType == RESTRICTED_ZOON_DETECT)
	{
		h_restrictedzoon = (HANDLE)_beginthreadex(0,0,&restrictedzoonThread,this,0,0);
	}
	return;
}

unsigned int WINAPI CVideo::restrictedzoonThread(void* param)
{
	CVideo* p_EventDetect = (CVideo*) param;
	string para_file;
	int m = p_EventDetect->needProcessId -1;
	if (p_EventDetect->video_mode == LOCAL_VIDEO)
	{
		switch (m)
		{
		case 0:
			para_file = para_T1;
			break;
		case 1:
			para_file = para_T2;
			break;
		case 2:
			para_file = para_T3;
			break;
		case 3:
			para_file = para_T4;
			break;
		default:
			break;
		}
	}
	else if (p_EventDetect->video_mode == CAMERA_VIDEO)
	{
		switch (m)
		{
		case 0:
			para_file = para_M1;
			break;
		case 1:
			para_file = para_M2;
			break;
		case 2:
			para_file = para_M3;
			break;
		case 3:
			para_file = para_M4;
			break;
		default:
			break;
		}
	}

	QImage qimg;
	DrawInfo df;
	Mat img,gray,fgmask;
	Boundary bd;//

	FileStorage fs(para_file, FileStorage::READ);
	FileNode fb = fs["MonitorZoonInfo"];
	if (fb.empty())
	{
		return 2;
		//未设置
	}
	bd.flag = (int)fb["BoundaryType"];
	fb["BoundaryPoints"] >> bd.point_boundray;
	fs.release();

	p_EventDetect->cap >> img;
	Mat bd_mask(img.rows,img.cols,CV_8UC1);
	Point bd_points[1][20];//20应大于bd边界点数目
	for (int i=0;i<bd.point_boundray.size();i++)
	{
		bd_points[0][i] = bd.point_boundray[i];
	}
	const Point* ppt[1] = { bd_points[0] };
	int npt[] = { bd.point_boundray.size() };

	Rect roi_monitor;//边界的外接矩形，可以一定程度上缩小计算面积
	roi_monitor = boundingRect(bd.point_boundray);

	fillPoly( bd_mask,
		ppt,
		npt,
		1,
		255,
		8 );

	
	//这里禁区检测暂时使用的是混合高斯模型，以后会修改成使用vibe进行前景提取
	initModule_video();
	Ptr<BackgroundSubtractorMOG2> fgbg = Algorithm::create<BackgroundSubtractorMOG2>("BackgroundSubtractor.MOG2");
	if (fgbg.empty())
	{
		std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
		return -1;
	}

	fgbg->set("history",50);//当目标运动速度比较慢时，可以增大此值来减少拖影
	//fgbg->set("nFrames",learningFrames);
	fgbg->set("nShadowDetection",0);//阴影检测，将检测到的阴影置0
	fgbg->set("backgroundRatio",0.01);


	while (true)
	{
		p_EventDetect->cap >> img;
		if (img.empty())
		{
			break;
		}
		Mat mon_img;
		img.copyTo(mon_img,bd_mask);
		(*fgbg)(mon_img,fgmask,-1);
		imshow("fgmask",fgmask);
		waitKey(20);
	}

	_endthreadex(0);
	return 0;
}

unsigned int WINAPI CVideo::retrogradeThread(void* param)
{
	CVideo* p_EventDetect = (CVideo*) param;
	string para_file;
	int m = p_EventDetect->needProcessId -1;
	if (p_EventDetect->video_mode == LOCAL_VIDEO)
	{
		switch (m)
		{
		case 0:
			para_file = para_T1;
			break;
		case 1:
			para_file = para_T2;
			break;
		case 2:
			para_file = para_T3;
			break;
		case 3:
			para_file = para_T4;
			break;
		default:
			break;
		}
	}
	else if (p_EventDetect->video_mode == CAMERA_VIDEO)
	{
		switch (m)
		{
		case 0:
			para_file = para_M1;
			break;
		case 1:
			para_file = para_M2;
			break;
		case 2:
			para_file = para_M3;
			break;
		case 3:
			para_file = para_M4;
			break;
		default:
			break;
		}
	}

	int direction = 0;
	int winSize = 0;
	int featuresNum = 0;
	int errorLevel = 0;
	int refreshRate = 0;
	Boundary bd;

	FileStorage fs(para_file, FileStorage::READ);
	FileNode fr = fs["RetrogradeInfo"];
	if (fr.empty())
	{
		//未设置
		return 2;
	}

	direction = (int)fr["direction"];
	winSize = (int)fr["winSize"];
	featuresNum = (int)fr["featuresNum"];
	errorLevel = (int)fr["errorLevel"];
	refreshRate = (int)fr["refreshRate"];

	FileNode fb = fs["MonitorZoonInfo"];
	if (fb.empty())
	{
		return 2;
		//未设置
	}
	bd.flag = (int)fb["BoundaryType"];
	fb["BoundaryPoints"] >> bd.point_boundray;
	fs.release();

	Rect roi_monitor;
	roi_monitor = boundingRect(bd.point_boundray);
	TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.03);
	Size subPixWinSize(10,10), win_Size(winSize,winSize);

	const int MAX_COUNT = featuresNum;
	bool needToInit = true;
	bool nightMode = false;
	int cnt = 0;
	Mat gray, prevGray,img,rgb;
	QImage qimg;
	DrawInfo df;
	vector<Point2f> points[2];
	while (1)
	{
		p_EventDetect->cap >> img;
		if (img.empty())
		{
			break;
		}
		Mat mon_img;
		mon_img = img(roi_monitor);
		cvtColor(mon_img,gray,COLOR_BGR2GRAY);
		if( needToInit )
		{
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			cornerSubPix(gray, points[1], subPixWinSize, Size(-1,-1), termcrit);
			needToInit = false;
			std::swap(points[1], points[0]);
		}

		if (!points[0].empty())
		{
			vector<uchar> status;
			vector<float> err;
			vector<Point> movePoints;
			if(prevGray.empty())
				gray.copyTo(prevGray);
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, win_Size,
				3, termcrit, 0, 0.001);

			int k=0;
			for (int i=0;i<points[1].size();i++)
			{
				if (status[i] == 0)
					continue;

				int x = points[1][i].x - points[0][i].x;
				int y = points[1][i].y - points[0][i].y;
				int fz = sqrt(x*x+y*y);
				double angle = 0;
				if (y>0 && x!=0)
				{
					angle = 270+atan(y/x)*(180.00/3.1415);
				}
				else if(y<0 && x!=0)
				{
					angle = 90+atan(y/x)*(180/3.1415);
				}
				else if (y==0 && x>0)
				{
					angle = 0;
				}
				else if (y==0 && x<0)
				{
					angle = 180;
				}
				else if (x ==0 && y>0)
				{
					angle = 270;
				}
				else if (x ==0 && y<0)
				{
					angle = 90;
				}
				
				int ag = abs(angle-direction);
				if (direction>=90 && direction<=270)
				{
					if (ag<90)
					{
						double f = fz*cos(ag*3.1415/180);
						if (f > errorLevel)
						{
							circle( mon_img, points[1][i], 3, Scalar(0,255,0), -1, 8);
						}
					}
				}
				else if (0<= direction && direction <90)
				{
					if (ag<90)
					{
						double f = fz*cos(ag*3.1415/180);
						if (f > errorLevel)
						{
							circle( mon_img, points[1][i], 3, Scalar(0,255,0), -1, 8);
						}
					}
					else if ((270-angle+direction)<0)
					{
						double f = fz*cos((270-angle+direction)*3.1415/180);
						if (f > errorLevel)
						{
							circle( mon_img, points[1][i], 3, Scalar(0,255,0), -1, 8);
						}
					}
				}
				else if (270<direction && direction<360)
				{
					if (ag<90)
					{
						double f = fz*cos(ag*3.1415/180);
						if (f > errorLevel)
						{
							circle( mon_img, points[1][i], 3, Scalar(0,255,0), -1, 8);
						}
					}
					else if (270-direction+angle<0)
					{

						double f = fz*cos((270-direction+angle)*3.1415/180);
						if (f > errorLevel)
						{
							circle( mon_img, points[1][i], 3, Scalar(0,255,0), -1, 8);
						}
					}

				}

			}
			//points[1].resize(k);
		}
		if (img.empty())
		{
			//cvtColor(no_picture,rgb,CV_BGR2RGB);
		}
		else
		{
			cvtColor(img,rgb,CV_BGR2RGB);
		}

		qimg =  QImage((uchar*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888);
		df.id = m;
		df.qimage = qimg;
		if (1)
		{
			emit p_EventDetect->sendDrawMess(df);
		}
		Sleep(33);
		if (cnt != 0)
		{
			std::swap(points[1], points[0]);
			cv::swap(prevGray, gray);
		}
		cnt++;
		if (cnt>refreshRate)
		{
			//points[0].clear();
			//points[1].clear();
			cnt = 0;
			needToInit = true;
		}
	}

	_endthreadex(0);
	return 0;
	
}

unsigned int WINAPI CVideo::crowdestimateThread(void* param)
{
	CVideo* p_EventDetect = (CVideo*) param;
	string para_file;
	int m = p_EventDetect->needProcessId -1;
	if (p_EventDetect->video_mode == LOCAL_VIDEO)
	{
		switch (m)
		{
		case 0:
			para_file = para_T1;
			break;
		case 1:
			para_file = para_T2;
			break;
		case 2:
			para_file = para_T3;
			break;
		case 3:
			para_file = para_T4;
			break;
		default:
			break;
		}
	}
	else if (p_EventDetect->video_mode == CAMERA_VIDEO)
	{
		switch (m)
		{
		case 0:
			para_file = para_M1;
			break;
		case 1:
			para_file = para_M2;
			break;
		case 2:
			para_file = para_M3;
			break;
		case 3:
			para_file = para_M4;
			break;
		default:
			break;
		}
	}

	Boundary bd;
	int bgType = 0;
	int densType = 0;
	int learningFrames = 0;
	int historyFrames = 0;

	FileStorage fs(para_file, FileStorage::READ);
	FileNode fr = fs["CrowdEstimateInfo"];
	if (fr.empty())
	{
		//未设置
		return 2;
	}
	bgType = (int)fr["BackgroundBuiltMode"];
	densType = (int)fr["densityClassifyType"];
	learningFrames = (int)fr["learningFrames"];
	historyFrames = (int)fr["historyFrames"];

	FileNode fb = fs["MonitorZoonInfo"];
	if (fb.empty())
	{
		return 2;
		//未设置
	}
	bd.flag = (int)fb["BoundaryType"];
	fb["BoundaryPoints"] >> bd.point_boundray;
	fs.release();

	double bg_area = 0;
	double fg_area = 0;

	if (bd.flag == BOUNDRY_POLYGON)
	{
		bg_area = contourArea(bd.point_boundray);
	}

	Rect roi_monitor;
	roi_monitor = boundingRect(bd.point_boundray);
	Mat img,frame,rgb,fgimg,fgmask,bgimg;
	QImage qimg;
	DrawInfo df;
	if (bgType == M_MOG2)
	{
		initModule_video();
		Ptr<BackgroundSubtractorMOG2> fgbg = Algorithm::create<BackgroundSubtractorMOG2>("BackgroundSubtractor.MOG2");
		if (fgbg.empty())
		{
			std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
			return -1;
		}

		fgbg->set("history",historyFrames);//当目标运动速度比较慢时，可以增大此值来减少拖影
		//fgbg->set("nFrames",learningFrames);
		fgbg->set("nShadowDetection",0);//阴影检测，将检测到的阴影置0
		fgbg->set("backgroundRatio",0.01);

		double fgbgRatio;
		for (;;)
		{
			p_EventDetect->cap >> img;
			if (img.empty())
				break;
			frame = img(roi_monitor);
			(*fgbg)(frame,fgmask,-1);
			fg_area = countNonZero(fgmask);
			fgbgRatio = fg_area/bg_area;
			int temp_ratio = 100*fgbgRatio;
			std::stringstream ss;
			std::string str_ratio;
			ss<< temp_ratio;
			ss>>str_ratio;
			str_ratio = str_ratio + '%';

			putText(img, str_ratio, Point(frame.cols-100, 40), FONT_HERSHEY_PLAIN, 3.0, CV_RGB(255,0,0), 2.0);
			if (img.empty())
			{
				//cvtColor(no_picture,rgb,CV_BGR2RGB);
			}
			else
			{
				cvtColor(img,rgb,CV_BGR2RGB);
			}

			qimg =  QImage((uchar*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888);
			df.id = m;
			df.qimage = qimg;
			if (1)
			{
				emit p_EventDetect->sendDrawMess(df);
			}
			Sleep(33);
		}

	}
	else if (bgType == OCL_MOG || bgType == OCL_MOG2)
	{
		cv::ocl::MOG  mog;
		cv::ocl::MOG2 mog2;
		oclMat d_frame;
		oclMat d_fgmask, d_fgimg, d_bgimg;
		//d_fgimg.create(d_frame.size(), d_frame.type());
		//mog(d_frame, d_fgmask, 0.01f);
		mog2.history = historyFrames;
		mog2.backgroundRatio = 0.01;
		double fgbgRatio;
		for (;;)
		{
			p_EventDetect->cap >> img;
			if (img.empty())
				break;
			frame = img(roi_monitor);
			d_frame.upload(frame);
			switch (bgType)
			{
			case OCL_MOG:
				mog(d_frame, d_fgmask, 0.01f);
				//mog.getBackgroundImage(d_bgimg);
				break;

			case OCL_MOG2:
				mog2(d_frame, d_fgmask);
				//mog2.getBackgroundImage(d_bgimg);
				break;
			}

			threshold(d_fgmask,d_fgmask,200,255,THRESH_BINARY);
			fg_area = countNonZero(d_fgmask);
			fgbgRatio = fg_area/bg_area;
			int temp_ratio = 100*fgbgRatio;
			std::stringstream ss;
			std::string str_ratio;
			ss<< temp_ratio;
			ss>>str_ratio;
			str_ratio = str_ratio + '%';

			putText(img, str_ratio, Point(frame.cols-100, 40), FONT_HERSHEY_PLAIN, 3.0, CV_RGB(255,0,0), 2.0);
			if (img.empty())
			{
				//cvtColor(no_picture,rgb,CV_BGR2RGB);
			}
			else
			{
				cvtColor(img,rgb,CV_BGR2RGB);
			}

			qimg =  QImage((uchar*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888);
			df.id = m;
			df.qimage = qimg;
			if (1)
			{
				emit p_EventDetect->sendDrawMess(df);
			}
			Sleep(33);
			
		}

	}


	_endthreadex(0);
	return 0;
}