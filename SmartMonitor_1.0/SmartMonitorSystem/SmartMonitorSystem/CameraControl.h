#ifndef _CAMERA_CONTROL_H_
#define _CAMERA_CONTROL_H_

#include "HCNetSDK.h"
#include <windows.h>
#include <string>

using namespace std;


#define CAM_MESS ":554/h264/ch1/main/av_stream"
#define CAM_PROTOCOL "rtsp://"

class CameraControl
{
public:
	CameraControl();
	~CameraControl();

	int scene_id;//�����������
	LONG cameraID;//SDK��½����ͷ���ؾ��
	bool logState;

	string usrMess;
	string camMess;
	string camProtocol;

	bool loginCamera(char *cIP, WORD cPortNum, char *cUserName, char *cPassword);
	void logoutCamera();


private:

};



#endif

