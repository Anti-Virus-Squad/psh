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

	//�������ļ�����·��
	string in_filepath;
	printf("����Ҫ�����ļ���·��: ");
	getline(cin, in_filepath);
	const char *filepath;  //�ļ�����·��
	filepath = in_filepath.c_str();

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
	
	string server_ip;
	cout << "�������������ip��ַ��";
	cin >> server_ip;
	const char *server;
	server = server_ip.c_str();
	ser_addr.sin_addr.s_addr = inet_addr(server);
	int server_port;
	cout << "������������Ķ˿ںţ�";
	cin >> server_port;
	ser_addr.sin_port = htons(server_port);
	
	//ser_addr.sin_addr.s_addr = inet_addr("192.168.0.106");
	//ser_addr.sin_port = htons(1234);

	if (connect(sock, (SOCKADDR*)&ser_addr, sizeof(SOCKADDR)) < 0)	//��������
	{/*����ʧ�� */
		fprintf(stderr, "Connect Error:%s\n", strerror(errno));
		closesocket(sock);
		exit(1);
	}
	

	char buffer[BUF_SIZE];  //�ļ�������
	//���շ��͹�����ԭ�ļ�·��
	recv(sock, buffer, BUF_SIZE, 0);
	cout <<"�յ���"<<buffer << endl;

	int flag = 0;	//0----���͵����ļ��������ƣ�����.����1----���͵����ļ�����(��.)
	for (int i = 0; buffer[i] != '\0'; i++)
	{
		if (buffer[i] == '.')
			flag = 1;
	}
	while (flag == 0)	//���͵����ļ������ƣ��ж��Ƿ��д��ļ��У�û�оʹ���һ��
	{
		//���Լ������·��ƴ�ӳɱ����ļ���·��
		char filefoldername[200] = { 0 };
		strcpy(filefoldername, filepath);
		strcat(filefoldername, "\\");
		strcat(filefoldername, buffer);

		if (0!=access(filefoldername,0))
		{
			//�����ڴ��ļ��У����´���һ��
			mkdir(filefoldername);
			cout << "�����ļ��У�" << filefoldername << endl;
		}

		//��֪����ˣ������յ��˴���Ϣ
		strcpy(buffer, "CilentGet");	
		send(sock, buffer, sizeof(buffer), 0);

		//������һ����Ϣ���ж����ļ����������ļ���
		recv(sock, buffer, BUF_SIZE, 0);
		cout << "�յ���" << buffer << endl;
		for (int i = 0; buffer[i] != '\0'; i++)
		{
			if (buffer[i] == '.')
				flag = 1;
		}
	}
	while (flag == 1)		//���͵����ļ���
	{
		//�������·��ƴ�ӳɱ����ļ�·��
		char filename_path[200] = { 0 };
		strcpy(filename_path, filepath);
		strcat(filename_path, "\\");
		strcat(filename_path, buffer);

		FILE *fp = fopen(filename_path, "wb");  //�Զ����Ʒ�ʽ�򿪣��������ļ�
	
		//��֪����������յ����ļ���
		strcpy(buffer, "CilentGet");	
		send(sock, buffer, sizeof(buffer), 0);

		//ѭ���������ݣ�ֱ���ļ�������ϣ�ÿ�յ�һ���ļ����ݱ�ش�һ��"CilentRecivedDATA."��Ϣ��
		int nCount;
		while ((nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0)
		{
			if (strcmp(buffer, "ThisFileIsEnd.") == 0)
			{
				cout <<"�յ���"<< buffer << endl;
				strcpy(buffer, "CilentRecivedFile.");
				send(sock, buffer, sizeof(buffer), 0);
				break;
			}
			fwrite(buffer, nCount, 1, fp);
			strcpy(buffer, "CilentRecivedDATA.");
			send(sock, buffer, sizeof(buffer), 0);
		}
		fclose(fp);
		
		cout << filename_path << " Recived" << endl;
		
		recv(sock, buffer, BUF_SIZE, 0);	//���մ��ļ���������һ����Ϣ
		cout << "�յ���"<<buffer << endl;
		flag = 0;
		for (int i = 0; buffer[i] != '\0'; i++)	//�ж���Ϣ�Ƿ��.(�ļ���־)
		{
			if (buffer[i] == '.')
				flag = 1;
		}
		if (strcmp(buffer, "ThisSendIsEnd.") == 0)//�жϴ����Ƿ����
		{
			flag = 0;
			strcpy(buffer, "CilentRecivedAllFile.");
			send(sock,buffer, sizeof(buffer), 0);
		}
	}
	puts("�ļ�����ɹ�!");
	//�ļ�������Ϻ�ֱ�ӹر��׽��֣��������shutdown()
	closesocket(sock);
	WSACleanup();
	system("pause");
	return 0;
}