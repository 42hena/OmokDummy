#pragma once
class CProtocol
{
public:
	enum class en_Protocol : USHORT{
		en_RequestLogin = 1,
		en_ResponseLogin = 2,

		en_RequestCreateRoom = 201,
		en_ResponseCreateRoom = 202,

		en_RequestEnterRoom = 301,
		en_ResponseEnterRoom = 302,


		en_ResponseEnterAlarm = 305,

		en_RequestLeaverRoom = 401,
		en_ResponseLeaveRoom = 402,

		en_RequestChatting = 501,
		en_ResponseChatting = 502,


		en_Position = 1000,
		en_ChangePositionPlayerRequest,
		en_ChangePositionSpectatorRequest,
		en_ChangePositionPlayerResponse,
		en_ChangePositionSpectatorResponse,

		en_Game = 1100,
		en_ReadyRequest,
		en_ReadyResponse,
		en_CancelReadyRequest,
		en_CancelReadyResponse,
		en_GameStart,

		en_PutStoneRequest = 1108,
		en_PutStoneResponse,
		en_GameOver,
		en_GameStatus,
		//en_RequestRoomList = 13,
		//en_ResponseRoomList = 14,


		en_RequestGracefulShutdown = 59000,
		en_ResponseGracefulShutdown = 59001,

		en_RequestEcho = 60000,
		en_ResponseEcho = 60001,
	};

	enum class en_LobbyProtocol : USHORT {
		en_CreateRoom,
		en_EnterRoom,
		en_GetRoomList,
	};

	enum class en_RoomProtocol : USHORT {
		en_Chat,
		en_LeaveRoom,
		en_GetRoomList,
		en_ChangePosition1,
		en_ChangePosition2,
	};

public:
	static CPacket* MakeRequestLoginPacket(CDummy*);
	static CPacket* MakeRequestCreateRoomPacket(CDummy*);
	static CPacket* MakeRequestEnterRoomPacket(CDummy*);
	static CPacket* MakeRequestRoomListPacket(CDummy*);
	static CPacket* MakeRequestLeaveRoomPacket(CDummy*);
	static CPacket* MakeRequestChattingPacket(CDummy*);
	static CPacket* MakeRequestShutdownPacket(CDummy*);

public:
	static void LoginProcedure(CDummy*);
	static void CreateRoomProcedure(CDummy*);
	static void EnterRoomProcedure(CDummy*);

	static void ShutdownProcedure(CDummy*);
	static void LeaveRoomProcedure(CDummy*);
	static void ChattingProcedure(CDummy*);


public:
	static void RecvShutdown(CPacket*, CDummy*);
	static void RecvCreateRoom(CPacket*, CDummy*);
	static void RecvEnterRoom(CPacket*, CDummy*);
	static void RecvLeaveRoom(CPacket*, CDummy*);
	static void RecvChatting(CPacket*, CDummy*);
	

	// position
	static CPacket* MakeRequestChangePosition(CDummy*, BYTE, BYTE);
	static void ChangePositionProcedure(CDummy*, BYTE, BYTE);
	static void RecvChangePosition(CPacket* pPacket, CDummy* pDummy);

	// ready
	static CPacket* MakeRequestReady(CDummy*);
	static void ReadyProcedure(CDummy*);
	static void RecvReady(CPacket* pPacket, CDummy* pDummy);

	// cancel
	static CPacket* MakeRequestCancel(CDummy*);
	static void CancelProcedure(CDummy*);
	static void RecvCancel(CPacket* pPacket, CDummy* pDummy);

	// gameStart
	static void RecvGameStart(CPacket* pPacket, CDummy* pDummy);

	// Putstone
	static CPacket* MakeRequestPutStone(CDummy*);
	static void PutStoneProcedure(CDummy*);
	static void RecvPutStone(CPacket* pPacket, CDummy* pDummy);

	// gameOver
	static void RecvGameOver(CPacket* pPacket, CDummy* pDummy);
	static void RecvGameOverCheck(CPacket* pPacket, CDummy* pDummy);
};

