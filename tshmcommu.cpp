#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QSharedMemory>


#include"tshmcommu.h"
using namespace std;

#define C_HEAD_SIZE  4
#define C_SAFE_AREA_LEN  4
#define C_SAFE_AREA_VAL  0x58505053 //"SPPX"
#define C_MAX_BUF_SIZE 4194304




#define LOCK_TYPE_NONE		0x0
#define LOCK_TYPE_PRODUCER	0x1
#define LOCK_TYPE_COMSUMER	0x2

#define COMMU_ERR_SHMGET	-101
#define COMMU_ERR_SHMNEW	-102
#define COMMU_ERR_SHMMAP	-103

#define COMMU_ERR_FILEOPEN 	-111
#define COMMU_ERR_FILEMAP	-112
#define COMMU_ERR_MQFULL	-121
#define COMMU_ERR_MQEMPTY  	-122
#define COMMU_ERR_OTFBUFF	-123
#define COMMU_ERR_SHM_NULL	-124
#define COMMU_ERR_CHKFAIL   -126
#define COMMU_ERR_MSG_TIMEOUT   -127

#define COMMU_ERR_FLOCK     -201







//-------------------------------------------------------------------------------------------------
//SystemV的共享内存

//提供这组模拟对我来说仅仅是为了好玩，（当然也由于System V有广大的深厚群众基础，至少让你移植起来容易一点）
//我个人对System V的IPC没有爱，一方面毕竟不如POSIX IPC在标准上站住了脚，System V的IPC这方面要弱一点，另一方面System V IPC 的接口设计也不如POSIX那么优雅，

//创建或者访问一个共享内存区


ZEN_HANDLE shmget(int sysv_key, size_t size, int oflag)
{
#if defined (WIN32)

	DWORD nt_flag_protect = 0;


	//  nt_flag_protect = PAGE_READONLY;
	nt_flag_protect = PAGE_READWRITE;

	char map_file_name[PATH_MAX + 1];
	map_file_name[PATH_MAX] = 0;
	_snprintf(map_file_name, PATH_MAX, "SYSV_IPC_SHM_%d.$$$", sysv_key);

	LARGE_INTEGER longlong_value;
	longlong_value.QuadPart = size;

	//file_handle == ZEN_INVALID_HANDLE后，创建的共享内存不再文件里面，而在系统映射文件中 system paging file
	ZEN_HANDLE shm_handle = ::CreateFileMappingA(INVALID_HANDLE_VALUE,
		NULL,
		nt_flag_protect,
		longlong_value.HighPart,
		longlong_value.LowPart,
		map_file_name);

	//解释一下上面最后一行的参数，当使用key为IPC_PRIVATE，每次都创建一个无名的贡献内存，
	//如果Key不为IPC_PRIVATE，我统一给他起一个名字

	//如果出现错误
	if (shm_handle == 0)
	{
		return INVALID_HANDLE_VALUE;
	}



	return shm_handle;

#else
	return ::shmget(sysv_key, size, oflag);
#endif
}



//打开已经shmget的共享内存区
void * shmat(ZEN_HANDLE shmid, const void *shmaddr, int shmflg)
{
#if defined (WIN32)
	DWORD nt_flags = FILE_MAP_WRITE | FILE_MAP_READ;

	//偏移量
	LARGE_INTEGER longlong_value;
	longlong_value.QuadPart = 0;

	//size参数为0标识全部内存映射
	void *addr_mapping = ::MapViewOfFileEx(shmid,
		nt_flags,
		longlong_value.HighPart,
		longlong_value.LowPart,
		0,
		(LPVOID)shmaddr);

	//如果映射失败

	return addr_mapping;


#else
	return ::shmat(shmid, shmaddr, shmflg);
#endif
}

