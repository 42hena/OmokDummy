#include "network.h"

sockaddr_in g_servAddr;
int g_addrSize;

alignas (64) uintptr_t g_connectTotalCnt;
alignas (64) uintptr_t g_connectSuccessCnt;
alignas (64) uintptr_t g_connectFailCnt;
alignas (64) uintptr_t g_disconnectCnt;

alignas (64) uintptr_t g_totalCnt;
alignas (64) uintptr_t g_acceptCnt;
alignas (64) uintptr_t g_sendCnt;
alignas (64) uintptr_t g_recvCnt;

alignas (64) uintptr_t g_MinTime;
alignas (64) uintptr_t g_MaxTime;
alignas (64) uintptr_t g_totalTime;

alignas (64) uintptr_t g_MinSecTime;
alignas (64) uintptr_t g_MaxSecTime;
alignas (64) uintptr_t g_totalSecTime;
alignas (64) uintptr_t g_secCnt;



uintptr_t GetInitAcceptTPS()
{
	return InterlockedExchange(&g_acceptCnt, 0);
}
uintptr_t GetInitSendTPS()
{
	return InterlockedExchange(&g_sendCnt, 0);
}

uintptr_t GetInitRecvTPS()
{
	return InterlockedExchange(&g_recvCnt, 0);
}

uintptr_t GetTotalAvgTime()
{
	if (g_totalCnt == 0)
		return 0;
	else
		return g_totalTime / g_totalCnt;
}

uintptr_t GetTotalMinTime()
{
	if (g_totalCnt == 0)
		return 0;
	else
		return g_MinTime;
}

uintptr_t GetTotalMaxTime()
{
	if (g_totalCnt == 0)
		return 0;
	else
		return g_MaxTime;
}

uintptr_t GetSecAvgTime()
{
	if (g_secCnt == 0)
		return 0;
	else
	{
		return InterlockedExchange(&g_totalSecTime, 0) / InterlockedExchange(&g_secCnt, 0);
	}
}

uintptr_t GetSecMinTime()
{
	if (g_secCnt == 0)
		return 0;
	else
		return InterlockedExchange(&g_MinSecTime, 1e9);
}

uintptr_t GetSecMaxTime()
{
	if (g_secCnt == 0)
		return 0;
	else
		return InterlockedExchange(&g_MaxSecTime, 0);
}