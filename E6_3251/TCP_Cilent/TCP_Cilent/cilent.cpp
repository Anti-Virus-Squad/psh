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
char buffer[BUF_SIZE];  //�ļ�������

int main() {

	//�ȼ���ļ��Ƿ����
	const char *filename;  //�ļ���
	string filenames;
	cout << "������Ҫ�ϴ������������ļ�·����";
	getline(cin, filenames);
	filename = filenames.c_str();

	FILE *fp = fopen(filename, "rb");  //�Զ����Ʒ�ʽ���ļ�
	if (fp == NULL) {
		printf("Cannot open file, press any key to exit!\n");
		system("pause");
		exit(0);
	}

	/*�������ӵ�SOCKET */
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0)
	{/*����ʧ�� */
		fprintf(stderr, "socker Error:%s\n", strerror(errno));
		exit(1);
	}

	/* ��ʼ���ͻ��˵�ַ*/
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr("192.168.0.106");
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);		//Ĭ�ϰ󶨱�������ip
	sockAddr.sin_port = htons(2345);					//��һ���˿ڣ��˴��Լ�ѡ�����2345
	if (bind(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR)) < 0)
	{ /*����ʧ�� */
		fprintf(stderr, "Bind Error:%s\n", strerror(errno));
		system("pause");
		exit(1);
	}

	/* ��ʼ����������ַ*/
	sockaddr_in ser_addr;
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	ser_addr.sin_port = htons(54321);

	if (connect(sock, (SOCKADDR*)&ser_addr, sizeof(SOCKADDR)) < 0)	//��������
	{/*����ʧ�� */
		fprintf(stderr, "Connect Error:%s\n", strerror(errno));
		closesocket(sock);
		exit(1);
	}


	string afilename;	
	int backslashIndex;		// ��\'��λ��
	backslashIndex = filenames.find_last_of('\\');	// ʶ�����һ��'\'��λ�á�
	afilename = filenames.substr(backslashIndex + 1, -1);	// ·����β�����ļ���
	strcpy(buffer, afilename.c_str());
	send(sock, buffer, sizeof(buffer), 0);	//�ȷ����ļ���
	cout << "�����ļ�����" << afilename.c_str() << endl;
	
	while (1)
	{
		if (recv(sock, buffer, BUF_SIZE, 0) <= 0)
		{
			cout << "������ʧȥ����" << endl;
			break;
		}
			//ѭ���������ݣ�ֱ���ļ���β
			int nCount;
			while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) > 0) {
				send(sock, buffer, nCount, 0);
				if (recv(sock, buffer, BUF_SIZE, 0) <= 0)	//ȷ��cilent���յ���һ������
				{
					cout << "������ʧȥ����" << endl;
					break;
				}
			}
			fclose(fp);
		strcpy(buffer, "ThisFileIsEnd.");
		send(sock, buffer, sizeof(buffer), 0);
		if (recv(sock, buffer, BUF_SIZE, 0) <= 0)
		{
			cout << "������ʧȥ����" << endl;
			break;
		}
		cout << buffer << "  �ļ��������." << endl;

		strcpy(buffer, "CilentClosed.");
		send(sock, buffer, sizeof(buffer), 0);
		if (recv(sock, buffer, BUF_SIZE, 0) <= 0)
		{
			cout << "������ʧȥ����" << endl;
			break;
		}
		cout << buffer << "  �˴δ������." << endl;
		puts("�ļ�����ɹ�!");
		break;
	}
	shutdown(sock, SD_SEND);
	recv(sock, buffer, BUF_SIZE, 0);  //�������ȴ��ͻ��˽������
	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;
}