#include "ParaSetDialog.h"

#include "ParaFile.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

extern string para_T1 ;
extern string para_T2;
extern string para_T3;
extern string para_T4;

extern string para_M1;
extern string para_M2;
extern string para_M3;
extern string para_M4;
extern string file_tcp;


/************************************************************************/
/* 设置人群密度检测的参数信息                                           */
/************************************************************************/
ParaCrowdEstimate::ParaCrowdEstimate()
{
	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(saveOK()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(saveCancel()));
	BackgroundBuiltMode = NO_SELEECTED;
	densityClassifyType = NO_SELEECTED;
	learningFrames = 5;
	historyFrames = 10;
}


ParaCrowdEstimate::~ParaCrowdEstimate()
{

}

void ParaCrowdEstimate::saveOK()
{

	QString temp_str;
	int temp;

	temp = ui.comboBox->currentIndex();
	switch (temp)
	{
	case 0:
		BackgroundBuiltMode = M_MOG;
		break;
	case 1:
		BackgroundBuiltMode = M_MOG2;
		break;
	case 2:
		BackgroundBuiltMode = OCL_MOG;
		break;
	case 3:
		BackgroundBuiltMode = OCL_MOG2;
		break;
	case 4:
		BackgroundBuiltMode = CUDA_MOG;
		break;
	case 5:
		BackgroundBuiltMode = CUDA_MOG2;
		break;
	case 6:
		BackgroundBuiltMode = M_GMG;
		break;
	default:
		break;
	}


	temp = ui.comboBox_2->currentIndex();
	switch (temp)
	{
	case 0:
		densityClassifyType = DENSITY_FOUR_LEVELS;
		break;
	case 1:
		densityClassifyType = DENSITY_PERCENTAGE;
		break;
	default:
		break;
	}

	temp_str = ui.comboBox_3->currentText();
	learningFrames = temp_str.toInt();

	temp_str = ui.comboBox_5->currentText();
	historyFrames = temp_str.toInt();

	string  filename;
	if (work_mode == 1)//测试模式
	{
		switch (scene_id)
		{
		case 0:
			filename = para_T1;
			break;
		case 1:
			filename = para_T2;
			break;
		case 2:
			filename = para_T3;
			break;
		case 3:
			filename = para_T4;
			break;
		}
	} 
	else
	{
		switch (scene_id)
		{
		case 0:
			filename = para_M1;
			break;
		case 1:
			filename = para_M2;
			break;
		case 2:
			filename = para_M3;
			break;
		case 3:
			filename = para_M4;
			break;
		}
	}
	string startwords = "CrowdEstimateInfo";
	string endwords = "endCrowdEstimateInfo";
	prepForModifyData(filename,startwords,endwords);
	FileStorage fs(filename,FileStorage::APPEND);

	fs <<"CrowdEstimateInfo" << "{";
	fs <<"BackgroundBuiltMode" << BackgroundBuiltMode;
	fs <<"densityClassifyType" << densityClassifyType;
	fs <<"learningFrames" << learningFrames;
	fs <<"historyFrames" << historyFrames;
	fs <<"endCrowdEstimateInfo"<<0;
	fs << "}";
	fs.release();

}

void ParaCrowdEstimate::saveCancel()
{
	close();
}

/************************************************************************/
/* 设置禁区检测信息参数                                                 */
/************************************************************************/


