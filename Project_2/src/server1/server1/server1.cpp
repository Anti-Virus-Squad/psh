// serve本机.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
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

SOCKET clntSock;
SOCKADDR clntAddr;
char buffer[BUF_SIZE] = { 0 };  //缓冲区
int main() {

	void getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& names, string &fatherpath);
	vector<string>files;		//存文件夹内所有子文件的全路径
	vector<string>names;		//存文件夹内所有子文件的文件名
	string fatherpath = "";		//用于截取文件夹名称

	//先检查文件是否存在
	const char *filename;  //文件名
	string filenames;
	cout << "请输入要上传到服务器的文件路径：";
	getline(cin, filenames);
	filename = filenames.c_str();
	int flag = 1;//0表示文件 1表示文件夹
	//通过是否含“.”判断是文件夹还是文件名
	for (int i = 0; i < filenames.length(); i++)
	{
		if (filenames[i] == '.')
			flag = 0;
	}
	//如果是单文件
	if (flag == 0) {
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
		
		string server_ip;
		cout << "请输入服务器的ip地址：";
		cin >> server_ip;
		const char *server;
		server = server_ip.c_str();
		sockAddr.sin_addr.s_addr = inet_addr(server);
		
		//sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");
		
		int server_port;
		cout << "请输入服务器的端口号：";
		cin >> server_port;
		sockAddr.sin_port = htons(server_port);
		
		//sockAddr.sin_port = htons(1234);

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
		
		int nSize = sizeof(SOCKADDR);
		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

		if (clntSock <= 0)
		{
			fprintf(stderr, "Accept Error:%s\n", strerror(errno));
		}
		else
		{
			
			FILE *fp = fopen(filename, "rb");  //以二进制方式打开文件
			if (fp == NULL) {
				printf("Cannot open file, press any key to exit!\n");
				system("pause");
				exit(0);
			}
			string afilename;
			// ‘\'的位置
			int backslashIndex;
			// 识别最后一个'\'的位置。
			backslashIndex = filenames.find_last_of('\\');
			// 路径名尾部是文件名
			afilename = filenames.substr(backslashIndex + 1, -1);
			//先发送文件名
			strcpy(buffer, afilename.c_str());
			send(clntSock, buffer, sizeof(buffer), 0);
			cout <<"发送文件名："<<afilename.c_str() << endl;
			recv(clntSock, buffer, BUF_SIZE, 0);
			//循环发送数据，直到文件结尾
			int nCount;
			while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
				send(clntSock, buffer, nCount, 0);
				recv(clntSock, buffer, BUF_SIZE, 0);	//确认cilent接收到上一条数据
			}
			fclose(fp);
			
			
			strcpy(buffer, "ThisFileIsEnd.");
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);
			cout << buffer << "  文件接收完毕." << endl;

			strcpy(buffer, "ThisSendIsEnd.");
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);
			cout << buffer<<"  此次传输完毕." << endl;
			
		}
		shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
		recv(clntSock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕
		closesocket(clntSock);
		closesocket(servSock);
		WSACleanup();
		system("pause");
		return 0;
	}
	//如果是文件夹
	else {
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
		
		string server_ip;
		cout << "请输入服务器的ip地址：";
		cin >> server_ip;
		const char *server;
		server = server_ip.c_str();
		sockAddr.sin_addr.s_addr = inet_addr(server);
		
		//sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");
		
		int server_port;
		cout << "请输入服务器的端口号：";
		cin >> server_port;
		sockAddr.sin_port = htons(server_port);
		
		//sockAddr.sin_port = htons(1234);

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
		int nSize = sizeof(SOCKADDR);
		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);


		getFiles(filenames, files, names, fatherpath);//获取文件夹下所有子文件对应的名称与路径，并发送文件夹名给cilent
		for (int i = 0; i < files.size(); i++)		//将所有文件逐个发送
		{
			FILE *fp = fopen(files[i].c_str(), "rb");  //以二进制方式打开文件
			if (fp == NULL) {
				printf("Cannot open file, press any key to exit!\n");
				system("pause");
				exit(0);
			}
			//先发送文件名
			cout << "发送文件名:" << names[i] << endl;
			strcpy(buffer, names[i].c_str());
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);	//接收到cilent的回应
			//循环发送数据，直到文件结尾
			int nCount;
			while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
				send(clntSock, buffer, nCount, 0);
				recv(clntSock, buffer, BUF_SIZE, 0);	//确认cilent接收到上一条数据
			}
			fclose(fp);
			strcpy(buffer, "ThisFileIsEnd.");
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);
			cout << buffer << "  文件接收完毕." << endl;
		}
		strcpy(buffer, "ThisSendIsEnd.");
		send(clntSock, buffer, sizeof(buffer), 0);
		recv(clntSock, buffer, BUF_SIZE, 0);
		cout << buffer << "  此次传输完毕." << endl;


		shutdown(clntSock, SD_SEND);  //文件读取完毕，断开输出流，向客户端发送FIN包
		recv(clntSock, buffer, BUF_SIZE, 0);  //阻塞，等待客户端接收完毕
		closesocket(clntSock);
		closesocket(servSock);
		WSACleanup();

		system("pause");
		return 0;
	}
}


void getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& names, string &fatherpath)
{
	/*	path----输入的全路径 D:\sql（内包括一个kkk子文件夹）
		files----存着所有子文件的全路径
		names----存着要发送给客户端的文件名（带路径）
		比如：sql\111.txt	sql\kkk\111.txt
		fatherpath----存着文件夹的名称（子文件夹带路径）
		比如：sql	sql\kkk
	*/

	string filefoldername;
	// ‘\'的位置
	int backslashIndex;
	// 识别最后一个'\'的位置。
	backslashIndex = path.find_last_of('\\');
	// 路径名尾部是文件名
	filefoldername = path.substr(backslashIndex + 1, -1);
	fatherpath += filefoldername;
	cout << "发送文件夹名：" << fatherpath << endl;
	strcpy(buffer, fatherpath.c_str());
	send(clntSock, buffer, sizeof(buffer), 0);
	recv(clntSock, buffer, BUF_SIZE, 0);	//接收到cilent的回应

	//文件句柄，win10用long long，win7用long就可以了
	long long hFile = 0;
	//文件信息 
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之 //如果不是,加入列表 
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					string subfatherpath = fatherpath+"\\";
					//getFiles(p.assign(path).append("\\").append(fileinfo.name), files, names, fatherpath.append("\\"));
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files, names, subfatherpath);
				}
			}
			else
			{
				string filename = fatherpath + "\\" + fileinfo.name;
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				names.push_back(filename);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
