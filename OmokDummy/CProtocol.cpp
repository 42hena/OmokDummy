#include <string>
#include <WinSock2.h>
#include <Windows.h>
#include "CDummy.h"
#include "CPacket.h"
#include "CProtocol.h"
#include "network.h"

extern std::wstring g_chat[10];
static CPacket* InitPacket()
{
	CPacket* pNewPacket = CPacket::Alloc();
	pNewPacket->Clear();

	auto refCount = pNewPacket->AddRef();
	if (refCount != 1)
		DebugBreak();

	return pNewPacket;
}

/**
 * @brief This function make Request Login Packet.
 * @param pDummy - The dummy client Information.
 * @return CPacket pointer - The completion of Login packet.
 */
CPacket* CProtocol::MakeRequestLoginPacket(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_RequestLogin);
	//const USHORT authType = 1;
	
	auto pLoginPacket = InitPacket();		// type: CPacket*
	auto accountNo = pDummy->_accountNo;	// type: uintptr_t(8)
	//char sessionkey[64] = { 0 };
	const std::wstring nick = pDummy->_nickName;
	auto nickLen = static_cast<BYTE>(nick.size());
	*pLoginPacket << packetType  << accountNo << nickLen;
	pLoginPacket->PutData(reinterpret_cast<const char*>(nick.c_str()), nickLen * 2);

	//pLoginPacket->PutData(sessionkey, sizeof(sessionkey));

	return pLoginPacket;
}

// len | type | len, name[]
CPacket* CProtocol::MakeRequestCreateRoomPacket(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_RequestCreateRoom);
	
	auto pCreateRoomPacket = InitPacket();		// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)

	*pCreateRoomPacket << packetType << accountNo;

	return pCreateRoomPacket;
}

/**
 * @brief len(2) | type(2) | accountNo(8) tryRoomNo(2)
 * @param pDummy - The dummy client Information.
 * @return CPacket pointer - The completion of Enter packet.
 */
CPacket* CProtocol::MakeRequestEnterRoomPacket(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_RequestEnterRoom);

	USHORT tryRoomNo;
	auto pEnterRoomPacket = InitPacket();		// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)

	tryRoomNo = rand() % 10 + 1;
	tryRoomNo = 1;
	*pEnterRoomPacket << packetType << accountNo << tryRoomNo;
	
	return pEnterRoomPacket;
}

//CPacket* CProtocol::MakeRequestRoomListPacket(CDummy* pDummy)
//{
//	const auto packetType = static_cast<USHORT>(en_Protocol::en_RequestRoomList);
//
//	BYTE pageNo;
//	auto pRoomListPacket = InitPacket();		// type: CPacket*
//	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)
//
//	pageNo = rand() % 2;
//	*pRoomListPacket << packetType << accountNo << pageNo;	// 0 ~ 12
//
//	return pRoomListPacket;
//}

/**
 * @brief len(2) | type(2) | accountNo(8) currentRoomNo(2)
 * @param pDummy - The dummy client Information.
 * @return CPacket pointer - The completion of Enter packet.
 */
CPacket* CProtocol::MakeRequestLeaveRoomPacket(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_RequestLeaverRoom);

	USHORT currentRoomNo;
	auto pLeaveRoomPacket = InitPacket();		// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)

	currentRoomNo = pDummy->_roominfo._curRoomNo;
	*pLeaveRoomPacket << packetType << accountNo << currentRoomNo;

	return pLeaveRoomPacket;
}

// len(5) | type(2) | accountNo(8) roomNo(2 add) chatLen(1) chat(chatLen)
CPacket* CProtocol::MakeRequestChattingPacket(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_RequestChatting);

	auto pChatPacket = InitPacket();			// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)

	// idx로 랜덤 체팅을 보내야 함.


	int idx = rand() % 10;
	pDummy->_roominfo.SaveChatIdx(idx);
	std::wstring chat(g_chat[idx]);
	BYTE chatLen;
	chatLen = chat.size();

	if (chatLen > 128)
	{
		DebugBreak();
		return nullptr;
	}

	auto currentRoomNo = pDummy->_roominfo._curRoomNo;	// type: USHORT
	*pChatPacket << packetType << accountNo << currentRoomNo << chatLen;
	pChatPacket->PutData(reinterpret_cast<const char*>(chat.c_str()), chatLen * 2);

	return pChatPacket;
}

CPacket* CProtocol::MakeRequestShutdownPacket(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_RequestGracefulShutdown);

	auto pExitPacket = InitPacket();			// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)

	*pExitPacket << packetType << accountNo;

	return pExitPacket;
}

