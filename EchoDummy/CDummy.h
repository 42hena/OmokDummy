#pragma once

#include "CRBuffer.h"

class CDummy
{
public:
	enum class en_Status
	{
		None,
		Session,
		Login,
		Echo,
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
	CRBuffer _recvBuffer;

	en_Status _status;

	uintptr_t _refCount;
	uintptr_t _waitCount;
	DWORD _lastUpdateTime;
	DWORD _waitFlag;
	DWORD _socketFlag;
	uintptr_t curData;

	bool _errorFlag;
	SRWLOCK _lock;
private:
};
