#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__ 
#include <includes.h>
#include "arch/cc.h"
#include "includes.h"
 
#ifdef SYS_ARCH_GLOBALS
#define SYS_ARCH_EXT
#else
#define SYS_ARCH_EXT extern
#endif
 
#define MAX_QUEUES        		OS_CFG_MSG_POOL_SIZE	// 消息邮箱的数量
#define MAX_QUEUE_ENTRIES 		20	// 每个消息邮箱的大小


typedef OS_SEM		sys_sem_t;
typedef OS_Q			sys_mbox_t;
typedef OS_MUTEX	sys_mutex_t;
typedef CPU_INT08U	sys_thread_t;

#endif 
