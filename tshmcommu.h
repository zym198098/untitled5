#pragma once

#include <map>
#include <iostream>
#include <string>
#include <stdint.h>
#include <fstream>
#include <QMutex>
#include <QAtomicInt>
//#include "oi_channel.h"
using namespace std;



#if defined (WIN32)
#include<windows.h>
#include<stdlib.h>
#define PATH_MAX _MAX_PATH
typedef HANDLE   ZEN_HANDLE;
#else
typedef int   ZEN_HANDLE;
#endif


typedef struct
{
	unsigned usedlen_;
	unsigned freelen_;
	unsigned totallen_;
	unsigned shmkey_;
	unsigned shmid_;
	unsigned shmsize_;
} TMQStat;

typedef struct
{
	volatile uint32_t  msg_count;     // msg count in the queue
	//volatile uint32_t  process_count; // processed count last interval
	//volatile uint32_t  flag;          // need_notify flag
	//volatile uint32_t  curflow;       // current flow
} Q_STATINFO;

class CShmMQ
{
public:
	CShmMQ();
	~CShmMQ();
	int init(int shmkey, int shmsize);
	int clear();
	int enqueue(const void* data, unsigned data_len, unsigned flow);
	int dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);
	int GetKey()
	{
		return shmkey_;
	}
	int GetSize()
	{
		return shmsize_;
	}
	inline void* memory()
	{
		return shmmem_;
	}


	inline bool  isEmpty()
	{
		return pstat_->msg_count == 0;
	}

	inline int GetMsgCount()
	{
		return pstat_->msg_count;
	}
protected:
	int shmkey_;
	int shmsize_;
	int shmid_;
	void* shmmem_;
	map<string, void *> m_vShm;
	Q_STATINFO* pstat_;

	volatile uint32_t * head_;
	volatile uint32_t* tail_;
	volatile uint32_t* pid_;       // pid of process doing clear operation

	char* block_;
	uint32_t blocksize_;

	bool do_check(unsigned head, unsigned tail); // check if head & tail is valid
	int getmemory(int shmkey, int shmsize);
	virtual void fini();
public:
	void lock();
	void unlock();

public:
	void  *m_pshare_obj;
	void * m_pmmap;
	QMutex m_mutex;

};

class CShmProducer
{
public:
	CShmProducer();
	virtual ~CShmProducer();
	virtual   int init(int shmkey, int shmsize);
	virtual   int clear();



	virtual int produce(const void* data, unsigned data_len, unsigned flow);

	virtual void fini();


	CShmMQ * mq_;

};

class CShmProducerL :public CShmProducer
{
public:
	CShmProducerL();
	~CShmProducerL();
	int init(int shmkey, int shmsize);
	int clear();
	virtual int produce(const void* data, unsigned data_len, unsigned flow);
protected:
	std::fstream mfd_;
	char mfile_[128];
	void fini();
	QMutex mutex;
};


class CShmComsumer
{
public:
	CShmComsumer();
	virtual ~CShmComsumer();
	virtual   int init(int shmkey, int shmsize);
	virtual  int comsume(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);

	virtual    int clear();

	void fini();
public:
	CShmMQ* mq_;
};

class CShmComsumerL : public CShmComsumer
{
public:
	CShmComsumerL();
	~CShmComsumerL();
	virtual  int comsume(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);
	virtual  int clear();
	void fini();
	int init(int shmkey, int shmsize);

protected:
	std::fstream mfd_;
	char mfile_[128];
	QMutex mutex;
};


class CShmCommu
{
public:
	CShmCommu();
	virtual ~CShmCommu();
	virtual   int Init(int shmkey, int shmsize);
	virtual  int Put(const void* data, unsigned data_len, unsigned flow);
	virtual  int PutL(const void* data, unsigned data_len, unsigned flow);
	virtual  int Get(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);
	virtual  int GetL(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow);
	virtual    int clear();
	void fini();
public:
	CShmMQ* mq_;
	int m_iSize;
	int m_iKey;
};






class CShmQueue
{
public:
	CShmQueue(int iDefSize = 40960);
	~CShmQueue();
	int Put(int id, char *val, int size);
	int Get(int id, void* buf, unsigned buf_size, unsigned& data_len);
	int PutL(int id, char *val, int size);
	int GetL(int id, void* buf, unsigned buf_size, unsigned& data_len);
	void Clear(int id);

private:
	map<int, CShmCommu* > m_vqueue;
	int m_iDefSize;
};



