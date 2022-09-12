#pragma once
#include <WS2tcpip.h>
#include <stdio.h>
#include <thread>
#include <Windows.h>
#include <mutex>
#include <mutex>

using namespace std;

class UDPServer
{
	SOCKET socket;
	bool isReceiving;
	bool isSending;
	thread* pReceivingThread;
	thread* pSendingThread;
	struct sockaddr_in* pTukkiAddress;
	struct sockaddr_in* pChampAddress;
	char latestMessageFromTukki[20];
	char latestMessageFromChamp[20];
	HANDLE hTimerQueue;
	mutex mutex;

public:
	UDPServer(const char* ip, int port);
	~UDPServer();

	bool StartTransmitting();
	void ReceiveProc();
	void SendProc();
	void Send();
	void StopTransmitting();
};