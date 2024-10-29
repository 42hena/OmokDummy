#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include <locale> 
#include <Windows.h>


#include "Util.h"
#include "network.h"
#include "CDummy.h"
#include "CPacket.h"
#include "CProtocol.h"
#include "Contents.h"



extern int g_threadCnt;
extern int g_dummyCnt;
extern CDummy g_dummy[5000];
extern alignas (64) uintptr_t g_exitCount;
extern alignas (64) uintptr_t g_echoCount;
extern alignas (64) uintptr_t g_dddd;

extern int g_loginDelay;
extern int g_actionDelay;

extern int g_randDisconnect;
extern int g_randContent;
extern int g_randConnect;

unsigned int MonitoringThread(LPVOID param)
{
	int c = g_dummyCnt;
	int t = g_threadCnt;
	int replyWaitCnt = 0;
	int downClientCnt = 0;
	for (;;)
	{
		downClientCnt = 0;
		replyWaitCnt = 0;
		for (int i = 0; i < c * t; ++i)
		{
			replyWaitCnt += g_dummy[i]._waitFlag;
			downClientCnt += g_dummy[i]._errorFlag;
		}
		printf("================================================================================\n");
		printf("Client per Thread        : %4d       |   Thread Count           : %4d\n", c, t);
		printf("Login Delay              : %4d(ms)   |   Action Delay           : %4d(ms)\n", g_loginDelay, g_actionDelay);
		printf("Connect probability      : %3d%%       |   content probability  : %3d%%\n", g_randConnect, g_randContent);
		printf("Disconnect probability   : %3d%%\n", g_randDisconnect);
		printf("================================================================================\n");
		printf("Connect Total            : %llu\n", g_connectTotalCnt);
		printf("Connect Fail             : %llu\n", g_connectFailCnt);
		printf("Down Client              : %d\n", downClientCnt);
		printf("Reply Wait               : %d\n", replyWaitCnt);
		printf("================================================================================\n");
		printf("ConnetPacket TPS         : %llu\n", GetInitSendTPS());
		printf("SendPacket TPS           : %llu\n", GetInitSendTPS());
		printf("RecvPacket TPS           : %llu\n", GetInitRecvTPS());
		printf("================================================================================\n");
		printf("Action Delay Avg         : %llu ms\n", GetTotalAvgTime());
		printf("Action Delay Min         : %llu ms\n", GetTotalMinTime());
		printf("Action Delay Max         : %llu ms\n", GetTotalMaxTime());
		printf("Check: %llu %llu\n", g_dddd, g_exitCount);
		printf("================================================================================\n");
		printf("packet Alloc: %d / Use: %d\n", CPacket::GetUseNode(), CPacket::GetUseNode());
		Sleep(1000);
	}
}