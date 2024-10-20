#pragma once
#include <ws2tcpip.h>

extern sockaddr_in g_servAddr;
extern int g_addrSize;

extern alignas (64) uintptr_t g_connectTotalCnt;
extern alignas (64) uintptr_t g_connectSuccessCnt;
extern alignas (64) uintptr_t g_connectFailCnt;
extern alignas (64) uintptr_t g_disconnectCnt;
extern alignas (64) uintptr_t g_packetCnt;
extern alignas (64) uintptr_t g_sendCnt;
extern alignas (64) uintptr_t g_recvCnt;