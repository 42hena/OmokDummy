#include <iostream>
#include <Windows.h>

void WSAStartUpError(DWORD errCode)
{
	wprintf(L"StartupError:%d\n", errCode);
	switch (errCode)
	{
	case WSASYSNOTREADY:
	case WSAVERNOTSUPPORTED:
	case WSAEINPROGRESS:
	case WSAEPROCLIM:
	case WSAEFAULT:
		break;
	default:
		DebugBreak();
		break;
	}
}

void SelectError(DWORD errCode)
{
	switch (errCode)
	{
	case WSAEINTR:
		printf("A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.\n");
		break;
	case WSAEFAULT:
		printf("The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space\n");
		break;
	case WSAEINVAL:
		printf("The time-out value is not valid, or all three descriptor parameters were null.\n");
		break;
	case WSAEINPROGRESS:
		printf("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.\n");
		break;
	case WSAENOTSOCK:
		printf("One of the descriptor sets contains an entry that is not a socket.\n");
		break;
	case WSAENETDOWN:
		printf("The network subsystem has failed.\n");
		break;
	case WSANOTINITIALISED:
		printf("Need WSAStartup\n");
		break;
	default:
		printf("ErrorCode:%d\n", errCode);
		break;
	}
}
