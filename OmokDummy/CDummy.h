#pragma once

#include <string>
#include "CRBuffer.h"

struct RoomInfo
{
	USHORT _curRoomNo;			// �� ��ȣ
	int _chatIdx;				// ���� ���� ü���� index
	int _board[15][15];			// ����ϰ� �ִ� board��
	int _bWeight[15][15];		// ��� ����ġ
	int _wWeight[15][15];		// ��� ����ġ
	bool _turn;					// Black or White turn
	BYTE _sendX, _sendY;		// �������� ���� ��ġ

public:
	const USHORT GetCurrentRoomNo() const
	{
		return _curRoomNo;
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

	void PutStone(int y, int x, int position)
	{
		_board[y][x] = position;
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


	// -

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
