#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

#include "network.h"
#include "CDummy.h"

CDummy::CDummy()
	: _loginCount(1)
{
	//InitializeSRWLock(&_lock);
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

	LINGER linger;
	linger.l_linger = 0;
	linger.l_onoff = 1;
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_LINGER, (char *)& linger, sizeof(LINGER)) == SOCKET_ERROR) {
		DebugBreak();
	}
	
	_sendBuffer.ClearBuffer();
	_recvBuffer.ClearBuffer();

	//InterlockedDecrement(&_waitFlag);
	_waitFlag = 0;
	_normalFlag = 0;
	auto loginCnt = InterlockedDecrement(&_loginCount);
	if (loginCnt != 0)
		DebugBreak();
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