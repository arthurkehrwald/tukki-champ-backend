#include "TCPServer.h"

TCPServer::TCPServer(const char* ip, int port)
{
	champSocket = NULL;
	tukkiSocket = NULL;

	sockaddr_in address;
	int err;

	socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET)
	{
		cout << "[TCP] Create socket failed with error: " << WSAGetLastError() << "\n";
		throw;
	}
	
	ZeroMemory(&address, sizeof(address));
	address.sin_family = AF_INET;
	err = inet_pton(AF_INET, ip, &address.sin_addr.s_addr);
	if (err != 1)
	{
		cout << "[TCP] Generate address failed with error: " << WSAGetLastError() << "\n";
		throw;
	}
	address.sin_port = htons(port);

	err = bind(socket, (SOCKADDR*)&address, sizeof(address));
	if (err == SOCKET_ERROR)
	{
		cout << "[TCP] Bind socket failed with error: " << WSAGetLastError() << "\n";
		closesocket(socket);
		throw;
	}

	err = listen(socket, SOMAXCONN);
	if (err == SOCKET_ERROR)
	{
		cout << "[TCP] Listen on socket failed with error: " << WSAGetLastError() << "\n";
		closesocket(socket);
		throw;
	}
}

TCPServer::~TCPServer()
{
	closesocket(socket);
}

void TCPServer::CheckIn(char* msg)
{
	int err;
	char sendBuffer[270];

	tukkiSocket = accept(socket, NULL, NULL);
	sprintf_s(sendBuffer, sizeof(sendBuffer), "roletukki:");
	strcat_s(sendBuffer, sizeof(sendBuffer), msg);
	err = send(tukkiSocket, sendBuffer, (int)strlen(sendBuffer), 0);
	if (err == SOCKET_ERROR)
	{
		printf("[TCP] Send (to assign role) failed (%d)\n", WSAGetLastError());
		throw;
	}
	printf("[TCP] Client 1 connected, assigned role: tukki\n");

	champSocket = accept(socket, NULL, NULL);
	sprintf_s(sendBuffer, sizeof(sendBuffer), "rolechamp:");
	strcat_s(sendBuffer, sizeof(sendBuffer), msg);
	err = send(champSocket, sendBuffer, (int)strlen(sendBuffer), 0);
	if (err == SOCKET_ERROR)
	{
		printf("[TCP] Send (to assign role) failed (%d)\n", WSAGetLastError());
		throw;
	}
	printf("[TCP] Client 2 connected, assigned role: champ\n");
}

void TCPServer::Send(char* msg)
{
	char msgcpy[256];
	strcpy_s(msgcpy, sizeof(msgcpy), msg);
	int iResult;
	if (tukkiSocket != NULL)
	{
		iResult = send(tukkiSocket, msg, (int)strlen(msg), 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("[TCP] Send failed (%d)\n", WSAGetLastError());
		}
	}

	if (champSocket != NULL)
	{
		iResult = send(champSocket, msg, (int)strlen(msg), 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("[TCP] Send failed (%d)\n", WSAGetLastError());
		}
	}
	cout << "Sent: " << msgcpy << "\n";
}