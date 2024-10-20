#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

#include "network.h"
#include "CDummy.h"

CDummy::CDummy()
	:_refCount(0),
	_socketFlag(0)
{
	InitializeSRWLock(&_lock);
}

CDummy::~CDummy()
{

}

void CDummy::Init(int mode)
{
	
	_status = en_Status::None;

	_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	u_long blockingMode = mode;	// 0: block, 1: non-block

	auto ret = ioctlsocket(_serverSocket, FIONBIO, &blockingMode);

	auto rets = InterlockedIncrement(&_socketFlag);
	if (rets != 1)
	{
		DebugBreak();
	}

	_sendBuffer.ClearBuffer();
	_recvBuffer.ClearBuffer();
}

void CDummy::Connect()
{
	int retConnect = connect(_serverSocket, reinterpret_cast<sockaddr*>(&g_servAddr), static_cast<int>(g_addrSize));
	if (retConnect == SOCKET_ERROR)
	{
		auto errCode = GetLastError();
		if (errCode != WSAEWOULDBLOCK)
		{
			printf("%d\n", errCode);
			DebugBreak();
		}
	}
	InterlockedIncrement(&g_connectTotalCnt);
}