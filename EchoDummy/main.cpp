#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <Windows.h>

#include "network.h"
#include "CDummy.h"
#include "CPacket.h"

int g_threadCnt = 1;
int g_dummyCnt = 1;
uintptr_t g_data;
int g_overlapCnt;
int g_kind;


CDummy g_dummy [5000];

unsigned int MonitoringThread(LPVOID param)
{
	//DWORD first = timeGetTime();
	int c = g_dummyCnt;
	int t = g_threadCnt;
	int o = g_overlapCnt;
	for (;;)
	{
		int total = 0;
		for (int i = 0; i < c * t; ++i)
		{
			total += g_dummy[i]._waitCount;
		}
		printf("===================================================\n");
		printf("Client: %d | Thread: %d | overlap: %d\n", c, t, o);
		printf("===================================================\n");

		printf("Wait Echo Count: %d\n", total);
		//printf("Max Laytency: %dms\n\n", max);
		
		printf("Connect\n");
		printf("Connect Total : %llu\n", g_connectTotalCnt);
		printf("Connect Success : %llu\n", g_connectSuccessCnt);

		printf("Error\n");
		printf("Connect Fail : %llu\n", g_connectFailCnt);
		printf("Disconnect   : %llu\n", g_disconnectCnt);
		printf("packet       : %llu\n", g_packetCnt);

		printf("packet Use    : %d\n", CPacket::GetUseNode());
		printf("SendPacket TPS: %llu\n", g_sendCnt);
		printf("RecvPacket TPS: %llu\n", g_recvCnt);
		Sleep(1000);
	}
}

unsigned int RecvThread(LPVOID param)
{
	FD_SET readFds, exceptFds;
	FD_ZERO(&readFds);
	FD_ZERO(&exceptFds);

	int overlapCnt = g_overlapCnt;
	int headerSize = 2;

	CDummy* pDummy[50];
	
	for (int i = 0; i < g_dummyCnt; ++i)
	{
		pDummy[i] = &g_dummy[i];
		pDummy[i]->Init(1);
		pDummy[i]->Connect();
	}

	for (;;)
	{
		for (int i = 0; i < g_dummyCnt; ++i)
		{
			InterlockedIncrement(&pDummy[i] ->_refCount);
			FD_SET(pDummy[i]->_serverSocket, &readFds);
			FD_SET(pDummy[i]->_serverSocket, &exceptFds);
		}

		auto responseCount = select(0, &readFds, nullptr, &exceptFds, nullptr);
		if (responseCount > 0)
		{
			for (int i = 0; i < g_dummyCnt; ++i)
			{
				char buf[801];
				bool flag = false;
				if (FD_ISSET(pDummy[i]->_serverSocket, &readFds))
				{
					int ret = recv(pDummy[i]->_serverSocket, buf, 800, 0);
					if (ret > 0)
					{
						printf("ret:%d\n", ret);
						buf[ret] = 0;
						pDummy[i]->_recvBuffer.Enqueue(buf, ret);

						USHORT len; 
						long long data;
						LONG64 cc = ret / 10;
						while (ret > 0)
						{
							CPacket* pPacket = CPacket::Alloc();
							pPacket->AddRef();
							int peekSize = pDummy[i]->_recvBuffer.Peek((char*)&len, sizeof(USHORT));
							if (peekSize != headerSize)
							{
								pPacket->subRef();
								break;
							}
							if (len != 8)
							{
								printf("len is not 8\n");
								DebugBreak();
							}
							if (ret < headerSize + len)
							{
								pPacket->subRef();
								break;
							}
							pDummy[i]->_recvBuffer.Dequeue(pPacket->buffer, headerSize + len);
							*pPacket >> data;
							if (!((data <= pDummy[i]->curData) && data > (pDummy[i]->curData - overlapCnt)))
							{
								printf("data Error%d\n", data);
								pDummy[i]->_errorFlag = true;
							}
							ret -= 10;
							pPacket->subRef();
						}
						auto wc = InterlockedAdd64((LONG64 *)&pDummy[i]->_waitCount, -cc);
						if (wc == 0 && !(pDummy[i]->_errorFlag))
							InterlockedDecrement(&pDummy[i]->_waitFlag);
					}
					else if (ret == 0)
					{
						flag = true;
					}
					else
					{
						DebugBreak();
					}
					// Read
					// read 실행 시 0 -> flag 변경
				}

				if (FD_ISSET(pDummy[i]->_serverSocket, &exceptFds))
				{
					InterlockedIncrement(&g_connectFailCnt);
					flag = true;
					// read 실행 시 0 -> flag 변경
				}
				auto refCount = InterlockedDecrement(&pDummy[i]->_refCount);
				if ((refCount == 0) && flag)
				{
					InterlockedDecrement(&pDummy[i]->_socketFlag);
					// Error
					AcquireSRWLockExclusive(&pDummy[i]->_lock);
					SOCKET tmpSock = pDummy[i]->_serverSocket;
					
					pDummy[i]->_serverSocket = INVALID_SOCKET;
					closesocket(tmpSock);

					pDummy[i]->Init(1);
					pDummy[i]->Connect();
					ReleaseSRWLockExclusive(&pDummy[i]->_lock);
				}
			}
		}
		else	// 말이 안됌.
		{
			auto errCode = WSAGetLastError();
			if (responseCount == SOCKET_ERROR)
			{

			}
			wprintf(L"RecvThread count %d, %d\n", responseCount, errCode);
			DebugBreak();
		}
	}
	return 0;
}

