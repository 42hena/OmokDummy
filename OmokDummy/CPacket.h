#ifndef  __CPacket_H__
#define  __CPacket_H__
#include <Windows.h>
#include "CBucketPool.h"


class CPacket
{
public:

	// ������
	CPacket();

	// ��� ��� + �Ҹ���
	virtual ~CPacket();

	// ####################################################################################################
	// #                   Utils                                                                          #
	// ####################################################################################################

	// ##################################################
	// # ���� ó�� ���·� �ǵ�����                      #
	// #                                                #
	// # Param  : (None)                                # 
	// # return : (None)                                # 
	// ##################################################
	void	Clear(void);

	// ##################################################
	// # �ִ� ������ ũ��                               #
	// #                                                #
	// # Param  : (None)                                # 
	// # return : (int : Max Buffer size)               # 
	// ##################################################
	int	GetBufferSize(void) { return iBufferSize; }

	// ##################################################
	// # ���� ���� ���� ������ ������                   #
	// #                                                #
	// # Param  : (None)                                #
	// # return : (int : using Buffer size)             #
	// ##################################################
	__int64	GetDataSize(void) { return pPush - pPop; }

	// ##################################################
	// # ������ ó�� ��ġ�� ��ȯ                        #
	// #                                                #
	// # Param  : (None)                                #
	// # return : (char * : buffer_ptr)                 #
	// ##################################################
	char* GetBufferPtr(void) { return buffer; }

	//////////////////////////////////////////////////////////////////////////
	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	int GetLastPtr(void) { return (int)(pPush - buffer); }


	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ���.
	//
	// Parameters: (char *)Dest ������. (int)Size.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int		GetData(char* chpDest, int iSize);
	void	GetCheckData(char* chpDest, int iSrcSize);
	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ����.
	//
	// Parameters: (char *)Src ������. (int)SrcSize.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int		PutData(const char* chpSrc, int iSrcSize);


	/* ============================================================================= */
	// ������ �����ε�
	/* ============================================================================= */
	//CPacket& operator = (CPacket& clSrCPacket);

	//////////////////////////////////////////////////////////////////////////
	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator << (bool);

	CPacket& operator << (char);
	CPacket& operator << (unsigned char);

	CPacket& operator << (short);
	CPacket& operator << (unsigned short);

	CPacket& operator << (int);
	CPacket& operator << (unsigned int);

	CPacket& operator << (long);
	CPacket& operator << (unsigned long);

	CPacket& operator << (__int64);
	CPacket& operator << (unsigned __int64);

	CPacket& operator << (float);
	CPacket& operator << (double);

	//////////////////////////////////////////////////////////////////////////
	// ����.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	CPacket& operator >> (bool&);

	CPacket& operator >> (char&);
	CPacket& operator >> (unsigned char&);

	CPacket& operator >> (short&);
	CPacket& operator >> (unsigned short&);

	CPacket& operator >> (int&);
	CPacket& operator >> (unsigned int&);

	CPacket& operator >> (long&);
	CPacket& operator >> (unsigned long&);

	CPacket& operator >> (__int64&);
	CPacket& operator >> (unsigned __int64&);

	CPacket& operator >> (float&);
	CPacket& operator >> (double&);

	//protected:
	char* pPush;
	char* pPop;
	char* buffer;
	char* checksum;
	int refCount;

	int	iBufferSize;
	int headerSize;
	static CBucketPool<CPacket> pool;
	long encodeFlag; // Change

	int SetHeader(char* dest, int pos, int size);
	int GetHeader(char* dest, int pos, int size);
	int Encoding();
	int Decoding();

	static CPacket* Alloc()
	{
		CPacket* ptr;

		ptr = pool.Alloc();
		if (ptr == nullptr)
		{
			wprintf(L"Alloc Error\n");
			return nullptr;
		}
		if (ptr->GetRefCount() != 0)
		{
			DebugBreak();
		}
		ptr->Clear();
		return ptr;
	}

	static void Free(CPacket* packet)
	{
		// CBucketPool<Node>
		pool.Free(packet);
	}

	static int GetUseBucket()
	{
		return pool.GetUseBucket();
	}
	static int GetTotalBucket()
	{
		return pool.GetTotalBucket();
	}
	static int GetTotalNode()
	{
		return pool.GetTotalNode();
	}

	static int GetUseNode()
	{
		return pool.GetUseNode();
	}

	long GetRefCount()
	{
		return refCount;
	}

	long AddRef()
	{
		return InterlockedIncrement((long*)&refCount);
	}


	long subRef()
	{
		long nowCount;
		nowCount = InterlockedDecrement((long*)&refCount);
		if (nowCount == 0)
		{
			CPacket::Free(this);
		}
		else if (nowCount < 0)
		{
			DebugBreak();
		}
		return nowCount;
	}


	char* GetCheckPtr()
	{
		return checksum;
	}
	int GetTotal()
	{
		int total = 0;
		char* s = checksum + 1;
		while (s != pPush)
		{
			total += *s;
			s++;
		}
		return total;
	}

};

#endif