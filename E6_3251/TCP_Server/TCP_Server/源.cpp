#define _CRT_SECURE_NO_WARNINGS
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

char buffer[BUF_SIZE] = { 0 };  //������
int main() {

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
		sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		sockAddr.sin_port = htons(54321);

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
		SOCKADDR clntAddr;
		int nSize = sizeof(SOCKADDR);
		SOCKET clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &nSize);

		if (clntSock <= 0)
		{
			fprintf(stderr, "Accept Error:%s\n", strerror(errno));
		}
		while(1)
		{
			//�������ļ�����·��
			string in_filepath;
			printf("����Ҫ�����ļ���·��: ");
			getline(cin, in_filepath);
			const char *filepath;  //�ļ�����·��
			filepath = in_filepath.c_str();

			//���շ��͹�����ԭ�ļ�·��
			if (recv(clntSock, buffer, BUF_SIZE, 0) <= 0)
			{
				printf("�ͻ��˷�����%s�Ͽ����ӣ�\n",buffer);
				break;
			}
			cout << "�յ���" << buffer << endl;
			char filename[100] = {0};
			strcpy(filename,buffer);
			int flag = 1;
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
				send(clntSock, buffer, sizeof(buffer), 0);

				//ѭ���������ݣ�ֱ���ļ�������ϣ�ÿ�յ�һ���ļ����ݱ�ش�һ��"CilentRecivedDATA."��Ϣ��
				int nCount;
				while ((nCount = recv(clntSock, buffer, BUF_SIZE, 0)) > 0)
				{
					if (strcmp(buffer, "ThisFileIsEnd.") == 0)
					{
						cout << "�յ���" << buffer << endl;
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
					printf("�ͻ��˷�����%s�Ͽ����ӣ�\n", filename);
					break;
				}
				cout << filename_path << " Recived" << endl;

				if (recv(clntSock, buffer, BUF_SIZE, 0) <= 0)	//���մ��ļ���������һ����Ϣ
				{
					printf("�ͻ��˷�����%s�Ͽ����ӣ�\n", filename);
					break;
				}
				cout << "�յ���" << buffer << endl;
				if (strcmp(buffer, "CilentClosed.") == 0)//�жϴ����Ƿ����
				{
					flag = 0;
					strcpy(buffer, "ServerRecivedCloseRequest.");
					send(clntSock, buffer, sizeof(buffer), 0);
					puts("�ļ����ճɹ���");
				}
			}
			break;
		}
		shutdown(clntSock, SD_SEND);  //�ļ���ȡ��ϣ��Ͽ����������ͻ��˷���FIN��
		//recv(clntSock, buffer, BUF_SIZE, 0);  //�������ȴ��ͻ��˽������
		closesocket(clntSock);
		closesocket(servSock);
		WSACleanup();
		system("pause");
		return 0;
}