
#include "smartmonitorsystem.h"
#include "HCNetSDK.h"
#include "CameraControl.h"
#include "Scene.h"
#include "ParaFile.h"
#include "TcpServer.h"
#include<QtCore>//Lcd显示时间增加

#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

using namespace std;



#ifndef PARA_SAVE_FILE
#define PARA_SAVE_FILE
//参数保存路径
string para_T1 = "Test_scene_1.yaml";
string para_T2 = "Test_scene_2.yaml";
string para_T3 = "Test_scene_3.yaml";
string para_T4 = "Test_scene_4.yaml";

string para_M1 = "Monitor_scene_1.yaml";
string para_M2 = "Monitor_scene_2.yaml";
string para_M3 = "Monitor_scene_3.yaml";
string para_M4 = "Monitor_scene_4.yaml";

string file_tcp = "tcpInfo.yaml";
#endif



SmartMonitorSystem::SmartMonitorSystem(QWidget *parent)
	: QMainWindow(parent)
{
	/*****************/
	//设置界面背景颜色
	QPalette palette;
	palette.setBrush(QPalette::Background, QBrush(QColor(54,210,255)));//红色
	setPalette(palette);
	/*****************/


	for (int i=0;i<SCENE_NUM;i++)
	{
		pCam[i] = new CameraControl;
		p_Video[i] = new CVideo();
	}

	if (fileIsNone(para_M1))
	{
		InitParaFile(para_M1);
	}
	if (fileIsNone(para_M2))
	{
		InitParaFile(para_M2);
	}
	if (fileIsNone(para_M3))
	{
		InitParaFile(para_M3);
	}
	if (fileIsNone(para_M4))
	{
		InitParaFile(para_M4);
	}
	if (fileIsNone(para_T1))
	{
		InitParaFile(para_T1);
	}
	if (fileIsNone(para_T2))
	{
		InitParaFile(para_T2);
	}
	if (fileIsNone(para_T3))
	{
		InitParaFile(para_T3);
	}
	if (fileIsNone(para_T4))
	{
		InitParaFile(para_T4);
	}

	if (fileIsNone(file_tcp))
	{
		InitParaFile(file_tcp);
	}

	setParaFlag = false;
	socket_server = new SocketServer;

	ui.setupUi(this);
	ptimer = new QTimer(this);
	connect(ptimer,SIGNAL(timeout()),this,SLOT(warningAlarm()));

	qRegisterMetaType<DrawInfo> ("DrawInfo");//自定义的符号,若希望在信号和槽中传递，则必须加上qRegisterMetaType
	qRegisterMetaType<Mat> ("Mat");

	connect(p_Video[0],SIGNAL(sendDrawMess(DrawInfo)),this,SLOT(getDrawMess(DrawInfo)));
	connect(p_Video[1],SIGNAL(sendDrawMess(DrawInfo)),this,SLOT(getDrawMess(DrawInfo)));
	connect(p_Video[2],SIGNAL(sendDrawMess(DrawInfo)),this,SLOT(getDrawMess(DrawInfo)));
	connect(p_Video[3],SIGNAL(sendDrawMess(DrawInfo)),this,SLOT(getDrawMess(DrawInfo)));

	connect(ui.pushButton_26,SIGNAL(clicked()),this,SLOT(preView()));
	connect(ui.pushButton_4,SIGNAL(clicked()),this,SLOT(openLocalVideo()));
	connect(ui.pushButton_5,SIGNAL(clicked()),this,SLOT(colseLocalVideo()));
	connect(ui.pushButton_6,SIGNAL(clicked()),this,SLOT(pauseORgoVideo()));
	connect(ui.pushButton_11,SIGNAL(clicked()),this,SLOT(saveTestInfo()));
	connect(ui.pushButton_10,SIGNAL(clicked()),this,SLOT(setZoon()));
	connect(ui.pushButton_27,SIGNAL(clicked()),this,SLOT(saveLogInfo()));
	connect(ui.pushButton_7,SIGNAL(clicked()),this,SLOT(saveZoon()));

	connect(ui.pushButton_3,SIGNAL(clicked()),this,SLOT(setParaRetrograde()));
	connect(ui.pushButton_24,SIGNAL(clicked()),this,SLOT(setParaRestricted()));
	connect(ui.pushButton_25,SIGNAL(clicked()),this,SLOT(setParaCrowdEstimate()));
	connect(ui.pushButton_28,SIGNAL(clicked()),this,SLOT(setParaAbnormal()));
	connect(ui.pushButton_36,SIGNAL(clicked()),this,SLOT(setTcpSocket()));

	connect(ui.pushButton_31,SIGNAL(clicked()),this,SLOT(doRetrograde()));
	connect(ui.pushButton_32,SIGNAL(clicked()),this,SLOT(doRestricted()));
	connect(ui.pushButton_33,SIGNAL(clicked()),this,SLOT(doCrowdEstimate()));
	connect(ui.pushButton_34,SIGNAL(clicked()),this,SLOT(doAbnormal()));

	connect(ui.pushButton_35,SIGNAL(clicked()),this,SLOT(resetAlarm()));
	connect(socket_server,SIGNAL(sendTcpInfo(ushort)),this,SLOT(getTcpInfo(ushort)));

	ui.radioButton->setChec ked(true);//初始状态为监控模式，监控画面以16×9显示
	workMode = MONITOR_MODE;
	show_scale = RESLUTION_16X9;

	alarmflag = false;
	selectflag = false;
	redImg.load("red.png");
	greenImg.load("green.png");
	whiteImg.load("white.png");
	ui.label_8->setPixmap(greenImg);
	ui.label_8->show();
	ptimer->start(300);
	InitializeSRWLock(&lock_rw);
	
	h_tcpThread = (HANDLE)_beginthreadex(0,0,&StartSocket,socket_server,0,0);


}

