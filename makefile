#############################################################################
# Makefile for building: Operating Systems: Project3                        #
#############################################################################


#uses makefiles in subdirectories: myport, port-master, vessel.

.PHONY: allmake
allmake:
	$(MAKE) -C myport
	$(MAKE) -C port-master
	$(MAKE) -C vessel

.PHONY: clean
clean:
	$(MAKE) -C myport clean
	$(MAKE) -C port-master clean
	$(MAKE) -C vessel clean
