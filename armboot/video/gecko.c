/*
	mini - a Free Software replacement for the Nintendo/BroadOn IOS.
	USBGecko support code

Copyright (c) 2008		Nuke - <wiinuke@gmail.com>
Copyright (C) 2008, 2009	Hector Martin "marcan" <marcan@marcansoft.com>
Copyright (C) 2008, 2009	Sven Peter <svenpeter@gmail.com>
Copyright (C) 2009		Andre Heider "dhewg" <dhewg@wiibrew.org>

# This code is licensed to you under the terms of the GNU GPL, version 2;
# see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
*/
#include "common/vsprintf.h"
#include "common/string.h"
#include "common/types.h"
#include "common/utils.h"
#include "system/memory.h"
#include "gecko.h"

static u8 gecko_enabled = 0;

void gecko_init(void)
{
	if(read16(0x01200002) == 0XDEB6)
	{
		gecko_enabled = read8(0x01200001);
		write8(0x01200001, 0);
		dc_flushrange((void*)0x01200000, 32);
	}
}

int gecko_printf(const char *fmt, ...)
{
	va_list args;
	char buffer[256];
	u32 timeout;

	if(!gecko_enabled)
		return 0;

	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer)-1, fmt, args);
	va_end(args);

	fmt = buffer;
	while(*fmt)
	{
		timeout = read32(HW_TIMER) + 38000;

		do dc_invalidaterange((void*)0x01200000, 32);
		while(read8(0x01200000) && (read32(HW_TIMER) < timeout));

		write8(0x01200000, *(fmt++));
		dc_flushrange((void*)0x01200000, 32);
	}

	return 0;
}