CPacket* PacketInit()
{
	CPacket* pNewPacket;

	pNewPacket = CPacket::Alloc();
	if (pNewPacket->GetRefCount() != 0)
		DebugBreak();
	pNewPacket->AddRef();
	pNewPacket->Clear();

	return pNewPacket;
}


void DummyContents(CDummy* pDummy)
{
	CPacket* pPacket[100];
	USHORT len;
	len = 8;
	uintptr_t data = 0;
	int overlapCnt = g_overlapCnt;


	// Get New Packet
	pDummy->curData = InterlockedAdd64((LONG64 *)& g_data, overlapCnt);
	data = pDummy->curData - overlapCnt + 1;
	for (int i = 0; i < overlapCnt; ++i)
	{
		pPacket[i] = PacketInit();
		*pPacket[i] << (data + i);
		pPacket[i]->SetHeader((char*)&len, 0, 2);
	}

	InterlockedAdd((LONG *)& pDummy->_waitCount, overlapCnt);
	InterlockedIncrement(&pDummy->_waitFlag);


	// header Seting;
	// HeaderSetting(pCreateRoomPacket, len);

	// Enqueueing
	for (int i = 0; i < overlapCnt; ++i)
	{
		pDummy->_sendBuffer.Enqueue(pPacket[i]->buffer, pPacket[i]->GetDataSize() + 2);
		pPacket[i]->subRef();
	}
}

//void Echo(CDummy* pDummy)
//{
//	int r = rand();
//	if (r < 100)
//	{
//		closesocket(pDummy->_serverSocket);
//	}
//	else
//	{
//		DummyContents(pDummy);
//	}
//}

void Kind(CDummy* pDummy)
{
	switch (pDummy->_status)
	{
	case CDummy::en_Status::None:
		DummyContents(pDummy);
		break;
	case CDummy::en_Status::Session:
	case CDummy::en_Status::Login:
	case CDummy::en_Status::Echo:
	case CDummy::en_Status::Exit:
		DebugBreak();
	default:
		DebugBreak();
		break;
	}
}

bool Test(int idxs[], int fdSize)
{
	CDummy* pDummy;
	for (int idx = 0; idx < fdSize; ++idx)
	{
		pDummy = &g_dummy[idxs[idx]];
		int useSize = pDummy->_sendBuffer.GetUseSize();
		if (useSize > 0)
		{
			int ret = send(pDummy->_serverSocket, pDummy->_sendBuffer.GetRearBufferPtr(), useSize, 0);
			if (ret <= 0)
			{
				return false;
			}
		}
	}
	return true;
}

