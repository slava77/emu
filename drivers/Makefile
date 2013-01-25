DAQSUBDIRS= e1000hook/eth_hook_2_nobigphysxxx e1000hook/eth_hook_3_nobigphysxxx e1000hook/eth_hook_4_nobigphysxxx e1000hook/eth_hook_5_nobigphysxxx e1000hook/e1000-7.0.39/src
DAQSUBDIRS904= dl2khook/eth_hook_2_nobigphysxxx dl2khook/dl2k_driver
PCVMEDIRS= gbit-vme/eth_hook_jtag_simple gbit-vme/dl2k_driver
PCVMESLC5DIRS= gbit-vme-slc5/eth_hook_jtag_simple gbit-vme-slc5/dl2k_driver
DAQPCVMESUBDIRS= dlk2-daq-pcvme/dl2k_driver dlk2-daq-pcvme/eth_hook_3_nobigphysxxx dlk2-daq-pcvme/eth_hook_jtag_simple
IGBEMUSUBDIRS= igb_emu/eth_hook_2_daq igb_emu/eth_hook_2_vme igb_emu/eth_hook_3_daq igb_emu/eth_hook_4_daq igb_emu/eth_hook_5_daq igb_emu/igb-4.1.2/src
SUBDIRS=$(DAQSUBDIRS) $(DAQSUBDIRS904) $(PCVMEDIRS) $(DAQPCVMESUBDIRS) $(IGBEMUSUBDIRS)

all:
	@echo "*** Error: No target specified. ***"
	@echo "Use any of the following targets:"
	@echo "   clean"
	@echo "   daqdriver"
	@echo "   daqdriverrpm"
	@echo "   daqdriver904"
	@echo "   daqdriverrpm904"
	@echo "   pcvme"
	@echo "   pcvmerpm"
	@echo "   pcvmeslc5"
	@echo "   pcvmeslc5rpm"
	@echo "   daqpcvme"
	@echo "   daqpcvmerpm"
	@echo "   igbemu"
	@echo "   igbemurpm"
	@echo "For example, to compile the local DAQ drivers and pack them in an RPM:"
	@echo "   make daqdriverrpm"
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

daqdriverrpm:	daqdriver
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

daqdriverrpm904:	daqdriver904
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-daqdriver904.spec



###################################
# Perihperal Controller Gbit driver
###################################
pcvme:
	@for i in $(PCVMEDIRS); do \
        echo "make in $$i..."; \
	(cd $$i; make); \
	done

pcvmerpm:	pcvme
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-pcvme.spec


############################################
# Perihperal Controller Gbit driver for SLC5
############################################
pcvmeslc5:
	@for i in $(PCVMESLC5DIRS); do \
        echo "make in $$i..."; \
	(cd $$i; make); \
	done

pcvmeslc5rpm:	pcvmeslc5
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-pcvme-slc5.spec


####################################
# Local DAQ and PC VME DLink drivers
####################################
daqpcvme:
	@for i in $(DAQPCVMESUBDIRS); do \
        echo "make in $$i..."; \
	(cd $$i; make); \
	done

daqpcvmerpm:	daqpcvme
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-daqpcvme.spec

#################################################################
# Local DAQ and PC VME driver for the Intel dual port I350-F2 NIC
#################################################################
igbemu:
	@for i in $(IGBEMUSUBDIRS); do \
        echo "make in $$i..."; \
	(cd $$i; make); \
	done

igbemurpm:	igbemu
	mkdir -p rpm/{BUILD,RPMS}
	rpmbuild -bb -vv emu-igb_emu.spec

