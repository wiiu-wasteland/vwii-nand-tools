include starlet.mk

CFLAGS += -I$(CURDIR)

# enabled usbgecko support
CFLAGS += -DCAN_HAZ_USBGECKO
# disables debug spew over usbgecko
#CFLAGS += -DNDEBUG
# enables LFCR linefeeds for usbgecko output, useful for some terminal apps
#CFLAGS += -DGECKO_LFCR
# uses the 'safe' version of the usbgecko receive and send functions
#CFLAGS += -DGECKO_SAFE

ASFLAGS += -D_LANGUAGE_ASSEMBLY
CFLAGS += -DCAN_HAZ_IRQ
LDSCRIPT = mini.ld
LIBS = -lgcc

ELFLOADER = target/elfloader.bin
MAKEBIN = $(CURDIR)/makebin.py

TARGET = target/armboot-sym.elf
TARGET_STRIPPED = target/armboot.elf
TARGET_BIN = target/armboot.bin

SRCS =	. \
	common \
	storage \
	system \
	video

C_SRC = $(foreach dir,$(SRCS),$(wildcard $(dir)/*.c))
S_SRC = $(foreach dir,$(SRCS),$(wildcard $(dir)/*.S))
OBJS = $(S_SRC:.S=.o) $(C_SRC:.c=.o)

RAW2C = $(DEVKITPRO)/tools/bin/raw2c
NSWITCH = ./../nswitch/source

include common.mk


#all: $(TARGET_BIN)

all:
	@$(MAKE) $(TARGET_BIN)
	@$(RAW2C) $(TARGET_BIN)
	@mv armboot.c $(NSWITCH)/
	@mv armboot.h $(NSWITCH)/

main.o: main.c

$(TARGET_STRIPPED): $(TARGET)
	@echo  "STRIP	$@"
	@$(STRIP) $< -o $@

$(TARGET_BIN): $(TARGET_STRIPPED) $(ELFLOADER)
	@echo  "MAKEBIN	$@"
	@$(MAKEBIN) $(ELFLOADER) $< $@

upload: $(TARGET_BIN)
	@$(WIIDEV)/bin/bootmii -a $<

clean: myclean

$(ELFLOADER):
	@$(MAKE) -C elfloader

myclean:
	-rm -f $(TARGET) $(TARGET_STRIPPED) $(TARGET_BIN)
	@$(MAKE) -C elfloader clean
