#pragma once
#include <iostream>

//#define BUCKET_COUNT 20
//#define NODE_COUNT 1000

#define BUCKET_COUNT 1000
#define NODE_COUNT 100
template <typename T>
class CBucketPool {
public:
	struct Node
	{
		Node()
			: next(nullptr)
		{}

		T data;
		struct Node* next;
	};

	struct Bucket
	{
		Bucket()
			//: top(nullptr), next(nullptr), nodeCount(0)
			: top(nullptr), next(nullptr)
		{}
		Node* top;
		struct Bucket* next;
		//int nodeCount;
	};

	struct RootPool
	{
		RootPool()
			: allocBucketList(nullptr), freeBucketList(nullptr), useBucketCount(0), totalBucketCount(0)
		{ }
		Bucket* allocBucketList;
		Bucket* freeBucketList;
		DWORD useBucketCount;
		DWORD totalBucketCount;
	};

	struct TLSPool
	{
		TLSPool()
			: allocList(nullptr), freeList(nullptr), useCount(0), totalCount(0), freeCount(0)
			//: allocList(nullptr), freeList(nullptr), totalCount(0), freeCount(0)
		{ }

		Node* allocList;
		Node* freeList;
		DWORD useCount;
		DWORD freeCount;
		DWORD totalCount;
	};

public:
	CBucketPool()
		:poolIndex(-1)
	{
		tlsIndex = TlsAlloc();
		if (tlsIndex == TLS_OUT_OF_INDEXES)
		{
			wprintf(L"CBucketPool TLS_OUT_OF_INDEXES\n");
			exit(0);
		}
		InitializeSRWLock(&srw);

		Node* newNode;
		Node* tmpNode;
		Bucket* newBucket;
		Bucket* tmpBucket;

		tmpBucket = nullptr;
		for (int i = 0; i < BUCKET_COUNT; ++i)
		{
			newBucket = new Bucket;

			tmpNode = nullptr;
			for (int j = 0; j < NODE_COUNT; ++j)
			{
				newNode = new Node;
				newNode->next = tmpNode;
				tmpNode = newNode;
			}
			newBucket->top = newNode;
			//newBucket->nodeCount = NODE_COUNT;

			newBucket->next = tmpBucket;
			tmpBucket = newBucket;
		}
		root.allocBucketList = newBucket;

		root.totalBucketCount = BUCKET_COUNT;
		root.useBucketCount = 0;
		printf("End\n");
	}

	~CBucketPool()
	{

	}

	Node* NodeAlloc()
	{
		TLSPool* tlsPool;
		Bucket* newBucket;
		Node* newNode;
		// -----

			// thread ���� Ǯ ptr �� ���
		tlsPool = (TLSPool*)TlsGetValue(tlsIndex);
		if (tlsPool->allocList == nullptr)
		{
			AcquireSRWLockExclusive(&srw);

			// Root Ǯ���� Bucket�� �ϳ� �Ҵ� ����.
			newBucket = BucketAlloc();

			//Bucket���� top���� ����.
			tlsPool->allocList = newBucket->top;
			newBucket->top = nullptr;

			// tlsPool�� �� ������ ���� ��Ų��.
			tlsPool->totalCount += NODE_COUNT;
			root.useBucketCount++;

			ReleaseSRWLockExclusive(&srw);
		}

		// �� Node �Ҵ� �� List �ѱ��.
		newNode = tlsPool->allocList;

		tlsPool->allocList = newNode->next;
		newNode->next = nullptr; // �߰�

		//// useCount ����.
		tlsPool->useCount += 1;

		return newNode;
	}

	T* Alloc()
	{
		DWORD ret;
		// -----

		if (TlsGetValue(tlsIndex) == nullptr)
		{
			ret = InterlockedAdd((long*)&poolIndex, 1);
			if (ret == 500)
				DebugBreak();
			TlsSetValue(tlsIndex, &tlspool[ret]);
		}

		return (T*)NodeAlloc();
	}

	void NodeFree(Node* delNode)
	{
		TLSPool* tlsPool;
		int nodeCount = NODE_COUNT;
		// -----

			// thread ���� Ǯ ptr �� ���
		tlsPool = (TLSPool*)TlsGetValue(tlsIndex);

		delNode->next = tlsPool->freeList;
		tlsPool->freeList = delNode;

		// ��� ���� ���߱�.
		tlsPool->freeCount += 1;
		tlsPool->useCount -= 1; // TODO: �߰�

		// ��ȯ ������ ����
		if (tlsPool->freeCount >= nodeCount)
		{
			AcquireSRWLockExclusive(&srw);

			BucketFree(delNode);
			tlsPool->freeList = nullptr;
			tlsPool->totalCount -= nodeCount;
			tlsPool->freeCount = 0;

			ReleaseSRWLockExclusive(&srw);
		}
	}


	void Free(T* delnode)
	{
		DWORD ret;
		// -----

		if (TlsGetValue(tlsIndex) == nullptr)
		{
			wprintf(L"In Free: no TLSptr!\n");
			ret = InterlockedAdd((long*)&poolIndex, 1);
			if (ret == 10)
				DebugBreak();
			TlsSetValue(tlsIndex, &tlspool[ret]);
		}

		NodeFree((Node*)delnode);
	}

	Bucket* BucketAlloc(void)
	{
		Bucket* newBucket;
		Node* newNode;
		Node* tmpNode;
		// -----

		newBucket = root.allocBucketList;
		if (newBucket == nullptr)
		{
			wprintf(L"BucketAlloc Empty\n");

			newBucket = new Bucket;
			tmpNode = nullptr;
			for (int i = 0; i < NODE_COUNT; ++i)
			{
				newNode = new Node;
				newNode->next = tmpNode;
				tmpNode = newNode;
			}
			newBucket->top = newNode;
			InterlockedIncrement(&root.totalBucketCount);
		}

		root.allocBucketList = newBucket->next;

		newBucket->next = root.freeBucketList;
		root.freeBucketList = newBucket;

		return newBucket;
	}

	void BucketFree(Node* delnode)
	{
		Bucket* delBucket;
		// -----

			// root�� FreeBucket���� �̱�
		delBucket = root.freeBucketList;

		// top����
		delBucket->top = delnode;

		// �� ĭ �ű��
		root.freeBucketList = delBucket->next;

		// Alloc Bucket�� ���̰�, �ٲٱ�.
		delBucket->next = root.allocBucketList;
		root.allocBucketList = delBucket;

		root.useBucketCount--;
	}

	DWORD GetTotalBucket()
	{
		return root.totalBucketCount;
	}
	DWORD GetUseBucket()
	{
		return root.useBucketCount;
	}
	DWORD GetTotalNode()
	{
		return root.useBucketCount * NODE_COUNT;
	}
	DWORD GetUseNode()
	{
		DWORD total = 0;
		for (int i = 0; i < tlsIndex; ++i)
		{
			total += tlspool[i].useCount;
		}
		return total;
	}

public:

	TLSPool tlspool[500];
	DWORD tlsIndex;
	DWORD poolIndex;

	RootPool root;
	SRWLOCK srw;
private:
};


