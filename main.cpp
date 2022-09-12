#include "TCPServer.h"
#include "UDPServer.h"
#include "TargetGenerator.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	cout << "Tukki & Champ Networking Backend\n";

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		cout << "Fatal Error: WSAStartup failed with error: " << err << "\n";
		return 1;
	}

	try
	{
		TCPServer tcpServer("127.0.0.1", 20000);
		UDPServer udpServer("127.0.0.1", 20001);
		TargetGenerator targetGenerator;
		char* targetsMessage = targetGenerator.Generate(15, -7, 7, -4, 4);
		tcpServer.CheckIn(targetsMessage);
		udpServer.StartTransmitting();
		getchar();
	}
	catch(...)
	{
		cout << "Fatal Error: Setup failed.\n";
		WSACleanup();
		return 1;
	}

	WSACleanup();
	return 0;
}