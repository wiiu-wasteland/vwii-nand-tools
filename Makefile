ARMBOOT = armboot
NSWITCH = nswitch/boot.dol

all: boot.dol

boot.dol:
	@make -C nswitch
	@make -C $(ARMBOOT)
	@cp $(NSWITCH) ./

clean:
	@make -C $(ARMBOOT) clean
	@make -C nswitch clean
	@rm boot.dol