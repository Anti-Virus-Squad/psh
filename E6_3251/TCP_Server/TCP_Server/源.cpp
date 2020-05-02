#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <winsock2.h>
#include<string>
#include<iostream>
#include<io.h>
#include<fstream>
#include<vector>
using namespace std;

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#define BUF_SIZE 1024

char buffer[BUF_SIZE] = { 0 };  //缓冲区
int main() {

		/*创建连接的SOCKET*/
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
		if (servSock < 0)
		{/*创建失败 */
			fprintf(stderr, "socker Error:%s\n", strerror(errno));
			exit(1);
		}
		/*初始化服务器地址*/
		sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		sockAddr.sin_port = htons(54321);

		if (bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) < 0)
		{/*绑定失败*/
			fprintf(stderr, "Bind Error:%s\n", strerror(errno));
			exit(1);
		}

		/*侦听客户端请求*/
		if (listen(servSock, 20) < 0)
		{
			fprintf(stderr, "Listen Error:%s\n", strerror(errno));
			closesocket(servSock);
			exit(1);
		}

		cout << "等待客户端接入..." << endl;

		/* 等待接收客户连接请求*/
		SOCKADDR clntAddr;
		int nSize = sizeof(SOCKADDR);
		SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

		if (clntSock <= 0)
		{
			fprintf(stderr, "Accept Error:%s\n", strerror(errno));
		}
		while(1)
		{
			//先输入文件保存路径
			string in_filepath;
			printf("输入要保存文件的路径: ");
			getline(cin, in_filepath);
			const char *filepath;  //文件保存路径
			filepath = in_filepath.c_str();

			//接收发送过来的原文件路径
			if (recv(clntSock, buffer, BUF_SIZE, 0) <= 0)
			{
				printf("客户端发来的%s断开连接！\n",buffer);
				break;
			}
			cout << "收到：" << buffer << endl;
			char filename[100] = {0};
			strcpy(filename,buffer);
			int flag = 1;
			while (flag == 1)		//发送的是文件名
			{
				//与输入的路径拼接成本地文件路径
				char filename_path[200] = { 0 };
				strcpy(filename_path, filepath);
				strcat(filename_path, "\\");
				strcat(filename_path, buffer);

				FILE *fp = fopen(filename_path, "wb");  //以二进制方式打开（创建）文件

				//告知服务端我们收到此文件名
				strcpy(buffer, "CilentGet");
				send(clntSock, buffer, sizeof(buffer), 0);

				//循环接收数据，直到文件传输完毕，每收到一段文件内容遍回传一段"CilentRecivedDATA."信息。
				int nCount;
				while ((nCount = recv(clntSock, buffer, BUF_SIZE, 0)) > 0)
				{
					if (strcmp(buffer, "ThisFileIsEnd.") == 0)
					{
						cout << "收到：" << buffer << endl;
						strcpy(buffer, "ServerRecivedFile.");
						send(clntSock, buffer, sizeof(buffer), 0);
						break;
					}
					fwrite(buffer, nCount, 1, fp);
					strcpy(buffer, "ServerRecivedDATA.");
					send(clntSock, buffer, sizeof(buffer), 0);
				}
				fclose(fp);

				if (nCount <= 0)
				{
					printf("客户端发来的%s断开连接！\n", filename);
					break;
				}
				cout << filename_path << " Recived" << endl;

				if (recv(clntSock, buffer, BUF_SIZE, 0) <= 0)	//接收此文件传完后的下一条信息
				{
					printf("客户端发来的%s断开连接！\n", filename);
					break;
				}
				cout << "收到：" << buffer << endl;
				if (strcmp(buffer, "CilentClosed.") == 0)//判断传输是否结束
				{
					flag = 0;
					strcpy(buffer, "ServerRecivedCloseRequest.");
					send(clntSock, buffer, sizeof(buffer), 0);
					puts("文件接收成功！");
				}
			}
			break;
		}
		shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
		//recv(clntSock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕
		closesocket(clntSock);
		closesocket(servSock);
		WSACleanup();
		system("pause");
		return 0;
}