SmartMonitorSystem::~SmartMonitorSystem()//析构函数
{

}



//设置菜单控制项
void SmartMonitorSystem::OnSceneSet()//显示设置场景那一页
{
	ui.toolBox->setCurrentIndex(1);
}

void SmartMonitorSystem::OnCameraSet()//显示摄像头参数配置那一页
{
	ui.toolBox->setCurrentIndex(2);
}

void SmartMonitorSystem::OnCloudplatformSet()//显示云台控制那一页
{
	ui.toolBox->setCurrentIndex(3);
}

void SmartMonitorSystem::OnLoginSet()//登陆设置控制
{
	ui.toolBox->setCurrentIndex(0);
}

void SmartMonitorSystem::OnTestMode()//测试模式选择
{
	ui.toolBox->setCurrentIndex(4);
}

void SmartMonitorSystem::OnDetectType()//检测类型选择
{
	ui.toolBox->setCurrentIndex(5);
}


//注册设备，登陆，注销等
void SmartMonitorSystem::login()//登陆
{
	if (!ui.radioButton->isChecked())
	{
		workMode = TEST_MODE;
		ui.label->setText(QStringLiteral("请选择监控模式"));//在标签上显示时间
		ui.label->show();
		return;
	}

	show_scale = ui.comboBox_7->currentIndex();
	workMode = MONITOR_MODE;
	int scene_id = ui.comboBox_5->currentIndex(); //获取场景序号

	QString str=ui.lineEdit->text();//ip地址
	char* ch;
	QByteArray ba = str.toLatin1();    
	ch=ba.data();

	QString DevicePort=ui.lineEdit_2->text();//端口号
	char* th;
	QByteArray ta = DevicePort.toLatin1();  
	th=ta.data();
	int n=atoi(th);

	QString DeviceUser=ui.lineEdit_3->text();//用户名
	char* uh;
	QByteArray ua = DeviceUser.toLatin1();  
	uh=ua.data();

	QString DevicePassword=ui.lineEdit_4->text();//密码
	char* ph;
	QByteArray pa = DevicePassword.toLatin1();  
	ph=pa.data();

	pCam[scene_id]->loginCamera(ch,n,uh,ph);
	if (pCam[scene_id]->logState)
	{
		p_Video[scene_id]->video_mode = CAMERA_VIDEO;
		ui.label->setText(QStringLiteral("登陆成功"));//在标签上显示时间
		ui.label->show();
		return;
	}
	else
	{
		ui.label->setText(QStringLiteral("登陆失败"));//在标签上显示时间
		ui.label->show();
		return;
	}

}

