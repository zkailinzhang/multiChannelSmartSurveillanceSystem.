#ifndef _SMART_TCP_SERVER_H
#define _SMART_TCP_SERVER_H

#include <Windows.h>
#include <process.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <QtWidgets/QWidget>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

extern string file_tcp;

enum CommunicationState
{
	CLIENT_OK = 1,//客户端准备就绪，可以接收
	SERVER_OK = 2,
	TYPE_MONITOR = 3,
	TYPE_TEST = 4,
	RECV_COMPLETE = 5//客户端接收文件结束
};

class SocketServer: public QObject
{
		Q_OBJECT
public:
	SocketServer();
	~SocketServer();


	WSADATA Ws;
	SOCKET ServerSocket,ClientSocket;
	struct sockaddr_in LocalAddr,ClientAddr;
	bool initFlag;




	void InitTcpServer();
private:


	int maxRecv;
	int port_id;
	string server_ip;
	string client_ip[4];

	//static DWORD WINAPI ClientThread(LPVOID lpParameter);
	
signals:
	void sendTcpInfo(ushort para);
};

unsigned int WINAPI StartSocket(void* param);




#endif