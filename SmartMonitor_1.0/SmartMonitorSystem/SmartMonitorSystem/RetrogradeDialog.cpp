#include "RetrogradeDialog.h"
#include "ParaFile.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

extern string para_T1 ;
extern string para_T2;
extern string para_T3;
extern string para_T4;

extern string para_M1;
extern string para_M2;
extern string para_M3;
extern string para_M4;



ParaRetrograde::ParaRetrograde()
{
	

	ui.setupUi(this);
	connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(saveOK()));
	connect(ui.pushButton_2,SIGNAL(clicked()),this,SLOT(saveCancel()));

	direction = 0;
	winSize = 21;
	featuresNum = 100;
	refreshRate = 5;
	errorLevel = 1;


}
ParaRetrograde::~ParaRetrograde()
{
	
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
	if (work_mode == 1)//≤‚ ‘ƒ£ Ω
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
	
}