void Start(CDummy* pDummy, CPacket* pPacket)
{
	USHORT len = pPacket->GetDataSize();
	pPacket->SetHeader((char*)&len, 0, 2);

	pDummy->_sendBuffer.Enqueue(pPacket->buffer, static_cast<int>(pPacket->GetDataSize() + 2));
	pDummy->_lastUpdateTime = timeGetTime();
	pPacket->subRef();

	InterlockedIncrement(&pDummy->_waitFlag);
}

void CProtocol::LoginProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestLoginPacket(pDummy);
	auto loginCnt = InterlockedIncrement(&pDummy->_loginCount);
	if (loginCnt != 1)
		DebugBreak();
	//InterlockedExchange((long *)&pDummy->_status, CDummy::en_Status::Login);
	pDummy->_status = CDummy::en_Status::Login;
	Start(pDummy, pPacket);
}

void CProtocol::CreateRoomProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestCreateRoomPacket(pDummy);

	Start(pDummy, pPacket);
}

void CProtocol::EnterRoomProcedure(CDummy* pDummy)
{
	if (pDummy->_status != CDummy::en_Status::Lobby)
		DebugBreak();
	CPacket * pPacket = MakeRequestEnterRoomPacket(pDummy);

	Start(pDummy, pPacket);
}

void CProtocol::ShutdownProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestShutdownPacket(pDummy);

	Start(pDummy, pPacket);
}

void CProtocol::LeaveRoomProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestLeaveRoomPacket(pDummy);

	Start(pDummy, pPacket);
}

void CProtocol::ChattingProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestChattingPacket(pDummy);



	Start(pDummy, pPacket);
}

// Game

void End(CDummy* pDummy)
{
	DWORD cur = timeGetTime() - pDummy->_lastUpdateTime;
	InterlockedAdd64((LONG64 *)& g_totalTime, cur);
	InterlockedAdd64((LONG64*)&g_totalSecTime, cur);
	if (cur > g_MaxTime)
		g_MaxTime = cur;
	if (cur < g_MinTime)
		g_MinTime = cur;

	if (cur > g_MaxSecTime)
		g_MaxSecTime = cur;
	if (cur < g_MinSecTime)
		g_MinSecTime = cur;
	InterlockedIncrement(&g_secCnt);
	InterlockedIncrement(&g_totalCnt);
	InterlockedDecrement(&pDummy->_waitFlag);
}

void CProtocol::RecvShutdown(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;

	*pPacket >> recvAccountNo;

	if (recvAccountNo != pDummy->_accountNo)
		DebugBreak();
	if (pDummy->_roominfo._curRoomNo != 0)
		DebugBreak();
	if (pDummy->_status != CDummy::en_Status::Lobby)
		DebugBreak();
	pDummy->_status = CDummy::en_Status::Exit;
	closesocket(pDummy->_serverSocket);
	pDummy->Init(1);
	pDummy->Connect();
}

void CProtocol::RecvCreateRoom(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;
	USHORT recvRoomNo;

	*pPacket >> recvAccountNo >> recvRoomNo;
	if (recvAccountNo != pDummy->_accountNo)
		DebugBreak();
	if (pDummy->_status != CDummy::en_Status::Lobby)
		DebugBreak();

	pDummy->_roominfo.MoveRoom(recvRoomNo);
	pDummy->_status = CDummy::en_Status::Room;
	//InterlockedExchange((long*)&pDummy->_status, CDummy::en_Status::Room);
	InterlockedDecrement(&pDummy->_waitFlag);
}

void CProtocol::RecvEnterRoom(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;
	USHORT recvRoomNo;
	BYTE status;

	*pPacket >> recvAccountNo >> recvRoomNo >> status;
	if (recvAccountNo != pDummy->_accountNo)
		DebugBreak();
	if (pDummy->_status != CDummy::en_Status::Lobby)
		DebugBreak();
	if (status > 0)
	{
		pDummy->_roominfo.MoveRoom(recvRoomNo);
		pDummy->_status = CDummy::en_Status::Room;
		//InterlockedExchange((long*)&pDummy->_status, CDummy::en_Status::Room);
		if (status == 1)
			End(pDummy);
	}
}

// 게임중이라면 특정 패킷 받아야 함.
//BYTE [BYTE, BYTE]

void CProtocol::RecvLeaveRoom(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;
	USHORT recvRoomNo;
	BYTE recvStatus;

	*pPacket >> recvAccountNo >> recvRoomNo >> recvStatus;
	if (recvAccountNo != pDummy->_accountNo)
		DebugBreak();
	if (!(pDummy->_status >= CDummy::en_Status::Room && pDummy->_status <= CDummy::en_Status::RP2))
		DebugBreak();
	if (recvStatus)
	{
		pDummy->_status = CDummy::en_Status::Lobby;
		//InterlockedExchange((long*)&pDummy->_status, CDummy::en_Status::Lobby);
		pDummy->_roominfo.MoveRoom(0);
	}
	End(pDummy);
}

