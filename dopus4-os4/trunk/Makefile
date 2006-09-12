# Master Dopus makefile

ifeq ($(findstring ver, $(MAKECMDGOALS)),ver)
export REVISE:= ver
endif
ifeq ($(findstring rev, $(MAKECMDGOALS)),rev)
export REVISE:= rev
endif
ifeq ($(findstring sub, $(MAKECMDGOALS)),sub)
export REVISE:= sub
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