ParaRetrograde::ParaRetrograde()
{


	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(saveOK()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(saveCancel()));

}
ParaRetrograde::~ParaRetrograde()
{

}
void ParaRetrograde::initPara()
{
	string  filename;
	if (work_mode == 1)//测试模式
	{
		switch (scene_id)
		{
		case 0:
			filename = para_T1;
			break;
		case 1:
			filename = para_T2;
			break;
		case 2:
			filename = para_T3;
			break;
		case 3:
			filename = para_T4;
			break;
		}
	} 
	else
	{
		switch (scene_id)
		{
		case 0:
			filename = para_M1;
			break;
		case 1:
			filename = para_M2;
			break;
		case 2:
			filename = para_M3;
			break;
		case 3:
			filename = para_M4;
			break;
		}
	}

	FileStorage fs(filename, FileStorage::READ);
	FileNode fr = fs["RetrogradeInfo"];
	if (fr.empty())
	{
		//未设置
		return;
	}
	direction = (int)fr["direction"];
	winSize = (int)fr["winSize"];
	featuresNum = (int)fr["featuresNum"];
	errorLevel = (int)fr["errorLevel"];
	refreshRate = (int)fr["refreshRate"];
	fs.release();

	ui.lineEdit->setText(QString::number(direction));
	ui.lineEdit_2->setText(QString::number(winSize));
	ui.comboBox_2->setCurrentText(QString::number(featuresNum));
	ui.comboBox->setCurrentIndex(errorLevel/2 - 1);
	ui.comboBox_3->setCurrentText(QString::number(refreshRate));

}

void ParaRetrograde::saveOK()
{
	QString temp_str;
	temp_str  = ui.lineEdit->text();
	direction = temp_str.toInt();

	temp_str  = ui.lineEdit_2->text();
	winSize = temp_str.toInt();

	temp_str = ui.comboBox_2->currentText();
	featuresNum = temp_str.toInt();

	temp_str = ui.comboBox_3->currentText();
	refreshRate = temp_str.toInt();

	errorLevel = (ui.comboBox->currentIndex()+1)*2;

	string  filename;
	if (work_mode == 1)//测试模式
	{
		switch (scene_id)
		{
		case 0:
			filename = para_T1;
			break;
		case 1:
			filename = para_T2;
			break;
		case 2:
			filename = para_T3;
			break;
		case 3:
			filename = para_T4;
			break;
		}
	} 
	else
	{
		switch (scene_id)
		{
		case 0:
			filename = para_M1;
			break;
		case 1:
			filename = para_M2;
			break;
		case 2:
			filename = para_M3;
			break;
		case 3:
			filename = para_M4;
			break;
		}
	}

	string startwords = "RetrogradeInfo";
	string endwords = "endRetrogradeInfo";
	prepForModifyData(filename,startwords,endwords);
	FileStorage fs(filename,FileStorage::APPEND);
	fs <<"RetrogradeInfo" << "{";
	fs <<"direction" << direction;
	fs <<"winSize" << winSize;
	fs <<"featuresNum" << featuresNum;
	fs <<"errorLevel" << errorLevel;
	fs <<"refreshRate" << refreshRate;
	fs <<"endRetrogradeInfo"<<0;
	fs << "}";
	fs.release();

}

void ParaRetrograde::saveCancel()
{
	return;
}

/************************************************************************/
/* 设置禁区检测参数                                                                     */
/************************************************************************/
ParaRestricted::ParaRestricted()
{
	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(saveOK()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(saveCancel()));
}

ParaRestricted::~ParaRestricted()
{

}

void ParaRestricted::initPara()
{
	string  filename;
	if (work_mode == 1)//测试模式
	{
		switch (scene_id)
		{
		case 0:
			filename = para_T1;
			break;
		case 1:
			filename = para_T2;
			break;
		case 2:
			filename = para_T3;
			break;
		case 3:
			filename = para_T4;
			break;
		}
	} 
	else
	{
		switch (scene_id)
		{
		case 0:
			filename = para_M1;
			break;
		case 1:
			filename = para_M2;
			break;
		case 2:
			filename = para_M3;
			break;
		case 3:
			filename = para_M4;
			break;
		}
	}

	FileStorage fs(filename, FileStorage::READ);
	FileNode fr = fs["RestrictedZoonInfo"];
	if (fr.empty())
	{
		//未设置
		return;
	}
	objArea = (int)fr["objArea"];
	objType = (int)fr["objType"];

	fs.release();
	ui.comboBox->setCurrentText(QString::number(objArea));
	ui.comboBox_2->setCurrentText(QString::number(objType));

}

