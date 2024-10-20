#pragma once
#include <ws2tcpip.h>

extern sockaddr_in g_servAddr;
extern int g_addrSize;

extern alignas (64) uintptr_t g_connectTotalCnt;
extern alignas (64) uintptr_t g_connectSuccessCnt;
extern alignas (64) uintptr_t g_connectFailCnt;
extern alignas (64) uintptr_t g_disconnectCnt;
extern alignas (64) uintptr_t g_totalCnt;
extern alignas (64) uintptr_t g_acceptCnt;
extern alignas (64) uintptr_t g_sendCnt;
extern alignas (64) uintptr_t g_recvCnt;

extern alignas (64) uintptr_t g_MinTime;
extern alignas (64) uintptr_t g_MaxTime;
extern alignas (64) uintptr_t g_totalTime;

extern alignas (64) uintptr_t g_MinSecTime;
extern alignas (64) uintptr_t g_MaxSecTime;
extern alignas (64) uintptr_t g_totalSecTime;
extern alignas (64) uintptr_t g_secCnt;

enum class en_StatusCount {
	LobbyCnt = 1,
};

uintptr_t GetInitAcceptTPS();
uintptr_t GetInitSendTPS();
uintptr_t GetInitRecvTPS();

uintptr_t GetTotalAvgTime();
uintptr_t GetTotalMinTime();
uintptr_t GetTotalMaxTime();
uintptr_t GetSecAvgTime();
uintptr_t GetSecMinTime();
uintptr_t GetSecMaxTime();