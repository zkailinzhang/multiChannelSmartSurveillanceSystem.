#include "CrowdEstimateDialog.h"
#include "ParaFile.h"
#include <opencv2/core/core.hpp>

using namespace cv;

extern string para_T1 ;
extern string para_T2;
extern string para_T3;
extern string para_T4;

extern string para_M1;
extern string para_M2;
extern string para_M3;
extern string para_M4;


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