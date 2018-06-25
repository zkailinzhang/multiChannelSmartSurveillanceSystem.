
#include "smartmonitorsystem.h"
#include "HCNetSDK.h"
#include "CameraControl.h"
#include "Scene.h"
#include "ParaFile.h"
#include "TcpServer.h"
#include<QtCore>//Lcd��ʾʱ������

#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

using namespace std;



#ifndef PARA_SAVE_FILE
#define PARA_SAVE_FILE
//��������·��
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
	//���ý��汳����ɫ
	QPalette palette;
	palette.setBrush(QPalette::Background, QBrush(QColor(54,210,255)));//��ɫ
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

	qRegisterMetaType<DrawInfo> ("DrawInfo");//�Զ���ķ���,��ϣ�����źźͲ��д��ݣ���������qRegisterMetaType
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

	ui.radioButton->setChec ked(true);//��ʼ״̬Ϊ���ģʽ����ػ�����16��9��ʾ
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

SmartMonitorSystem::~SmartMonitorSystem()//��������
{

}



//���ò˵�������
void SmartMonitorSystem::OnSceneSet()//��ʾ���ó�����һҳ
{
	ui.toolBox->setCurrentIndex(1);
}

void SmartMonitorSystem::OnCameraSet()//��ʾ����ͷ����������һҳ
{
	ui.toolBox->setCurrentIndex(2);
}

void SmartMonitorSystem::OnCloudplatformSet()//��ʾ��̨������һҳ
{
	ui.toolBox->setCurrentIndex(3);
}

void SmartMonitorSystem::OnLoginSet()//��½���ÿ���
{
	ui.toolBox->setCurrentIndex(0);
}

void SmartMonitorSystem::OnTestMode()//����ģʽѡ��
{
	ui.toolBox->setCurrentIndex(4);
}

void SmartMonitorSystem::OnDetectType()//�������ѡ��
{
	ui.toolBox->setCurrentIndex(5);
}


//ע���豸����½��ע����
void SmartMonitorSystem::login()//��½
{
	if (!ui.radioButton->isChecked())
	{
		workMode = TEST_MODE;
		ui.label->setText(QStringLiteral("��ѡ����ģʽ"));//�ڱ�ǩ����ʾʱ��
		ui.label->show();
		return;
	}

	show_scale = ui.comboBox_7->currentIndex();
	workMode = MONITOR_MODE;
	int scene_id = ui.comboBox_5->currentIndex(); //��ȡ�������

	QString str=ui.lineEdit->text();//ip��ַ
	char* ch;
	QByteArray ba = str.toLatin1();    
	ch=ba.data();

	QString DevicePort=ui.lineEdit_2->text();//�˿ں�
	char* th;
	QByteArray ta = DevicePort.toLatin1();  
	th=ta.data();
	int n=atoi(th);

	QString DeviceUser=ui.lineEdit_3->text();//�û���
	char* uh;
	QByteArray ua = DeviceUser.toLatin1();  
	uh=ua.data();

	QString DevicePassword=ui.lineEdit_4->text();//����
	char* ph;
	QByteArray pa = DevicePassword.toLatin1();  
	ph=pa.data();

	pCam[scene_id]->loginCamera(ch,n,uh,ph);
	if (pCam[scene_id]->logState)
	{
		p_Video[scene_id]->video_mode = CAMERA_VIDEO;
		ui.label->setText(QStringLiteral("��½�ɹ�"));//�ڱ�ǩ����ʾʱ��
		ui.label->show();
		return;
	}
	else
	{
		ui.label->setText(QStringLiteral("��½ʧ��"));//�ڱ�ǩ����ʾʱ��
		ui.label->show();
		return;
	}

}

void SmartMonitorSystem::logout()//ע��
{
	if (!ui.radioButton->isChecked())
	{
		ui.label->setText(QStringLiteral("��ѡ����ģʽ"));
		return;
	}

	int scene_id = ui.comboBox_5->currentIndex();
	if (!pCam[scene_id]->logState)
	{
		//�Ѿ�ע��
		return;
	}
	else
	{
		pCam[scene_id]->logoutCamera();
		if (!pCam[scene_id]->logState)
		{
			p_Video[scene_id]->needProcessId = scene_id +1;
			p_Video[scene_id]->closePlay();
			ui.label->setText(QStringLiteral("ע���ɹ�"));//�ڱ�ǩ����ʾ
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
		ui.label->setText(QStringLiteral("�豸δ��½"));//�ڱ�ǩ����ʾ
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
	ui.label->setText("hello");//�ڱ�ǩ����ʾʱ��
	ui.label->show();

	//label_18,label_19,label_20,label_21�ֱ��Ӧ����1,2,3,4
	/************************************************************************/
	/*  ����para��ռ�����ֽ�
	      ���� 0x0000   0000      0000   0000
	           ���   �������  �����
	           ������ݶ�Ӧ��0001  ��Ⱥ�ܶ�
	                         0010  �������*/               
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
		str = "CD:"+str; //��Ⱥ�ܶȱ�ʶ
	}
	else if(stype == 2)
	{
		str = "FZ:"+str; //��������ʶ
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

	if (setParaFlag)     //�����Ҫ���ò������򽫵�ǰ֡���棬���Ժ��洦��
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

	//label_23,label_24,label_25,label_26�ֱ��Ӧ����1,2,3,4
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
	if (!ui.radioButton_2->isChecked())//�ж��Ƿ�Ϊ����ģʽ
	{
		workMode = MONITOR_MODE;
		ui.label->setText(QStringLiteral("��ѡ�����ģʽ"));
		ui.label->show();
		return;
	}

	int showScale = ui.comboBox_10->currentIndex();//ѡ����ʾ�ߴ�
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
	int rate = ui.comboBox_9->currentIndex() + 1;//�����ٶ�

	p_Video[scene_id]->needProcessId = scene_id+1;//������ĳ������

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
		ui.label->setText(QStringLiteral("��Ƶ�򿪳ɹ�"));//
		ui.label->show();
		p_Video[scene_id]->startPlay();
		return;
	}
	else
	{
		ui.label->setText(QStringLiteral("��Ƶ��ʧ��"));
		ui.label->show();
		return;
	}


}

void SmartMonitorSystem::colseLocalVideo()
{
	if (!ui.radioButton_2->isChecked())
	{
		ui.label->setText(QStringLiteral("��ѡ�����ģʽ"));//
		ui.label->show();
		return;
	}

	int scene_id = ui.comboBox_6->currentIndex();
	p_Video[scene_id]->needProcessId = scene_id +1;

	p_Video[scene_id]->closePlay();

	ui.label->setText(QStringLiteral("������Ƶ�ر�"));
	ui.label->show();
	return;

}

void SmartMonitorSystem::pauseORgoVideo()
{
	if (!ui.radioButton_2->isChecked())
	{
		ui.label->setText(QStringLiteral("��ѡ�����ģʽ"));
		ui.label->show();
		return;
	}

	int scene_id = ui.comboBox_6->currentIndex();
	p_Video[scene_id]->needProcessId = scene_id+1;

	p_Video[scene_id]->pausePlay();
	ui.label->setText(QStringLiteral("������Ƶ��ͣ"));
	ui.label->show();
	return;
}


void SmartMonitorSystem::saveTestInfo()
{
	if (!ui.radioButton_2->isChecked())
	{
		ui.label->setText(QStringLiteral("��ѡ�����ģʽ"));
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
		ui.label->setText(QStringLiteral("��ȡͼƬ�ɹ�"));//
		ui.label->show();
	}
	else
	{
		ui.label->setText(QStringLiteral("��ȡͼƬʧ��"));//
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