//短接这个内存区
int  shmdt(const void *shmaddr)
{
#if defined (WIN32)
	BOOL ret_bool = ::UnmapViewOfFile(shmaddr);

	if (ret_bool == FALSE)
	{
		return -1;
	}

	return 0;

#else
	return ::shmdt(shmaddr);
#endif
}

//对共享内存区提供多种操作
int  shmctl(ZEN_HANDLE shmid, int cmd, struct shmid_ds *buf)
{
#if defined (WIN32)

	//微软的映射内存当引用计数为0时会自己释放的，所以我姑且相信你使用正确把
	if (0 == cmd)
	{
		return 0;
	}

	return -1;

#else
	return ::shmctl(shmid, cmd, buf);
#endif
}



CShmMQ::CShmMQ() : shmkey_(0), shmsize_(0), shmid_(0),
shmmem_(NULL), pstat_(NULL), head_(NULL), tail_(NULL), block_(NULL), blocksize_(0)
{
	m_pmmap = NULL;
	m_pshare_obj = NULL;
}

CShmMQ::~CShmMQ()
{
	fini();
}
#if 0
int CShmMQ::getmemory(int shmkey, int shmsize)
{
	shmsize += sizeof(Q_STATINFO);

	char sMemName[64];
	sprintf(sMemName, "shmmem%d", shmkey);
	QSharedMemory * pShm;
	try
	{
		QString s;

		pShm = (QSharedMemory *)new QSharedMemory(sMemName);
		m_pshare_obj = pShm;
		if (!pShm->create(shmsize, QSharedMemory::ReadWrite))
		{
			if (!pShm->attach())
			{
				s = pShm->errorString();
				return COMMU_ERR_SHMMAP;
			}
		}
		s = pShm->errorString();
		shmmem_ = pShm->data();

		pstat_ = (Q_STATINFO*)shmmem_;
		new (pstat_)Q_STATINFO();
		shmmem_ = (void*)((unsigned long)shmmem_ + sizeof(Q_STATINFO));
	}
	catch (...)
	{
		return COMMU_ERR_SHMMAP;
	}

	return 0;
}
#endif