void CProtocol::RecvChatting(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;
	USHORT recvRoomNo;
	BYTE nickLen;
	WCHAR buf[128];
	std::wstring nickName;
	BYTE chatLen;
	BYTE status;

	*pPacket >> recvAccountNo >> recvRoomNo;
	if (recvRoomNo != pDummy->_roominfo._curRoomNo)
		DebugBreak();
	if (recvAccountNo == pDummy->_accountNo)
	{
		*pPacket >> nickLen;
		pPacket->GetData(reinterpret_cast<char*>(buf), nickLen * 2);
		buf[nickLen] = 0;
		pDummy->_nickName = buf;
		*pPacket >> chatLen;
		pPacket->GetData(reinterpret_cast<char*>(buf), chatLen * 2);
		buf[chatLen] = 0;
		std::wstring tmpChat = buf;
		
		if (g_chat[pDummy->_roominfo.GetChatIdx()] != tmpChat)
			DebugBreak();
		if (!(pDummy->_status >= CDummy::en_Status::Room))
			DebugBreak();
		End(pDummy);
	}
}


//void CProtocol::RecvChangePosition(CPacket* pPacket, CDummy* pDummy)
//{
//	uintptr_t recvAccountNo;
//	USHORT recvRoomNo;
//	BYTE nickLen;
//	WCHAR buf[128];
//	std::wstring nickName;
//	BYTE chatLen;
//	BYTE status;
//
//	*pPacket >> recvAccountNo >> recvRoomNo;
//	if (recvRoomNo != pDummy->_roominfo._curRoomNo)
//		DebugBreak();
//	if (recvAccountNo == pDummy->_accountNo)
//	{
//		*pPacket >> nickLen;
//		pPacket->GetData(reinterpret_cast<char*>(buf), nickLen * 2);
//		buf[nickLen] = 0;
//		pDummy->_nickName = buf;
//		*pPacket >> chatLen;
//		pPacket->GetData(reinterpret_cast<char*>(buf), chatLen * 2);
//		buf[chatLen] = 0;
//		std::wstring tmpChat = buf;
//
//		if (g_chat[pDummy->_roominfo.GetChatIdx()] != tmpChat)
//			DebugBreak();
//		if (!(pDummy->_status >= CDummy::en_Status::Room && pDummy->_status <= CDummy::en_Status::P2))
//			DebugBreak();
//		End(pDummy);
//	}
//}

// =============================================
CPacket* CProtocol::MakeRequestChangePosition(CDummy* pDummy, BYTE from, BYTE to)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_ChangePositionPlayerRequest);

	auto pChangePacket = InitPacket();			// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)
	auto currentRoomNo = pDummy->_roominfo._curRoomNo;	// type: USHORT
	BYTE nickLen = pDummy->_nickName.size();

	*pChangePacket << packetType << accountNo << currentRoomNo << from << to << nickLen;
	pChangePacket->PutData(reinterpret_cast<const char*>(pDummy->_nickName.c_str()), nickLen * 2);

	return pChangePacket;

	//*pPacket >> recvAccountNo >> recvRoomNo >> from >> to;
}
void CProtocol::ChangePositionProcedure(CDummy* pDummy, BYTE from, BYTE to)
{
	CPacket* pPacket = MakeRequestChangePosition(pDummy, from, to);

	Start(pDummy, pPacket);
}
void CProtocol::RecvChangePosition(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;
	USHORT recvRoomNo;
	BYTE from, to, status;
	BYTE nickLen;
	WCHAR buf[20];

	
	*pPacket >> recvAccountNo >> recvRoomNo;
	if (recvAccountNo == pDummy->_accountNo)
	{
		*pPacket >>  from >> to >> nickLen;
		pPacket->GetData(reinterpret_cast<char*>(buf), nickLen * 2);
		*pPacket >> status;
		
		if (from == to)
			DebugBreak();
		if (recvRoomNo != pDummy->_roominfo._curRoomNo)
			DebugBreak();
		if (status > 0)
		{
			if (to == 1)
				pDummy->_status = CDummy::en_Status::P1;
				//InterlockedExchange((long*)&pDummy->_status, CDummy::en_Status::P1);
			else if (to == 2)
				pDummy->_status = CDummy::en_Status::P2;
				//InterlockedExchange((long*)&pDummy->_status, CDummy::en_Status::P2);
			else if (to == 3)
				pDummy->_status = CDummy::en_Status::Room;
				//InterlockedExchange((long*)&pDummy->_status, CDummy::en_Status::Room);
		}
		End(pDummy);
	}
}


