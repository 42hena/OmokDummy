#pragma once

#include <string>
#include "CRBuffer.h"



struct RoomInfo
{
	/*enum Color
	{
		None,
		Black,
		White
	};*/

	USHORT _curRoomNo;			// 방 번호
	int _chatIdx;				// 내가 보낸 체팅의 index
	int _board[15][15];			// 기억하고 있는 board판
	int _visited[15][15];
	bool _turn;					// Black or White turn
	bool _color;					// Black or White
	BYTE _sendX = 7, _sendY = 7;		// 마지막에 보낸 위치

public:

	void FindCandidates(int x, int y);

	const USHORT GetCurrentRoomNo() const
	{
		return _curRoomNo;
	}
	
	void SetColor(int color)
	{
		_color = color;
	}

	void ClearColor()
	{
		SetColor(0);
	}

	void MoveRoom(const USHORT roomNo)
	{
		_curRoomNo = roomNo;
	}

	const int GetChatIdx() const
	{
		return _chatIdx;
	}

	void SaveChatIdx(const int idx)
	{
		_chatIdx = idx;
	}


	void Clear()
	{
		for (int i = 0; i < 15; ++i)
		{
			for (int j = 0; j < 15; ++j)
			{
				_board[i][j] = 0;
			}
		}
	}

	void PutStone(int x, int y, int position)
	{
		_board[y][x] = position;
	}

	int GetBoard(int x, int y)
	{
		return _board[y][x];
	}
};

class CDummy
{
public:
	enum en_Status
	{
		None,
		Session,
		Login,
		Lobby,
		Room,
		P1,
		P2,
		RP1,
		RP2,
		Black,
		White,
		Error,
		Exit
	};

public:
	CDummy();
	~CDummy();

public:	// delete Default
	CDummy(const CDummy& rhs) = delete;
	CDummy(CDummy&& rhs) = delete;
	CDummy& operator=(const CDummy& rhs) = delete;
	CDummy& operator=(CDummy&& rhs) = delete;

	void Init(int mode);
	void Connect();

public:
	SOCKET _serverSocket;
	CRBuffer _sendBuffer;
	DWORD _loginCount;			// 
	CRBuffer _recvBuffer;

	en_Status _status;

	DWORD _lastUpdateTime;		// loginDelay, contentsDelay
	DWORD _waitFlag;			// Echo
	DWORD _errorFlag;			// 서버가 강제로 끊은 경우
	DWORD _normalFlag;			// 종료를 요청했는지 확인하는 Flag

	uintptr_t _accountNo;		// 회원 번호
	std::wstring _nickName;		// 닉네임
	RoomInfo _roominfo;			// 방 정보

	int lastPacket;				// 마지막 전송 패킷 타입
};