void ParaRestricted::saveOK()
{
	string  filename;
	if (work_mode == 1)//测试模式
	{
		switch (scene_id)
		{
		case 0:
			filename = para_T1;
			break;
		case 1:
			filename = para_T2;
			break;
		case 2:
			filename = para_T3;
			break;
		case 3:
			filename = para_T4;
			break;
		}
	} 
	else
	{
		switch (scene_id)
		{
		case 0:
			filename = para_M1;
			break;
		case 1:
			filename = para_M2;
			break;
		case 2:
			filename = para_M3;
			break;
		case 3:
			filename = para_M4;
			break;
		}
	}

	QString temp_str;
	temp_str = ui.comboBox->currentText();
	objArea = temp_str.toInt();

	temp_str = ui.comboBox_2->currentText();
	objType = temp_str.toInt();

	string startwords = "RestrictedZoonInfo";
	string endwords = "endRestrictedZoonInfo";
	prepForModifyData(filename,startwords,endwords);
	FileStorage fs(filename,FileStorage::APPEND);
	fs <<"RestrictedZoonInfo" << "{";
	fs <<"objArea" << objArea;
	fs <<"objType" << objType;
	fs <<"endRestrictedZoonInfo"<<0;
	fs << "}";
	fs.release();

}

void ParaRestricted::saveCancel()
{
	return;
}

/************************************************************************/
/* 设置通信端口信息 */
/************************************************************************/

TcpServer::TcpServer()
{
	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(saveOK()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(saveCancel()));
}

TcpServer::~TcpServer()
{
}

void TcpServer::initPara()
{
	string filename = file_tcp;
	FileStorage fs(filename, FileStorage::READ);
	FileNode fr = fs["TcpSocketInfo"];
	if (fr.empty())
	{
		//未设置
		return;
	}
	serverIp = (string)fr["ServerIp"];
	clientIp[0] = (string)fr["client_1_ip"];
	clientIp[1] = (string)fr["client_2_ip"];
	clientIp[2] = (string)fr["client_3_ip"];
	clientIp[3] = (string)fr["client_4_ip"];
	portID = (int)fr["PORT"];
	max_recv = (int)fr["MaxRecv"];
	fs.release();

	ui.lineEdit->setText(QString::fromStdString(serverIp));
	ui.lineEdit_2->setText(QString::fromStdString(clientIp[0]));
	ui.lineEdit_3->setText(QString::fromStdString(clientIp[1]));
	ui.lineEdit_4->setText(QString::fromStdString(clientIp[2]));
	ui.lineEdit_5->setText(QString::fromStdString(clientIp[3]));
	ui.lineEdit_6->setText(QString::number(max_recv));
	ui.lineEdit_7->setText(QString::number(portID));
}

void TcpServer::saveOK()
{
	
	QString temp_str;
	temp_str  = ui.lineEdit->text();
	serverIp = temp_str.toStdString();

	temp_str = ui.lineEdit_2->text();
	clientIp[0] = temp_str.toStdString();

	temp_str = ui.lineEdit_3->text();
	clientIp[1] = temp_str.toStdString();

	temp_str = ui.lineEdit_4->text();
	clientIp[2] = temp_str.toStdString();

	temp_str = ui.lineEdit_5->text();
	clientIp[3] = temp_str.toStdString();

	temp_str = ui.lineEdit_6->text();
	max_recv = temp_str.toInt();

	temp_str = ui.lineEdit_7->text();
	portID = temp_str.toInt();
	string filename = file_tcp;

	string startwords = "TcpSocketInfo";
	string endwords = "endTcpSocketInfo";
	prepForModifyData(filename,startwords,endwords);
	FileStorage fs(filename,FileStorage::APPEND);
	fs <<"TcpSocketInfo" << "{";
	fs <<"ServerIP" << serverIp;
	fs <<"client_1_ip" << clientIp[0];
	fs <<"client_2_ip" << clientIp[1];
	fs <<"client_3_ip" << clientIp[2];
	fs <<"client_4_ip" << clientIp[3];
	fs <<"PORT" << portID;
	fs << "MaxRecv" << max_recv;
	fs <<"endTcpSocketInfo"<<0;
	fs << "}";
	fs.release();
	

}

void TcpServer::saveCancel()
{
	return;
}

