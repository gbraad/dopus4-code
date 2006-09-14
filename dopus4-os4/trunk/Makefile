# Master Dopus makefile

# Read revision arguments from the "Make" command line:
# Acceptable arguments are ver, rev, sub, revision or touch.
ifeq ($(findstring ver, $(MAKECMDGOALS)),ver) #bump version
export REVISE:= ver
endif
ifeq ($(findstring rev, $(MAKECMDGOALS)),rev) #bump revision
export REVISE:= rev
endif
ifeq ($(findstring sub, $(MAKECMDGOALS)),sub) #bump subrevision
export REVISE:= sub
endif
ifeq ($(filter touch, $(MAKECMDGOALS)),touch) #Update compile date
export REVISE:= touch
endif

# Read default revision argument from environmental variable
# DOPUSREVDEFAULT if no arguments entered on "Make" command line.
# Possible settings are ver, rev, sub or touch.
ifndef REVISE
ifdef DOPUSREVDEFAULT

ifeq ($(filter ver, $(DOPUSREVDEFAULT)),ver) #bump version
export REVISE:= ver
endif
ifeq ($(filter rev, $(DOPUSREVDEFAULT)),rev) #bump revision
export REVISE:= rev
endif
ifeq ($(filter sub, $(DOPUSREVDEFAULT)),sub) #bump subrevision
export REVISE:= sub
endif
ifeq ($(filter touch, $(DOPUSREVDEFAULT)),touch) #Update compile date
export REVISE:= touch
endif

endif
endif


all : config dopusrt print library program

.PHONY : config
config:
	$(MAKE) -C config-os4

.PHONY : dopusrt
dopusrt:
	$(MAKE) -C dopusrt-os4

.PHONY : print
print:
	$(MAKE) -C dopus_print-os4

.PHONY : library
library:
	$(MAKE) -C library-os4

.PHONY : program
program:
	$(MAKE) -C program-os4


cleanall: clean-config clean-rt clean-print clean-lib clean-prog

.PHONY : clean-config
clean-config:
	$(MAKE) -C config-os4 clean

.PHONY : clean-rt
clean-rt:
	$(MAKE) -C dopusrt-os4 clean

.PHONY : clean-print
clean-print:
	$(MAKE) -C dopus_print-os4 clean

.PHONY : clean-lib
clean-lib:
	$(MAKE) -C library-os4 clean

.PHONY : clean-prog
clean-prog:
	$(MAKE) -C program-os4 clean

# Dummy target to quietly dispose of revision
# commands if one is entered as a goal for "make"
# on the command line (like: make rev all).

.PHONY : ver rev sub
ver: sub
rev: sub
sub: 
	-@echo -n
