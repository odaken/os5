#include <cpu.h>
#include <intr.h>
#include <excp.h>
#include <console_io.h>
#include <timer.h>
#include <shell.h>
#include <io_port.h>
#include <uptime.h>

struct tss shell_tss;

int main(void)
{
	unsigned char mask;
	unsigned int limit, base;
	unsigned char i;
	extern unsigned char timer_handler;

	/* Setup console */
	cli();
	cursor_pos.y += 2;
	update_cursor();
	cli();

	/* Setup GDT for shell_tss */
	limit = sizeof(shell_tss);
	gdt[3].limit0 = limit & 0x0000ffff;
	gdt[3].limit1 = (limit & 0x000f0000) >> 16;

	base = (unsigned int)&shell_tss;
	gdt[3].base0 = base & 0x0000ffff;
	gdt[3].base1 = (base & 0x00ff0000) >> 16;
	gdt[3].base2 = (base & 0xff000000) >> 24;

	gdt[3].type = 9;
	gdt[3].p = 1;

	/* Setup exception handler */
	for (i = 0; i < EXCEPTION_NUM; i++)
		intr_set_handler(i, (unsigned int)&exception_handler);

	/* Setup Task Register */
	load_task_register();
	put_str("task loaded.\r\n");

	/* Setup GDT for uptime_tss */
	limit = sizeof(uptime_tss);
	gdt[4].limit0 = limit & 0x0000ffff;
	gdt[4].limit1 = (limit & 0x000f0000) >> 16;

	base = (unsigned int)&uptime_tss;
	gdt[4].base0 = base & 0x0000ffff;
	gdt[4].base1 = (base & 0x00ff0000) >> 16;
	gdt[4].base2 = (base & 0xff000000) >> 24;

	gdt[4].type = 9;
	gdt[4].p = 1;

	/* Setup uptime_tss */
	uptime_tss.eip = (unsigned int)uptime;
	uptime_tss.esp = 0x00085000;
	uptime_tss.eflags = 0x00000200;
	uptime_tss.es = 0x0010;
	uptime_tss.cs = 0x0008;
	uptime_tss.ss = 0x0010;
	uptime_tss.ds = 0x0010;
	uptime_tss.fs = 0x0010;
	uptime_tss.gs = 0x0010;

	/* Setup interrupt handler and mask register */
	con_init();
	intr_set_handler(32, (unsigned int)&timer_handler);
	intr_set_handler(INTR_NUM_KB, (unsigned int)&keyboard_handler);
	intr_init();
	mask = intr_get_mask_master();
	mask &= ~(0x01 | INTR_MASK_BIT_KB);
	intr_set_mask_master(mask);
	sti();

	/* Setup PIT */
	outb_p(0x34, 0x0043);
	/* 割り込み周期11932(0x2e9c)サイクル(=100Hz、10ms毎)に設定 */
	outb_p(0x9c, 0x0040);
	outb_p(0x2e, 0x0040);
	put_str("Timer initialized.\r\n");

	/* Start main task */
	start_shell();

	return 0;
}
