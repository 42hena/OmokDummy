#include "CPacket.h"
#include "CDummy.h"
#include "CProtocol.h"

extern int g_loginDelay;
extern int g_actionDelay;

void ContentsCP1(CDummy* pDummy)
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
		pDummy->lastPacket = 51;
		break;
	case 1:	// ChatRoom
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 52;
		break;
	case 2: // to P2
		CProtocol::ChangePositionProcedure(pDummy, 1, 2);
		pDummy->lastPacket = 53;
		break;
	case 3: // to Spec
		CProtocol::ChangePositionProcedure(pDummy, 1, 3);
		pDummy->lastPacket = 54;
		break;
	case 4:	// ready
		CProtocol::CancelProcedure(pDummy);
		pDummy->lastPacket = 55;
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

	int p2Cnt = 5;

	int type = rand() % p2Cnt;
	switch (type)
	{
	case 0:	// LeaveRoom
		CProtocol::LeaveRoomProcedure(pDummy);
		pDummy->lastPacket = 61;
		break;
	case 1:	// ChatRoom
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 62;
		break;
	case 2: // to P1
		CProtocol::ChangePositionProcedure(pDummy, 2, 1);
		pDummy->lastPacket = 63;
		break;
	case 3: // to Spec
		CProtocol::ChangePositionProcedure(pDummy, 2, 3);
		pDummy->lastPacket = 64;
		break;
	case 4:
		CProtocol::CancelProcedure(pDummy);
		pDummy->lastPacket = 65;
		break;
	default:
		DebugBreak();
		break;
	}
}

void ContentsBlack(CDummy* pDummy)
{
	DebugBreak();
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}
	if (!pDummy->_roominfo._turn)
		CProtocol::ChangePositionProcedure(pDummy, 1, 2);
	else
		CProtocol::ChattingProcedure(pDummy);
}

void ContentsWhite(CDummy* pDummy)
{
	DebugBreak();
	DWORD now = timeGetTime();

	if (now <= pDummy->_lastUpdateTime + g_actionDelay)
	{
		return;
	}

	if (pDummy->_roominfo._turn)
	{
		CProtocol::ChattingProcedure(pDummy);
		pDummy->lastPacket = 71;
	}
	else
	{
		CProtocol::PutStoneProcedure(pDummy);
		pDummy->lastPacket = 72;
	}
}

