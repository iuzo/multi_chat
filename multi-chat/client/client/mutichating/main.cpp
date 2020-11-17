// 2019년 1학기 네트워크프로그래밍 숙제 3번

// 성명: 배경환 학번: 14011019

// 플랫폼:VS2017

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"
#include "time.h"
#include <string.h>

#define  LINE 1024

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS //sprintf 에러위함
#define BUFSIZE    512

char userID[10];		// 유저ID
char line[LINE], chatdata[LINE + 1];
struct sockaddr_in server_addr;
SOCKET serversocket;	// 서버 연결 소켓
char *escape = "/quit";	// 종료 명령
BOOL	bIsQuit;		// 종료 flag

DWORD WINAPI ProcessInputSend(LPVOID arg);
char* timeToString(struct tm *t);
// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI Receiver(LPVOID arg);
SOCKET sock; // 소켓
char sendbuf[BUFSIZE + 1]; // 데이터 송수신 버퍼
char sendbuf2[BUFSIZE + 1]; // 데이터 송수신 버퍼
char ip[20];
char hname[20];
char hname2[20];

char hport[10];
int k = 0;
int namechangei = 0;
int kk = 0;
int kkk = 0;
int kkkk = 0;
int namechange = 0;
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hSendButton2; // 보내기 버튼
HWND hSendButtonip; // 보내기 버튼
HWND hSendButtonport; // 보내기 버튼
HWND hSendButtonname; // 보내기 버튼
HWND hSendButtonname2; // 보내기 버튼
HWND hEdit1, hEdit2; // 편집 컨트롤
HWND hEdit3, hEdit4; // 편집 컨트롤
HWND hEditip; // 편집 컨트롤
HWND hEditname; // 편집 컨트롤
HWND hEditname2; // 편집 컨트롤
HWND hEditport; // 편집 컨트롤

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;

	// 소켓 통신 스레드 생성
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
	// 대화상자 생성
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// 이벤트 제거
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char temp[20];
	char *ptr;
	int i,j;
	char *iparr[4];
	char portarr[5];
	int flag = 0;
	int ip1;
	int ip2;
	int ip3;
	int ip4;
	switch (uMsg) {
	case WM_INITDIALOG:
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		hEdit4 = GetDlgItem(hDlg, IDC_EDIT3);
		hEdit3 = GetDlgItem(hDlg, IDC_EDIT4);
		hEditip = GetDlgItem(hDlg, IDC_ip);
		hEditname = GetDlgItem(hDlg, IDC_name);
		hEditname2 = GetDlgItem(hDlg, IDC_name2);
		hEditport = GetDlgItem(hDlg, IDC_port);

		hSendButton = GetDlgItem(hDlg, IDOK);
		hSendButton2 = GetDlgItem(hDlg, IDC_IDOK2);
		hSendButtonip = GetDlgItem(hDlg, IDC_ipOK);
		hSendButtonport = GetDlgItem(hDlg, IDC_portOK);
		hSendButtonname = GetDlgItem(hDlg, IDC_nameOK);
		hSendButtonname2 = GetDlgItem(hDlg, IDC_nameOK2);

		EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hSendButton2, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hSendButtonport, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hSendButtonname, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hSendButtonname2, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hEdit1, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hEdit3, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hEditname, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hEditname2, FALSE); // 보내기 버튼 비활성화
		EnableWindow(hEditport, FALSE); // 보내기 버튼 비활성화
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			k = 1;
			EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_EDIT1, sendbuf, BUFSIZE + 1);
			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			SetFocus(hEdit1);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			
			return TRUE;
		case IDC_IDOK2:
			k = 2;
			EnableWindow(hSendButton2, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_EDIT4, sendbuf2, BUFSIZE + 1);
			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			SetFocus(hEdit3);
			SendMessage(hEdit3, EM_SETSEL, 0, -1);
			
			return TRUE;
		case IDC_ipOK:
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_ip, ip, 20);
			SetEvent(hWriteEvent); // 쓰기 완료 알리기

			
			strcpy(temp, ip);
			
			j = 0;
			for (i = 0; i < strlen(temp); i++)
			{
				
				if ((temp[i] >= '0'&&temp[i] <= '9'))
				{
					j++;
				}
				else if (temp[i] == '.')
				{
					flag++;
				}
				
			}
			i = 0;

			if (j== (strlen(temp)-3)&&flag == 3)
			{

				ptr = strtok(temp, ".");      // " " 공백 문자를 기준으로 문자열을 자름, 포인터 반환
				while (ptr != NULL)               // 자른 문자열이 나오지 않을 때까지 반복
				{
					iparr[i] = ptr;
					ptr = strtok(NULL, ".");      // 다음 문자열을 잘라서 포인터를 반환	
					i++;
				}

				ip1 = atoi(iparr[0]);
				ip2 = atoi(iparr[1]);
				ip3 = atoi(iparr[2]);
				ip4 = atoi(iparr[3]);

				if (ip1 <= 255 && ip1 >= 0 && ip2 >= 0 && ip3 >= 0 && ip4 >= 0 && ip2 <= 255 && ip3 <= 255 && ip4 <= 255 && i == 4)
				{
					EnableWindow(hEditip, FALSE);
					EnableWindow(hSendButtonip, FALSE);
					SetFocus(hEditport);
					EnableWindow(hSendButtonport, TRUE);
					EnableWindow(hEditport, TRUE);
					SendMessage(hEditip, EM_SETSEL, 0, -1);
				}
				else
				{
					MessageBox(hDlg, "옳바른 ip형식이 아닙니다.", "메시지 박스", MB_OK);
				}
			}
			else
			{
				MessageBox(hDlg, "옳바른 ip형식이 아닙니다.", "메시지 박스", MB_OK);
			}
			return TRUE;
		case IDC_portOK:
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_port, hport, 10);
			SetEvent(hWriteEvent); // 쓰기 완료 알리기

			if (strlen(hport) <= 5 && strlen(hport) >= 4)
			{
				for (i = 0; i < strlen(hport); i++)
				{
					if (hport[i] >= '0'&&hport[i] <= '9')
					{
						flag++;
					}

				}
				if (flag == strlen(hport))
				{

					if (atoi(hport) > 1023 && atoi(hport) < 49152)
					{
						EnableWindow(hSendButtonport, FALSE); //  버튼 비활성화
						SetFocus(hEditname);
						EnableWindow(hEditname, TRUE); // 보내기 버튼 비활성화
						EnableWindow(hSendButtonname, TRUE); // 보내기 버튼 활성화
						EnableWindow(hEditname2, TRUE); // 보내기 버튼 비활성화
						EnableWindow(hSendButtonname2, TRUE); // 보내기 버튼 활성화
						EnableWindow(hEditport, FALSE); // 보내기 버튼 비활성화
						SendMessage(hEditport, EM_SETSEL, 0, -1);
					}
					else
					{

						MessageBox(hDlg, "옳바른 port형식이 아닙니다.", "메시지 박스", MB_OK);
					}
				}
				else
				{

					MessageBox(hDlg, "옳바른 port형식이 아닙니다.", "메시지 박스", MB_OK);
				}
			}
			else
			{
				MessageBox(hDlg, "옳바른 port형식이 아닙니다.", "메시지 박스", MB_OK);
			}
			flag = 0;
			return TRUE;
		case IDC_nameOK:
			k = 1;
			EnableWindow(hSendButtonname, FALSE); //  버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_name, hname, 20);
			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			SetFocus(hEdit1);
			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			EnableWindow(hEdit1, TRUE); // 보내기 버튼 비활성화
			EnableWindow(hEditname, FALSE); // 보내기 버튼 비활성화SendMessage(hEditname, EM_SETSEL, 0, -1);
			SendMessage(hEditname, EM_SETSEL, 0, -1);
			
			return TRUE;
		case IDC_nameOK2:
			k = 2;
			EnableWindow(hSendButtonname2, FALSE); //  버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_name2, hname2, 20);
			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			SetFocus(hEdit3);
			EnableWindow(hSendButton2, TRUE); // 보내기 버튼 활성화
			EnableWindow(hEdit3, TRUE); // 보내기 버튼 비활성화
			EnableWindow(hEditname2, FALSE); // 보내기 버튼 비활성화SendMessage(hEditname, EM_SETSEL, 0, -1);
			SendMessage(hEditname2, EM_SETSEL, 0, -1);
			
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;

		}
		return FALSE;
	}
	return FALSE;
}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}
void DisplayText2(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE + 256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit4);
	SendMessage(hEdit4, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit4, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	// 윈속 초기화
	while (1)
	{
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

			// 문자열 길이가 0이면 보내지 않음
		if (k == 1) {
			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent); // 읽기 완료 알리기
			break;
		}
		else if (k == 2)
		{
			EnableWindow(hSendButton2, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent); // 읽기 완료 알리기
			break;
		}
		SetEvent(hReadEvent); // 읽기 완료 알리기

	}

	WSADATA wsa;

	HANDLE hThread; // 스레드
	DWORD ThreadId;
	int size;
	char sname[20];
	bIsQuit = FALSE;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		exit(1);

	// socket()
	if ((serversocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("fail make socket\n");
		exit(0);
	}
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET; // 소켓 인자들 설정
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(atoi(hport));

	if (connect(serversocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		printf("fail connect 2 server\n");
		exit(0);
	}
	else {
		printf("connected server\n");
	}
	if (k == 1)
	{
		DisplayText("한번더 입력해 주세요.\r\n");
	}
	else if (k == 2)
	{

		DisplayText2("한번더 입력해 주세요.\r\n");
	}

	hThread = CreateThread(NULL, 0, ProcessInputSend, 0, 0, &ThreadId);
	if (hThread == NULL) {
		printf("fail make thread\n");
	}
	else {
		CloseHandle(hThread);
	}

	while (!bIsQuit) {
		ZeroMemory(chatdata, sizeof(chatdata));
		if ((size = recv(serversocket, chatdata, LINE, 0)) == INVALID_SOCKET) {
			printf("recv ERROR\n");
			exit(0);
		}

		if (strcmp(chatdata, "1") == 0)
		{
			if ((size = recv(serversocket, sname, 20, 0)) == INVALID_SOCKET) {
				printf("recv ERROR\n");
				exit(0);
			}
			if ((size = recv(serversocket, chatdata, LINE, 0)) == INVALID_SOCKET) {
				printf("recv ERROR\n");
				exit(0);
			}
			DisplayText(strcat(strcat(sname," : "), strcat(chatdata, "\r\n")));
		}
		else if(strcmp(chatdata, "2") == 0)
		{
			if ((size = recv(serversocket, sname, 20, 0)) == INVALID_SOCKET) {
				printf("recv ERROR\n");
				exit(0);
			}
			if ((size = recv(serversocket, chatdata, LINE, 0)) == INVALID_SOCKET) {
				printf("recv ERROR\n");
				exit(0);
			}

			DisplayText2(strcat(strcat(sname, " : "), strcat(chatdata, "\r\n")));
		}
		
	}

	closesocket(serversocket);
	WSACleanup();

}

DWORD WINAPI ProcessInputSend(LPVOID arg)
{

	while (TRUE) {
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기


		if (strlen(sendbuf) == 0 && k == 1) {

			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent); // 읽기 완료 알리기
			continue;
		}
		else if(k == 2 && strlen(sendbuf2) == 0)
		{
			EnableWindow(hSendButton2, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent); // 읽기 완료 알리기
			continue;
		}

		if (k == 1)
		{
			if (send(serversocket, "1", 2, 0) < 0) {
				printf("Write fail\n");
			}
			if (send(serversocket, hname, 20, 0) < 0) {
				printf("Write fail\n");
			}
			if (send(serversocket, sendbuf, strlen(sendbuf), 0) < 0) {
				printf("Write fail\n");
			}
		}
		else if (k == 2)
		{
			if (send(serversocket, "2", 2, 0) < 0) {
				printf("Write fail\n");
			}
			if (send(serversocket, hname2, 20, 0) < 0) {
				printf("Write fail\n");
			}
			if (send(serversocket, sendbuf2, strlen(sendbuf2), 0) < 0) {
				printf("Write fail\n");
			}
		}

		if (k == 1)
		{
			EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent);
		}
		else if (k == 2)
		{
			EnableWindow(hSendButton2, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent);
		}


		/*
		if (fgets(chatdata, LINE, stdin)) {		// 스트림 read
			chatdata[strlen(chatdata) - 1] = '\0';
			sprintf(line, "\n[%s] : %s", userID, chatdata);		// 버퍼의 첫 내용을 출력
			if (send(serversocket, line, strlen(line), 0) < 0) {
				printf("Write fail\n");
			}
			if (strstr(line, escape) != 0) {		// 종료명령어
				printf("Bye\n");
				closesocket(serversocket);
				bIsQuit = TRUE;
				exit(0);
			}
		}*/
	}
	return 0;
}