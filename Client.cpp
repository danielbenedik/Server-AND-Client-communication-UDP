#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;
#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <string>

#define TIME_PORT	27015

int Requests_menu();
void Send_Massage(SOCKET connSocket, char* sendBuff, sockaddr_in server);
void Get_Massage(SOCKET connSocket, char* recvBuff);
void GetClientToServerDelayEstimation(SOCKET connSocket, char* sendBuff, char* recvBuff, sockaddr_in server);
void MeasureRTT(SOCKET connSocket, char * sendBuff, char * recvBuff, sockaddr_in server);


void main()
{
	// Initialize Winsock (Windows Sockets).
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
	}

	// Create a socket and connect to an internet address.
	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}
	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	char sendBuff[255];
	char recvBuff[255];
	int option;
	bool flag = true;
	string resNum;
	sendBuff[1] = '\0';

	while(flag)
	{
		 option = Requests_menu();
		 sendBuff[0]= 48 + option;
		 sendBuff[1] ='\0';


		 switch (option) {
		 case 4:
			 GetClientToServerDelayEstimation(connSocket, sendBuff, recvBuff, server);
			 break;
		 case 5:
			 sendBuff[0] = '1';
			 MeasureRTT(connSocket, sendBuff, recvBuff, server);
			 break;
		 case 10:
			 sendBuff[0] = 'A';
			 goto doIt;
			 break;
		 case 11:
			 sendBuff[0] = 'B';
			 goto doIt;
			 break;
		 case 0:
			 cout << "Time Client: Closing Connection.\n";
			 closesocket(connSocket);
			 WSACleanup();
			 flag = false;
			 default:
				 doIt:  Send_Massage(connSocket, sendBuff, server);
						Get_Massage(connSocket, recvBuff);
			 break;

		 }
		 cout << endl;
	}
	
}

void MeasureRTT(SOCKET connSocket, char * sendBuff, char * recvBuff, sockaddr_in server)
{
	double first, secend;
	double total=0;

	for (int i = 0; i < 100; i++)
	{
		first = GetTickCount();
		Send_Massage(connSocket, sendBuff, server);
		Get_Massage(connSocket, recvBuff);
		secend = GetTickCount();
		total = secend - first;
	}
	double avg = total / 100;
	cout << "The RTT Is: " << avg << "ms\n" << endl;
}

void GetClientToServerDelayEstimation(SOCKET connSocket, char* sendBuff, char* recvBuff, sockaddr_in server)
{
	double first, secend;
	double total=0, rest;
	for (int i = 0; i < 100; i++)
	{
		Send_Massage(connSocket, sendBuff, server);
	}
	for (int i = 0; i < 100; i++)
	{
		Get_Massage(connSocket, recvBuff);
		if (i == 0)
		{
			first = atol(recvBuff);
		}
		else
		{
			secend = atol(recvBuff);
			rest = secend - first;
			total += rest;
			first = secend;
		}
	}

	double avg = total / 99; // thare is 99 rest (n-1)// n=100
	cout << "The Client To Server Delay Estimation Is: " << avg << "ms\n" << endl;

}

int Requests_menu()
{
	int res;
	cout << "Pleas press the option that you want? \n";
	cout << "1. GetTime \n" << "2. GetTimeWithoutDate \n" << "3. GetTimeSinceEpoch \n";
	cout << "4. GetClientToServerDelayEstimation \n" << "5. MeasureRTT \n" << "6. GetTimeWithoutDateOrSeconds \n";
	cout << "7. GetYear \n" << "8. GetMonthAndDay \n" << "9. GetSecondsSinceBeginingOfMonth \n" <<"10. GetDayOfYear\n"<<"11. GetDaylightSavings\n" <<"press other key to EXIT\n";
	cout << "your answer: ";
	cin >> res;

	if (res > 11 || res < 1)
	{
		res = 0;
	}

	return res;
}

void Send_Massage(SOCKET connSocket,char* sendBuff, sockaddr_in server)
{
	int bytesSent = 0;
	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr *)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return;
	}
	cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
}

void Get_Massage(SOCKET connSocket,char* recvBuff)
{
	int bytesRecv = 0;
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return;
	}

	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
}