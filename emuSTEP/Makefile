
SUBDIRS = \
emuSTEPConfigRead.t \
emuSTEPApplication.t \
emuSTEPManager.t \
emuSTEPTests.t

.PHONY: all $(MAKECMDGOALS)
all $(MAKECMDGOALS) : $(SUBDIRS)

%.t:
	make $(MAKECMDGOALS) -C $* 

clean:
	rm -f *~

show:
        @echo 'SUBDIRS = $(SUBDIRS)'