unsigned int ContentsThread(LPVOID param)
{
	timeval times;
	times.tv_sec = 0;
	times.tv_usec = 0;

	int idx = reinterpret_cast<int>(param);


	FD_SET writeFds;
	int idxs[FD_SETSIZE];
	int fdSize = 0;
	CDummy* pDummy;

	for (;;)
	{
		// Init Select
		FD_ZERO(&writeFds);
		fdSize = 0;

		for (int i = 0; i < g_threadCnt * g_dummyCnt; ++i)
		{
			pDummy = &g_dummy[i];
			if (InterlockedOr((LONG *)& pDummy->_socketFlag, 0))
			{
				InterlockedIncrement(&pDummy->_refCount);
				//AcquireSRWLockShared(&pDummy->_lock);
				FD_SET(pDummy->_serverSocket, &writeFds);
				//ReleaseSRWLockShared(&pDummy->_lock);
				idxs[fdSize] = i;
				fdSize++;
			}

			// 64개 꽉참.
			/*
			if (fdSize == FD_SETSIZE)
			{
				auto responseCount = select(0, nullptr, &writeFds, nullptr, &times);
				if (responseCount > 0)
				{
					// Test(idxs, fdSize);
					for (int idx = 0; idx < fdSize; ++idx)
					{
						pDummy = &g_dummy[idxs[idx]];
						if (FD_ISSET(pDummy->_serverSocket, &writeFds))
						{

						}
						bool flag = 0;
						int useSize = pDummy->_sendBuffer.GetUseSize();
						if (useSize > 0)
						{
							int ret = send(pDummy->_serverSocket, pDummy->_sendBuffer.GetRearBufferPtr(), useSize, 0);
							if (ret <= 0)
							{
								flag = true;
							}
						}

						auto refCount = InterlockedDecrement(&pDummy->_refCount);
						if (refCount == 0 && flag)
						{
							pDummy->Init(1);
							pDummy->Connect();
						}
					}

				}
				else if (responseCount == 0)
				{
					;// Not Error
				}
				else
				{
					if (responseCount == SOCKET_ERROR)
					{
						DebugBreak();
					}
					printf("%d\n", GetLastError());
					DebugBreak();
				}
				
				FD_ZERO(&writeFds);
				fdSize = 0;
			}
			*/
		}

		// 64 미만.
		if (fdSize > 0)
		{
			auto responseCount = select(0, nullptr, &writeFds, nullptr, &times);
			for (int idx = 0; idx < fdSize; ++idx)
			{
				pDummy = &g_dummy[idxs[idx]];
				if (FD_ISSET(pDummy->_serverSocket, &writeFds))
				{
					if (pDummy->_status == CDummy::en_Status::None)
					{
						pDummy->_status = CDummy::en_Status::Session;
						InterlockedIncrement(&g_connectSuccessCnt);
					}
				}
				bool flag = 0;
				int useSize = pDummy->_sendBuffer.GetUseSize();
				if (useSize > 0)
				{
					int ret = send(pDummy->_serverSocket, pDummy->_sendBuffer.GetFrontBufferPtr(), useSize, 0);
					if (ret <= 0)
					{
						auto code = GetLastError();
						flag = true;
					}
					else
					{
						pDummy->_sendBuffer.Dequeue(pDummy->_sendBuffer.GetFrontBufferPtr(), ret);
					}
				}

				auto refCount = InterlockedDecrement(&pDummy->_refCount);
				if (refCount == 0 && flag)
				{
					InterlockedDecrement(&pDummy->_socketFlag);
					SOCKET tmpSock = pDummy->_serverSocket;

					pDummy->_serverSocket = INVALID_SOCKET;
					closesocket(tmpSock);

					pDummy->Init(1);
					pDummy->Connect();
				}
			}
			
			if (responseCount == 0)
			{
				int a = 0;
			}
			else if (responseCount < 0)
			{
				printf("%d\n", GetLastError());
				if (responseCount == SOCKET_ERROR)
				{
					DebugBreak();
				}
				DebugBreak();
			}
		}

		// Contents  생성.
		for (int i = 0; i < g_threadCnt * g_dummyCnt; ++i)
		{
			pDummy = &g_dummy[i];
			if (pDummy->_status != CDummy::en_Status::None)
			{
				if (!(InterlockedOr((LONG*)&pDummy->_waitFlag, 0)))
				{
					DummyContents(pDummy);
				}
			}
		}
	}

	return 0;
}