#if 1
int CShmMQ::getmemory(int shmkey, int shmsize)
{
	//只能在本进程内通讯
	shmsize += sizeof(Q_STATINFO);

	char sMemName[64];
	sprintf(sMemName, "shmmem%d", shmkey);
	if (m_vShm.find(string(sMemName)) == m_vShm.end())
	{
		//new shm
		shmmem_ = new char[shmsize];
		memset(shmmem_, 0, shmsize);
		m_vShm.insert(pair<string, void *>(string(sMemName), shmmem_));
	}
	else
	{
		shmmem_ = m_vShm[string(sMemName)];
	}
	pstat_ = (Q_STATINFO*)shmmem_;
	pstat_->msg_count = 0;
	shmmem_ = (void*)((char *)shmmem_ + sizeof(Q_STATINFO));
	return 0;
}
#endif
#if 0
int CShmMQ::getmemory(int shmkey, int shmsize)
{
	//只能在本进程内通讯
	shmsize += sizeof(Q_STATINFO);

	char sMemName[64];
	ZEN_HANDLE h = shmget(shmkey, shmsize, 0);
	if (h == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
	shmmem_ = shmat(h, NULL, 0);

	if (shmmem_ == NULL)
	{
		return -2;
	}
	pstat_ = (Q_STATINFO*)shmmem_;
	new (pstat_)Q_STATINFO();
	shmmem_ = (void*)((unsigned long)shmmem_ + sizeof(Q_STATINFO));
	return 0;
}
#endif
void CShmMQ::fini()
{
	if (m_pmmap) delete m_pmmap;
	if (m_pshare_obj) delete m_pshare_obj;
}
void CShmMQ::lock()
{
	m_mutex.lock();
	// ((QSharedMemory *)m_pshare_obj)->lock();
}
void CShmMQ::unlock()
{
	m_mutex.unlock();
	//  ((QSharedMemory *)m_pshare_obj)->unlock();
}
int CShmMQ::init(int shmkey, int shmsize)
{
	fini();

	shmkey_ = shmkey;
	shmsize_ = shmsize;
	int ret = 0;

	/*
	ret>0:create new shm segment
	ret=0:use shm segment of the same key
	ret<0:error occurs
	*/
	if ((ret = getmemory(shmkey_, shmsize_)) > 0)
	{
		memset(shmmem_, 0x0, sizeof(unsigned)* 2);
	}
	else if (ret < 0)
	{
		switch (ret)
		{
		case COMMU_ERR_SHMMAP:
			break;
		case COMMU_ERR_SHMNEW:
			break;
		default:
			fprintf(stderr, "CShmMQ::getmomory() return %d, can not be recognised\n", ret);
		}

		return ret;
	}
	//Q_STATINFO + head + tail + pid  + blocks
	//the head of data section
	head_ = (uint32_t*)shmmem_;
	//the tail of data section
	tail_ = head_ + 1;
	// pid field
	pid_ = (uint32_t *)(tail_ + 1);
	//data section base address
	block_ = (char*)(pid_ + 1);
	//data section length
	blocksize_ = shmsize_ - sizeof(unsigned)* 2 - sizeof(int);

	//*pid_ = 0;//

	return 0;
}

int CShmMQ::clear()
{
	if (!shmmem_)
	{
		fprintf(stderr, "shmmem_ is NULL.\n");
		return COMMU_ERR_SHM_NULL;
	}

	//clear head and tail of shm queue
//	*pid_ = 0;
//	*head_ = 0;
//	*tail_ = 0;
	//*pid_ = 0;

	pstat_->msg_count = 0;

	return 0;
}

bool CShmMQ::do_check(unsigned head, unsigned tail)
{

	if ((head <= blocksize_ && tail <= blocksize_))
	{
		// head & tail 鍚堟硶
		return true;
	}



	return false;

}


int CShmMQ::enqueue(const void* data, unsigned data_len, unsigned flow)
{

	//data store format
	// sppx + totallen +  data  + sppx
	uint32_t head = *head_;
	uint32_t tail = *tail_;

	// check if head & tail is valid
	if ((!do_check(head, tail)))
	{
		return COMMU_ERR_CHKFAIL;
	}

	uint32_t free_len = head > tail ? head - tail : head + blocksize_ - tail;
	uint32_t tail_len = blocksize_ - tail;

	char sHead[C_HEAD_SIZE + C_SAFE_AREA_LEN] = { 0 };
	uint32_t total_len = data_len + C_HEAD_SIZE + 2 * C_SAFE_AREA_LEN;

	//as to 4 possible queue free_len,enqueue data in 4 ways
	//  first, if no enough space?
	if ((free_len <= total_len))
	{
		return COMMU_ERR_MQFULL;
	}

	*(unsigned*)sHead = C_SAFE_AREA_VAL;//set magic number "SPPX"
	memcpy(sHead + C_SAFE_AREA_LEN, &total_len, sizeof(unsigned));


	//  copy 8 byte, copy data
	if (tail_len >= total_len)
	{
		//append head
		memcpy(block_ + tail, sHead, C_SAFE_AREA_LEN + C_HEAD_SIZE);
		//append data
		memcpy(block_ + tail + C_SAFE_AREA_LEN + C_HEAD_SIZE, data, data_len);
		//append C_SAFE_AREA_VAL
		*(unsigned*)(block_ + tail + C_SAFE_AREA_LEN + C_HEAD_SIZE + data_len) = C_SAFE_AREA_VAL;
		//recalculate tail_ position
		*tail_ += (data_len + C_HEAD_SIZE + C_SAFE_AREA_LEN * 2);
	}
	//  third, if tail space > 4+8 && < 4+8+len+4
	else if (tail_len >= C_HEAD_SIZE + C_SAFE_AREA_LEN && tail_len < C_HEAD_SIZE + 2 * C_SAFE_AREA_LEN + data_len)
	{
		//append head, 4+8 byte
		memcpy(block_ + tail, sHead, C_SAFE_AREA_LEN + C_HEAD_SIZE);

		//separate data into two parts
		uint32_t first_len = tail_len - C_SAFE_AREA_LEN - C_HEAD_SIZE;
		uint32_t second_len = data_len + C_SAFE_AREA_LEN - first_len;

		if (second_len >= C_SAFE_AREA_LEN)
		{
			//append first part of data, tail-4-8
			memcpy(block_ + tail + C_SAFE_AREA_LEN + C_HEAD_SIZE, data, first_len);
			//append left, second part of data
			memcpy(block_, ((char*)data) + first_len, second_len - C_SAFE_AREA_LEN);
			*(unsigned *)(block_ + second_len - C_SAFE_AREA_LEN) = C_SAFE_AREA_VAL;
		}
		else
		{
			// copy user data
			memcpy(block_ + tail + C_SAFE_AREA_LEN + C_HEAD_SIZE, data, first_len - C_SAFE_AREA_LEN + second_len);

			//append "SPPX" to the data
			if (second_len == 3)
			{
				*(char*)(block_ + blocksize_ - 1) = 'S';
				*(char*)(block_) = 'P';
				*(char*)(block_ + 1) = 'P';
				*(char*)(block_ + 2) = 'X';
			}
			else if (second_len == 2)
			{
				*(char*)(block_ + blocksize_ - 2) = 'S';
				*(char*)(block_ + blocksize_ - 1) = 'P';
				*(char*)(block_) = 'P';
				*(char*)(block_ + 1) = 'X';
			}
			else if (second_len == 1)
			{
				*(char*)(block_ + blocksize_ - 3) = 'S';
				*(char*)(block_ + blocksize_ - 2) = 'P';
				*(char*)(block_ + blocksize_ - 1) = 'P';
				*(char*)(block_) = 'X';
			}
		}

		//recalculate tail_ position
		// fixed by ericsha 2010-04-23; maybe make share memory queue confused
		/*
		*tail_ += (data_len + 2 * C_SAFE_AREA_LEN + C_HEAD_SIZE);
		*tail_ -= blocksize_;
		*/
		int offset = (int)total_len - (int)blocksize_;
		*tail_ += offset;
	}

	//  fourth, if tail space < 4+8
	else
	{
		//append first part of head, copy tail byte
		memcpy(block_ + tail, sHead, tail_len);

		//append second part of head, copy 8-tail byte
		uint32_t second_len = C_SAFE_AREA_LEN + C_HEAD_SIZE - tail_len;
		memcpy(block_, sHead + tail_len, second_len);

		//append data
		memcpy(block_ + second_len, data, data_len);

		//append C_SAFE_AREA_VAL
		*(uint32_t*)(block_ + second_len + data_len) = C_SAFE_AREA_VAL;

		//recalculate tail_ position
		*tail_ = second_len + data_len + C_SAFE_AREA_LEN;
	}

	// 鏁版嵁鍖呰鏁?
	//atomic_inc32( &(pstat_->msg_count));
	pstat_->msg_count++;
	return pstat_->msg_count;
}

//buf_size must >= real_data_len + 4
int CShmMQ::dequeue(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{

	unsigned head = *head_;
	unsigned tail = *tail_;

	// check if head & tail is valid
	if ((!do_check(head, tail)))
	{
		data_len = 0;
		return -1;
	}

	if (head == tail)
	{
		data_len = 0;
		return -2;
	}
	pstat_->msg_count--;
	//atomic_dec32(&(pstat_->msg_count));
	// sppx + totallen +  data  + sppx

	unsigned used_len = tail > head ? tail - head : tail + blocksize_ - head;
	char sHead[C_SAFE_AREA_LEN + C_HEAD_SIZE];

	// get head
	// if head + 8 > block_size
	if (head + C_SAFE_AREA_LEN + C_HEAD_SIZE > blocksize_)
	{
		unsigned first_size = blocksize_ - head;
		unsigned second_size = C_SAFE_AREA_LEN + C_HEAD_SIZE - first_size;
		memcpy(sHead, block_ + head, first_size);
		memcpy(sHead + first_size, block_, second_size);
		head = second_size;
	}
	else
	{
		memcpy(sHead, block_ + head, C_SAFE_AREA_LEN + C_HEAD_SIZE);
		head += (C_HEAD_SIZE + C_SAFE_AREA_LEN);
	}

	// now head point  to data
	// sppx + totallen +  data  + sppx
	//get meta data
	unsigned total_len = *(unsigned*)(&sHead[C_SAFE_AREA_LEN]);
	//   flow = *(unsigned*)(sHead + C_SAFE_AREA_LEN + sizeof(unsigned));

	//check safe area
	if (*(unsigned*)(sHead) != C_SAFE_AREA_VAL)
	{

		if (do_check(*head_, *tail_))
		{
			// head&tail鏄悎娉曠殑锛屼篃闇�瑕佹竻绌哄叡浜唴瀛? 闈炴硶鏃讹紝鍦╠o_check鍐呴儴宸茬粡鎵ц娓呯┖鍔ㄤ綔
			clear();
		}

		data_len = 0;

		return -3;
	}

	/*瑙ｅ喅鍏变韩鍐呭瓨涔遍棶棰?
	* 褰撳唴瀛樹贡鎺夛紙total_len <= used_le)鐨勬椂鍊欙紝let head:=tail,
	* 涔熷氨鏄叡浜唴瀛樻竻绌?
	* start*/

	//assert(total_len <= used_len);
	if ((total_len > used_len))
	{

		if (do_check(*head_, *tail_))
		{
			// 娓呯┖鍏变韩鍐呭瓨
			clear();
		}

		data_len = 0;
		return -4;
	}
	// sppx + totallen +  data  + sppx
	data_len = total_len - C_HEAD_SIZE - C_SAFE_AREA_LEN - C_SAFE_AREA_LEN;

	if ((data_len > buf_size))
	{

		if (do_check(*head_, *tail_))
		{
			// 娓呯┖鍏变韩鍐呭瓨
			clear();
		}

		data_len = 0;

		return -5;
	}

	if (head + data_len + C_SAFE_AREA_LEN> blocksize_)
	{
		unsigned first_size = blocksize_ - head;
		unsigned second_size = data_len + C_SAFE_AREA_LEN - first_size; //  include tail sppx


		if (second_size>C_SAFE_AREA_LEN)
		{
			memcpy(buf, block_ + head, first_size);
			memcpy(((char*)buf) + first_size, block_, second_size - C_SAFE_AREA_LEN);
			*head_ = second_size;

		}
		else
		{
			memcpy(buf, block_ + head, data_len);
			*head_ = second_size;
		}


	}
	else
	{
		memcpy(buf, block_ + head, data_len);
		*head_ = head + data_len + C_SAFE_AREA_LEN;
	}

#if 0
	data_len -= C_SAFE_AREA_LEN;

	if (*(unsigned*)(((char*)buf) + data_len) != C_SAFE_AREA_VAL)
	{

		if (do_check(*head_, *tail_))
		{
			// 娓呯┖鍏变韩鍐呭瓨
			clear();
		}

		data_len = 0;

		return -6;
	}
#endif
	//   pstat_->curflow = flow;
	/* 涓嬮潰鍙湪鍑洪槦鏃舵洿鏂帮紝鍙笉鐢ㄥ師瀛?*/
	//flow=  atomic_inc32(&(pstat_->process_count));

	return pstat_->msg_count;
}
/////////////////////////////////////////////////////////////////////////////////////////////
CShmProducer::CShmProducer() : mq_(NULL)
{
}

CShmProducer::~CShmProducer()
{
	fini();

}

int CShmProducer::init(int shmkey, int shmsize)
{
	fini();

	mq_ = new CShmMQ;
	return mq_->init(shmkey, shmsize);
}

void CShmProducer::fini()
{
	if (mq_ != NULL)
	{
		delete mq_;
		mq_ = NULL;
	}

}

int CShmProducer::clear()
{
	int ret = 0;
	ret = mq_->clear();
	if (ret)
	{

	}
	return ret;
}

int CShmProducer::produce(const void* data, unsigned data_len, unsigned flow)
{
	int ret = 0;
	ret = mq_->enqueue(data, data_len, flow);
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////
CShmProducerL::CShmProducerL()
{

}
CShmProducerL::~CShmProducerL()
{
	fini();
}

int CShmProducerL::init(int shmkey, int shmsize)
{
	fini();

	int ret = CShmProducer::init(shmkey, shmsize);
	if (ret != 0)
	{
		return ret;
	}

	return 0;
}

void CShmProducerL::fini()
{
	CShmProducer::fini();

	if (mfd_.is_open())
	{
		mfd_.close();
	}
}

int CShmProducerL::clear()
{
	int ret = 0;
	mutex.lock();
	CShmProducer::clear();
	mutex.unlock();

	return ret;
}

int CShmProducerL::produce(const void* data, unsigned data_len, unsigned flow)
{
	mutex.lock();
	int  ret = CShmProducer::produce(data, data_len, flow);
	mutex.unlock();
	return ret;

}

/////////////////////////////////////////////////////////////////////////////////////////////
CShmComsumer::CShmComsumer() : mq_(NULL)
{
}
CShmComsumer::~CShmComsumer()
{
	fini();
}
int CShmComsumer::init(int shmkey, int shmsize)
{
	fini();

	mq_ = new CShmMQ;
	return mq_->init(shmkey, shmsize);
}
void CShmComsumer::fini()
{
	if (mq_ != NULL)
	{
		delete mq_;
		mq_ = NULL;
	}
}
int CShmComsumer::clear()
{
	int ret = 0;
	ret = mq_->clear();
	return ret;
}
int CShmComsumer::comsume(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
	return mq_->dequeue(buf, buf_size, data_len, flow);
}

/////////////////////////////////////////////////////////////////////////////////////////////
CShmComsumerL::CShmComsumerL()
{

}
CShmComsumerL::~CShmComsumerL()
{
	fini();
}
int CShmComsumerL::init(int shmkey, int shmsize)
{
	fini();

	int ret = CShmComsumer::init(shmkey, shmsize);

	if (ret != 0)
	{
		return ret;
	}

	return 0;
}

void CShmComsumerL::fini()
{
	CShmComsumer::fini();

	if (mfd_.is_open())
	{
		mfd_.close();
	}

}

int CShmComsumerL::clear()
{
	int ret = 0;
	mutex.lock();
	CShmComsumer::clear();
	mutex.unlock();

	return ret;
}

inline int CShmComsumerL::comsume(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
	if (mq_->isEmpty())
	{
		return -122;
	}
	mutex.lock();
	int ret = mq_->dequeue(buf, buf_size, data_len, flow);
	mutex.unlock();

	return ret;

}





CShmCommu::CShmCommu()
{
	mq_ = NULL;
}
CShmCommu::~CShmCommu()
{
	fini();
}
int CShmCommu::Init(int shmkey, int shmsize)
{
	fini();
	m_iSize = shmsize;
	m_iKey = shmkey;
	mq_ = new CShmMQ;
	return mq_->init(shmkey, shmsize);

}
int CShmCommu::Put(const void* data, unsigned data_len, unsigned flow)
{
	int ret = 0;
	ret = mq_->enqueue(data, data_len, flow);
	return ret;

}
int CShmCommu::PutL(const void* data, unsigned data_len, unsigned flow)
{
	int iRet;
	mq_->lock();
	iRet = Put(data, data_len, flow);
	mq_->unlock();
	return iRet;

}
int CShmCommu::Get(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
	return mq_->dequeue(buf, buf_size, data_len, flow);

}
int CShmCommu::GetL(void* buf, unsigned buf_size, unsigned& data_len, unsigned& flow)
{
	int iRet;
	mq_->lock();
	iRet = Get(buf, buf_size, data_len, flow);
	mq_->unlock();
	return iRet;
}
int CShmCommu::clear()
{
	int ret = 0;
	mq_->lock();
	ret = mq_->clear();
	mq_->unlock();
	return ret;
}
void CShmCommu::fini()
{
	if (mq_ != NULL)
	{
		delete mq_;
		mq_ = NULL;
	}

}






CShmQueue::CShmQueue(int iDefSize)
{
	m_iDefSize = iDefSize;
}
CShmQueue:: ~CShmQueue()
{}
int CShmQueue::Put(int id, char *val, int size)
{
	map<int, CShmCommu* >::iterator it = m_vqueue.find(id);
	CShmCommu* pShm;
	if (it == m_vqueue.end())
	{
		pShm = new CShmCommu();
		if (pShm->Init(id, m_iDefSize)<0)
		{
			return -1;
		}
		pShm->clear();
		m_vqueue.insert(pair<int, CShmCommu *>(id, pShm));

	}
	else
	{
		pShm = it->second;
	}
	return pShm->Put(val, size, 0);

}
int CShmQueue::Get(int id, void* buf, unsigned buf_size, unsigned& data_len)
{
	map<int, CShmCommu* >::iterator it = m_vqueue.find(id);
	CShmCommu* pShm;
	if (it == m_vqueue.end())
	{
		pShm = new CShmCommu();
		if (pShm->Init(id, m_iDefSize)<0)
		{
			return -1;
		}
		pShm->clear();
		m_vqueue.insert(pair<int, CShmCommu *>(id, pShm));

	}
	else
	{
		pShm = it->second;
	}
	unsigned int flow;
	return pShm->Get(buf, buf_size, data_len, flow);

}
int CShmQueue::PutL(int id, char *val, int size)
{
	map<int, CShmCommu* >::iterator it = m_vqueue.find(id);
	CShmCommu* pShm;
	if (it == m_vqueue.end())
	{
		pShm = new CShmCommu();
		if (pShm->Init(id, m_iDefSize)<0)
		{
			return -1;
		}
		pShm->clear();
		m_vqueue.insert(pair<int, CShmCommu *>(id, pShm));

	}
	else
	{
		pShm = it->second;
	}
	return pShm->PutL(val, size, 0);
}
int CShmQueue::GetL(int id, void* buf, unsigned buf_size, unsigned& data_len)
{
	map<int, CShmCommu* >::iterator it = m_vqueue.find(id);
	CShmCommu* pShm;
	if (it == m_vqueue.end())
	{
		pShm = new CShmCommu();
		if (pShm->Init(id, m_iDefSize)<0)
		{
			return -1;
		}
		pShm->clear();
		m_vqueue.insert(pair<int, CShmCommu *>(id, pShm));

	}
	else
	{
		pShm = it->second;
	}
	unsigned int flow;
	return pShm->GetL(buf, buf_size, data_len, flow);
}
void CShmQueue::Clear(int id)
{
	map<int, CShmCommu* >::iterator it;
	CShmCommu* pShm;
	if (id == 0)
	{
		for (it = m_vqueue.begin(); it != m_vqueue.end(); it++)
		{
			pShm = it->second;
			pShm->clear();
		}
	}
	else
	{
		it = m_vqueue.find(id);
		if (it != m_vqueue.end())
		{
			pShm = it->second;
			pShm->clear();
		}

	}
}

