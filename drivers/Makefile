DAQSUBDIRS= e1000hook/eth_hook_2_nobigphysxxx e1000hook/eth_hook_3_nobigphysxxx e1000hook/eth_hook_4_nobigphysxxx e1000hook/eth_hook_5_nobigphysxxx e1000hook/e1000-7.0.39/src
DAQSUBDIRS904= dl2khook/eth_hook_2_nobigphysxxx dl2khook/dl2k_driver
SUBDIRS=$(DAQSUBDIRS) $(DAQSUBDIRS904)

all:
	@echo "*** Error: No target specified. ***"
	@echo "Use any of the following targets:"
	@echo "   clean"
	@echo "   daqdriver"
	@echo "   daqdriverrpm"
	@echo "   daqdriver904"
	@echo "   daqdriverrpm904"
	@echo "For example, to compile the local DAQ drivers and pack them in an RPM:"
	@echo "   make daqdriver daqdriverrpm"
	@echo "Exiting."
	@exit 1

###########
# Clean all
###########
clean:
	@for i in $(SUBDIRS); do \
        echo "make clean in $$i..."; \
	(cd $$i; make clean); \
	done


#################
# Local DAQ at P5
#################
daqdriver:
	@for i in $(DAQSUBDIRS); do \
        echo "make in $$i..."; \
	(cd $$i; make); \
	done

daqdriverrpm:
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-daqdriver.spec



###########################
# Local DAQ in building 904
###########################
daqdriver904:
	@for i in $(DAQSUBDIRS904); do \
        echo "make in $$i..."; \
	(cd $$i; make); \
	done

daqdriverrpm904:
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-daqdriver904.spec
