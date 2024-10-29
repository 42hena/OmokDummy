#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Winmm.lib")
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <locale> 
#include <Windows.h>


#include "Util.h"
#include "network.h"
#include "CDummy.h"
#include "CPacket.h"
#include "CProtocol.h"
#include "Contents.h"

#include "monitorTh.h"

#include "ini.h"

int g_threadCnt = 1;
int g_dummyCnt = 1;
int g_overlapCnt;
int g_kind;
int g_loginDelay;
int g_actionDelay;

int g_randDisconnect;
int g_randContent;
int g_randConnect;

int g_headerSize = 2;

CDummy g_dummy[5000];

uintptr_t llog1[1000];
uintptr_t llog2[1000];
uintptr_t lidx;

alignas (64) uintptr_t g_recvCounts[100];
std::wstring g_chat[10];

alignas (64) uintptr_t g_data;
alignas (64) uintptr_t g_dddd;
alignas (64) uintptr_t g_exitCount;
alignas (64) uintptr_t g_echoCount;
//alignas (64) uintptr_t g_totalTime;
//alignas (64) uintptr_t g_totalCount;
alignas (64) uintptr_t g_tttttt;

void RecvLogin(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;

	*pPacket >> recvAccountNo;
	if (recvAccountNo != pDummy->_accountNo)
		DebugBreak();
	if (pDummy->_status != CDummy::en_Status::Login)
		DebugBreak();
	InterlockedExchange((long*)&pDummy->_status, CDummy::en_Status::Lobby);
	//pDummy->_status = CDummy::en_Status::Lobby;

	InterlockedDecrement(&pDummy->_waitFlag);
}

void RecvGracefulShutdown(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;

	*pPacket >> recvAccountNo;
	if (recvAccountNo != pDummy->_accountNo)
		DebugBreak();

	pDummy->_status = CDummy::en_Status::Exit;
	closesocket(pDummy->_serverSocket);
	pDummy->Init(1);
	pDummy->Connect();
}

void OnMessage(CPacket* pPacket, CDummy* pDummy)
{
	InterlockedIncrement(&g_recvCnt);
	USHORT type;

	*pPacket >> type;
	switch (type)
	{
	case 2:	// Login
		RecvLogin(pPacket, pDummy);
		break;
	case 202:	// Create
		CProtocol::RecvCreateRoom(pPacket, pDummy);
		break;
	case 302:	// Enter
		CProtocol::RecvEnterRoom(pPacket, pDummy);
		break;
	
	case 303:	// 무시.
	case 304:	// 무시.
	case static_cast<USHORT>(CProtocol::en_Protocol::en_ResponseEnterAlarm):	// 305
		break;

	case 402:	// Leave
		CProtocol::RecvLeaveRoom(pPacket, pDummy);
		break;
	case 403:	// 무시.
		break;
	case 502:	// Chatting
		CProtocol::RecvChatting(pPacket, pDummy);
		break;

	case 1001:
		DebugBreak();
		break;
	case 1002:
		DebugBreak();
		break;
	case 1003:
	case 1004:
		CProtocol::RecvChangePosition(pPacket, pDummy);
		break;

	case 1102:	//en_ReadyResponse
		CProtocol::RecvReady(pPacket, pDummy);
		break;
	case 1104:	//en_CancelReadyResponse
		CProtocol::RecvCancel(pPacket, pDummy);
		break;
	case 1105:	//en_StartResponse,
		CProtocol::RecvGameStart(pPacket, pDummy);
		break;
	case 1109:	//PutStonere
		CProtocol::RecvPutStone(pPacket, pDummy);
		break;
	case 1110:
		CProtocol::RecvGameOverCheck(pPacket, pDummy);
		break;
	case 1111:
		CProtocol::RecvGameOver(pPacket, pDummy);
		break;

	case 59001:	// Exit
		CProtocol::RecvShutdown(pPacket, pDummy);
		break;
	default:
		DebugBreak();
		break;
	}
}



