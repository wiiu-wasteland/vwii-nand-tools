/*
 * vWii SLC nand tools 
 * Copyright (C) 2020 rw-r-r-0644
 * 
 * License: GNU GPLv2
 * 
 * Based on:
 * nswitch - Simple neek/realnand switcher to embed in a channel
 * Copyright (C) 2011 OverjoY
 */
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include "runtimeiospatch.h"
#include "armboot.h"

typedef struct armboot_config armboot_config;
struct armboot_config
{	char str;
	u8 debug_config;
	u16 debug_magic;
};

armboot_config *armconfig = (armboot_config*)0x81200000;

void VideoInitDefault();
void LoadMINI(const void *ptr, size_t size);

int main(int argc, char **argv)
{
	if (IosPatch_RUNTIME(true, false, false, false) < 0) {
		return 0;
	}
	VideoInitDefault();
	ISFS_Initialize();

	printf("\n\n//////////////////////////\n");
#if IS_NAND_WRITER == 1
	printf("// SLC-NAND-WRITER v0.5 //\n");
#else
	printf("// SLC-NAND-READER v0.5 //\n");
#endif
	printf("//////////////////////////\n\n");

	// setup redirected gecko config
	armconfig->str = '\0';
	armconfig->debug_config = 3;
	armconfig->debug_magic = 0xDEB6;
	DCFlushRange(armconfig, 32);

	// start mini (armboot)
	LoadMINI(armboot, armboot_size);
	printf("Loaded startlet code; waiting for output ...\n");

	// wait for gecko_init
	do DCInvalidateRange(armconfig, 32);
	while (armconfig->debug_config);

	// output text from mini
	char* miniDebug = (char*)armconfig;
	char received[] = {'\0', '\0'};
	while(true)
	{
		// wait for a text character
		do DCInvalidateRange(miniDebug, 32);
		while (!*miniDebug);

		received[0] = *miniDebug;

		// notify the character was received
		*miniDebug = '\0';
		DCFlushRange(miniDebug, 32);

		// output the character
		printf(received);
	}

	return 0;
}

// initialize default video mode
void VideoInitDefault()
{
	static void *xfb = NULL;
	GXRModeObj *rmode;
	
	VIDEO_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	
	if (xfb)
		free(MEM_K1_TO_K0(xfb));
	
	xfb = SYS_AllocateFramebuffer(rmode);
	VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);
	xfb = MEM_K0_TO_K1(xfb);
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();
}

// boot mini without BootMii IOS code by Crediar
void LoadMINI(const void *ptr, size_t size)
{
	const unsigned char ES_ImportBoot2[16] = {0x68, 0x4B, 0x2B, 0x06, 0xD1, 0x0C, 0x68, 0x8B, 0x2B, 0x00, 0xD1, 0x09, 0x68, 0xC8, 0x68, 0x42};
	
	for (u32 i = 0x939F0000; i < 0x939FE000; i += 2)
	{
		if (memcmp((void*)(i), ES_ImportBoot2, sizeof(ES_ImportBoot2)) == 0)
		{
			DCInvalidateRange( (void*)i, 0x20 );
			
			*(vu32*)(i+0x00)	= 0x48034904;	// LDR R0, 0x10, LDR R1, 0x14
			*(vu32*)(i+0x04)	= 0x477846C0;	// BX PC, NOP
			*(vu32*)(i+0x08)	= 0xE6000870;	// SYSCALL
			*(vu32*)(i+0x0C)	= 0xE12FFF1E;	// BLR
			*(vu32*)(i+0x10)	= 0x10100000;	// offset
			*(vu32*)(i+0x14)	= 0x0000FF01;	// version
			
			DCFlushRange( (void*)i, 0x20 );
			
			__IOS_ShutdownSubsystems();
			
			void *mini = (void*)0x90100000;
			memcpy(mini, ptr, size);
			DCFlushRange( mini, size );
			
			s32 fd = IOS_Open( "/dev/es", 0 );
			
			u8 *buffer = (u8*)memalign( 32, 0x100 );
			memset( buffer, 0, 0x100 );
			
			IOS_IoctlvAsync( fd, 0x1F, 0, 0, (ioctlv*)buffer, NULL, NULL );
		}
	}
}