void SmartMonitorSystem::logout()//注销
{
	if (!ui.radioButton->isChecked())
	{
		ui.label->setText(QStringLiteral("请选择监控模式"));
		return;
	}

	int scene_id = ui.comboBox_5->currentIndex();
	if (!pCam[scene_id]->logState)
	{
		//已经注销
		return;
	}
	else
	{
		pCam[scene_id]->logoutCamera();
		if (!pCam[scene_id]->logState)
		{
			p_Video[scene_id]->needProcessId = scene_id +1;
			p_Video[scene_id]->closePlay();
			ui.label->setText(QStringLiteral("注销成功"));//在标签上提示
			ui.label->show();
			return;
		}
	}
}


void SmartMonitorSystem::preView()
{
	if (workMode!=MONITOR_MODE)
		return;

	int showScale = ui.comboBox_10->currentIndex();
	switch (showScale)
	{
	case 0:
		show_scale = RESLUTION_16X9;
		break;
	case 1:
		show_scale = RESLUTION_4X3;
		break;
	}

	int scene_id =  ui.comboBox_5->currentIndex();
	p_Video[scene_id]->needProcessId = scene_id +1;

	string protoCAM = pCam[scene_id]->camProtocol;
	string usrMess = pCam[scene_id]->usrMess;
	string camMess = pCam[scene_id]->camMess;

	string VideoFrom = protoCAM + usrMess + camMess;

	if (pCam[scene_id]->logState)
	{
		p_Video[scene_id]->cap.open(VideoFrom);
		if (p_Video[scene_id]->cap.isOpened())
		{
			p_Video[scene_id]->videoHaveOpened = true;
			p_Video[scene_id]->needToDraw = true;
		}
		p_Video[scene_id]->startPlay();
		return;
	}
	else
	{
		ui.label->setText(QStringLiteral("设备未登陆"));//在标签上提示
		ui.label->show();
		return;
	}
}

void SmartMonitorSystem::saveLogInfo()
{
	if (workMode!=MONITOR_MODE)
		return;
	int scene_id =  ui.comboBox_5->currentIndex();
	string para_file;
	switch (scene_id)
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

	string usrInfo = pCam[scene_id]->usrMess;//admin:12345@10.1.34.9
	int m,a;
	m = usrInfo.find(":");
	a = usrInfo.find("@");
	string usrName,usrPassword,usrIP;
	usrName = usrInfo.substr(0,m);
	usrPassword = usrInfo.substr(m+1,a-m-1);
	usrIP = usrInfo.substr(a+1,sizeof(usrInfo)-a-1);

	string startwords = "usrInfo";
	string endwords = "endusrInfo";
	prepForModifyData(para_file,startwords,endwords);
	FileStorage fs(para_file,FileStorage::APPEND);

	fs <<"usrInfo" << "{";
	fs <<"usrName" << usrName;
	fs <<"usrPassword" << usrPassword;
	fs <<"usrIP" << usrIP ;
	fs <<"endusrInfo"<<0;
	fs << "}";
	//fs <<"LOG_INFO" << LOG_INFO_SET;
	fs.release();

}

void SmartMonitorSystem::getTcpInfo(ushort para)
{
	ui.label->setText("hello");//在标签上显示时间
	ui.label->show();

	//label_18,label_19,label_20,label_21分别对应场景1,2,3,4
	/************************************************************************/
	/*  参数para共占两个字节
	      比如 0x0000   0000      0000   0000
	           编号   检测内容  检测结果
	           检测内容对应：0001  人群密度
	                         0010  禁区检测*/               
	/************************************************************************/
	int sid = 0;
	int stype = 0;
	int svalue = 0;
	sid = para/1024;
	stype = (para & 0x0F00)>>8;
	svalue = para % 256;
	QString str = QString::number(svalue,10);
	if (stype == 1)
	{
		str = "CD:"+str; //人群密度标识
	}
	else if(stype == 2)
	{
		str = "FZ:"+str; //禁区检测标识
	}

	switch (sid)
	{
	case 1:
		ui.label_18->setText(str);
		ui.label_18->show();
		break;
	case 2:
		ui.label_19->setText(str);
		ui.label_19->show();
		break;
	case 3:
		ui.label_20->setText(str);
		ui.label_20->show();
		break;
	case 4:
		ui.label_21->setText(str);
		ui.label_21->show();
		break;
	default:
		break;
	}

}