unsigned int A(LPVOID param)
{
	g_dummy[0].Init(1);
	g_dummy[0].Connect();
	FD_SET readFds, exceptFds;
	FD_ZERO(&readFds);
	FD_ZERO(&exceptFds);
	FD_SET(g_dummy[0]._serverSocket, &readFds);
	FD_SET(g_dummy[0]._serverSocket, &exceptFds);
	auto r = select(0, &readFds, nullptr, &exceptFds, nullptr);
	if (r > 0)
	{
		if (FD_ISSET(g_dummy[0]._serverSocket, &exceptFds))
		{
			printf("hi\n");
			closesocket(g_dummy[0]._serverSocket);
		}
	}
	return 0;
}

unsigned int B(LPVOID param)
{
	Sleep(1000);
	FD_SET writeFds;
	FD_ZERO(&writeFds);
	FD_SET(g_dummy[0]._serverSocket, &writeFds);

	auto rr = select(0, nullptr, &writeFds, nullptr, nullptr);
	if (rr == -1)
	{
		printf("%d", GetLastError());
	}

	FD_ZERO(&writeFds);
	FD_SET(g_dummy[0]._serverSocket, &writeFds);

	auto rrr = select(0, nullptr, &writeFds, nullptr, nullptr);
	if (rrr == -1)
	{
		printf("%d", GetLastError());
	}
	return 0;
}
int main()
{
	WSAData wsa;
	auto startRet = WSAStartup(MAKEWORD(2, 2), &wsa);
	

	printf("1. connect, 2. disconnect  XX\n");
	scanf_s("%d", &g_kind);


	printf("thread 1. 1, 2. 50, 3. 100\n");
	scanf_s("%d", &g_kind);
	switch (g_kind)
	{
	case 1:
		g_threadCnt = 1;
		break;
	case 2:
		g_threadCnt = 2;
		break;
	case 3:
		g_threadCnt = 10;
		break;
	case 4:
		g_threadCnt = 100;
		break;
	default:
		DebugBreak();
	}

	printf("client 1. 1, 2. 2, 3. 50\n");
	scanf_s("%d", &g_kind);
	switch (g_kind)
	{
	case 1:
		g_dummyCnt = 1;
		break;
	case 2:
		g_dummyCnt = 2;
		break;
	case 3:
		g_dummyCnt = 50;
		break;
	default:
		DebugBreak();
	}
	
	printf("overlap 1. 1, 2. 50, 3. 100\n");
	scanf_s("%d", &g_kind);
	switch (g_kind)
	{
	case 1:
		g_overlapCnt = 1;
		break;
	case 2:
		g_overlapCnt = 2;
		break;
	case 3:
		g_overlapCnt = 10;
		break;
	case 4:
		g_overlapCnt = 100;
		break;
	default:
		DebugBreak();
	}




	g_servAddr.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &g_servAddr.sin_addr);
	g_servAddr.sin_port = htons(6000);
	g_addrSize = sizeof(g_servAddr);

	//connect(g_dummy[0]._serverSocket, (sockaddr *)&g_servAddr, g_addrSize);
	

	
	_beginthreadex(nullptr, 0, A, (void*)1, 0, nullptr);
	_beginthreadex(nullptr, 0, B, (void*)1, 0, nullptr);
	
	
	/*for (int i = 0 ; i < g_threadCnt ; ++i)
		_beginthreadex(nullptr, 0, RecvThread, (void*)1, 0, nullptr);
	_beginthreadex(nullptr, 0, ContentsThread, nullptr, 0, nullptr);
	_beginthreadex(nullptr, 0, MonitoringThread, nullptr, 0, nullptr);*/
	
	Sleep(INFINITE);
	
	WSACleanup();
	return 0;
}