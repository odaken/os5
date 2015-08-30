#ifndef _SCHED_H_
#define _SCHED_H_

#include <cpu.h>

#define TASK_NUM	2

struct task {
	struct tss tss;
	void (*context_switch)(void);
};

extern struct task task[TASK_NUM];

void schedule(void);

#endif /* _SCHED_H_ */
