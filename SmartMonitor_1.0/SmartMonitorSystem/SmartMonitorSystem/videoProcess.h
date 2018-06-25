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

//��Ƶ��Դ
#define UNKNOWN_VIDEO 0
#define LOCAL_VIDEO 1
#define CAMERA_VIDEO 2

//�����������
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


//Mat��QImage�໥ת��
QImage Mat2QImage(Mat& mat);
Mat QImage2Mat(QImage& qimage);


class CVideo : public QObject
{
	Q_OBJECT
public:
	CVideo();
	~CVideo();

	VideoCapture cap;
	bool videoHaveOpened; //��Ƶ�Ѵ򿪱�־
	bool needToDraw;      //�Ƿ���Ҫ����ͼ��
	int typeDetection;    //�������
	bool pauseVideoFlag;       //��Ƶ��ͣ��־
	bool closeVideoFlag; 
	bool DoEventDetect;

	Mat img_v;

	int playRate;
	string videoPath;//����ģʽ��

	int video_mode;//��Ƶģʽ��������Ƶ���Ǽ����Ƶ
	bool doSmartProcess;//�Ƿ�������ܻ�����


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

	void testEventDetect(int dType);//���Լ������

	static unsigned int WINAPI VideoPlayThread(void* param);
	static unsigned int WINAPI retrogradeThread(void* param);//���м��
	static unsigned int WINAPI crowdestimateThread(void* param);//��Ⱥ�ܶȼ��
	static unsigned int WINAPI restrictedzoonThread(void* param);//�������
	void setVideoState(int id,int stateType,int value);
signals:
	void sendDrawMess(DrawInfo df);
	void sendMatImg(Mat img);

	private slots:

private:

};


#endif