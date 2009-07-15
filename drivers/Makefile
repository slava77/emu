DAQSUBDIRS= e1000hook/eth_hook_2_nobigphysxxx e1000hook/eth_hook_3_nobigphysxxx e1000hook/eth_hook_4_nobigphysxxx e1000hook/eth_hook_5_nobigphysxxx e1000hook/e1000-7.0.39/src
SUBDIRS=$(DAQSUBDIRS)

all:
	@echo "No target specified."
	@echo "Use one of the followint targets:  clean daqdrivers daqdriverrpm"
	@echo "Exiting."
	exit 1

clean:
	@for i in $(SUBDIRS); do \
        echo "make clean in $$i..."; \
	(cd $$i; make clean); \
	done

daqdrivers:
	@for i in $(DAQSUBDIRS); do \
        echo "make all in $$i..."; \
	(cd $$i; make); \
	done

daqdriverrpm:
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-daqdrivers.spec