unsigned int RecvThread(LPVOID param)
{
	FD_SET readFds, writeFds, exceptFds;
	FD_ZERO(&readFds);
	FD_ZERO(&writeFds);
	FD_ZERO(&exceptFds);

	int idx = reinterpret_cast<int>(param);
	printf("[%d]\n", param);
	int overlapCnt = g_overlapCnt;
	int headerSize = 2;

	CDummy* pDummy;	// 바꿔야 함. 나중가면 다 안돌아야 함.
	idx *= g_dummyCnt;
	for (int i = 0; i < g_dummyCnt; ++i)
	{
		pDummy = &g_dummy[idx + i];
		pDummy->Init(1);
		pDummy->Connect();
	}
	int value = -1;
	int fdArray[64];
	char buf[800];
	for (;;)
	{
		int fdSize = 0;
		FD_ZERO(&readFds);
		FD_ZERO(&writeFds);
		FD_ZERO(&exceptFds);
		for (int i = 0; i < g_dummyCnt; ++i)
		{
			pDummy = &g_dummy[idx + i];
			if (!(pDummy->_errorFlag))
			{
				FD_SET(pDummy->_serverSocket, &readFds);
				if (pDummy->_status == CDummy::en_Status::None)	// connect 확인.
					FD_SET(pDummy->_serverSocket, &writeFds);
				FD_SET(pDummy->_serverSocket, &exceptFds);
				fdArray[fdSize] = idx + i;
				fdSize++;
			}
		}

		if (fdSize > 0)
		{
			auto responseCount = select(0, &readFds, &writeFds, &exceptFds, nullptr);
			if (responseCount > 0)
			{
				for (int i = 0; i < fdSize; ++i)
				{
					pDummy = &g_dummy[fdArray[i]];
					bool flag = false;
					if (FD_ISSET(pDummy->_serverSocket, &readFds))
					{
						value = pDummy->_recvBuffer.DirectEnqueueSize();
						int rr = recv(pDummy->_serverSocket, buf, 800, 0);
						if (rr == -1)
						{
							auto vv = GetLastError();
							printf("%d\n", vv);
						}
						int ret = pDummy->_recvBuffer.Enqueue(buf, rr);
						if (ret != rr)
							DebugBreak();
						//int ret = recv(pDummy->_serverSocket, pDummy->_recvBuffer.GetRearBufferPtr(), value, 0);
						if (ret > 0)
						{
							//pDummy->_recvBuffer.MoveRear(ret);

							USHORT len;
							while (ret > 0)
							{
								CPacket* pPacket = CPacket::Alloc();
								pPacket->AddRef();
								int peekSize = pDummy->_recvBuffer.Peek((char*)&len, sizeof(USHORT));
								if (peekSize != headerSize)
								{
									pPacket->subRef();
									break;
								}
								if (ret < headerSize + len)
								{
									pPacket->subRef();
									break;
								}
								pDummy->_recvBuffer.Dequeue(pPacket->buffer, headerSize + len);


								OnMessage(pPacket, pDummy);
								if (pDummy->_status == CDummy::en_Status::Exit)
									flag = true;

								pPacket->subRef();
								ret -= (headerSize + len);
							}

						}
						else if (ret == 0)
						{
							flag = true;
						}
						else
						{
							auto errCode = GetLastError();
							printf("%d", errCode);
							if (errCode != 10053 && errCode != 10054)
							{
								DebugBreak();
							}
							flag = true;
						}
						// Read
						// read 실행 시 0 -> flag 변경
					}

					if (FD_ISSET(pDummy->_serverSocket, &writeFds))
					{
						pDummy->_status = CDummy::en_Status::Session;	// 0 -> 1로 바꾸는 거 하나.
						pDummy->_lastUpdateTime = timeGetTime();			// Login Time 재기.
						//printf("Time:%d\n", pDummy->_lastUpdateTime);
						InterlockedIncrement(&g_connectSuccessCnt);
					}

					if (FD_ISSET(pDummy->_serverSocket, &exceptFds))
					{
						printf("Exception[%d]\n", GetLastError());
						DebugBreak();
						InterlockedIncrement(&g_connectFailCnt);
						flag = true;
						// read 실행 시 0 -> flag 변경
						SOCKET tmpSock = pDummy->_serverSocket;

						// pDummy[i]->_serverSocket = INVALID_SOCKET;
						closesocket(tmpSock);

						pDummy->Init(1);
						pDummy->Connect();

						continue;
					}

					if (flag)
					{
						DWORD normalValue = InterlockedOr((unsigned long long*) & pDummy->_normalFlag, 0);
						if (normalValue)
						{
							SOCKET tmpSock = pDummy->_serverSocket;

							// pDummy[i]->_serverSocket = INVALID_SOCKET;
							closesocket(tmpSock);

							pDummy->Init(1);
							pDummy->Connect();
						}
						else
						{
							InterlockedIncrement(&pDummy->_errorFlag);
						}
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
		else
		{
			wprintf(L"Thread Sleep\n");
			Sleep(INFINITE);
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

// ==================================================================================

void Contents(CDummy* pDummy)
{
	
	switch (pDummy->_status)
	{
	case CDummy::en_Status::None:		// connect 이전 상태.
		//DebugBreak();
		InterlockedIncrement(&g_dddd);
		break;
	case CDummy::en_Status::Session:	// 단순 session 상태
		InLogin(pDummy);
		break;
	case CDummy::en_Status::Login:		// 로그인 성공 요청 상태(contents)
		DebugBreak();
		break;
	case CDummy::en_Status::Lobby:		// Echo랑 Exit (contents)
		InLobby(pDummy);
		break;
	case CDummy::en_Status::Room:		// 일반 유저.(체팅 및 Player(p1, p2)가 될 수 있음)
		InRoom(pDummy);
		break;
	case CDummy::en_Status::P1:			// P1에 들어간 상태.
		ContentsP1(pDummy);
		break;
	case CDummy::en_Status::P2:			// P2에 들어간 상태.
		ContentsP2(pDummy);
		break;
	case CDummy::en_Status::RP1:
		ContentsCP1(pDummy);
		break;
	case CDummy::en_Status::RP2:
		ContentsCP2(pDummy);
		break;
	case CDummy::en_Status::Black:
		ContentsBlack(pDummy);
		break;
	case CDummy::en_Status::White:
		ContentsWhite(pDummy);
		break;
	// Game
	default:
		DebugBreak();
		break;
	}
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
			if (pDummy->_status != CDummy::en_Status::None)
			{
				int useSize = pDummy->_sendBuffer.GetUseSize();
				if (useSize > 0)
				{
					if (!(InterlockedOr((LONG*)&pDummy->_errorFlag, 0)))
					{
						FD_SET(pDummy->_serverSocket, &writeFds);
						idxs[fdSize] = i;
						fdSize++;
					}
				}
			}

			// 64개 꽉참.
			if (fdSize == FD_SETSIZE)
			{
				auto responseCount = select(0, nullptr, &writeFds, nullptr, &times);
				if (responseCount > 0)
				{
					for (int idx = 0; idx < fdSize; ++idx)
					{
						pDummy = &g_dummy[idxs[idx]];
						bool flag = 0;
						if (FD_ISSET(pDummy->_serverSocket, &writeFds))
						{
							int ret = send(pDummy->_serverSocket, pDummy->_sendBuffer.GetFrontBufferPtr(), pDummy->_sendBuffer.DirectDequeueSize(), 0);
							if (ret <= 0)
							{
								auto code = GetLastError();
								printf("send %d\n", code);
								if (!(code == 10053 || code == 10054))
									DebugBreak();
								flag = true;
							}
							else
							{
								pDummy->_sendBuffer.MoveFront(ret);
							}
						}
						if (flag)
						{
							InterlockedIncrement(&pDummy->_waitFlag);
							DebugBreak();
						}
					}
				}
				else if (responseCount == 0)
				{
					int a = 0;
				}
				else if (responseCount < 0)
				{
					auto cc = GetLastError();
					printf("contents Select Error: %d\n", GetLastError());
					SelectError(cc);
					DebugBreak();
				}
				
				FD_ZERO(&writeFds);
				fdSize = 0;
			}
		}

		// fd 64 미만.
		if (fdSize > 0)
		{
			auto responseCount = select(0, nullptr, &writeFds, nullptr, &times);
			if (responseCount > 0)
			{
				for (int idx = 0; idx < fdSize; ++idx)
				{
					pDummy = &g_dummy[idxs[idx]];
					bool flag = 0;
					if (FD_ISSET(pDummy->_serverSocket, &writeFds))
					{
						int ret = send(pDummy->_serverSocket, pDummy->_sendBuffer.GetFrontBufferPtr(), pDummy->_sendBuffer.DirectDequeueSize(), 0);
						if (ret <= 0)
						{
							auto code = GetLastError();
							printf("send %d\n", code);
							if (!(code == 10053 || code == 10054))
								DebugBreak();
							flag = true;

							// pDummy->_sendBuffer.Dequeue(pDummy->_sendBuffer.GetFrontBufferPtr(), useSize);
						}
						else
						{
							//pDummy->_sendBuffer.Dequeue(pDummy->_sendBuffer.GetFrontBufferPtr(), ret);
							pDummy->_sendBuffer.MoveFront(ret);
						}
					}
					if (flag)
					{
						InterlockedIncrement(&pDummy->_waitFlag);
						DebugBreak();
					}
				}
			}
			else if (responseCount == 0)
			{
				int a = 0;
			}
			else if (responseCount < 0)
			{
				auto cc = GetLastError();
				printf("contents Select Error: %d\n", GetLastError());
				SelectError(cc);
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
					Contents(pDummy);
				}
			}
		}
	}

	return 0;
}

void Input()
{
	inih::INIReader r{"./config.ini"};

	const auto& threadNumber = r.Get<int>("dummy_client", "thread_number");
	const auto& clientNumber = r.Get<int>("dummy_client", "client_number");
	const auto& actionDelay = r.Get<int>("delay", "action_delay");
	const auto& loginDelay = r.Get<int>("delay", "login_delay");
	
	const auto& randConnect = r.Get<int>("probability", "rand_connect");
	const auto& randDisconnect = r.Get<int>("probability", "rand_disconnect");
	const auto& randContent = r.Get<int>("probability", "rand_content");

	g_threadCnt = threadNumber;
	g_dummyCnt = clientNumber;
	g_actionDelay = actionDelay;
	g_loginDelay = loginDelay;

	g_randConnect = randConnect;
	g_randDisconnect = randDisconnect;
	g_randContent = randContent;

	const auto& chat = r.GetVector<std::wstring>("chatting", "1"); // ["1", "2", "3"]

	for (int i = 0 ; i < chat.size() ; ++i)
		g_chat[i] = chat[i];
}

void Init()
{
	std::setlocale(LC_ALL, "ko_KR.UTF-8");
	g_servAddr.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &g_servAddr.sin_addr);
	g_servAddr.sin_port = htons(12001);
	g_addrSize = sizeof(g_servAddr);
}

int main()
{
	timeBeginPeriod(1);
	WSAData wsa;
	auto startRet = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (startRet != 0)
	{
		auto errCode = GetLastError();
		WSAStartUpError(errCode);
		return 0;
	}

	Input();
	
	Init();

	for (int i = 0; i < 5000; ++i)
	{
		g_dummy[i]._accountNo = (i + 1);
		g_dummy[i]._nickName = L"Dummy" + std::to_wstring(i + 1);
	}
	
	HANDLE hArray[102];
	int idx = 0;
	//g_threadCnt = 1;
	for (idx = 0; idx < g_threadCnt; ++idx)
	{
		hArray[idx] = (HANDLE)_beginthreadex(nullptr, 0, RecvThread, (void*)idx, 0, nullptr);
		if (hArray[idx] == 0)
			return 0;
	}
	hArray[idx++] = (HANDLE)_beginthreadex(nullptr, 0, ContentsThread, nullptr, 0, nullptr);
	hArray[idx++] = (HANDLE)_beginthreadex(nullptr, 0, MonitoringThread, nullptr, 0, nullptr);

	for (int i = 0; i < idx; ++i)
	{
		WaitForSingleObject(hArray[i], INFINITE);
	}

	WSACleanup();
	timeEndPeriod(1);
	return 0;
}
