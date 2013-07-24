ARMBOOT = armboot
ARMBOOT-BIN = $(ARMBOOT)/target/armboot.bin
ARMBOOT-DEPS = $(ARMBOOT)/* $(ARMBOOT)/stub/* $(ARMBOOT)/stubsb1/* $(ARMBOOT)/elfloader/* $(ARMBOOT-BIN)
NSWITCH = nswitch/boot.dol
NSWITCH-SRC = nswitch/source
NSWITCH-DEPS = $(NSWITCH-SRC)/* $(NSWITCH-SRC)/armboot.c $(NSWITCH-SRC)/armboot.h

all: boot.dol

boot.dol: $(NSWITCH)
	@cp $(NSWITCH) ./

$(NSWITCH): $(NSWITCH-DEPS)
	@make -C nswitch

$(NSWITCH-SRC)/armboot.c: $(ARMBOOT-DEPS)
	@make -C $(ARMBOOT)

$(NSWITCH-SRC)/armboot.h: $(ARMBOOT-DEPS)
	@make -C $(ARMBOOT)

clean:
	@make -C $(ARMBOOT) clean
	@make -C nswitch clean