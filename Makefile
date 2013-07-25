ARMBOOT = armboot
NSWITCH = nswitch/boot.dol

all: boot.dol

boot.dol:
	@make -C $(ARMBOOT)
	@make -C nswitch
	@cp $(NSWITCH) ./

clean:
	@make -C $(ARMBOOT) clean
	@make -C nswitch clean
	@rm boot.dol