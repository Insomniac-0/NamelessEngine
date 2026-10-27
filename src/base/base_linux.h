#ifndef BASE_LINUX_H
#define BASE_LINUX_H


#include <dirent.h>
#include <errno.h>
#include <execinfo.h>
#include <features.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <linux/futex.h>
#include <poll.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <ucontext.h>




pid_t gettid(void);
int pthread_setname_np(pthread_t thread, const char *name);
int pthread_getname_np(pthread_t thread, char *name, size_t size);

typedef struct tm tm;
typedef struct timespec timespec;

// Linux Call Interuption Retry Helper
#define LINUX_RETRY_ON_EINTR(expr)						\
	(__extension__({														\
	__typeof__(expr) __ret;											\
	do {																				\
	__ret = (expr);															\
	} while ((__ret == -1) && errno == EINTR);	\
	__ret;																			\
		}))
	


typedef struct LNX_FileIter LNX_FileIter;
struct LNX_FileIter
{
	DIR *dir;
	struct dirent *dp;
	String8 path;
};
StaticAssert(sizeof(Member(FileIter, memory)) >= sizeof(LNX_FileIter), linux_file_iter_size_check);

typedef struct LinuxSafeCallChain LinuxSafeCallChain;
struct LinuxSafeCallChain
{
	LinuxSafeCallChain *next;
	ThreadEntryPointFunctionType *fail_handler;
	void *ptr;
};


typedef enum LinuxEntityKind
{
	LinuxEntityKind_Thread,
	LinuxEntityKind_Mutex,
	LinuxEntityKind_RWMutex,
	LinuxEntityKind_ConditionVariable,
	LinuxEntityKind_Barrier,

	LinuxEntityKind_COUNT,
} LinuxEntityKind;

typedef struct LinuxEntity LinuxEntity;

struct LinuxEntity
{
	LinuxEntity* next;
	LinuxEntityKind kind;
	union
	{
		struct
		{
			pthread_t handle;
			ThreadEntryPointFunctionType* func;
			void* ptr;
		} thread;
		pthread_mutex_t mutex_handle;
		pthread_rwlock_t rwmutex_handle;
		struct
		{
			pthread_cond_t cond_handle;
			pthread_mutex_t rwlock_mutex_handle;
		} cv;
		pthread_barrier_t barrier;
	};
};

typedef struct LinuxState LinuxState;
struct LinuxState
{
	Arena* arena;
	SystemInfo system_info;
	ProcessInfo process_info;
	pthread_mutex_t entity_mutex;
	Arena* entity_arena;
	LinuxEntity* entity_free;
	U64 default_env_count;
	char** default_env;
};

// Globals
global LinuxState linux_state = {0};
thread_static LinuxSafeCallChain *linux_safe_call_chain = 0;
thread_static B32 linux_in_memory_fault_callback;

// Helpers
internal DateTime lnx_date_time_from_tm(tm in, U32 msec);
internal tm lnx_tm_from_date_time(DateTime dt);
internal timespec lnx_timespec_from_date_time(DateTime dt);
internal DenseTime lnx_dense_time_from_timespec(timespec in);
internal FileProperties lnx_file_properites_from_stat(struct stat *st);

// Entities
internal LinuxEntity* linux_entity_alloc(LinuxEntityKind kind);
internal void linux_entity_release(LinuxEntity* entity);

// Thread Entry
internal void* linux_thread_entry_point(void *ptr);

#endif //BASE_LINUX_H
