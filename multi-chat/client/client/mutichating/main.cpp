// 2019�� 1�б� ��Ʈ��ũ���α׷��� ���� 3��

// ����: ���ȯ �й�: 14011019

// �÷���:VS2017

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
#define _CRT_SECURE_NO_WARNINGS //sprintf ��������
#define BUFSIZE    512

char userID[10];		// ����ID
char line[LINE], chatdata[LINE + 1];
struct sockaddr_in server_addr;
SOCKET serversocket;	// ���� ���� ����
char *escape = "/quit";	// ���� ���
BOOL	bIsQuit;		// ���� flag

DWORD WINAPI ProcessInputSend(LPVOID arg);
char* timeToString(struct tm *t);
// ��ȭ���� ���ν���
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// ���� ��Ʈ�� ��� �Լ�
void DisplayText(char *fmt, ...);
// ���� ��� �Լ�
void err_quit(char *msg);
void err_display(char *msg);
// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char *buf, int len, int flags);
// ���� ��� ������ �Լ�
DWORD WINAPI ClientMain(LPVOID arg);
DWORD WINAPI Receiver(LPVOID arg);
SOCKET sock; // ����
char sendbuf[BUFSIZE + 1]; // ������ �ۼ��� ����
char sendbuf2[BUFSIZE + 1]; // ������ �ۼ��� ����
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
HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ
HWND hSendButton; // ������ ��ư
HWND hSendButton2; // ������ ��ư
HWND hSendButtonip; // ������ ��ư
HWND hSendButtonport; // ������ ��ư
HWND hSendButtonname; // ������ ��ư
HWND hSendButtonname2; // ������ ��ư
HWND hEdit1, hEdit2; // ���� ��Ʈ��
HWND hEdit3, hEdit4; // ���� ��Ʈ��
HWND hEditip; // ���� ��Ʈ��
HWND hEditname; // ���� ��Ʈ��
HWND hEditname2; // ���� ��Ʈ��
HWND hEditport; // ���� ��Ʈ��

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return 1;

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);
	// ��ȭ���� ����
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

	// �̺�Ʈ ����
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);

	// closesocket()
	closesocket(sock);

	// ���� ����
	WSACleanup();

	return 0;
}

// ��ȭ���� ���ν���
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

		EnableWindow(hSendButton, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hSendButton2, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hSendButtonport, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hSendButtonname, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hSendButtonname2, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hEdit1, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hEdit3, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hEditname, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hEditname2, FALSE); // ������ ��ư ��Ȱ��ȭ
		EnableWindow(hEditport, FALSE); // ������ ��ư ��Ȱ��ȭ
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			k = 1;
			EnableWindow(hSendButton, FALSE); // ������ ��ư ��Ȱ��ȭ
			WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_EDIT1, sendbuf, BUFSIZE + 1);
			SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
			SetFocus(hEdit1);
			SendMessage(hEdit1, EM_SETSEL, 0, -1);
			
			return TRUE;
		case IDC_IDOK2:
			k = 2;
			EnableWindow(hSendButton2, FALSE); // ������ ��ư ��Ȱ��ȭ
			WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_EDIT4, sendbuf2, BUFSIZE + 1);
			SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
			SetFocus(hEdit3);
			SendMessage(hEdit3, EM_SETSEL, 0, -1);
			
			return TRUE;
		case IDC_ipOK:
			WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_ip, ip, 20);
			SetEvent(hWriteEvent); // ���� �Ϸ� �˸���

			
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

				ptr = strtok(temp, ".");      // " " ���� ���ڸ� �������� ���ڿ��� �ڸ�, ������ ��ȯ
				while (ptr != NULL)               // �ڸ� ���ڿ��� ������ ���� ������ �ݺ�
				{
					iparr[i] = ptr;
					ptr = strtok(NULL, ".");      // ���� ���ڿ��� �߶� �����͸� ��ȯ	
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
					MessageBox(hDlg, "�ǹٸ� ip������ �ƴմϴ�.", "�޽��� �ڽ�", MB_OK);
				}
			}
			else
			{
				MessageBox(hDlg, "�ǹٸ� ip������ �ƴմϴ�.", "�޽��� �ڽ�", MB_OK);
			}
			return TRUE;
		case IDC_portOK:
			WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_port, hport, 10);
			SetEvent(hWriteEvent); // ���� �Ϸ� �˸���

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
						EnableWindow(hSendButtonport, FALSE); //  ��ư ��Ȱ��ȭ
						SetFocus(hEditname);
						EnableWindow(hEditname, TRUE); // ������ ��ư ��Ȱ��ȭ
						EnableWindow(hSendButtonname, TRUE); // ������ ��ư Ȱ��ȭ
						EnableWindow(hEditname2, TRUE); // ������ ��ư ��Ȱ��ȭ
						EnableWindow(hSendButtonname2, TRUE); // ������ ��ư Ȱ��ȭ
						EnableWindow(hEditport, FALSE); // ������ ��ư ��Ȱ��ȭ
						SendMessage(hEditport, EM_SETSEL, 0, -1);
					}
					else
					{

						MessageBox(hDlg, "�ǹٸ� port������ �ƴմϴ�.", "�޽��� �ڽ�", MB_OK);
					}
				}
				else
				{

					MessageBox(hDlg, "�ǹٸ� port������ �ƴմϴ�.", "�޽��� �ڽ�", MB_OK);
				}
			}
			else
			{
				MessageBox(hDlg, "�ǹٸ� port������ �ƴմϴ�.", "�޽��� �ڽ�", MB_OK);
			}
			flag = 0;
			return TRUE;
		case IDC_nameOK:
			k = 1;
			EnableWindow(hSendButtonname, FALSE); //  ��ư ��Ȱ��ȭ
			WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_name, hname, 20);
			SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
			SetFocus(hEdit1);
			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			EnableWindow(hEdit1, TRUE); // ������ ��ư ��Ȱ��ȭ
			EnableWindow(hEditname, FALSE); // ������ ��ư ��Ȱ��ȭSendMessage(hEditname, EM_SETSEL, 0, -1);
			SendMessage(hEditname, EM_SETSEL, 0, -1);
			
			return TRUE;
		case IDC_nameOK2:
			k = 2;
			EnableWindow(hSendButtonname2, FALSE); //  ��ư ��Ȱ��ȭ
			WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
			GetDlgItemText(hDlg, IDC_name2, hname2, 20);
			SetEvent(hWriteEvent); // ���� �Ϸ� �˸���
			SetFocus(hEdit3);
			EnableWindow(hSendButton2, TRUE); // ������ ��ư Ȱ��ȭ
			EnableWindow(hEdit3, TRUE); // ������ ��ư ��Ȱ��ȭ
			EnableWindow(hEditname2, FALSE); // ������ ��ư ��Ȱ��ȭSendMessage(hEditname, EM_SETSEL, 0, -1);
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

