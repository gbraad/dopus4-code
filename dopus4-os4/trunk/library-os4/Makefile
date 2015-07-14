# Makefile for dopus.library

NAME := dopus.library
DESTDIR  := ../.bin
TARGET   := $(DESTDIR)/$(NAME)

CC     = ppc-amigaos-gcc 
STRIP  = ppc-amigaos-strip
TOUCH = touch
MAKEDIR = mkdir -p
REMOVE = rm -r -f

# Change these as required
OPTIMIZE = -O3
DEBUG = -gstabs
#CFLAGS = -maltivec -mcrt=newlib -Wall -Wwrite-strings -I../include-os4 -I. $(OPTIMIZE) $(DEBUG)
CFLAGS = -mcrt=newlib -Wall -Wwrite-strings -I../include-os4 -I. $(OPTIMIZE) $(DEBUG)

# Flags passed to gcc during linking
#LINK = -maltivec -mcrt=newlib
LINK = -mcrt=newlib

ifdef REVISE
BUMPREV:= revision
endif

# -------------------------------------------------------------

# "main" Interface Source Files
main_SRCS = main/Obtain.c main/Release.c main/FileRequest.c main/Do3DBox.c \
    main/Do3DStringBox.c main/Do3DCycleBox.c main/DoArrow.c main/LStrCat.c \
    main/LStrnCat.c main/LStrCpy.c main/LStrnCpy.c main/LStrCmp.c \
    main/LStrnCmp.c main/LStrCmpI.c main/LStrnCmpI.c main/StrCombine.c \
    main/StrConcat.c main/TackOn.c main/StampToStr.c \
    main/StrToStamp.c main/AddListView.c main/ListViewIDCMP.c \
    main/RefreshListView.c main/RemoveListView.c main/DrawCheckMark.c \
    main/FixSliderBody.c main/FixSliderPot.c main/GetSliderPos.c \
    main/LAllocRemember.c main/LFreeRemember.c main/SetBusyPointer.c \
    main/GetWBScreen.c main/SearchPathList.c main/CheckExist.c \
    main/StrToUpper.c main/StrToLower.c main/RawkeyToStr.c \
    main/DoRMBGadget.c main/AddGadgets.c main/ActivateStrGad.c \
    main/RefreshStrGad.c main/CheckNumGad.c main/CheckHexGad.c \
    main/HiliteGad.c main/DoSimpleRequest.c main/ReadConfig.c main/SaveConfig.c \
    main/DefaultConfig.c main/GetDevices.c main/AssignGadget.c \
    main/AssignMenu.c main/FindSystemFile.c main/Do3DFrame.c main/FreeConfig.c \
    main/DoCycleGadget.c main/UScoreText.c main/DisableGadget.c \
    main/EnableGadget.c main/GhostGadget.c main/DrawRadioButton.c \
    main/GetButtonImage.c main/ShowSlider.c main/CheckConfig.c \
    main/GetCheckImage.c main/OpenDORequester.c main/CloseRequester.c \
    main/AddRequesterObject.c main/RefreshRequesterObject.c main/ObjectText.c \
    main/DoGlassImage.c main/ReadStringFile.c \
    main/FreeStringFile.c main/LFreeRemEntry.c main/AddGadgetBorders.c \
    main/CreateGadgetBorders.c main/SelectGadget.c main/FSSetMenuStrip.c \
    main/extras.c main/data.c

# library Initialization Source File
SRCS = init.c

OBJS = $(main_SRCS:.c=.o)  $(SRCS:.c=.o)

# Rules for building

all: $(BUMPREV) $(TARGET)

.PHONY: debug setdebug
debug: setdebug all
setdebug:
	$(eval override CFLAGS += -DDEBUG)

%.o : %.c
	@echo ">>>>>Compiling $<"
	@$(CC) -c $< $(CFLAGS) -o $@

$(TARGET): $(OBJS)
	@$(MAKEDIR) $(DESTDIR)
	$(CC) $(LINK) -nostartfiles -o $(TARGET).debug $(OBJS)
	$(STRIP) -s $(TARGET).debug -o $(TARGET)

init.o: init.c dopus.library_rev.h

.PHONY: clean
clean:
	@$(REMOVE) $(TARGET) $(TARGET).debug $(OBJS)

.PHONY: revision
revision:
	revbump $(NAME) $(REVISE)

