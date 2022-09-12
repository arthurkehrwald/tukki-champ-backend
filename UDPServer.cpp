#include "UDPServer.h"

UDPServer::UDPServer(const char* ip, int port)
{
	sockaddr_in address;
	int err;

	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socket == INVALID_SOCKET)
	{
		printf("[UDP] Create socket failed (%d)\n", WSAGetLastError());
		throw;
	}

	ZeroMemory(&address, sizeof(address));
	address.sin_family = AF_INET;
	err = inet_pton(AF_INET, ip, &address.sin_addr.s_addr);
	if (err != 1)
	{
		printf("[UDP] Generate address failed (%d)\n", WSAGetLastError());
		throw;
	}
	address.sin_port = htons(port);
	
	err = bind(socket, (SOCKADDR*)&address, sizeof(address));
	if (err == SOCKET_ERROR)
	{
		printf("[UDP] Bind socket failed (%d)\n", WSAGetLastError());
		closesocket(socket);
		throw;
	}

	isReceiving = false;
	pReceivingThread = NULL;
	isSending = false;
	pSendingThread = NULL;
	pTukkiAddress = NULL;
	pChampAddress = NULL;
	hTimerQueue = NULL;
}

UDPServer::~UDPServer()
{
	StopTransmitting();
	closesocket(socket);
}

VOID CALLBACK SendRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
	UDPServer* pCaller = (UDPServer*)lpParam;
	pCaller->Send();
}

bool UDPServer::StartTransmitting()
{
	isReceiving = true;
	pReceivingThread = new thread(&UDPServer::ReceiveProc, this);

	isSending = true;
	pSendingThread = new thread(&UDPServer::SendProc, this);

	return true;
}

void UDPServer::ReceiveProc()
{
	char buffer[20];
	while (isReceiving)
	{
		struct sockaddr_in sender;
		int senderSize = sizeof(sender);
		int messageSize = recvfrom(socket, buffer, sizeof(buffer), 0, (SOCKADDR*) &sender, &senderSize);
		if (messageSize < 1)
		{
			continue;
		}
		if (messageSize >= sizeof(buffer))
		{
			printf("UDP socket failed to transmit message because it was too long for the buffer\n");
			continue;
		}
		buffer[messageSize] = '\0';
		//printf("UDP socket received: %s\n", buffer);

		char senderRole[6];
		strncpy_s(senderRole, sizeof(senderRole), buffer, 5);
		if (strcmp(senderRole, "tukki") == 0)
		{
			pTukkiAddress = &sender;
			strcpy_s(latestMessageFromTukki, sizeof(latestMessageFromTukki), buffer);
		}
		else if (strcmp(senderRole, "champ") == 0)
		{
			pChampAddress = &sender;
			strcpy_s(latestMessageFromChamp, sizeof(latestMessageFromChamp), buffer);
		}
	}
}

void UDPServer::SendProc()
{
	while (isSending)
	{
		Send();
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

void UDPServer::Send()
{
	int iResult;

	mutex.lock();

	// if we have tukki's address and a message from champ
	if (pTukkiAddress != NULL && strstr(latestMessageFromChamp, "champ") != NULL)
	{
		iResult = sendto(socket,
			latestMessageFromChamp, strlen(latestMessageFromChamp), 0,
			(SOCKADDR*)pTukkiAddress, sizeof(*pTukkiAddress));
		if (iResult == SOCKET_ERROR)
		{
			printf("[UDP] Send failed (%d)\n", WSAGetLastError());
		}
		else
		{
			//printf("[UDP] Sent message '%s' to tukki\n", latestMessageFromChamp);
		}
	}

	// if we have champ's address and a message from tukki
	if (pChampAddress != NULL && strstr(latestMessageFromTukki, "tukki") != NULL)
	{
		iResult = sendto(socket,
			latestMessageFromTukki, strlen(latestMessageFromTukki), 0,
			(SOCKADDR*)pChampAddress, sizeof(*pChampAddress));
		if (iResult == SOCKET_ERROR)
		{
			printf("[UDP] Send failed (%d)\n", WSAGetLastError());
		}
		else
		{
			printf("[UDP] Sent message '%s' to champ\n", latestMessageFromTukki);
		}
	}
	mutex.unlock();
}

void UDPServer::StopTransmitting()
{
	isReceiving = false;
	if (pReceivingThread->joinable())
	{
		pReceivingThread->join();
	}
	delete pReceivingThread;
	pReceivingThread = NULL;
	isSending = false;
	if (pSendingThread->joinable())
	{
		pSendingThread->join();
	}
	delete pSendingThread;
	pSendingThread = NULL;
}
