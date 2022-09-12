#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include <mutex>

using namespace std;

class TCPServer
{
	SOCKET socket;
	SOCKET tukkiSocket;
	SOCKET champSocket;
	mutex mutex;

public:
	TCPServer(const char* ip, int port);
	~TCPServer();
	
	void CheckIn(char* msg);
	void Send(char* msg);
};