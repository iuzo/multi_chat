// 2019�� 1�б� ��Ʈ��ũ���α׷��� ���� 3��

// ����: ���ȯ �й�: 14011019

// �÷���:VS2017

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINE		1024
#define MAXCLIENT	5
#define	SERV_PORT	8787


int maxfdp1;
int chatuser;	// ä�� ������ ��
int chatusername;
unsigned int client_s[MAXCLIENT];

unsigned int client_names[100];
char client_name[100][20];
char *escape = "/quit";	// ���� ���


/* i��° ���� �������� */
void disconCli(int i)
{
	closesocket(client_s[i]);	// i��° ���� ����
	if (i == chatuser - 1) {
		client_s[i] = client_s[chatuser - 1];
		chatuser--;		// �������� ����
		printf("Now On the net %d users\n", chatuser);
	}
}


/* ���� ä�� �����ڼ��� �˻� */
int maxuser(int user)
{
	unsigned int max = user;
	int i;
	for (i = 0; i < chatuser; i++) {
		if (max < client_s[i])
			max = client_s[i];
	}
	return max;
}


int main()
{
	char readline[LINE];	// read buffer
	char readlineR[2];	// read buffer
	char readlineN[20];	// read buffer

	char *start = "Now you connected Server\n";		// ���Ӿ˸�
	int	i, j, n,n1,n2, addrlen,flag=0,k;
	SOCKET serversocket, clientsocket, clilen;		// ���� ����
	unsigned short	port;
	fd_set read_fds;
	struct sockaddr_in clientd_addr, server_addr;	// ���� �ּ�
	WSADATA wsa;		// �������� ���ϼ���
	int	iError;


	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {		// ���� �ʱ�ȭ
		exit(1);
	}

	if ((serversocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {		// ���� ����
		printf("fail make socket\n");
		exit(0);
	}

	port = SERV_PORT;
	ZeroMemory(&server_addr, sizeof(server_addr));		// �ʱ�ȭ
	server_addr.sin_family = AF_INET;			// ���� �ɼ� ����
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if (bind(serversocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("fail bind\n");
		exit(0);
	}

	listen(serversocket, SOMAXCONN);	// ���� ���, SOMAXCONN : �ִ� �����
	maxfdp1 = serversocket + 1;			// �ִ���� + 1

	while (1)
	{
		FD_ZERO(&read_fds);		// ���� ��ũ���� �ʱ�ȭ
		FD_SET(serversocket, &read_fds);
		for (i = 0; i < chatuser; i++) {
			FD_SET(client_s[i], &read_fds);
		}
		maxfdp1 = maxuser(serversocket) + 1;

		if (select(maxfdp1, &read_fds, (fd_set *)0, (fd_set *)0, NULL) < 0) {
			iError = WSAGetLastError();
			printf("fail select function\n");
			exit(0);
		}

		if (FD_ISSET(serversocket, &read_fds)) {
			// ���ϵ�ũ���Ϳ� ���� �����Ͱ� ������
			addrlen = sizeof(clientd_addr); // �ּұ��� ����
			// ���� ����
			clilen = sizeof(clientd_addr);
			clientsocket = accept(serversocket, (struct sockaddr *)&clientd_addr, &addrlen);

			if (clientsocket == -1) {
				printf("fail accept\n");
				exit(0);
			}

			client_s[chatuser] = clientsocket;
			chatuser++;

			send(clientsocket, start, strlen(start), 0);
			printf("%d user connected\n", chatuser);		// �������Ӿ˸�
		}

		// ��ε� ĳ����
		for (k = 0; k < chatuser; k++) {		// ������ �ִ� ��������ŭ
			memset(readlineR, '\0', 2);
			memset(readlineN, '\0', 20);
			memset(readline, '\0', LINE);

			if (FD_ISSET(client_s[k], &read_fds)) {		// ���� ��ũ���� ���� Ȯ��
				if ((n1 = recv(client_s[k], readlineR, 2, 0)) <= 0) {
					disconCli(k);
					continue;
				}if ((n2 = recv(client_s[k], readlineN, 20, 0)) <= 0) {
					disconCli(k);
					continue;
				}
				chatusername++;
				if ((n = recv(client_s[k], readline, LINE, 0)) <= 0) {
					disconCli(k);
					continue;
				}
				if (strstr(readline, escape)) {		// FIXME : ����� ������ ���� Ű���� ó�� ����
					disconCli(k); // ���� ���� ����
					continue;
				}


				readlineR[n1-1] = '\0';
				readline[n] = '\0';
				readlineN[n2-1] = '\0';
				flag = 0;
				for (i = 0; i < chatusername; i++)
				{
					printf("%s\n", client_name[i]);
					printf("5 %s %s\n", client_name[i], readlineN);
					if (strcmp(readline,"Display") == 0)
					{
						strcpy(readline, "");
						for (j = 0; j < chatusername; j++)
						{
							strcat(readline, client_name[j]);
							strcat(readline, " ");
						}
					}
					else if (strcmp(client_name[i], readlineN)==0 && client_names[i] == client_s[k])
					{
						chatusername--;
						break;
					}
					else
					{
						flag++;
					}
				}
				printf("%d %d\n",flag,chatusername);
				if (flag == chatusername)
				{
					client_names[flag - 1] = clientsocket;
					strcpy(client_name[flag-1],readlineN) ;
					printf("5 %s 5 %s\n", client_name[i], readlineN);
				}
				for (j = 0; j < chatuser; j++) {	// ��ε�ĳ����
					send(client_s[j], readlineR, n1, 0);
					send(client_s[j], readlineN, n2, 0);
					send(client_s[j], readline, strlen(readline), 0);
				}

				printf("%s\n", readline);
			}
		}
	}
	closesocket(serversocket);
	WSACleanup();
	return 0;
}