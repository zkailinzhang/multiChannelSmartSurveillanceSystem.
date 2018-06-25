#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <QtWidgets/QWidget>
#include <windows.h>
#include <process.h>
#include <string>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

#include "ParaSetDialog.h"
using namespace cv;
using namespace std;

#ifndef CAM_NUM
#define CAM_NUM 4
#endif

//视频来源
#define UNKNOWN_VIDEO 0
#define LOCAL_VIDEO 1
#define CAMERA_VIDEO 2

//检测类型配置
#define NOTHING_DETECT 0
#define CROWD_DENSITY_DETECT 1
#define RETROGRADE_DETECT 2
#define RESTRICTED_ZOON_DETECT 4
#define ABNORMAL_DETECT 8



struct DrawInfo 
{
	int id;
	QImage qimage;
};


//Mat与QImage相互转换
QImage Mat2QImage(Mat& mat);
Mat QImage2Mat(QImage& qimage);


class CVideo : public QObject
{
	Q_OBJECT
public:
	CVideo();
	~CVideo();

	VideoCapture cap;
	bool videoHaveOpened; //视频已打开标志
	bool needToDraw;      //是否需要绘制图像
	int typeDetection;    //检测类型
	bool pauseVideoFlag;       //视频暂停标志
	bool closeVideoFlag; 
	bool DoEventDetect;

	Mat img_v;

	int playRate;
	string videoPath;//测试模式下

	int video_mode;//视频模式，本地视频或是监控视频
	bool doSmartProcess;//是否进行智能化处理


	SRWLOCK rw_lock;
	Mat no_picture;

	HANDLE h_showThread;
	HANDLE h_retrograde;
	HANDLE h_crowdestimate;
	HANDLE h_restrictedzoon;

	int needProcessId;
	bool closePlay();
	bool startPlay();
	void pausePlay();

	void testEventDetect(int dType);//测试检测类型

	static unsigned int WINAPI VideoPlayThread(void* param);
	static unsigned int WINAPI retrogradeThread(void* param);//逆行检测
	static unsigned int WINAPI crowdestimateThread(void* param);//人群密度检测
	static unsigned int WINAPI restrictedzoonThread(void* param);//禁区检测
	void setVideoState(int id,int stateType,int value);
signals:
	void sendDrawMess(DrawInfo df);
	void sendMatImg(Mat img);

	private slots:

private:

};


#endif