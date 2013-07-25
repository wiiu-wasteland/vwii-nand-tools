ARMBOOT = armboot
ARMBOOT-BIN = $(ARMBOOT)/target/armboot.bin
ARMBOOT-DEPS = $(ARMBOOT)/* $(ARMBOOT)/stub/* $(ARMBOOT)/stubsb1/* $(ARMBOOT)/elfloader/* $(ARMBOOT-BIN)
NSWITCH = nswitch/boot.dol
NSWITCH-SRC = nswitch/source
NSWITCH-DEPS = $(NSWITCH-SRC)/* $(NSWITCH-SRC)/armboot.c $(NSWITCH-SRC)/armboot.h

all: boot.dol

boot.dol: $(NSWITCH)
	@make -C nswitch
	@make -C $(ARMBOOT)
	@cp $(NSWITCH) ./

clean:
	@make -C $(ARMBOOT) clean
	@make -C nswitch clean
	@rm boot.dol