//============================= Pass
CPacket* CProtocol::MakeRequestReady(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_ReadyRequest);

	auto pReadyPacket = InitPacket();			// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)
	auto currentRoomNo = pDummy->_roominfo._curRoomNo;	// type: USHORT
	BYTE position;
	if (pDummy->_status == CDummy::en_Status::P1)
	{
		position = 1;
	}
	else if (pDummy->_status == CDummy::en_Status::P2)
	{
		position = 2;
	}
	else
	{
		DebugBreak();
		return nullptr;
	}
	*pReadyPacket << packetType << accountNo << currentRoomNo << position;

	return pReadyPacket;
}

void CProtocol::ReadyProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestReady(pDummy);

	Start(pDummy, pPacket);
}

void CProtocol::RecvReady(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;
	USHORT recvRoomNo;
	BYTE position, status;

	*pPacket >> recvAccountNo;
	if (recvAccountNo == pDummy->_accountNo)
	{
		*pPacket >> recvRoomNo >> position >> status;

		if (pDummy->_status == CDummy::en_Status::P1)
		{
			if (position != 1)
				DebugBreak();
			if (status > 0)
			{
				pDummy->_status = CDummy::en_Status::RP1;
				End(pDummy);
			}
		}
		if (pDummy->_status == CDummy::en_Status::P2)
		{
			if (position != 2)
				DebugBreak();
			if (status > 0)
			{
				pDummy->_status = CDummy::en_Status::RP2;
				End(pDummy);
			}
		}
	}
}

CPacket* CProtocol::MakeRequestCancel(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_CancelReadyRequest);

	auto pReadyPacket = InitPacket();			// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)
	auto currentRoomNo = pDummy->_roominfo._curRoomNo;	// type: USHORT
	BYTE position;
	if (pDummy->_status == CDummy::en_Status::RP1)
	{
		position = 1;
	}
	else if (pDummy->_status == CDummy::en_Status::RP2)
	{
		position = 2;
	}
	else
	{
		DebugBreak();
		return nullptr;
	}
	*pReadyPacket << packetType << accountNo << currentRoomNo << position;

	return pReadyPacket;
}

void CProtocol::CancelProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestCancel(pDummy);

	Start(pDummy, pPacket);
}

void CProtocol::RecvCancel(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t recvAccountNo;
	USHORT recvRoomNo;
	BYTE position, status;

	*pPacket >> recvAccountNo;
	if (recvAccountNo == pDummy->_accountNo)
	{
		*pPacket >> recvRoomNo >> position >> status;

		if (pDummy->_status == CDummy::en_Status::RP1)
		{
			if (position != 1)
				DebugBreak();
			if (status > 0)
			{
				pDummy->_status = CDummy::en_Status::P1;
				End(pDummy);
			}
		}
		if (pDummy->_status == CDummy::en_Status::RP2)
		{
			if (position != 2)
				DebugBreak();
			if (status > 0)
			{
				pDummy->_status = CDummy::en_Status::P2;
				End(pDummy);
			}
		}
	}
}


void CProtocol::RecvGameStart(CPacket* pPacket, CDummy* pDummy)
{
	USHORT recvRoomNo;

	*pPacket >> recvRoomNo;
	
	if (pDummy->_status == CDummy::en_Status::RP1)
	{
		pDummy->_status = CDummy::en_Status::Black;
	}
	if (pDummy->_status == CDummy::en_Status::RP2)
	{
		pDummy->_status = CDummy::en_Status::White;
	}
}


CPacket* CProtocol::MakeRequestPutStone(CDummy* pDummy)
{
	const auto packetType = static_cast<USHORT>(en_Protocol::en_PutStoneRequest);

	auto pStonePacket = InitPacket();			// type: CPacket*
	auto accountNo = pDummy->_accountNo;		// type: uintptr_t(8)
	auto currentRoomNo = pDummy->_roominfo._curRoomNo;	// type: USHORT
	BYTE position;
	BYTE x, y;
	if (pDummy->_status == CDummy::en_Status::Black)
	{
		position = 1;
	}
	else if (pDummy->_status == CDummy::en_Status::White)
	{
		position = 2;
	}
	else
	{
		DebugBreak();
		return nullptr;
	}
	x = rand() % 15;
	y = rand() % 15;
	*pStonePacket << accountNo << currentRoomNo << position << x << y;
	return pStonePacket;
}

void CProtocol::PutStoneProcedure(CDummy* pDummy)
{
	CPacket* pPacket = MakeRequestPutStone(pDummy);
	Start(pDummy, pPacket);
}

void CProtocol::RecvPutStone(CPacket* pPacket, CDummy* pDummy)
{
	uintptr_t reccvAccountNo;
	USHORT recvRoomNo;
	BYTE status;
	BYTE position, recvX, recvY;
	

	*pPacket >> reccvAccountNo >> recvRoomNo >> status >> recvX >> recvY >> position;
	pDummy->_roominfo._board[recvX][recvY] = position;
	if (reccvAccountNo == pDummy->_accountNo)
	{

		End(pDummy);
	}
}