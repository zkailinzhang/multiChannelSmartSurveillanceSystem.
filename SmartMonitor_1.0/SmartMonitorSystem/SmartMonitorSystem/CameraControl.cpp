#include "CameraControl.h"

CameraControl::CameraControl()
{

	cameraID = -1;
	logState = false;
	camMess = CAM_MESS;
	camProtocol = CAM_PROTOCOL;


}

CameraControl::~CameraControl()
{
}



bool CameraControl::loginCamera(char *cIP, WORD cPortNum, char *cUserName, char *cPassword)
{
	if (logState)
	{
		//发送弹窗“该设备已经登陆”
		return true;
	}
	else
	{
		LONG usrID;
		NET_DVR_Init(); 
		NET_DVR_SetConnectTime(2000, 1); 
		NET_DVR_SetReconnect(10000, true); 
		NET_DVR_DEVICEINFO_V30 struDeviceInfo1;
		usrID = NET_DVR_Login_V30(cIP,cPortNum,cUserName,cPassword, &struDeviceInfo1);

		if (usrID<0)
		{
			NET_DVR_Cleanup();
			return false;
		}
		else
		{
			cameraID = usrID;
			logState = true;//admin:12345@10.1.34.9

			string a = cUserName;
			string b = cPassword;
			string c = cIP;
			usrMess = a + ":"+ b +"@"+ c;
			return true;
		}


	}
}

void CameraControl::logoutCamera()
{
	if (logState)
	{
		NET_DVR_Logout(cameraID);
		//释放SDK资源
		NET_DVR_Cleanup();
		logState = false;
	}
}

