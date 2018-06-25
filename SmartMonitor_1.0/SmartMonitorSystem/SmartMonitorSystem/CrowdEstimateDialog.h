#ifndef CROWD_ESTIMATE_DIALOG_H
#define CROWD_ESTIMATE_DIALOG_H

#include "ui_Para_CrowdEstimate_Set.h"
#include <QtWidgets/QDialog>
#include <QPushButton>
#include <string>
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

#endif