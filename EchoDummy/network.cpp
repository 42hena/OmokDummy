#include "network.h"

sockaddr_in g_servAddr;
int g_addrSize;

alignas (64) uintptr_t g_connectTotalCnt;
alignas (64) uintptr_t g_connectSuccessCnt;
alignas (64) uintptr_t g_connectFailCnt;
alignas (64) uintptr_t g_disconnectCnt;
alignas (64) uintptr_t g_packetCnt;
alignas (64) uintptr_t g_sendCnt;
alignas (64) uintptr_t g_recvCnt;