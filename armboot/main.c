/*
	mini - a Free Software replacement for the Nintendo/BroadOn IOS.

Copyright (C) 2008, 2009	Haxx Enterprises <bushing@gmail.com>
Copyright (C) 2008, 2009	Sven Peter <svenpeter@gmail.com>
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcansoft.com>
Copyright (C) 2009			Andre Heider "dhewg" <dhewg@wiibrew.org>
Copyright (C) 2009		John Kelley <wiidev@kelley.ca>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/

#include "common/types.h"
#include "common/utils.h"
#include "start.h"
#include "system/hollywood.h"
#include "storage/sdhc.h"
#include "common/string.h"
#include "system/memory.h"
#include "video/gecko.h"
#include "storage/ff.h"
#include "system/panic.h"
#include "system/irq.h"
#include "system/exception.h"
#include "system/crypto.h"
#include "storage/nand.h"
#include "dump.h"

#define LOG_FILE "/log.txt"
#define PPC_BOOT_FILE "/bootmii/ppcboot.elf"

FATFS fatfs;

u32 _main(void *base)
{
	FRESULT fres;
	(void)base;
    
	gecko_init();

	write32(HW_RESETS, 0x031018ff);
	//write32(0x0D8005E0, 0x7);
	udelay(100000);
	write32(HW_RESETS, 0xFFFFFFFF);
	//write32(0x0D8005E0, 0xFFFFFFFF);

	gecko_printf("Initializing exceptions...\n");
	exception_initialize();
	gecko_printf("Configuring caches and MMU...\n");
	mem_initialize();

	gecko_printf("IOSflags: %08x %08x %08x\n", read32(0xffffff00), read32(0xffffff04), read32(0xffffff08));
	gecko_printf("          %08x %08x %08x\n", read32(0xffffff0c), read32(0xffffff10), read32(0xffffff14));

	irq_initialize();
	irq_enable(IRQ_GPIO1);
	irq_enable(IRQ_RESET);
	irq_enable(IRQ_TIMER);
	irq_set_alarm(20, 1);
	gecko_printf("Interrupts initialized\n");

	crypto_initialize();
	gecko_printf("crypto support initialized\n");

	gecko_printf("Initializing SDHC...\n");
	sdhc_init();

	gecko_printf("Mounting SD...\n");
	fres = f_mount(0, &fatfs);

	if (read32(0x0d800190) & 2) {
		goto shutdown;
	}
	
	if(fres != FR_OK) {
		gecko_printf("Error %d while trying to mount SD\n", fres);
		panic2(0, PANIC_MOUNT);
	}

	/*int i = 0;
	while(i < 300) {
		gecko_printf("Test %d\n", i++);
		udelay(1000000);
	}*/

	// dump slc nand
	nand_dump_slc_raw(NAND_BANK_SLC);

shutdown:
	gecko_printf("Shutting down SDHC...\n");
	sdhc_exit();
 	gecko_printf("Shutting down interrupts...\n");
	irq_shutdown();
	gecko_printf("Shutting down caches and MMU...\n");
	mem_shutdown();
	
	systemReset();
	return 0;
}
