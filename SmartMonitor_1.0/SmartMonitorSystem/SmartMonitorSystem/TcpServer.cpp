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
/* ARM�˸�����һЩ�򵥵Ĺ��ܣ����������⣬��Ⱥ�ܶȹ��ƣ���ARM����Զ˵�
   ͨ���У��������¹������ͨ�ţ�
   1.ARM��Ϊ�ͻ��ˣ����Զ�Ϊ�����
   2.ARMÿ�η��������ֽڵ����ݣ����ݰ����¸�ʽ���룺
     (1)��8λ��ų�����źͼ�����ݣ���8λ��ż����
	 ���� 0x0000   0000      0000   0000
	        ���   �������  �����
      ������ݶ�Ӧ��0001  ��Ⱥ�ܶ�
	                0010  �������*/
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
		//δ����
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

	//��ʼ��Windows Socket
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
	cout <<"������Ѿ�����" << endl;
	cout << "�ͻ�������" << inet_ntoa(ClientAddr.sin_addr)<<":"<<ClientAddr.sin_port<<endl;
	/************************************************************************/
	/* ��������ͻ����ڽ�������֮��ͨ���������£�
	   ��1���ͻ��������˷��Ϳͻ�����Լ��ͻ�������־
	   ��2���������ָ���ͻ��˷��ͳ���������Ϣ��
	   ��3���ͻ��˽��ճ���������Ϣ�󣬴洢�ڱ��أ�Ȼ���ʼ���������á�
	   ��4���ͻ���ÿ��һ����߼�⵽�쳣���ʱ����������ط���ˡ�
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

	while (initFlag)//���Խ���ʵʱ�������
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