void SmartMonitorSystem::getDrawMess(DrawInfo df)
{
	int id;
	QImage qimg,qimg_show;
	id = df.id;
	qimg  = df.qimage;

	if (setParaFlag)     //如果需要设置参数，则将当前帧保存，用以后面处理
	{
		setParaFlag = false;
		img_process[id] = QImage2Mat(qimg);
		return;
	}
	if (show_scale == RESLUTION_16X9)
	{
		qimg_show = qimg.scaled(QSize(WIDTH_SHOW,HEIGHT_SHOW_16X9),Qt::IgnoreAspectRatio);
	}
	else
	{
		qimg_show = qimg.scaled(QSize(WIDTH_SHOW,HEIGHT_SHOW_4X3),Qt::IgnoreAspectRatio);
	}
	//qimg_show = qimg.scaled()

	QPixmap pixImg;
	pixImg = pixImg.fromImage(qimg_show);

	//label_23,label_24,label_25,label_26分别对应场景1,2,3,4
	switch (id)
	{
	case 0:
		ui.label_23->setPixmap(pixImg);
		ui.label_23->show();
		break;
	case 1:
		ui.label_24->setPixmap(pixImg);
		ui.label_24->show();
		break;
	case 2:
		ui.label_25->setPixmap(pixImg);
		ui.label_25->show();
		break;
	case 3:
		ui.label_26->setPixmap(pixImg);
		ui.label_26->show();
		break;
	default:
		break;
	}

}



void SmartMonitorSystem::openLocalVideo()
{
	if (!ui.radioButton_2->isChecked())//判断是否为测试模式
	{
		workMode = MONITOR_MODE;
		ui.label->setText(QStringLiteral("请选择测试模式"));
		ui.label->show();
		return;
	}

	int showScale = ui.comboBox_10->currentIndex();//选择显示尺寸
	switch (showScale)
	{
	case 0:
		show_scale = RESLUTION_16X9;
		break;
	case 1:
		show_scale = RESLUTION_4X3;
		break;
	}

	workMode = TEST_MODE;
	int scene_id = ui.comboBox_6->currentIndex();
	int rate = ui.comboBox_9->currentIndex() + 1;//播放速度

	p_Video[scene_id]->needProcessId = scene_id+1;//待处理的场景序号

	QString file = QFileDialog::getOpenFileName(this, tr("open file"), " ", 
		tr("Allfile(*.*);;videofile1(*.avi);;videofile2(*.mp4);;videofile3(*.mpg);;videofile4(*.mov)")); 
	string VideoFrom = file.toStdString();
	p_Video[scene_id]->videoPath = VideoFrom;


	p_Video[scene_id]->cap.open(VideoFrom);
	if (p_Video[scene_id]->cap.isOpened())
	{
		p_Video[scene_id]->videoHaveOpened = true;
		p_Video[scene_id]->needToDraw = true;
		p_Video[scene_id]->playRate = rate;
		p_Video[scene_id]->video_mode = LOCAL_VIDEO;
		ui.label->setText(QStringLiteral("视频打开成功"));//
		ui.label->show();
		p_Video[scene_id]->startPlay();
		return;
	}
	else
	{
		ui.label->setText(QStringLiteral("视频打开失败"));
		ui.label->show();
		return;
	}


}

void SmartMonitorSystem::colseLocalVideo()
{
	if (!ui.radioButton_2->isChecked())
	{
		ui.label->setText(QStringLiteral("请选择测试模式"));//
		ui.label->show();
		return;
	}

	int scene_id = ui.comboBox_6->currentIndex();
	p_Video[scene_id]->needProcessId = scene_id +1;

	p_Video[scene_id]->closePlay();

	ui.label->setText(QStringLiteral("测试视频关闭"));
	ui.label->show();
	return;

}

