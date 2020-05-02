// serve����.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
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

#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll
#define BUF_SIZE 1024

SOCKET clntSock;
SOCKADDR clntAddr;
char buffer[BUF_SIZE] = { 0 };  //������
int main() {

	void getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& names, string &fatherpath);
	vector<string>files;		//���ļ������������ļ���ȫ·��
	vector<string>names;		//���ļ������������ļ����ļ���
	string fatherpath = "";		//���ڽ�ȡ�ļ�������

	//�ȼ���ļ��Ƿ����
	const char *filename;  //�ļ���
	string filenames;
	cout << "������Ҫ�ϴ������������ļ�·����";
	getline(cin, filenames);
	filename = filenames.c_str();
	int flag = 1;//0��ʾ�ļ� 1��ʾ�ļ���
	//ͨ���Ƿ񺬡�.���ж����ļ��л����ļ���
	for (int i = 0; i < filenames.length(); i++)
	{
		if (filenames[i] == '.')
			flag = 0;
	}
	//����ǵ��ļ�
	if (flag == 0) {
		/*�������ӵ�SOCKET*/
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
		if (servSock < 0)
		{/*����ʧ�� */
			fprintf(stderr, "socker Error:%s\n", strerror(errno));
			exit(1);
		}
		/*��ʼ����������ַ*/
		sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		
		string server_ip;
		cout << "�������������ip��ַ��";
		cin >> server_ip;
		const char *server;
		server = server_ip.c_str();
		sockAddr.sin_addr.s_addr = inet_addr(server);
		
		//sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");
		
		int server_port;
		cout << "������������Ķ˿ںţ�";
		cin >> server_port;
		sockAddr.sin_port = htons(server_port);
		
		//sockAddr.sin_port = htons(1234);

		if (bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) < 0)
		{/*��ʧ��*/
			fprintf(stderr, "Bind Error:%s\n", strerror(errno));
			exit(1);
		}

		/*�����ͻ�������*/
		if (listen(servSock, 20) < 0)
		{
			fprintf(stderr, "Listen Error:%s\n", strerror(errno));
			closesocket(servSock);
			exit(1);
		}

		cout << "�ȴ��ͻ��˽���..." << endl;

		/* �ȴ����տͻ���������*/
		
		int nSize = sizeof(SOCKADDR);
		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

		if (clntSock <= 0)
		{
			fprintf(stderr, "Accept Error:%s\n", strerror(errno));
		}
		else
		{
			
			FILE *fp = fopen(filename, "rb");  //�Զ����Ʒ�ʽ���ļ�
			if (fp == NULL) {
				printf("Cannot open file, press any key to exit!\n");
				system("pause");
				exit(0);
			}
			string afilename;
			// ��\'��λ��
			int backslashIndex;
			// ʶ�����һ��'\'��λ�á�
			backslashIndex = filenames.find_last_of('\\');
			// ·����β�����ļ���
			afilename = filenames.substr(backslashIndex + 1, -1);
			//�ȷ����ļ���
			strcpy(buffer, afilename.c_str());
			send(clntSock, buffer, sizeof(buffer), 0);
			cout <<"�����ļ�����"<<afilename.c_str() << endl;
			recv(clntSock, buffer, BUF_SIZE, 0);
			//ѭ���������ݣ�ֱ���ļ���β
			int nCount;
			while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
				send(clntSock, buffer, nCount, 0);
				recv(clntSock, buffer, BUF_SIZE, 0);	//ȷ��cilent���յ���һ������
			}
			fclose(fp);
			
			
			strcpy(buffer, "ThisFileIsEnd.");
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);
			cout << buffer << "  �ļ��������." << endl;

			strcpy(buffer, "ThisSendIsEnd.");
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);
			cout << buffer<<"  �˴δ������." << endl;
			
		}
		shutdown(clntSock, SD_SEND);  //�ļ���ȡ��ϣ��Ͽ����������ͻ��˷���FIN��
		recv(clntSock, buffer, BUF_SIZE, 0);  //�������ȴ��ͻ��˽������
		closesocket(clntSock);
		closesocket(servSock);
		WSACleanup();
		system("pause");
		return 0;
	}
	//������ļ���
	else {
		/*�������ӵ�SOCKET*/
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		SOCKET servSock = socket(AF_INET, SOCK_STREAM, 0);
		if (servSock < 0)
		{/*����ʧ�� */
			fprintf(stderr, "socker Error:%s\n", strerror(errno));
			exit(1);
		}
		/*��ʼ����������ַ*/
		sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));
		sockAddr.sin_family = AF_INET;
		
		string server_ip;
		cout << "�������������ip��ַ��";
		cin >> server_ip;
		const char *server;
		server = server_ip.c_str();
		sockAddr.sin_addr.s_addr = inet_addr(server);
		
		//sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");
		
		int server_port;
		cout << "������������Ķ˿ںţ�";
		cin >> server_port;
		sockAddr.sin_port = htons(server_port);
		
		//sockAddr.sin_port = htons(1234);

		if (bind(servSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) < 0)
		{/*��ʧ��*/
			fprintf(stderr, "Bind Error:%s\n", strerror(errno));
			exit(1);
		}

		/*�����ͻ�������*/
		if (listen(servSock, 20) < 0)
		{
			fprintf(stderr, "Listen Error:%s\n", strerror(errno));
			closesocket(servSock);
			exit(1);
		}

		cout << "�ȴ��ͻ��˽���..." << endl;

		/* �ȴ����տͻ���������*/
		int nSize = sizeof(SOCKADDR);
		clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);


		getFiles(filenames, files, names, fatherpath);//��ȡ�ļ������������ļ���Ӧ��������·�����������ļ�������cilent
		for (int i = 0; i < files.size(); i++)		//�������ļ��������
		{
			FILE *fp = fopen(files[i].c_str(), "rb");  //�Զ����Ʒ�ʽ���ļ�
			if (fp == NULL) {
				printf("Cannot open file, press any key to exit!\n");
				system("pause");
				exit(0);
			}
			//�ȷ����ļ���
			cout << "�����ļ���:" << names[i] << endl;
			strcpy(buffer, names[i].c_str());
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);	//���յ�cilent�Ļ�Ӧ
			//ѭ���������ݣ�ֱ���ļ���β
			int nCount;
			while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
				send(clntSock, buffer, nCount, 0);
				recv(clntSock, buffer, BUF_SIZE, 0);	//ȷ��cilent���յ���һ������
			}
			fclose(fp);
			strcpy(buffer, "ThisFileIsEnd.");
			send(clntSock, buffer, sizeof(buffer), 0);
			recv(clntSock, buffer, BUF_SIZE, 0);
			cout << buffer << "  �ļ��������." << endl;
		}
		strcpy(buffer, "ThisSendIsEnd.");
		send(clntSock, buffer, sizeof(buffer), 0);
		recv(clntSock, buffer, BUF_SIZE, 0);
		cout << buffer << "  �˴δ������." << endl;


		shutdown(clntSock, SD_SEND);  //�ļ���ȡ��ϣ��Ͽ����������ͻ��˷���FIN��
		recv(clntSock, buffer, BUF_SIZE, 0);  //�������ȴ��ͻ��˽������
		closesocket(clntSock);
		closesocket(servSock);
		WSACleanup();

		system("pause");
		return 0;
	}
}


