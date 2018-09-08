// Cilent.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "../resource.h"
#pragma comment(lib, "ws2_32.lib")

BOOL CALLBACK DigProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadProc(LPVOID lParam);

SOCKET sClient;
sockaddr_in sin;
HANDLE hThread;
HWND g_hWnd;
DWORD Exit;
BOOL g_bFlage = TRUE;
BOOL bExit = TRUE;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	WSAData wd;
	WSAStartup(0x0202, &wd);

	sClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	sin.sin_family = AF_INET;
	sin.sin_addr.S_un.S_addr = inet_addr("192.168.149.1");
	sin.sin_port = htons(5052);
	bind(sClient, (SOCKADDR*)&sin, sizeof(sin));

	hThread = CreateThread(NULL, 0, ThreadProc, NULL, NULL, NULL);

	DialogBox((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), NULL, DigProc);

	WSACleanup();

	return 0;
}

BOOL CALLBACK DigProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			{
				HWND hEditInput = GetDlgItem(hWnd, IDC_INPUT);
				HWND hEditShow = GetDlgItem(hWnd, IDC_SHOW);
				SendMessage(hEditInput, EM_LIMITTEXT,(WPARAM)76, 0);
				EnableWindow(hEditInput, FALSE);
				EnableWindow(hEditShow, FALSE);
			}
			break;
		case WM_COMMAND:
			{
				if (BN_CLICKED == HIWORD(wParam))
				{
					if (IDC_BUTTON1 == LOWORD(wParam))
					{
						HWND hEditInput = GetDlgItem(hWnd, IDC_INPUT);
						HWND hEditShow = GetDlgItem(hWnd, IDC_SHOW);
						EnableWindow(hEditInput, TRUE);
						EnableWindow(hEditShow, TRUE);
						g_hWnd = hWnd;
						char szBuf[255] = "";
 						GetDlgItemText(hWnd, IDC_INPUT, szBuf, 255);
						sendto(sClient, szBuf, strlen(szBuf), 0, (SOCKADDR*)&sin, sizeof(sin));	
						HWND hButton = GetDlgItem(hWnd, IDC_BUTTON1);
						SetWindowText(hButton, "发送");

						if (g_bFlage)
						{
							sendto(sClient, "&***&Enter&***&", strlen("&***&Enter&***&"), 0, (SOCKADDR*)&sin, sizeof(sin));
							g_bFlage = FALSE;
						}
						
						SetWindowText(hWnd, "啊哈(已连接)");
						SetDlgItemText(hWnd, IDC_INPUT, "");
					}
				}
			}
			break;	
		case WM_CLOSE:
			{
				bExit = FALSE;
				sendto(sClient, "&***&Bye&***&", strlen("&***&Bye&***&"), 0, (SOCKADDR*)&sin, sizeof(sin));
				closesocket(sClient);
				EndDialog(hWnd, WM_CLOSE);
			}	
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

DWORD WINAPI ThreadProc(LPVOID lParam)
{
	while (bExit)
	{
		sockaddr_in sListen;
		int nLen = sizeof(sListen);
		char szBuf[255] = "";

		if (0 >= recvfrom(sClient, szBuf, 255, 0, (SOCKADDR*)&sListen, &nLen))
		{
			continue;
		}
		
		HWND hEdit = GetDlgItem(g_hWnd, IDC_SHOW);
		SendMessageA(hEdit, EM_SETSEL, -2, -1);
		SendMessageA(hEdit, EM_REPLACESEL, TRUE, (long)szBuf);
		SendMessageA(hEdit, EM_REPLACESEL, TRUE, (long)"\r\n\r\n");
		SendMessageA(hEdit, WM_VSCROLL, SB_BOTTOM, 0);
	}

	return 0;
}