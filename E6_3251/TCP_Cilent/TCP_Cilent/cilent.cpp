#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include<iostream>
#include<direct.h>
#include<io.h>
#include<string>
using namespace std;
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996) 

#define BUF_SIZE 1024
char buffer[BUF_SIZE];  //文件缓冲区

int main() {

	//先检查文件是否存在
	const char *filename;  //文件名
	string filenames;
	cout << "请输入要上传到服务器的文件路径：";
	getline(cin, filenames);
	filename = filenames.c_str();

	FILE *fp = fopen(filename, "rb");  //以二进制方式打开文件
	if (fp == NULL) {
		printf("Cannot open file, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	/*创建连接的SOCKET */
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
	{/*创建失败 */
		fprintf(stderr, "socker Error:%s\n", strerror(errno));
		exit(1);
	}

	/* 初始化客户端地址*/
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);		//默认绑定本机所有ip
	sockAddr.sin_port = htons(2345);					//绑定一个端口，此处自己选择绑定了2345
	if (bind(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) < 0)
	{ /*棒定失败 */
		fprintf(stderr, "Bind Error:%s\n", strerror(errno));
		system("pause");
		exit(1);
	}

	/* 初始化服务器地址*/
	sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ser_addr.sin_port = htons(54321);

	if (connect(sock, (SOCKADDR*)&ser_addr, sizeof(SOCKADDR)) < 0)	//请求连接
	{/*连接失败 */
		fprintf(stderr, "Connect Error:%s\n", strerror(errno));
		closesocket(sock);
		exit(1);
	}


	string afilename;	
	int backslashIndex;		// ‘\'的位置
	backslashIndex = filenames.find_last_of('\\');	// 识别最后一个'\'的位置。
	afilename = filenames.substr(backslashIndex + 1, -1);	// 路径名尾部是文件名
	strcpy(buffer, afilename.c_str());
	send(sock, buffer, sizeof(buffer), 0);	//先发送文件名
	cout << "发送文件名：" << afilename.c_str() << endl;
	
	while (1)
	{
		if (recv(sock, buffer, BUF_SIZE, 0) <= 0)
		{
			cout << "服务器失去连接" << endl;
			break;
		}
			//循环发送数据，直到文件结尾
			int nCount;
			while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
				send(sock, buffer, nCount, 0);
				if (recv(sock, buffer, BUF_SIZE, 0) <= 0)	//确认cilent接收到上一条数据
				{
					cout << "服务器失去连接" << endl;
					break;
				}
			}
			fclose(fp);
		strcpy(buffer, "ThisFileIsEnd.");
		send(sock, buffer, sizeof(buffer), 0);
		if (recv(sock, buffer, BUF_SIZE, 0) <= 0)
		{
			cout << "服务器失去连接" << endl;
			break;
		}
		cout << buffer << "  文件接收完毕." << endl;

		strcpy(buffer, "CilentClosed.");
		send(sock, buffer, sizeof(buffer), 0);
		if (recv(sock, buffer, BUF_SIZE, 0) <= 0)
		{
			cout << "服务器失去连接" << endl;
			break;
		}
		cout << buffer << "  此次传输完毕." << endl;
		puts("文件传输成功!");
		break;
	}
	shutdown(sock, SD_SEND);
	recv(sock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕
	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;
}