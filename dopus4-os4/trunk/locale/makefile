#
# DirectoryOpus GCC locale/catalog makefile
#

FLEXCAT = flexcat
MKDIR = mkdir -p
DELETE = rm -r -f
RENAME = mv

CATDIR = ../catalogs/
CATNAMES = DOpus4 DOpus4_Config DOpus4_Print
CATALOGS = german \
italian \
polish \
spanish \
czech \
french

#

.PHONY: all
all: $(patsubst %,${CATDIR}%-blank.ct,${CATNAMES}) \
     $(patsubst %,${CATDIR}%/$(word 1, ${CATNAMES}).catalog, ${CATALOGS}) \
     $(patsubst %,${CATDIR}%/$(word 2, ${CATNAMES}).catalog, ${CATALOGS}) \
     $(patsubst %,${CATDIR}%/$(word 3, ${CATNAMES}).catalog, ${CATALOGS})

#

${CATDIR}%-blank.ct: %.cd
	@echo "Creating blank translation file: $*-blank.ct"
	@${MKDIR} ${CATDIR}
	@$(FLEXCAT) $< NEWCTFILE $@

${CATDIR}%.catalog: %.ct $(subst cd,catalog,$(notdir $@))
	@echo "Creating locale catalog: $*.catalog"
	@${MKDIR} ${CATDIR}$(dir $*)
	@$(FLEXCAT) $(basename $(notdir $@)).cd $< CATALOG $@

#

.PHONY: updatect
updatect: \
$(patsubst %,%/$(word 1, ${CATNAMES}).new, ${CATALOGS}) \
$(patsubst %,%/$(word 2, ${CATNAMES}).new, ${CATALOGS}) \
$(patsubst %,%/$(word 3, ${CATNAMES}).new, ${CATALOGS})

%.new: %.ct  $(subst cd,ct,$(notdir $@))
	@echo " "
	@echo ">>> Updating: $<"
	-@${FLEXCAT}  $(basename $(notdir $@)).cd $< NEWCTFILE $@
	-@sh -c "if test -a $@; then \
	${DELETE} $<-bak; \
	${RENAME} $< $<-bak; \
	${RENAME} $@ $<; \
	else echo '!!! ERROR in translation file: $<'; fi"

#

.PHONY: clean
clean:
	${DELETE} ${CATDIR}

