/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "common/types.h"
#include "common/utils.h"
#include "common/string.h"
#include "storage/nand.h"
#include "video/gecko.h"

#include "storage/ff.h"

int _dump_slc_raw(u32 bank)
{
    #define PAGES_PER_ITERATION (0x10)
    #define TOTAL_ITERATIONS (NAND_MAX_PAGE / PAGES_PER_ITERATION)

	static u8 page_buf[PAGE_SIZE] ALIGNED(64) MEM2_BSS;
	static u8 ecc_buf[ECC_BUFFER_ALLOC] ALIGNED(128) MEM2_BSS;

	static u8 file_buf[PAGES_PER_ITERATION][PAGE_SIZE + PAGE_SPARE_SIZE] MEM2_BSS;

    const char* name = NULL;
    switch(bank) {
        case NAND_BANK_SLC: name = "SLC"; break;
        case NAND_BANK_SLCCMPT: name = "SLCCMPT"; break;
        default: return -2;
    }

    char path[64] = {0};
    sprintf(path, "%s.RAW", name);

    FIL file = {0}; FRESULT fres = 0; UINT btx = 0;
    fres = f_open(&file, path, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
    if(fres != FR_OK) {
        gecko_printf("Failed to open %s (%d).\n", path, fres);
        return -3;
    }

    gecko_printf("Initializing %s...\n", name);
    nand_initialize(bank);

    for(u32 i = 0; i < TOTAL_ITERATIONS; i++)
    {
        u32 page_base = i * PAGES_PER_ITERATION;
        for(u32 page = 0; page < PAGES_PER_ITERATION; page++)
        {
            nand_read_page(page_base + page, page_buf, ecc_buf);
            nand_wait();
            nand_correct(page_base + page, page_buf, ecc_buf);

            memcpy(file_buf[page], page_buf, PAGE_SIZE);
            memcpy(file_buf[page] + PAGE_SIZE, ecc_buf, PAGE_SPARE_SIZE);
        }

        fres = f_write(&file, file_buf, sizeof(file_buf), &btx);
        if(fres != FR_OK || btx != sizeof(file_buf)) {
            f_close(&file);
            gecko_printf("Failed to write %s (%d).\n", path, fres);
            return -4;
        }

        if((i % 0x100) == 0) {
            gecko_printf("%s-RAW: Page 0x%05lX completed\n", name, page_base);
        }
    }

    fres = f_close(&file);
    if(fres != FR_OK) {
        gecko_printf("Failed to close %s (%d).\n", path, fres);
        return -5;
    }
    
    gecko_printf("SLC dump completed successfully!\n");

    return 0;

    #undef PAGES_PER_ITERATION
    #undef TOTAL_ITERATIONS
}