void SmartMonitorSystem::pauseORgoVideo()
{
	if (!ui.radioButton_2->isChecked())
	{
		ui.label->setText(QStringLiteral("请选择测试模式"));
		ui.label->show();
		return;
	}

	int scene_id = ui.comboBox_6->currentIndex();
	p_Video[scene_id]->needProcessId = scene_id+1;

	p_Video[scene_id]->pausePlay();
	ui.label->setText(QStringLiteral("测试视频暂停"));
	ui.label->show();
	return;
}


void SmartMonitorSystem::saveTestInfo()
{
	if (!ui.radioButton_2->isChecked())
	{
		ui.label->setText(QStringLiteral("请选择测试模式"));
		ui.label->show();
		return;
	}
	int scene_id =  ui.comboBox_6->currentIndex();
	string para_file;
	switch (scene_id)
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

	string startwords = "LocalVideoInfo";
	string endwords = "endLocalVideoInfo";
	prepForModifyData(para_file,startwords,endwords);
	FileStorage	fs(para_file,FileStorage::APPEND);
	fs <<"LocalVideoInfo" << "{";
	fs <<"VideoPath" << p_Video[scene_id]->videoPath;
	fs <<"PlayRate" <<p_Video[scene_id]->playRate;
	fs <<"endLocalVideoInfo" <<0;
	fs << "}";
	fs.release();

}


void SmartMonitorSystem::setZoon()
{
	Mat img;
	int scene_id;


    scene_id = ui.comboBox_7->currentIndex();
	setParaFlag = true;

	Sleep(1000);
	if (!img_process[scene_id].empty())
	{
		img_process[scene_id].copyTo(img);
		ui.label->setText(QStringLiteral("获取图片成功"));//
		ui.label->show();
	}
	else
	{
		ui.label->setText(QStringLiteral("获取图片失败"));//
		ui.label->show();
		return;
	}
	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = false;
	p_Video[scene_id]->pauseVideoFlag = true;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);

	monitorZoon = setBoundary(img,BOUNDRY_POLYGON);
	setParaFlag = false;
	return;
}

void SmartMonitorSystem::saveZoon()
{
	int scene_id;
	string para_file;
	if (workMode == MONITOR_MODE)
	{
		scene_id =  ui.comboBox_7->currentIndex();
		switch (scene_id)
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
	else
	{
		scene_id =  ui.comboBox_7->currentIndex();
		switch (scene_id)
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

	string startwords = "MonitorZoonInfo";
	string endwords = "endMonitorZoonInfo";
	prepForModifyData(para_file,startwords,endwords);
	FileStorage fs(para_file,FileStorage::APPEND);
	fs <<"MonitorZoonInfo" << "{";
	fs <<"BoundaryType" << monitorZoon.flag;
	fs <<"BoundaryPoints" <<monitorZoon.point_boundray;
	fs << "endMonitorZoonInfo" <<0;
	fs << "}";
	fs.release();

	monitorZoon.flag = BOUNDARY_UNKNOWN;
	monitorZoon.point_boundray.clear();

	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = true;
	p_Video[scene_id]->pauseVideoFlag = false;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	
}

void SmartMonitorSystem::setParaRetrograde()
{
	
	int scene_id;

	if (workMode == TEST_MODE)
	{
		scene_id = ui.comboBox_6->currentIndex();
	}
	else
	{
		scene_id = ui.comboBox_5->currentIndex();
	}

	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = false;
	p_Video[scene_id]->pauseVideoFlag = true;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);

	DialogRetrograde =  new ParaRetrograde();
	DialogRetrograde->work_mode = workMode;
	DialogRetrograde->scene_id = scene_id;
	DialogRetrograde->initPara();
	DialogRetrograde->show();
	DialogRetrograde->exec();

	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = true;
	p_Video[scene_id]->pauseVideoFlag = false;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);

}
void SmartMonitorSystem::setParaCrowdEstimate()
{

	int scene_id;

	if (workMode == TEST_MODE)
	{
		scene_id = ui.comboBox_6->currentIndex();
	}
	else
	{
		scene_id = ui.comboBox_5->currentIndex();
	}
	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = false;
	p_Video[scene_id]->pauseVideoFlag = true;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);

	DialogCrowd =  new ParaCrowdEstimate();
	DialogCrowd->work_mode = workMode;
	DialogCrowd->scene_id = scene_id;

	DialogCrowd->show();
	DialogCrowd->exec();

	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = true;
	p_Video[scene_id]->pauseVideoFlag = false;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);

}
void SmartMonitorSystem::setParaRestricted()
{

	int scene_id;

	if (workMode == TEST_MODE)
	{
		scene_id = ui.comboBox_6->currentIndex();
	}
	else
	{
		scene_id = ui.comboBox_5->currentIndex();
	}
	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = false;
	p_Video[scene_id]->pauseVideoFlag = true;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);

	DialogRestricted = new ParaRestricted();
	DialogRestricted->work_mode = workMode;
	DialogRestricted->scene_id = scene_id;
	DialogRestricted->initPara();
	DialogRestricted->show();
	DialogRestricted->exec();


	AcquireSRWLockExclusive(&p_Video[scene_id]->rw_lock);
	p_Video[scene_id]->needToDraw = true;
	p_Video[scene_id]->pauseVideoFlag = false;
	ReleaseSRWLockExclusive(&p_Video[scene_id]->rw_lock);
}
void SmartMonitorSystem::setParaAbnormal()
{

}