void getFiles(std::string path, std::vector<std::string>& files, std::vector<std::string>& names, string &fatherpath)
{
	/*	path----�����ȫ·�� D:\sql���ڰ���һ��kkk���ļ��У�
		files----�����������ļ���ȫ·��
		names----����Ҫ���͸��ͻ��˵��ļ�������·����
		���磺sql\111.txt	sql\kkk\111.txt
		fatherpath----�����ļ��е����ƣ����ļ��д�·����
		���磺sql	sql\kkk
	*/

	string filefoldername;
	// ��\'��λ��
	int backslashIndex;
	// ʶ�����һ��'\'��λ�á�
	backslashIndex = path.find_last_of('\\');
	// ·����β�����ļ���
	filefoldername = path.substr(backslashIndex + 1, -1);
	fatherpath += filefoldername;
	cout << "�����ļ�������" << fatherpath << endl;
	strcpy(buffer, fatherpath.c_str());
	send(clntSock, buffer, sizeof(buffer), 0);
	recv(clntSock, buffer, BUF_SIZE, 0);	//���յ�cilent�Ļ�Ӧ

	//�ļ������win10��long long��win7��long�Ϳ�����
	long long hFile = 0;
	//�ļ���Ϣ 
	struct _finddata_t fileinfo;
	std::string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮ //�������,�����б� 
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
