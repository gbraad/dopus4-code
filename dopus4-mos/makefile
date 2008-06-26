INSTALL_DEST = Programs:Utilities/DirOpus4

SUBDIRS = library-mos program-mos config-mos dopus_print-mos dopusrt-mos

all:
%:
	@list='$(SUBDIRS)'; target='$@'; \
	for subdir in $$list; do \
	echo Making $$target in $$subdir; \
	make -C $$subdir $$target; \
	done;

install:
	Copy library-mos/dopus.library $(INSTALL_DEST)/libs
	Copy program-mos/DirectoryOpus $(INSTALL_DEST)
	Copy config-mos/ConfigOpus $(INSTALL_DEST)/modules
	Copy dopus_print-mos/DOpus_Print $(INSTALL_DEST)/modules
	Copy dopusrt-mos/DOpusRT $(INSTALL_DEST)/C