void SmartMonitorSystem::doRetrograde()
{
	int scene_id;
	int dType = RETROGRADE_DETECT;

	if (workMode == TEST_MODE)
	{
		scene_id = ui.comboBox_6->currentIndex();
	}
	else
	{
		scene_id = ui.comboBox_5->currentIndex();
	}
	p_Video[scene_id]->needProcessId = scene_id +1;
	p_Video[scene_id]->testEventDetect(dType);
}

void SmartMonitorSystem::doCrowdEstimate()
{
	int scene_id;
	int dType = CROWD_DENSITY_DETECT;

	if (workMode == TEST_MODE)
	{
		scene_id = ui.comboBox_6->currentIndex();
	}
	else
	{
		scene_id = ui.comboBox_5->currentIndex();
	}
	p_Video[scene_id]->needProcessId = scene_id +1;
	p_Video[scene_id]->testEventDetect(dType);

}

void SmartMonitorSystem::doRestricted()
{
	int scene_id;
	int dType = RESTRICTED_ZOON_DETECT;

	if (workMode == TEST_MODE)
	{
		scene_id = ui.comboBox_6->currentIndex();
	}
	else
	{
		scene_id = ui.comboBox_5->currentIndex();
	}
	p_Video[scene_id]->needProcessId = scene_id +1;
	p_Video[scene_id]->testEventDetect(dType);
}

void SmartMonitorSystem::doAbnormal()
{

}

void SmartMonitorSystem::warningAlarm(void)
{
	AcquireSRWLockExclusive(&lock_rw);
	selectflag = !selectflag;
	ReleaseSRWLockExclusive(&lock_rw);
	if (alarmflag)
	{
		if (selectflag)
		{
			ui.label_8->setPixmap(redImg);
			ui.label_8->show();
		} 
		else
		{
			ui.label_8->setPixmap(whiteImg);
			ui.label_8->show();
		}
	} 
	else
	{
		if (selectflag)
		{
			ui.label_8->setPixmap(greenImg);
			ui.label_8->show();
		} 
		else
		{
			ui.label_8->setPixmap(whiteImg);
			ui.label_8->show();
		}
	}
}

void SmartMonitorSystem::resetAlarm()
{
	AcquireSRWLockExclusive(&lock_rw);
	alarmflag = false;
	PlaySound(NULL,NULL,NULL);
	ReleaseSRWLockExclusive(&lock_rw);
}

void SmartMonitorSystem::setTcpSocket()
{
	DialogTcpSocket = new TcpServer;
	DialogTcpSocket->initPara();
	DialogTcpSocket->show();
	DialogTcpSocket->exec();
}

bool SmartMonitorSystem::initSocketServer()
{
	return true;
}

bool SmartMonitorSystem::initSystem()
{
	return true;
}

void SmartMonitorSystem::quickStart()
{

}
