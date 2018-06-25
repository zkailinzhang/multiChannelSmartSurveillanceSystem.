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


//����label �ؼ�
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


//���ֻ�ͼ�ؼ�
#include <QPaintEvent>  //��ͼ�¼�
#include <QTimer>  //��ʱ��
#include <QPainter>  
#include <QPixmap>   
#include <QImage>  //qtͼ�����



//�˵�����������
#include <QAction>  //QAction��
#include <QMenu>
#include <QMenuBar>  //�˵�����

//opencv����
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <process.h>

using namespace cv;
using namespace std;


#define  SCENE_NUM 4

//���ֹ���ģʽ
#define TEST_MODE 1
#define MONITOR_MODE 2

//��������״̬
#define LOG_INFO_SET        1
#define LOG_INFO_NO_SET     2

//��ʾ����
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
	ParaRetrograde* DialogRetrograde;//���м��������öԻ���
	ParaCrowdEstimate*  DialogCrowd;
	ParaRestricted* DialogRestricted;


	CameraControl* pCam[SCENE_NUM];//����ͷ����
	CVideo* p_Video[SCENE_NUM];//��Ƶ�źŴ���
	int workMode;//����ģʽ�����Ժͼ��
	bool setParaFlag;//���ñ߽������־λ�����ñ߽���������У���ȡһ֡ͼ�������趨

	Mat img_process[SCENE_NUM];
	QImage qimg_process;
	Boundary monitorZoon;

	int show_scale;//��ʾ������4x3,16x9

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

		//���ÿ��Ʋ˵�
		void OnSceneSet();//�������ÿ���
		void OnCameraSet();//����ͷ���ÿ���
		void OnCloudplatformSet();//��̨���ÿ���
		void OnLoginSet();//��½�������
		void OnTestMode();//����ģʽ
		void OnDetectType();//�������


		void getDrawMess(DrawInfo df);
		void getTcpInfo(ushort para);

		//�豸��½��Ϣ����
		//void OnButtonSetCompression();//����ѹ������
		void login();//��¼
		void logout();//ע��
		void preView();//Ԥ��
		void saveLogInfo();//�����½��Ϣ

		//����ģʽ�£��򿪱�����Ƶ
		void openLocalVideo();
		void colseLocalVideo();
		void pauseORgoVideo();
		void saveTestInfo();


		//������Ϣ������
		void setZoon();
		void saveZoon();


		//�����������
		void setParaRetrograde();
		void setParaCrowdEstimate();
		void setParaRestricted();
		void setParaAbnormal();

		//void selectDetectType();
		void doRetrograde();
		void doCrowdEstimate();
		void doRestricted();
		void doAbnormal();

		//��������
		void quickStart();

		//������Ϣ
		void warningAlarm(void);
		void resetAlarm();

		//��������������Ϣ
		void setTcpSocket();


	
};

#endif // SMARTMONITORSYSTEM_H