// ���� ��Ʈ�� ��� �Լ�
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

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

// ����� ���� ������ ���� �Լ�
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

// TCP Ŭ���̾�Ʈ ���� �κ�
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	// ���� �ʱ�ȭ
	while (1)
	{
		WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���

			// ���ڿ� ���̰� 0�̸� ������ ����
		if (k == 1) {
			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent); // �б� �Ϸ� �˸���
			break;
		}
		else if (k == 2)
		{
			EnableWindow(hSendButton2, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent); // �б� �Ϸ� �˸���
			break;
		}
		SetEvent(hReadEvent); // �б� �Ϸ� �˸���

	}

	WSADATA wsa;

	HANDLE hThread; // ������
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
	server_addr.sin_family = AF_INET; // ���� ���ڵ� ����
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
		DisplayText("�ѹ��� �Է��� �ּ���.\r\n");
	}
	else if (k == 2)
	{

		DisplayText2("�ѹ��� �Է��� �ּ���.\r\n");
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
		WaitForSingleObject(hWriteEvent, INFINITE); // ���� �Ϸ� ��ٸ���


		if (strlen(sendbuf) == 0 && k == 1) {

			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent); // �б� �Ϸ� �˸���
			continue;
		}
		else if(k == 2 && strlen(sendbuf2) == 0)
		{
			EnableWindow(hSendButton2, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent); // �б� �Ϸ� �˸���
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
			EnableWindow(hSendButton, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent);
		}
		else if (k == 2)
		{
			EnableWindow(hSendButton2, TRUE); // ������ ��ư Ȱ��ȭ
			SetEvent(hReadEvent);
		}


		/*
		if (fgets(chatdata, LINE, stdin)) {		// ��Ʈ�� read
			chatdata[strlen(chatdata) - 1] = '\0';
			sprintf(line, "\n[%s] : %s", userID, chatdata);		// ������ ù ������ ���
			if (send(serversocket, line, strlen(line), 0) < 0) {
				printf("Write fail\n");
			}
			if (strstr(line, escape) != 0) {		// �����ɾ�
				printf("Bye\n");
				closesocket(serversocket);
				bIsQuit = TRUE;
				exit(0);
			}
		}*/
	}
	return 0;
}