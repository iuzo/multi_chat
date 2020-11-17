// 2019년 1학기 네트워크프로그래밍 숙제 3번

// 성명: 배경환 학번: 14011019

// 플랫폼:VS2017

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LINE		1024
#define MAXCLIENT	5
#define	SERV_PORT	8787


int maxfdp1;
int chatuser;	// 채팅 참가자 수
int chatusername;
unsigned int client_s[MAXCLIENT];

unsigned int client_names[100];
char client_name[100][20];
char *escape = "/quit";	// 종료 명령


/* i번째 유저 계정삭제 */
void disconCli(int i)
{
	closesocket(client_s[i]);	// i번째 소켓 닫음
	if (i == chatuser - 1) {
		client_s[i] = client_s[chatuser - 1];
		chatuser--;		// 총유저수 줄임
		printf("Now On the net %d users\n", chatuser);
	}
}


/* 현재 채팅 참가자수를 검색 */
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

	char *start = "Now you connected Server\n";		// 접속알림
	int	i, j, n,n1,n2, addrlen,flag=0,k;
	SOCKET serversocket, clientsocket, clilen;		// 소켓 변수
	unsigned short	port;
	fd_set read_fds;
	struct sockaddr_in clientd_addr, server_addr;	// 소켓 주소
	WSADATA wsa;		// 윈도우즈 소켓선언
	int	iError;


	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {		// 소켓 초기화
		exit(1);
	}

	if ((serversocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {		// 소켓 생성
		printf("fail make socket\n");
		exit(0);
	}

	port = SERV_PORT;
	ZeroMemory(&server_addr, sizeof(server_addr));		// 초기화
	server_addr.sin_family = AF_INET;			// 소켓 옵션 지정
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if (bind(serversocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("fail bind\n");
		exit(0);
	}

	listen(serversocket, SOMAXCONN);	// 연결 대기, SOMAXCONN : 최대 연결수
	maxfdp1 = serversocket + 1;			// 최대소켓 + 1

	while (1)
	{
		FD_ZERO(&read_fds);		// 파일 디스크립터 초기화
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
			// 파일디스크립터에 읽을 데이터가 있으면
			addrlen = sizeof(clientd_addr); // 주소길이 지정
			// 소켓 연결
			clilen = sizeof(clientd_addr);
			clientsocket = accept(serversocket, (struct sockaddr *)&clientd_addr, &addrlen);

			if (clientsocket == -1) {
				printf("fail accept\n");
				exit(0);
			}

			client_s[chatuser] = clientsocket;
			chatuser++;

			send(clientsocket, start, strlen(start), 0);
			printf("%d user connected\n", chatuser);		// 유저접속알림
		}

		// 브로드 캐스팅
		for (k = 0; k < chatuser; k++) {		// 접속해 있는 유저수만큼
			memset(readlineR, '\0', 2);
			memset(readlineN, '\0', 20);
			memset(readline, '\0', LINE);

			if (FD_ISSET(client_s[k], &read_fds)) {		// 파일 디스크립터 버퍼 확인
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
				if (strstr(readline, escape)) {		// FIXME : 사용자 삭제를 위한 키워드 처리 수정
					disconCli(k); // 유저 계정 삭제
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
				for (j = 0; j < chatuser; j++) {	// 브로드캐스팅
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