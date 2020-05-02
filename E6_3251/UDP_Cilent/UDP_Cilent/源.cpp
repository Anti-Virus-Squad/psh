#pragma comment(lib,"Ws2_32.lib")

#include<WinSock2.h>
#include<WS2tcpip.h>
#include<stdio.h>
#include<Windows.h>

#include"FileHelper.h"

int main()
{

	WORD wVersionRequested;
	WSADATA wsaData;
	char sendData[BUFSIZ] = "我是cilent\n";
	char beginData[BUFSIZ] = "Begin\n";
	char overData[BUFSIZ] = "Over\n";
	char Filename[BUFSIZ] = {};
	FileHelper fh;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN addrServ;
	addrServ.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(4999);
	char recvBuf[BUFSIZ] = {};
	int length = sizeof(SOCKADDR);
	while (true)
	{
		SOCKET socketClient = socket(AF_INET, SOCK_DGRAM, 0);
		FILE *f = fh.selectfile();
		sendto(socketClient, beginData, BUFSIZ, 0, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));

		recvfrom(socketClient, recvBuf, BUFSIZ, 0, (SOCKADDR*)&addrServ, &length);

		strcpy(Filename, fh.getFileName());
		sendto(socketClient, Filename, BUFSIZ, 0, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
		recvfrom(socketClient, recvBuf, BUFSIZ, 0, (SOCKADDR*)&addrServ, &length);
		int count = 0;
		int sum = 0;
		while ((count = fread(sendData, 1, BUFSIZ, f)) > 0)
		{
			Sleep(1);
			//printf("%d\n", sum += count);
			sendto(socketClient, sendData, BUFSIZ, 0, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
		}
		sendto(socketClient, overData, BUFSIZ, 0, (SOCKADDR*)&addrServ, sizeof(SOCKADDR));
		if((recvfrom(socketClient, recvBuf, BUFSIZ, 0, (SOCKADDR*)&addrServ, &length)>0) && (strcmp("服务器接受完了文件", recvBuf) == 0))
		{
			printf("文件传输成功!\n");
			fclose(f);
			closesocket(socketClient);
			break;
		}
		else
		{
			printf("服务器IP:%s传输过程中失去连接\n", addrServ);
			closesocket(socketClient);
			break;
		}
	}
	WSACleanup();
	system("pause");
	return 0;

}