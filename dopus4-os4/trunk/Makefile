# Main DirectoryOpus4 makefile

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

# Read serial debug argument from the "Make" command line:
# Enter a component & debug arg or debug arg only to compile all.
ifeq ($(filter debug, $(MAKECMDGOALS)),debug) #Enable serial debug output
  SERDEBUG= debug
  ifeq ($(strip $(filter-out debug, $(MAKECMDGOALS))),)
    $(eval debug : all)
  endif
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

#

MKDIR = mkdir -p
CPYDIR = cp -r
CPY = cp -p
DELETE = rm -r -f
SETBITS := protect
HOST := $(shell uname)

#

all : dopusrt print config library program catalogs

.PHONY : dopusrt
dopusrt:
	@echo " "
	$(MAKE) -C dopusrt-os4 $(SERDEBUG)

.PHONY : print
print:
	@echo " "
	$(MAKE) -C dopus_print-os4 $(SERDEBUG)

.PHONY : config
config:
	@echo " "
	$(MAKE) -C config-os4 $(SERDEBUG)

.PHONY : library
library:
	@echo " "
	$(MAKE) -C library-os4 $(SERDEBUG)

.PHONY : program
program:
	@echo " "
	$(MAKE) -C program-os4 $(SERDEBUG)

.PHONY : catalogs
catalogs:
	@echo " "
	$(MAKE) -C locale

#

cleanall: clean-config clean-rt clean-print clean-lib \
          clean-prog clean-cats clean-release

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

.PHONY : clean-cats
clean-cats:
	$(MAKE) -C locale clean

.PHONY : clean-release
clean-release:
	${DELETE} .release

#

RELEASEDIR = .release/DirectoryOpus4

.PHONY : release
release: all
	@echo " "
	@echo ">>> Creating release directory and copying files."
	@echo ">>> WARNING: Amiga protection bits may not be correct."
	@${DELETE} .release
	@${MKDIR} $(RELEASEDIR)/catalogs
	@${MKDIR} $(RELEASEDIR)/C
	@${MKDIR} $(RELEASEDIR)/Docs
	@${MKDIR} $(RELEASEDIR)/History
	@${MKDIR} $(RELEASEDIR)/Libs
	@${MKDIR} $(RELEASEDIR)/Modules
	@${MKDIR} $(RELEASEDIR)/Rexx
	@${MKDIR} $(RELEASEDIR)/S
	@${CPYDIR} catalogs/czech $(RELEASEDIR)/catalogs/czech
	@${CPYDIR} catalogs/french $(RELEASEDIR)/catalogs/french
	@${CPYDIR} catalogs/german $(RELEASEDIR)/catalogs/german
	@${CPYDIR} catalogs/italian $(RELEASEDIR)/catalogs/italian
	@${CPYDIR} catalogs/polish $(RELEASEDIR)/catalogs/polish
	@${CPYDIR} catalogs/spanish $(RELEASEDIR)/catalogs/spanish
	@${CPYDIR} locale $(RELEASEDIR)/catalogs/_locale
	@${DELETE} $(RELEASEDIR)/catalogs/_locale/makefile
	@${DELETE} $(RELEASEDIR)/catalogs/_locale/dopus4_lib.cd
	@${CPY} catalogs/*.ct $(RELEASEDIR)/catalogs/_locale
	@${CPY} .bin/DirectoryOpus $(RELEASEDIR)
	@${CPY} .bin/DirectoryOpus.debug $(RELEASEDIR)
	@${CPY} .bin/ConfigOpus $(RELEASEDIR)/Modules
	@${CPY} .bin/dopus.library $(RELEASEDIR)/Libs
	@${CPY} .bin/DOpus_Print $(RELEASEDIR)/Modules
	@${CPY} .bin/DOpusRT $(RELEASEDIR)/C
	@${CPY} COPYING $(RELEASEDIR)
	@${CPY} docs/english/Readme.txt $(RELEASEDIR)
	@${CPY} docs/english/DirectoryOpus.guide $(RELEASEDIR)/Docs
	@${CPY} history-os4/*-ChangeLog $(RELEASEDIR)/History
	@${CPY} S/*.* $(RELEASEDIR)/S
	@${CPY} extras/default.dopus $(RELEASEDIR)/Rexx
	@${CPY} extras/DirectoryOpus.CFG $(RELEASEDIR)/S
	@${CPY} extras/DopusWBStart $(RELEASEDIR)/
	@${CPY} extras/icons/ConfigOpus.info $(RELEASEDIR)/Modules
	@${CPY} extras/icons/COPYING.info $(RELEASEDIR)
	@${CPY} extras/icons/DirectoryOpus.guide.info $(RELEASEDIR)/Docs
	@${CPY} extras/icons/DirectoryOpus.info $(RELEASEDIR)
	@${CPY} extras/icons/Docs.info $(RELEASEDIR)/
	@${CPY} extras/icons/Modules.info $(RELEASEDIR)/
	@${CPY} extras/icons/Readme.txt.info $(RELEASEDIR)
	@${CPY} extras/icons/DopusWBStart.info $(RELEASEDIR)/
	@${CPY} extras/icons/DirectoryOpus4.info .release
ifeq ($(HOST), AmigaOS)
	@${SETBITS} $(RELEASEDIR)/DopusWBStart srwd
endif

#

# Dummy target to quietly dispose of revision
# commands if one is entered as a goal for "make"
# on the command line (like: make rev all).

.PHONY : ver rev sub touch debug
debug: sub
ver: sub
rev: sub
touch: sub
sub:
	-@sh -c "echo -n"

