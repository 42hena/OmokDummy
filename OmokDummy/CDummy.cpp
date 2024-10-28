#include <iostream>
#include <queue>
#include <WinSock2.h>
#include <Windows.h>

#include "network.h"
#include "CDummy.h"

#include "CSimpleAI.h"

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

void RoomInfo::FindCandidates(int x, int y)
{
	const int dy[] = { 1, 1, 1, 0, -1, -1, -1, 0 };
	const int dx[] = { 1, 0, -1, 1, 1, 0, -1, -1 };

	memset(_visited, 0, sizeof(_visited));

	std::queue<std::pair<int, int>> q;
	std::queue<std::pair<int, int>> candiQ;

	q.push({ x, y });
	_visited[y][x] = 1;

	while (!q.empty())
	{
		std::pair<int, int> cur = q.front(); q.pop();

		for (int dir = 0; dir < 4; ++dir)
		{
			int nx = cur.first + dx[dir];
			int ny = cur.second + dy[dir];

			if (ny < 0 || ny >= 15 || nx < 0 || nx >= 15)
				continue;
			if (_visited[ny][nx])
				continue;
			if (_board[ny][nx] == 0)
				candiQ.push({ nx, ny });
			else
				q.push({ nx, ny });
			_visited[ny][nx] = 1;
		}
	}

	int maxY = 0;
	int maxX = 0;
	int maxValue = 0;
	while (!candiQ.empty())
	{
		std::pair<int, int> cur = candiQ.front();
		candiQ.pop();
		int myValue = CSimpleAI::FindOmokPattern(_board, cur.first, cur.second, _color);
		int oppValue = CSimpleAI::FindOmokPattern(_board, cur.first, cur.second, !_color);
		
		if (maxValue < max(myValue, oppValue))
		{
			maxX = cur.first;
			maxY = cur.second; 
			maxValue = max(myValue, oppValue);
		}
	}
	_sendX = maxX;
	_sendY = maxY;
}
