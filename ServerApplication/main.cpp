#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

int main()
{
	// Initialize WinSock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOK = WSAStartup(ver, &wsData);

	if (wsOK != 0)
	{
		cerr << "Cann't initialize WinSock! Quitting..." << endl;
		return 1;
	}


	// Create a socket
	SOCKET listerning = socket(AF_INET, SOCK_STREAM, 0);
	if (listerning == INVALID_SOCKET)
	{
		cerr << "Cann't create a socket! Quitting..." << endl;
		return 1;
	}

	// Bind an ip address and port to a socket
	sockaddr_in hint;

	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listerning, (sockaddr*)&hint, sizeof(hint));

	// Tell WinSock the socket is for listerning
	listen(listerning, SOMAXCONN);

	// Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listerning, (sockaddr*)&client, &clientSize);

	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Cann't accept" << endl;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	// Close the listerning socket  
	closesocket(listerning);

	// While loop: accept and echo message back to client
	char buff[4096];

	while (true)
	{
		ZeroMemory(buff, 4096);

		// Wait for client to send a data
		int bytesReceived = recv(clientSocket, buff, 4096, 0);

		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting..." << endl;
			break;
		}

		if(bytesReceived == 0)
		{
			cerr << "Client disconnected" << endl;
			break;
		}

		// Echo message back to client
		send(clientSocket, buff, bytesReceived + 1, 0);
	}

	// Close the socket
	closesocket(clientSocket);

	// Cleanup WinSock
	WSACleanup();

	return 0;
}