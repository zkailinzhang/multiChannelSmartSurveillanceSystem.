#ifndef _RETROGRADE_DIALOG_H_
#define _RETROGRADE_DIALOG_H_

#include <QtWidgets/QDialog>
#include <QPushButton>
#include "ui_Para_Retrograde_Set.h"
#include <string>
using namespace std;



class ParaRetrograde : public QDialog
{
	Q_OBJECT
public:
	ParaRetrograde();
	~ParaRetrograde();


	int work_mode;
	int scene_id;

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
#endif