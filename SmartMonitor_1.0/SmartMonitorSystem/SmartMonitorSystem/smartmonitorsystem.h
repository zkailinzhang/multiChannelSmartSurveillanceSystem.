#ifndef SMARTMONITORSYSTEM_H
#define SMARTMONITORSYSTEM_H

#include <QtWidgets/QMainWindow>
#include "ui_smartmonitorsystem.h"
#include "ParaSetDialog.h"
#include "Windows.h"
#include "CameraControl.h" 
#include "videoProcess.h"
#include "Scene.h"
#include "TcpServer.h"
#include <string>


#include <QProcess>


//包含label 控件
#include <Qlabel>
#include <QLineEdit>
#include <QStringList>
#include <QFileDialog>
#include <QDialog>
#include <QCheckBox>
#include <QDesktopWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QDebug>
#include <QToolBox>


//各种绘图控件
#include <QPaintEvent>  //绘图事件
#include <QTimer>  //定时器
#include <QPainter>  
#include <QPixmap>   
#include <QImage>  //qt图像变量



//菜单栏部件声明
#include <QAction>  //QAction类
#include <QMenu>
#include <QMenuBar>  //菜单栏类

//opencv声明
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <process.h>

using namespace cv;
using namespace std;


#define  SCENE_NUM 4

//两种工作模式
#define TEST_MODE 1
#define MONITOR_MODE 2

//参数设置状态
#define LOG_INFO_SET        1
#define LOG_INFO_NO_SET     2

//显示比例
#define RESLUTION_16X9      1
#define RESLUTION_4X3       2
#define WIDTH_SHOW          480
#define HEIGHT_SHOW_4X3     320
#define HEIGHT_SHOW_16X9    270






class SmartMonitorSystem : public QMainWindow
{
	Q_OBJECT

public:
	SmartMonitorSystem(QWidget *parent = 0);
	~SmartMonitorSystem();

	SocketServer* socket_server;

	TcpServer* DialogTcpSocket;
	ParaRetrograde* DialogRetrograde;//逆行检测参数设置对话框
	ParaCrowdEstimate*  DialogCrowd;
	ParaRestricted* DialogRestricted;


	CameraControl* pCam[SCENE_NUM];//摄像头控制
	CVideo* p_Video[SCENE_NUM];//视频信号处理
	int workMode;//工作模式：测试和监控
	bool setParaFlag;//设置边界参数标志位，设置边界参数过程中，提取一帧图像用以设定

	Mat img_process[SCENE_NUM];
	QImage qimg_process;
	Boundary monitorZoon;

	int show_scale;//显示比例：4x3,16x9

	HANDLE h_tcpThread;
	
	SRWLOCK lock_rw;



private:
	Ui::SmartMonitorSystemClass ui;

	QPixmap redImg;
	QPixmap greenImg;
	QPixmap whiteImg;
	bool alarmflag;
	bool selectflag;

	QTimer *ptimer;

	bool initSocketServer();
	bool initSystem();

	private slots:

		//设置控制菜单
		void OnSceneSet();//场景设置控制
		void OnCameraSet();//摄像头设置控制
		void OnCloudplatformSet();//云台设置控制
		void OnLoginSet();//登陆输入控制
		void OnTestMode();//测试模式
		void OnDetectType();//检测类型


		void getDrawMess(DrawInfo df);
		void getTcpInfo(ushort para);

		//设备登陆信息设置
		//void OnButtonSetCompression();//设置压缩参数
		void login();//登录
		void logout();//注销
		void preView();//预览
		void saveLogInfo();//保存登陆信息

		//测试模式下，打开本地视频
		void openLocalVideo();
		void colseLocalVideo();
		void pauseORgoVideo();
		void saveTestInfo();


		//场景信息下设置
		void setZoon();
		void saveZoon();


		//检测类型设置
		void setParaRetrograde();
		void setParaCrowdEstimate();
		void setParaRestricted();
		void setParaAbnormal();

		//void selectDetectType();
		void doRetrograde();
		void doCrowdEstimate();
		void doRestricted();
		void doAbnormal();

		//快速启动
		void quickStart();

		//警报信息
		void warningAlarm(void);
		void resetAlarm();

		//设置网络连接信息
		void setTcpSocket();


	
};

#endif // SMARTMONITORSYSTEM_H
