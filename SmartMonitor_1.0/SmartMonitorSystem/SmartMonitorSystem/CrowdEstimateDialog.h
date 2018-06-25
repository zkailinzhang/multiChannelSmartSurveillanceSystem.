#ifndef CROWD_ESTIMATE_DIALOG_H
#define CROWD_ESTIMATE_DIALOG_H

#include "ui_Para_CrowdEstimate_Set.h"
#include <QtWidgets/QDialog>
#include <QPushButton>
#include <string>
using namespace std;

//opencv�������з�������ϸ�˹������ģ����Ҫ����3ƪ�����е��㷨
//��ϵͳ��ֻ��д�������֣�OCL_MOG2�����ڴ����Կ��ĵ��Զˣ�M_MOG2������һ���豸������ARM
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