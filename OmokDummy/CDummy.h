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

	USHORT _curRoomNo;			// �� ��ȣ
	int _chatIdx;				// ���� ���� ü���� index
	int _board[15][15];			// ����ϰ� �ִ� board��
	int _visited[15][15];
	bool _turn;					// Black or White turn
	bool _color;					// Black or White
	BYTE _sendX = 7, _sendY = 7;		// �������� ���� ��ġ

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
	DWORD _errorFlag;			// ������ ������ ���� ���
	DWORD _normalFlag;			// ���Ḧ ��û�ߴ��� Ȯ���ϴ� Flag

	uintptr_t _accountNo;		// ȸ�� ��ȣ
	std::wstring _nickName;		// �г���
	RoomInfo _roominfo;			// �� ����

	int lastPacket;				// ������ ���� ��Ŷ Ÿ��
};
