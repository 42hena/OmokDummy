#include "CPacket.h"
#include "CDummy.h"
#include "CProtocol.h"

extern int g_loginDelay;
extern int g_actionDelay;

void InLogin(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_loginDelay)
	{
		return;
	}
	pDummy->_lastUpdateTime = now;

	CProtocol::LoginProcedure(pDummy);
}

void InLobby(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	int lobbyTypeCnt = 2;

	int type = rand() % lobbyTypeCnt;
	type = 0;
	switch (type)
	{
	case 0:	// EnterRoom
		CProtocol::EnterRoomProcedure(pDummy);
		pDummy->lastPacket = 11;
		break;
	case 1:	// CreateRoom
		CProtocol::CreateRoomProcedure(pDummy);
		pDummy->lastPacket = 12;
		break;
	case 2:	// Exit
		CProtocol::ShutdownProcedure(pDummy);
		break;
		// case 3:	// GetLst
	default:
		DebugBreak();
		break;
	}
}

void InRoom(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	int roomTypeCnt = 4;

	int type = rand() % roomTypeCnt;
	switch (type)
	{
	case 0:	// LeaveRoom
		CProtocol::LeaveRoomProcedure(pDummy);
		pDummy->lastPacket = 21;
		break;
	case 1:	// ChatRoom
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 22;
		break;
	case 2:	// P1
		CProtocol::ChangePositionProcedure(pDummy, 3, 1);
		pDummy->lastPacket = 23;
		break;
	case 3:	// P2
		CProtocol::ChangePositionProcedure(pDummy, 3, 2);
		pDummy->lastPacket = 24;
		break;
	default:
		DebugBreak();
		break;
	}
}


void ContentsP1(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	int p1Cnt = 5;

	int type = rand() % p1Cnt;
	switch (type)
	{
	case 0:	// LeaveRoom
		CProtocol::LeaveRoomProcedure(pDummy);
		pDummy->lastPacket = 31;
		break;
	case 1:	// ChatRoom
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 32;
		break;
	case 2: // to P2
		CProtocol::ChangePositionProcedure(pDummy, 1, 2);
		pDummy->lastPacket = 33;
		break;
	case 3: // to Spec
		CProtocol::ChangePositionProcedure(pDummy, 1, 3);
		pDummy->lastPacket = 34;
		break;
	case 4:	// ready
		CProtocol::ReadyProcedure(pDummy);
		break;
	default:
		DebugBreak();
		break;
	}
}

void ContentsP2(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	int p2Cnt = 5;

	int type = rand() % p2Cnt;
	switch (type)
	{
	case 0:	// LeaveRoom
		CProtocol::LeaveRoomProcedure(pDummy);
		pDummy->lastPacket = 41;
		break;
	case 1:	// ChatRoom
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 42;
		break;
	case 2: // to P1
		CProtocol::ChangePositionProcedure(pDummy, 2, 1);
		pDummy->lastPacket = 43;
		break;
	case 3: // to Spec
		CProtocol::ChangePositionProcedure(pDummy, 2, 3);
		pDummy->lastPacket = 44;
		break;
	case 4:
		CProtocol::ReadyProcedure(pDummy);
		break;
	default:
		DebugBreak();
		break;
	}
}


void ContentsCP1(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	int p1Cnt = 4;

	int type = rand() % p1Cnt;
	switch (type)
	{
	case 0:	// ChatRoom
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 52;
		break;
	case 1: // to P2
		CProtocol::ChangePositionProcedure(pDummy, 1, 2);
		pDummy->lastPacket = 53;
		break;
	case 2: // to Spec
		CProtocol::ChangePositionProcedure(pDummy, 1, 3);
		pDummy->lastPacket = 54;
		break;
	case 3:	// ready
		CProtocol::CancelProcedure(pDummy);
		pDummy->lastPacket = 55;
		break;
	case 4:	// LeaveRoom
		CProtocol::LeaveRoomProcedure(pDummy);
		pDummy->lastPacket = 51;
		break;
	default:
		DebugBreak();
		break;
	}
}

void ContentsCP2(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	int p2Cnt = 4;

	int type = rand() % p2Cnt;
	switch (type)
	{
	case 0:	// ChatRoom
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 62;
		break;
	case 1: // to P1
		CProtocol::ChangePositionProcedure(pDummy, 2, 1);
		pDummy->lastPacket = 63;
		break;
	case 2: // to Spec
		CProtocol::ChangePositionProcedure(pDummy, 2, 3);
		pDummy->lastPacket = 64;
		break;
	case 3:
		CProtocol::CancelProcedure(pDummy);
		pDummy->lastPacket = 65;
		break;
	case 4:	// LeaveRoom
		CProtocol::LeaveRoomProcedure(pDummy);
		pDummy->lastPacket = 61;
		break;
	default:
		DebugBreak();
		break;
	}
}

void ContentsBlack(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}
	if (!pDummy->_roominfo._turn)
	{
		CProtocol::PutStoneProcedure(pDummy);
		//CProtocol::ChangePositionProcedure(pDummy, 1, 2);
	}
	else
		CProtocol::ChattingProcedure(pDummy);
}

void ContentsWhite(CDummy* pDummy)
{
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	if (pDummy->_roominfo._turn)
	{
		CProtocol::PutStoneProcedure(pDummy);
		pDummy->lastPacket = 71;
	}
	else
	{
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 72;
	}
}

