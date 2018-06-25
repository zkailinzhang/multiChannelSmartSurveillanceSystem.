#ifndef _PARA_SET_DIALOG_H
#define _PARA_SET_DIALOG_H


#include "ui_Para_Retrograde_Set.h"
#include "ui_Para_CrowdEstimate_Set.h"
#include "ui_Para_Restricted_Set.h"
#include "ui_TcpCMC.h"
#include "Scene.h"


#include <QtWidgets/QDialog>
#include <QPushButton>

#include <Windows.h>
#include <iostream>
#include <string>

#pragma comment(lib,"ws2_32.lib")

using namespace std;


//opencv中有其中方法做混合高斯背景建模，主要基于3篇文章中的算法
//本系统中只编写其中两种，OCL_MOG2适用于带有显卡的电脑端，M_MOG2适用于一切设备，包括ARM
#define NO_SELEECTED     0
#define M_MOG            1
#define M_MOG2           2
#define OCL_MOG          3
#define OCL_MOG2         4
#define CUDA_MOG         5
#define CUDA_MOG2        6
#define M_GMG            7


#define DENSITY_FOUR_LEVELS 1
#define DENSITY_PERCENTAGE  2

class ParaCrowdEstimate:public QDialog
{
	Q_OBJECT
public:
	ParaCrowdEstimate();
	~ParaCrowdEstimate();

	int scene_id;
	int work_mode;

private:

	int BackgroundBuiltMode;
	int densityClassifyType;
	int learningFrames;
	int historyFrames;
	//double bgratio;
	//bool bShadowDetection;


private:
	Ui::CrowdEstimate ui;

	private slots:
		void saveOK();
		void saveCancel();


};


class ParaRetrograde : public QDialog
{
	Q_OBJECT
public:
	ParaRetrograde();
	~ParaRetrograde();


	int work_mode;
	int scene_id;
	void initPara();
private:
	int direction;
	int winSize;
	int featuresNum;
	int refreshRate;
	int errorLevel;

	Ui::ParaRetrograde ui;

	private slots:
		void saveOK();
		void saveCancel();


};

enum 
{
	Restricted_ALL = 1,
	Restricted_People = 2,
	Restricted_Animal = 3
};

class ParaRestricted : public QDialog
{
	Q_OBJECT
public:
	ParaRestricted();
	~ParaRestricted();


	int work_mode;
	int scene_id;
	void initPara();
private:

	int objArea;
	int objType;

	Ui::ParaRestricted ui;

	private slots:
		void saveOK();
		void saveCancel();


};

class TcpServer: public QDialog
{
	Q_OBJECT
public:
	TcpServer();
	~TcpServer();
	void initPara();
private:
	
	int max_recv;
	int portID;
	string serverIp;
	string clientIp[4];

	Ui::TcpServer ui;

	private slots:
		void saveOK();
		void saveCancel();

};

#endif

