// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "List.h"
#pragma comment(lib, "ws2_32.lib")

DWORD WINAPI ThreadProc(LPVOID lParam);


SOCKET sListen;
sockaddr_in sClient;
ADDR *pHead;
ADDR *pEnd;
char g_szBuf[255] = "";
bool bFlage = false;

int main(int argc, char* argv[])
{
	WSAData wd;
	WSAStartup(0x0202, &wd);

	sListen = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.S_un.S_addr = inet_addr("192.168.149.1");
	sin.sin_port = htons(5052);
	
	if (SOCKET_ERROR == bind(sListen, (SOCKADDR*)&sin, sizeof(sin)))
	{
		closesocket(sListen);
		return -1;
	}

	while (1)
	{
		int nLen = sizeof(sClient);
		char szBuf2[255] = "";
		char szBuf1[255] = "";
		recvfrom(sListen, szBuf1, 255, 0, (SOCKADDR*)&sClient, &nLen);

		if (szBuf1[0] == '\0')
		{
			continue;
		}

		bFlage = true;

		strcpy(g_szBuf, szBuf1);
		
		if (NULL == pHead)
		{
			ADDR *pNew1 = new ADDR;
			pNew1->m_addr = sClient;
			pHead = pNew1;
			pEnd = pNew1;
			pNew1->m_pNext = NULL;
			pNew1->m_pLast = NULL;
			HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, NULL, NULL);
			CloseHandle(hThread);
		}

		ADDR *pTemp = pHead;

		if (strcmp("&***&Bye&***&", szBuf1) && strcmp("&***&Enter&***&", szBuf1))
		{
			cout<<inet_ntoa(sClient.sin_addr)<<" Say"<<endl;
		}

		if (!strcmp("&***&Enter&***&", szBuf1))
		{
			cout<<inet_ntoa(sClient.sin_addr)<<" Enter"<<endl;
		}

		if (!strcmp("&***&Bye&***&", szBuf1))
		{
			while (NULL != pTemp)
			{
				if (!strcmp(inet_ntoa(pTemp->m_addr.sin_addr), inet_ntoa(sClient.sin_addr)) && pTemp->m_addr.sin_port == sClient.sin_port)
				{
					cout<<inet_ntoa(sClient.sin_addr)<<" Away"<<endl;
					sendto(sListen, inet_ntoa(pTemp->m_addr.sin_addr), strlen(inet_ntoa(pTemp->m_addr.sin_addr)), 0, (SOCKADDR*)&pTemp->m_addr, sizeof(pTemp->m_addr));

					if (pTemp == pHead)
					{
						if (pTemp == pEnd)
						{
							pHead = NULL;
							pEnd = NULL;
							delete pTemp;
							break;
						}

						pTemp->m_pNext->m_pLast = NULL;
						pHead = pTemp->m_pNext;
					}
					else
					{
						pTemp->m_pLast->m_pNext = pTemp->m_pNext;
						
						if (pTemp == pEnd)
						{
							pEnd = pTemp->m_pLast;
							delete pTemp;
							break;
						}

						pTemp->m_pNext->m_pLast = pTemp->m_pLast;
					}

					delete pTemp;
					break;
				}
				
				pTemp = pTemp->m_pNext;
			}
		}
		else
		{
			while (NULL != pTemp)
			{
				if (!strcmp(inet_ntoa(sClient.sin_addr), inet_ntoa(pTemp->m_addr.sin_addr)) && pTemp->m_addr.sin_port == sClient.sin_port)
				{
					break;
				}

				pTemp = pTemp->m_pNext;

				if(NULL == pTemp)
				{
					ADDR *pNew = new ADDR;
					pNew->m_addr = sClient;
					pNew->m_pNext = NULL;
					pEnd->m_pNext = pNew;
					pNew->m_pLast = pEnd;
					pEnd = pNew;
					HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, NULL, NULL);
					CloseHandle(hThread);
				}
			}
		}
	}
		
	WSACleanup();

	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lParam)
{
	sockaddr_in sin = sClient;
	int i = 1;

	while (i)
	{
		if (bFlage)
		{
			if (g_szBuf[0] == '\0')
			{
				continue;
			}
			
			char szBuf2[255] = "";
			strcat(szBuf2, inet_ntoa(sin.sin_addr));
			strcat(szBuf2, " : ");
			
			if (!strcmp("&***&Enter&***&", g_szBuf))
			{
				strcpy(szBuf2, inet_ntoa(sClient.sin_addr));
				strcat(szBuf2, "¿¥¡À");
			}	
			else if (!strcmp("&***&Bye&***&", g_szBuf))
			{
				if (!strcmp(inet_ntoa(sin.sin_addr), inet_ntoa(sClient.sin_addr)) && sin.sin_port == sClient.sin_port)
				{
					i = 0;
				}

				strcpy(szBuf2, inet_ntoa(sClient.sin_addr));
				strcat(szBuf2, "¡Ô¡À");
			}
			else
			{
				strcat(szBuf2, g_szBuf);
			}
			
			sendto(sListen, szBuf2, strlen(szBuf2), 0, (SOCKADDR*)&sin, sizeof(sin));
			bFlage = false;
		}
		
	}
	
	return 0;
}