#ifndef _SHELL_H_
#define _SHELL_H_

#define SHELL_GDT_IDX	3
#define SHELL_ID	0

void shell_context_switch(void);
void shell_init(void);
void shell_start(void);

#endif /* _SHELL_H_ */
