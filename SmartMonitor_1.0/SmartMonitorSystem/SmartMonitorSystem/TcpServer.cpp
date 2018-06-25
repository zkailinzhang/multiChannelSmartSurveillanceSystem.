#include "TcpServer.h"
#include <iostream>

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

/************************************************************************/
/* ARM端负责检测一些简单的功能，比如禁区检测，人群密度估计，在ARM与电脑端的
   通信中，按照以下规则进行通信：
   1.ARM端为客户端，电脑端为服务端
   2.ARM每次发送两个字节的内容，内容按如下格式编码：
     (1)高8位存放场景编号和检测内容，低8位存放检测结果
	 比如 0x0000   0000      0000   0000
	        编号   检测内容  检测结果
      检测内容对应：0001  人群密度
	                0010  禁区检测*/
/************************************************************************/

DWORD WINAPI RecvThread(LPVOID lpParameter)
{
	SocketServer* p_server = (SocketServer*)lpParameter;
	int Ret = 0;
	char RecvBuffer[MAX_PATH];
	while (true)
	{
		memset(RecvBuffer,0x00,sizeof(RecvBuffer));
		Ret = recv(p_server->ClientSocket,RecvBuffer,MAX_PATH,0);
		if (Ret==0 || Ret == SOCKET_ERROR)
		{
			break;
		}
		ushort a = RecvBuffer[0]+256*RecvBuffer[1];
		emit p_server->sendTcpInfo(a);
	}

	return 0;
}

SocketServer::SocketServer()
{
	initFlag = false;
}

SocketServer::~SocketServer()
{
}



void SocketServer::InitTcpServer()
{
	int Ret = 0;
	int AddrLen = 0;
	HANDLE hThread;
	FileStorage fs(file_tcp, FileStorage::READ);
	FileNode fr = fs["TcpSocketInfo"];
	if (fr.empty())
	{
		//未设置
		return;
	}
	server_ip = (string)fr["ServerIP"];
	client_ip[0] = (string)fr["client_1_ip"];
	client_ip[1] = (string)fr["client_2_ip"];
	client_ip[2] = (string)fr["client_3_ip"];
	client_ip[3] = (string)fr["client_4_ip"];
	port_id = (int)fr["PORT"];
	maxRecv = (int)fr["MaxRecv"];
	fs.release();

	//初始化Windows Socket
	if (WSAStartup(MAKEWORD(2,2),&Ws) != 0)
	{
		cout << "Init Windows Socket Failed::"<<GetLastError()<<endl;
		return;
	}

	//Create Socket
	ServerSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET)
	{
		cout << "Create Socket Failed::"<<GetLastError()<<endl;
		return;
	}

	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	LocalAddr.sin_port = htons(port_id);
	memset(LocalAddr.sin_zero,0x00,8);

	//Bind Socket
	Ret = bind(ServerSocket,(struct sockaddr*)&LocalAddr,sizeof(LocalAddr));
	if (Ret != 0)
	{
		cout << "Bind Socket Failed::"<<GetLastError()<<endl;
		return ;
	}

	Ret = listen(ServerSocket,10);
	if (Ret != 0)
	{
		cout << "Listen Socket Failed::"<<GetLastError()<<endl;
		return;
	}

	AddrLen = sizeof(ClientAddr);
	ClientSocket = accept(ServerSocket,(struct sockaddr*)&ClientAddr,&AddrLen);
	if (ClientSocket == INVALID_SOCKET)
	{
		cout<<"Accept Failed::"<<GetLastError()<<endl;
		return;
	}
	cout <<"服务端已经启动" << endl;
	cout << "客户端连接" << inet_ntoa(ClientAddr.sin_addr)<<":"<<ClientAddr.sin_port<<endl;
	/************************************************************************/
	/* 服务器与客户端在建立连接之后，通信流程如下：
	   （1）客户端向服务端发送客户编号以及客户就绪标志
	   （2）服务端向指定客户端发送场景参数信息。
	   （3）客户端接收场景参数信息后，存储在本地，然后初始化本地设置。
	   （4）客户端每隔一秒或者检测到异常情况时，将结果传回服务端。
	*/
	/************************************************************************/
	char tempBuffer[100];
	while (true)
	{
		memset(tempBuffer,0x00,sizeof(tempBuffer));
		Ret = recv(ClientSocket,tempBuffer,strlen(tempBuffer),0);
		if (Ret==0 || Ret == SOCKET_ERROR)
		{
			break;
		}
		if (tempBuffer[0] == CLIENT_OK)
		{
		
				fstream ff(para_M1);
				ostringstream ios;
				ios << ff.rdbuf();
				ff.close();
				string content(ios.str());
				const char* con = content.c_str();
				Ret = send(ClientSocket,con,(int)strlen(con),0);
				if (Ret==0 || Ret == SOCKET_ERROR)
				{
					break;
				}
				Ret = recv(ClientSocket,tempBuffer,strlen(tempBuffer),0);
				if (tempBuffer[0] == RECV_COMPLETE)
				{
					initFlag = true;
					break;
				}
		}
	}

	while (initFlag)//用以接收实时检测数据
	{
		hThread = CreateThread(NULL,0,RecvThread,this,0,NULL);
		if (hThread == NULL)
		{
			cout << "Create Thread Failed!" <<endl;
			break;
		}
		CloseHandle(hThread);
	}
	closesocket(ServerSocket);
	closesocket(ClientSocket);
	WSACleanup();
}


//void SocketServer::sendTcpInfo(QString para)
//{

//}
unsigned int WINAPI StartSocket(void* param)
{
	SocketServer* p_server = (SocketServer*)param;
	p_server->InitTcpServer();
	return 0;
}