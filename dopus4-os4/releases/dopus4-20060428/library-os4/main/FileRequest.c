/*
 *  $VER: FileRequest.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 *  (C) Copyright 2006 Hyperion Entertainment
 *      All rights reserved
 *
 * $Id$
 *
 * $Log$
 *
 *
 */

#include <string.h>

#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <proto/asl.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <graphics/gfxbase.h>
#include <libraries/dopus.h>
#include <libraries/asl.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <dopus/filereq.h>
#include <stdarg.h>

#include "extras.h"
//#include "FileRequest.h"

/****** dopus/main/FileRequest ******************************************
*
*   NAME
*      FileRequest -- Description
*
*   SYNOPSIS
*      int FileRequest(struct DOpusFileReq * freq);
*
*   FUNCTION
*
*   INPUTS
*       freq - 
*
*   RESULT
*       The result ...
*
*   EXAMPLE
*
*   NOTES
*
*   BUGS
*
*   SEE ALSO
*
*****************************************************************************
*
*/
struct DOpusIFace *IDOpus;

int gettingdirmsg(struct FileReqData *freqdata, ULONG class, USHORT code, USHORT qual, USHORT gadgetid);

const static struct PropInfo defaultprop =
{
	PROPBORDERLESS | FREEVERT,
	0,
	0,
	0,
	0xffff
};

const static struct Image defaultimage =
{
	0,
	0,
	8,
	22,
	1,
	NULL,
	0,
	1,
	NULL
};

struct IntuiText defaulttext[6] =
{
	{ 1, 0, JAM1, 0, 0, NULL, (UBYTE *) "File", NULL },
	{ 1, 0, JAM1, 0, 0, NULL, (UBYTE *) "Drawer", NULL },
	{ 1, 0, JAM1, 0, 2, NULL, (UBYTE *) "Accept", NULL },
	{ 1, 0, JAM1, 0, 2, NULL, (UBYTE *) "Cancel", NULL },
	{ 1, 0, JAM1, 0, 2, NULL, (UBYTE *) "Parent", NULL },
	{ 1, 0, JAM1, 0, 2, NULL, (UBYTE *) "Drives", NULL }
};

const static struct Gadget defaultgadgets[9] =
{
	{ NULL, 64, 47, 240, 8, GFLG_GADGHCOMP, GACT_RELVERIFY | GACT_TOGGLESELECT, GTYP_STRGADGET, NULL, NULL, NULL, NULL, NULL, FILENAME, NULL },
	{ NULL, 64, 34, 240, 8, GFLG_GADGHCOMP, GACT_RELVERIFY | GACT_TOGGLESELECT, GTYP_STRGADGET, NULL, NULL, NULL, NULL, NULL, DRAWERNAME, NULL },
	{ NULL, 8, 59, 132, 12, GFLG_GADGHCOMP, GACT_RELVERIFY, GTYP_BOOLGADGET, NULL, NULL, NULL, NULL, NULL, OKAY, NULL },
	{ NULL, 176, 59, 132, 12, GFLG_GADGHCOMP, GACT_RELVERIFY, GTYP_BOOLGADGET, NULL, NULL, NULL, NULL, NULL, CANCEL, NULL },
	{ NULL, 292, 14, 16, 8, GFLG_GADGHCOMP, GACT_IMMEDIATE | GACT_RELVERIFY, GTYP_BOOLGADGET, NULL, NULL, NULL, NULL, NULL, FILENAMEUP, NULL },
	{ NULL, 292, 22, 16, 8, GFLG_GADGHCOMP, GACT_IMMEDIATE | GACT_RELVERIFY, GTYP_BOOLGADGET, NULL, NULL, NULL, NULL, NULL, FILENAMEDOWN, NULL },
	{ NULL, 8, 14, 132, 12, GFLG_GADGHCOMP, GACT_RELVERIFY, GTYP_BOOLGADGET, NULL, NULL, NULL, NULL, NULL, PARENT, NULL },
	{ NULL, 176, 14, 132, 12, GFLG_GADGHCOMP, GACT_RELVERIFY, GTYP_BOOLGADGET, NULL, NULL, NULL, NULL, NULL, DRIVES, NULL },
	{ NULL, 296, 30, 8, 22, GFLG_GADGHNONE, GACT_IMMEDIATE | GACT_FOLLOWMOUSE | GACT_RELVERIFY, GTYP_PROPGADGET, NULL, NULL, NULL, NULL, NULL, FILENAMEPOS, NULL }
};

const static struct NewWindow defaultwindow =
{
	15,
	25,
	316,
	115,
	0,
	1,
	IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS,
	WFLG_SMART_REFRESH | WFLG_NOCAREREFRESH | WFLG_ACTIVATE | WFLG_RMBTRAP | WFLG_DRAGBAR,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	0,
	CUSTOMSCREEN
};

char *devtypes[5] =
{
	"  <DEV>",
	"  <ASN>",
	"  <VOL>",
	"  <DFR>",
	"  <NBD>"
};

extern char nullstring[];

inline void doprops(struct FileReqData *freqdata)
{
	IDOpus->FixSliderPot(freqdata->fr_Window, &freqdata->reqgads[8], freqdata->fileoffset, freqdata->fileentries, freqdata->freq->lines, 1);
}

inline void fixprop(struct FileReqData *freqdata)
{
	IDOpus->FixSliderBody(freqdata->fr_Window, &freqdata->reqgads[8], freqdata->fileentries, freqdata->freq->lines, 1);
}

inline void activatefilegad(struct FileReqData *freqdata)
{
	IDOpus->ActivateStrGad(&freqdata->reqgads[(freqdata->flags & DFRF_DIRREQ) ? 1 : 0], freqdata->fr_Window);
}

void deallocate_entries(struct FileReqData *freqdata)
{
	IDOpus->LFreeRemember(&freqdata->filekey);
	freqdata->firstfile = freqdata->firstdir = NULL;
	freqdata->fileentries = freqdata->fileoffset = 0;
	freqdata->oldfileoffset = -1;
}

void freemulti(struct DOpusFileReq *freq)
{
	if(freq->flags & DFRF_MULTI)
		IDOpus->LFreeRemember(&freq->filearraykey);
	freq->filearray = NULL;
}

void close_req(struct FileReqData *freqdata)
{
	struct direntry *file;
	struct DOpusFileReq *freq;
	char *buf;
	int b;

	if(freqdata)
	{
		freq = freqdata->freq;
		if(freqdata->fr_Window)
		{
			freqdata->freq->x = freqdata->fr_Window->LeftEdge;
			freqdata->freq->y = freqdata->fr_Window->TopEdge;
			IIntuition->CloseWindow(freqdata->fr_Window);
		}
		if(freqdata->flags & DFRF_MULTI && (file = freqdata->firstfile))
		{
			b = 1;
			while (file)
			{
				if(file->select)
					++b;
				file = file->next;
			}
			if(freq->filearray = (char **)IDOpus->LAllocRemember(&freq->filearraykey, sizeof(char *) * b, MEMF_CLEAR))
			{
				if(buf = (char *)IDOpus->LAllocRemember(&freq->filearraykey, b * FILEBUF_SIZE, MEMF_CLEAR))
				{
					file = freqdata->firstfile;
					b = 0;
					while (file)
					{
						if(file->select)
						{
							freq->filearray[b] = &buf[b * FILEBUF_SIZE];
							IDOpus->LStrCpy(freq->filearray[b], file->name);
							++b;
						}
						file = file->next;
					}
					freq->filearray[b] = &buf[b * FILEBUF_SIZE];
					freq->filearray[b][0] = 0;
				}
				else
					freemulti(freq);
			}
		}
		deallocate_entries(freqdata);
		if(freqdata->filetype)
			IExec->FreeMem(freqdata->filetype, sizeof(int) * freqdata->freq->lines);
		if(freqdata->finfo)
			IExec->FreeMem(freqdata->finfo, sizeof(struct FileInfoBlock));
		IExec->FreeMem(freqdata, sizeof(struct FileReqData));
	}
}

void displayfiles(struct FileReqData *freqdata)
{
	struct direntry *work;
	struct RastPort *rp;
	int a, b, y, nlim, slim;

	if(freqdata->fileoffset == freqdata->oldfileoffset)
		return;
	freqdata->oldfileoffset = freqdata->fileoffset;
	if((work = freqdata->firstfile))
	{
		for(a = 0; a < freqdata->fileoffset; a++)
			if(!(work = work->next))
				break;
	}
	rp = freqdata->fr_Window->RPort;

	y = freqdata->fy + freqdata->fb;
	nlim = freqdata->width - 7;
	slim = 7;
	if(nlim < 8)
	{
		nlim = 8;
		slim = freqdata->width - 8;
	}
	if(nlim > freqdata->width)
	{
		nlim = freqdata->width;
		slim = 0;
	}
	IGraphics->SetDrMd(rp, JAM2);
	for(b = 0; b < freqdata->freq->lines; b++)
	{
		IGraphics->Move(rp, 10, y);
		if(work)
		{
			freqdata->filetype[b] = work->type;
			if(work->type < 0)
			{
				if(work->select)
				{
					IGraphics->SetAPen(rp, 2);
					IGraphics->SetBPen(rp, 3);
				}
				else
					IGraphics->SetAPen(rp, 1);
			}
			else if(work->type > 0)
				IGraphics->SetAPen(rp, 3);
			else
				IGraphics->SetAPen(rp, 2);
			a = strlen(work->name);
			if(a > nlim)
				a = nlim;
			IGraphics->Text(rp, work->name, a);
			if(a < nlim)
				IGraphics->Text(rp, nullstring, nlim - a);
			if(slim)
				IGraphics->Text(rp, work->size, slim);
			if(work->select)
				IGraphics->SetBPen(rp, 0);
			work = work->next;
		}
		else
			IGraphics->Text(rp, nullstring, freqdata->width);
		y += freqdata->fh;
	}
}

void doposprop(struct FileReqData *freqdata)
{
	freqdata->fileoffset = IDOpus->GetSliderPos(&freqdata->reqgads[8], freqdata->fileentries, freqdata->freq->lines);
	displayfiles(freqdata);
}

void addfileentry(struct FileReqData *freqdata, char *name, int type, int size)
{
	struct direntry *add, *afterdir = NULL, *work, *new;
	int adir = 0;

	if(freqdata->firstfile)
	{
		if(type < 0)
		{
			if(freqdata->firstdir)
				add = freqdata->firstdir;
			else
			{
				add = freqdata->firstfile;
				while (add && add->type > 0)
					add = add->next;
			}
		}
		else
			add = freqdata->firstfile;
		while (add)
		{
			if(IDOpus->LStrCmpI(name, add->name) <= 0 || (type > 0 && add == freqdata->firstdir))
			{
				if(add->last)
					afterdir = add->last;
				else
					adir = 1;
				break;
			}
			add = add->next;
		}
	}

	if(!(new = (struct direntry *)IDOpus->LAllocRemember(&freqdata->filekey, sizeof(struct direntry), MEMF_CLEAR)))
		return;

	if(afterdir)
	{
		work = afterdir;
		add = afterdir->next;
		work->next = new;
		new->last = work;
		work = new;
		work->next = add;
		add->last = work;
	}
	else if(adir == 1)
	{
		add = freqdata->firstfile;
		work = new;
		work->next = add;
		add->last = work;
		freqdata->firstfile = work;
	}
	else
	{
		if(freqdata->firstfile)
		{
			work = freqdata->firstfile;
			while (work->next)
				work = work->next;
			work->next = new;
			new->last = work;
			work = new;
		}
		else
		{
			work = new;
			freqdata->firstfile = work;
		}
	}
	IDOpus->LStrCpy(work->name, name);
	if(type == 0)
		IDOpus->LStrCpy(work->size, devtypes[size]);
	else if(type < 0)
	{
		IUtility->SNPrintf(work->size, 10, "%7d", size);
		if((work->last && work->last->type > 0) || (!work->last && type < 0))
			freqdata->firstdir = work;
	}
	else
		IDOpus->LStrCpy(work->size, string_table[STR_FILEREQ_DIR]);
	work->type = type;
	++freqdata->fileentries;
}

int getintuimsg(struct Window *win, ULONG * class, USHORT * code, USHORT * qual, USHORT * gadgetid)
{
	struct IntuiMessage *msg;

	if(msg = (struct IntuiMessage *)IExec->GetMsg(win->UserPort))
	{
		*class = msg->Class;
		*code = msg->Code;
		if(qual)
			*qual = msg->Qualifier;
		if(msg->Class == IDCMP_GADGETDOWN || msg->Class == IDCMP_GADGETUP)
			*gadgetid = ((struct Gadget *)msg->IAddress)->GadgetID;
		IExec->ReplyMsg((struct Message *)msg);
		return (1);
	}
	return (0);
}

void refreshdrawergad(struct FileReqData *freqdata)
{
	freqdata->drawernamesinfo.BufferPos = strlen(freqdata->freq->dirbuf);
	IIntuition->RefreshGList(&freqdata->reqgads[1], freqdata->fr_Window, NULL, 1);
}

void checkdrawer(char *buf)
{
	int i;

	if((i = strlen(buf)) > 1)
	{
		--i;
		if(buf[i] != '/' && buf[i] != ':')
			IDOpus->LStrCat(buf, "/");
	}
}

void clearfiles(struct FileReqData *freqdata)
{
	deallocate_entries(freqdata);
	doprops(freqdata);
	fixprop(freqdata);
	displayfiles(freqdata);
}

int get_filenames(struct FileReqData *freqdata)
{
	BPTR lock;
	ULONG class;
	USHORT code, qual, gadgetid;
	int ret = 0;
	char buf[33];

	freqdata->firstfile = freqdata->firstdir = NULL;

	if(!(lock = IDOS->Lock(freqdata->freq->dirbuf, ACCESS_READ)))
	{
		freqdata->oldfileoffset = -1;
		freqdata->fileentries = 0;
		fixprop(freqdata);
		displayfiles(freqdata);
		return (0);
	}

	IDOS->Examine(lock, freqdata->finfo);
	if(freqdata->finfo->fib_DirEntryType < 0)
	{
		IDOS->UnLock(lock);
		freqdata->oldfileoffset = -1;
		displayfiles(freqdata);
		return (0);
	}

	while (IDOS->ExNext(lock, freqdata->finfo))
	{
		while (getintuimsg(freqdata->fr_Window, &class, &code, &qual, &gadgetid))
		{
			ret = gettingdirmsg(freqdata, class, code, qual, gadgetid);
			if(ret == OKAY || ret == CANCEL || ret == INTERRUPT)
			{
				IDOS->UnLock(lock);
				return (ret);
			}
		}
		if(freqdata->finfo->fib_DirEntryType < 0)
		{
			if(freqdata->flags & DFRF_FONT)
			{
				int32 i;
				IDOS->StrToLong(freqdata->finfo->fib_FileName, &i);
				LSprintf(buf, "%ld", i);
				if(IDOpus->LStrCmp(buf, freqdata->finfo->fib_FileName))
					continue;
			}
			else if(freqdata->flags & DFRF_DIRREQ)
				continue;
		}
		addfileentry(freqdata, freqdata->finfo->fib_FileName, freqdata->finfo->fib_DirEntryType, freqdata->finfo->fib_Size);
		freqdata->oldfileoffset = -1;
		fixprop(freqdata);
		doposprop(freqdata);
	}
	IDOS->UnLock(lock);
	freqdata->oldfileoffset = -1;
	displayfiles(freqdata);
	return (ret);
}

int getnewdrawer(struct FileReqData *freqdata)
{
	checkdrawer(freqdata->freq->dirbuf);
	refreshdrawergad(freqdata);
	activatefilegad(freqdata);
	clearfiles(freqdata);
	return (get_filenames(freqdata));
}

int getnew_file(struct FileReqData *freqdata, struct direntry *work)
{
	int i, b;

	if(work->type > 0)
	{
		IDOpus->TackOn(freqdata->freq->dirbuf, work->name, 256);
		i = getnewdrawer(freqdata);
		if(i != OKAY && i != CANCEL && i != INTERRUPT)
			i = INTERRUPT;
		return (i);
	}
	if(work->type == 0)
	{
		IDOpus->LStrCpy(freqdata->freq->dirbuf, work->name);
		i = getnewdrawer(freqdata);
		if(i != OKAY && i != CANCEL && i != INTERRUPT)
			i = INTERRUPT;
		return (i);
	}
	if((IDOpus->LStrCmp(freqdata->freq->filebuf, work->name)) == 0)
		b = 1;
	else
		b = 0;
	IDOpus->LStrCpy(freqdata->freq->filebuf, work->name);
	IIntuition->RefreshGList(&freqdata->reqgads[0], freqdata->fr_Window, NULL, 1);
	return (b);
}

int listdevices(struct FileReqData *freqdata)
{
	IExec->DebugPrintF("listdevices() Needs Fixing ASAP\n");

/*	char devname[32];
	struct DeviceList *devlist;
	struct RootNode *rootnode;
	struct DosInfo *dosinfo;
	ULONG class;
	USHORT code, qual, gadgetid;
	int ret;

	clearfiles(freqdata);
	rootnode = (struct RootNode *)DOSBase->dl_Root;
	dosinfo = (struct DosInfo *)BADDR(rootnode->rn_Info);
	devlist = (struct DeviceList *)BADDR(dosinfo->di_DevInfo);

	while(devlist)
	{
		while (getintuimsg(freqdata->fr_Window, &class, &code, &qual, &gadgetid))
		{
			ret = gettingdirmsg(freqdata, class, code, qual, gadgetid);
			if(ret == OKAY || ret == CANCEL || ret == INTERRUPT)
				return(ret);
		}
		if(devlist->dl_Type != DLT_DEVICE || devlist->dl_Task)
		{
			BtoCStr(devlist->dl_Name, devname, 32);
			LStrCat(devname, ":");
			addfileentry(freqdata, devname, 0, devlist->dl_Type);
		}
		devlist = (struct DeviceList *)BADDR(devlist->dl_Next);
	}
	freqdata->oldfileoffset = -1;
	fixprop(freqdata);
	doposprop(freqdata);
*/	return(-1);







/* this code works, needs adapting only:

	struct DosList *dl;
	int a = 0;
	int gap, i, j, k, p, l, d;
	char devname[16], pathname[256];
	struct Config *config;

	if(!(config = cstuff->config))
		return (0);

	dl = IDOS->LockDosList(LDF_DEVICES | LDF_READ);
	while((dl = IDOS->NextDosEntry(dl, LDF_DEVICES)))
	{
		if(dl->dol_Type == DLT_DEVICE && dl->dol_Task)
		{
			Self->BtoCStr((BPTR) dl->dol_Name, pathname, 256);
			Self->LStrCat(pathname, ":");
			Self->LStrnCpy(devname, pathname, 15);
			devname[15] = 0;
			AssignDrive(cstuff, a, devname, pathname);
			++a;
			if(a == DRIVECOUNT)
				break;
		}
	}
	IDOS->UnLockDosList(LDF_DEVICES | LDF_READ);
	for(gap = a / 2; gap > 0; gap /= 2)
	{
		for(i = gap; i < a; i++)
		{
			for(j = i - gap; j >= 0; j -= gap)
			{
				k = j + gap;
				if(IUtility->Stricmp(config->drive[j].name, config->drive[k].name) <= 0)
					break;
				SwapMem((char *)&config->drive[j], (char *)&config->drive[k], sizeof(struct dopusfunction));
			}
		}
	}

	d = a;
	if(a < DRIVECOUNT)
	{
		dl = IDOS->LockDosList(LDF_ASSIGNS | LDF_READ);
		while((dl = IDOS->NextDosEntry(dl, LDF_ASSIGNS)))
		{
			Self->BtoCStr((BPTR) dl->dol_Name, pathname, 256);
			Self->LStrCat(pathname, ":");
			Self->LStrnCpy(devname, pathname, 15);
			devname[15] = 0;
			AssignDrive(cstuff, a, devname, pathname);
			++a;
			if(a == DRIVECOUNT)
				break;
		}
		IDOS->UnLockDosList(LDF_DEVICES | LDF_READ);
	}
	p = a - d;
	for(gap = p / 2; gap > 0; gap /= 2)
	{
		for(i = gap; i < p; i++)
		{
			for(j = i - gap; j >= 0; j -= gap)
			{
				k = j + gap + d;
				l = j + d;
				if(IUtility->Stricmp(config->drive[l].name, config->drive[k].name) <= 0)
					break;
				SwapMem((char *)&config->drive[l], (char *)&config->drive[k], sizeof(struct dopusfunction));
			}
		}
	}
	if(a < DRIVECOUNT)
	{
		for(i = a; i < DRIVECOUNT; i++)
			AssignDrive(cstuff, i, NULL, NULL);
	}

	return 1;
*/













}

void repeatscroll(struct FileReqData *freqdata, int dir, int menu)
{
	ULONG class;
	USHORT code, gadgetid;
	int x, y, m;

	if((dir == -1 && freqdata->fileoffset == 0) || (dir == 1 && freqdata->fileoffset >= freqdata->fileentries - freqdata->freq->lines))
		return;
	freqdata->fileoffset += dir;
	displayfiles(freqdata);
	doprops(freqdata);
	if(menu)
		m = ((freqdata->freq->lines * freqdata->fh) / 2) + 30;
	IDOS->Delay(5);
	FOREVER
	{
		if(getintuimsg(freqdata->fr_Window, &class, &code, NULL, &gadgetid) && ((class == IDCMP_GADGETUP || (class == IDCMP_MOUSEBUTTONS && code == SELECTUP)) || (menu && class == IDCMP_MOUSEBUTTONS && code == MENUUP)))
			break;
		if(menu)
		{
			x = freqdata->fr_Window->MouseX;
			y = freqdata->fr_Window->MouseY;
			if(x > 9 && x < freqdata->ww + 10)
			{
				if(y < (m - freqdata->fh))
					dir = -1;
				else if(y > (m + freqdata->fh))
					dir = 1;
				else
					dir = 0;
			}
		}
		if((dir == -1 && freqdata->fileoffset == 0) || (dir == 1 && freqdata->fileoffset >= freqdata->fileentries - freqdata->freq->lines))
			break;
		freqdata->fileoffset += dir;
		displayfiles(freqdata);
		doprops(freqdata);
	}
}

int doparent(struct FileReqData *freqdata)
{
	int i, b;

	if((i = strlen(freqdata->freq->dirbuf)) < 2)
		return (0);
	--i;
	if(freqdata->freq->dirbuf[i] == ':')
		return (0);
	if(freqdata->freq->dirbuf[i] == '/')
		--i;
	for(b = i; b > -1; b--)
	{
		if(freqdata->freq->dirbuf[b] == '/')
			break;
		if(freqdata->freq->dirbuf[b] == ':')
		{
			++b;
			break;
		}
	}
	if(b < 0)
		b = 0;
	freqdata->freq->dirbuf[b] = 0;
	IIntuition->RefreshGList(&freqdata->reqgads[1], freqdata->fr_Window, NULL, 1);
	return (1);
}

struct direntry *getfileentry(struct FileReqData *freqdata, int which)
{
	int i, b;
	struct direntry *work;

	i = freqdata->fileoffset + which;
	if(!(work = freqdata->firstfile))
		return (0);
	for(b = 0; b < i; b++)
		if(!(work = work->next))
			return (0);
	return (work);
}

int do_idcmp(struct FileReqData *freqdata, ULONG class, USHORT code, USHORT qual, USHORT gadgetid)
{
	int ret = 0, x, y, a, b, ty;
	struct RastPort *rp;
	struct direntry *file;
	LONG sec, mic;

	switch (class)
	{
	case IDCMP_GADGETDOWN:
		switch (gadgetid)
		{
		case FILENAMEPOS:
			doposprop(freqdata);
			FOREVER
			{
				IExec->Wait(1 << freqdata->fr_Window->UserPort->mp_SigBit);
				while (getintuimsg(freqdata->fr_Window, &class, &code, &qual, &gadgetid))
				{
					if(class == IDCMP_GADGETUP)
						break;
					if(class == IDCMP_MOUSEMOVE)
						doposprop(freqdata);
				}
				if(class == IDCMP_GADGETUP)
					break;
			}
			break;
		case FILENAMEUP:
			repeatscroll(freqdata, -1, 0);
			break;
		case FILENAMEDOWN:
			repeatscroll(freqdata, 1, 0);
			break;
		}
		activatefilegad(freqdata);
		return (0);
	case IDCMP_GADGETUP:
		switch (gadgetid)
		{
		case FILENAMEPOS:
			doposprop(freqdata);
			break;
		case CANCEL:
			ret = CANCEL;
			break;
		case FILENAME:
			if(qual & IEQUALIFIER_LSHIFT || qual & IEQUALIFIER_RSHIFT)
			{
				IDOpus->ActivateStrGad(&freqdata->reqgads[1], freqdata->fr_Window);
				return (-1);
			}
			ret = OKAY;
			break;
		case OKAY:
			ret = OKAY;
			break;
		case DRAWERNAME:
			refreshdrawergad(freqdata);
			if(qual & IEQUALIFIER_LSHIFT || qual & IEQUALIFIER_RSHIFT)
			{
				activatefilegad(freqdata);
				return (-1);
			}
			if((ret = getnewdrawer(freqdata)) != OKAY && ret != CANCEL && ret != INTERRUPT)
				ret = INTERRUPT;
			return (ret);
		case PARENT:
			if(doparent(freqdata))
			{
				if((ret = getnewdrawer(freqdata)) != OKAY && ret != CANCEL && ret != INTERRUPT)
					ret = INTERRUPT;
			}
			break;
		case DRIVES:
			activatefilegad(freqdata);
			if((ret = listdevices(freqdata)) != OKAY && ret != CANCEL && ret != INTERRUPT)
				ret = INTERRUPT;
			break;
		}
		activatefilegad(freqdata);
		return (ret);
		break;
	case IDCMP_MOUSEBUTTONS:
		x = freqdata->fr_Window->MouseX;
		y = freqdata->fr_Window->MouseY;
		if(code == SELECTDOWN && x > 9 && x < freqdata->ww + 10 && y >= freqdata->fy && y < freqdata->fy + (freqdata->freq->lines * freqdata->fh))
		{
			a = (y - freqdata->fy) / freqdata->fh;
			if(file = getfileentry(freqdata, a))
			{
				b = 1;
				ty = freqdata->fy + (a * freqdata->fh);
				rp = freqdata->fr_Window->RPort;
				IGraphics->SetDrMd(rp, COMPLEMENT);
				IGraphics->RectFill(rp, 10, ty, freqdata->ww + 9, ty + (freqdata->fh - 1));
				if(freqdata->flags & DFRF_MULTI && file->type < 0)
				{
					file->select = 1 - file->select;
					b = 1;
				}
				else
				{
					freqdata->fr_Window->Flags |= WFLG_REPORTMOUSE;
					FOREVER
					{
						IExec->Wait(1 << freqdata->fr_Window->UserPort->mp_SigBit);
						while (getintuimsg(freqdata->fr_Window, &class, &code, &qual, &gadgetid))
						{
							if(class == IDCMP_MOUSEMOVE)
							{
								x = freqdata->fr_Window->MouseX;
								y = freqdata->fr_Window->MouseY;
								if(x < 10 || x > freqdata->ww + 9 || y < ty || y >= ty + freqdata->fh)
								{
									if(b)
									{
										b = 0;
										IGraphics->RectFill(rp, 10, ty, freqdata->ww + 9, ty + (freqdata->fh - 1));
									}
								}
								else if(!b)
								{
									b = 1;
									IGraphics->RectFill(rp, 10, ty, freqdata->ww + 9, ty + (freqdata->fh - 1));
								}
							}
							else if(class == IDCMP_MOUSEBUTTONS && code == SELECTUP)
								break;
						}
						if(class == IDCMP_MOUSEBUTTONS && code == SELECTUP)
							break;
					}
					freqdata->fr_Window->Flags &= ~WFLG_REPORTMOUSE;
				}
				if(b)
				{
					if(!(freqdata->flags & DFRF_MULTI))
						IGraphics->RectFill(rp, 10, ty, freqdata->ww + 9, ty + (freqdata->fh - 1));
					IGraphics->SetDrMd(rp, JAM2);
					IIntuition->CurrentTime((ULONG *) & sec, (ULONG *) & mic);
					if((ret = getnew_file(freqdata, file)) == 1 && IIntuition->DoubleClick(freqdata->prevsec, freqdata->prevmic, sec, mic))
						ret = OKAY;
					else if(ret != OKAY && ret != CANCEL && ret != INTERRUPT)
					{
						freqdata->prevsec = sec;
						freqdata->prevmic = mic;
						ret = 0;
					}
				}
				else
					IGraphics->SetDrMd(rp, JAM2);
			}
		}
		else if(code == MENUDOWN && x > 9 && x < freqdata->ww + 10 && y > 28 && y < 37 + (freqdata->freq->lines * freqdata->fh))
		{
			a = ((freqdata->freq->lines * freqdata->fh) / 2) + 30;
			if(y < (a - freqdata->fh))
				b = -1;
			else if(y > (a + freqdata->fh))
				b = 1;
			else
				b = 0;
			repeatscroll(freqdata, b, 1);
		}
		activatefilegad(freqdata);
		return (ret);
		break;
	default:
		activatefilegad(freqdata);
		break;
	}
	return (0);
}

int gettingdirmsg(struct FileReqData *freqdata, ULONG class, USHORT code, USHORT qual, USHORT gadgetid)
{
	int ret;

	if((ret = do_idcmp(freqdata, class, code, qual, gadgetid)) != -1)
	{
		freqdata->oldfileoffset = -1;
		return (ret);
	}
	return (0);
}

int initrequester(struct FileReqData *freqdata)
{
	int a, b;
	struct DOpusFileReq *freq;
	struct RastPort *rp;
	struct Screen *screen;
	struct TextFont *font;

	freq = freqdata->freq;
	freq->filearraykey = NULL;
	freq->filearray = NULL;
	freqdata->flags = freq->flags;
	if(freqdata->flags & DFRF_DIRREQ)
		freqdata->flags &= ~DFRF_MULTI;

	freqdata->version2 = 1;
	screen = (freq->window) ? freq->window->WScreen : ((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveScreen;

	freqdata->sfh = screen->RastPort.Font->tf_YSize;
	freqdata->sfw = screen->RastPort.Font->tf_XSize;
	if(freqdata->sfh < 8)
		freqdata->sfh = 8;
	if(freqdata->sfw < 6)
		freqdata->sfw = 6;

	if(screen->RastPort.Font->tf_Flags & FPF_PROPORTIONAL)
		font = ((struct GfxBase *)(IGraphics->Data.LibBase))->DefaultFont;
	else
		font = screen->RastPort.Font;

	freqdata->fh = font->tf_YSize;
	freqdata->fw = font->tf_XSize;
	freqdata->fb = font->tf_Baseline;

	IExec->CopyMem((char *)&defaultprop, (char *)&freqdata->filenameprop, sizeof(struct PropInfo));
	freqdata->filenamesinfo.Buffer = (STRPTR) freq->filebuf;
	freqdata->filenamesinfo.MaxChars = FILEBUF_SIZE - 1;
	freqdata->drawernamesinfo.Buffer = (STRPTR) freq->dirbuf;
	freqdata->drawernamesinfo.MaxChars = 256 - FILEBUF_SIZE;

	if(freq->lines < 5)
		freq->lines = 5;
	else if(freq->lines > ((screen->Height - (freqdata->sfh * 5) + 35) / freqdata->fh))
		freq->lines = (screen->Height - (freqdata->sfh * 5) + 35) / freqdata->fh;

	if(!(freqdata->filetype = IExec->AllocMem(sizeof(int) * freq->lines, MEMF_CLEAR)))
		return (0);

	IExec->CopyMem((char *)&defaultgadgets, (char *)&freqdata->reqgads, sizeof(struct Gadget) * 9);
	IExec->CopyMem((char *)&defaulttext, (char *)&freqdata->reqtext, sizeof(struct IntuiText) * 6);
	IExec->CopyMem((char *)&defaultimage, (char *)&freqdata->filenameimage, sizeof(struct Image));
	freqdata->reqgads[0].SpecialInfo = (APTR) & freqdata->filenamesinfo;
	freqdata->reqgads[1].SpecialInfo = (APTR) & freqdata->drawernamesinfo;
	freqdata->reqgads[8].GadgetRender = (APTR) & freqdata->filenameimage;
	freqdata->reqgads[8].SpecialInfo = (APTR) & freqdata->filenameprop;
	for(a = 0; a < 8; a++)
		freqdata->reqgads[a].NextGadget = &freqdata->reqgads[a + 1];
	for(a = 0; a < 4; a++)
		freqdata->reqgads[a].GadgetText = &freqdata->reqtext[a];
	for(a = 6; a < 8; a++)
		freqdata->reqgads[a].GadgetText = &freqdata->reqtext[a - 2];
	IExec->CopyMem((char *)&defaultwindow, (char *)&freqdata->reqwin, sizeof(struct NewWindow));
	freqdata->reqwin.Height = (freq->lines * freqdata->fh) + (freqdata->sfh * 5) + 31 + ((freqdata->flags & DFRF_DIRREQ) ? 0 : 4);
	if(freqdata->reqwin.Height > screen->Height)
	{
		freq->lines = (screen->Height - ((freqdata->sfh * 5) + 35)) / freqdata->fh;
		freqdata->reqwin.Height = (freq->lines * freqdata->fh) + (freqdata->sfh * 5) + 35;
	}
	if(freqdata->flags & DFRF_DIRREQ)
		freqdata->reqwin.Height -= freqdata->sfh + 2;
	freqdata->reqwin.Width = (freqdata->fw * 35) + 36;
	if(freqdata->reqwin.Width > screen->Width)
	{
		a = (screen->Width - 36) / freqdata->fw;
		freqdata->width = a;
		freqdata->reqwin.Width = (freqdata->fw * a) + 36;
	}
	else
		freqdata->width = 35;
	freqdata->ww = freqdata->width * freqdata->fw;

	freqdata->reqwin.Screen = screen;
	if(freq->x == -2)
		freqdata->reqwin.LeftEdge = (screen->Width - freqdata->reqwin.Width) / 2;
	else if(freq->x < 0)
		freqdata->reqwin.LeftEdge = 15;
	else
		freqdata->reqwin.LeftEdge = freq->x;
	if(freq->y == -2)
		freqdata->reqwin.TopEdge = (screen->Height - freqdata->reqwin.Height) / 2;
	else if(freq->y < 0)
		freqdata->reqwin.TopEdge = 25;
	else
		freqdata->reqwin.TopEdge = freq->y;
	if(freqdata->reqwin.LeftEdge + freqdata->reqwin.Width > screen->Width)
		freqdata->reqwin.LeftEdge = screen->Width - freqdata->reqwin.Width;
	if(freqdata->reqwin.TopEdge + freqdata->reqwin.Height > screen->Height)
		freqdata->reqwin.TopEdge = screen->Height - freqdata->reqwin.Height;
	if(freq->title)
		freqdata->reqwin.Title = (STRPTR) freq->title;
	else
		freqdata->reqwin.Title = (STRPTR) string_table[STR_FILEREQ_NAME];
	if(freqdata->flags & DFRF_SAVE)
		freqdata->reqwin.BlockPen = 2;

	if(!(freqdata->finfo = IExec->AllocMem(sizeof(struct FileInfoBlock), MEMF_CLEAR)))
		return (0);

	freqdata->oldfileoffset = -1;

	if(!(freqdata->fr_Window = IIntuition->OpenWindow(&freqdata->reqwin)))
		return (0);
	rp = freqdata->fr_Window->RPort;
	IGraphics->SetFont(rp, font);
	IGraphics->SetDrMd(rp, JAM2);
	freqdata->sfy = freqdata->fr_Window->BorderTop + 2;
	freqdata->fy = freqdata->sfy + freqdata->sfh + 7;

	a = freq->lines * freqdata->fh;
	freqdata->reqgads[1].TopEdge = freqdata->fy + a + 5;
	if(!(freqdata->flags & DFRF_DIRREQ))
		freqdata->reqgads[0].TopEdge = freqdata->reqgads[1].TopEdge + freqdata->sfh + 6;
	freqdata->reqgads[2].TopEdge = freqdata->reqgads[(freqdata->flags & DFRF_DIRREQ ? 1 : 0)].
		TopEdge + freqdata->sfh + 4;
	freqdata->reqgads[3].TopEdge = freqdata->reqgads[2].TopEdge;
	freqdata->reqgads[4].TopEdge = freqdata->fy + a - 15;
	freqdata->reqgads[5].TopEdge = freqdata->fy + a - 7;
	freqdata->reqgads[6].TopEdge = freqdata->reqgads[7].TopEdge = freqdata->sfy;
	freqdata->reqgads[8].TopEdge = freqdata->fy + 1;
	freqdata->reqgads[0].LeftEdge = freqdata->reqgads[1].LeftEdge = (freqdata->sfw * 6) + 16;
	freqdata->reqgads[3].LeftEdge = freqdata->reqgads[7].LeftEdge = freqdata->ww - 104;
	freqdata->reqgads[4].LeftEdge = freqdata->reqgads[5].LeftEdge = freqdata->ww + 12;
	freqdata->reqgads[8].LeftEdge = freqdata->ww + 16;
	freqdata->reqgads[0].Width = freqdata->reqgads[1].Width = (freqdata->ww + 24) - freqdata->reqgads[0].LeftEdge;
	if((freqdata->sfw * 6) > 132)
	{
		freqdata->reqgads[2].Width = freqdata->reqgads[3].Width = freqdata->reqgads[6].Width =
			freqdata->reqgads[7].Width = freqdata->sfw * 7;
		freqdata->reqgads[3].LeftEdge -= ((freqdata->sfw * 7) - 132);
		freqdata->reqgads[7].LeftEdge = freqdata->reqgads[3].LeftEdge;
	}
	freqdata->reqgads[0].Height = freqdata->reqgads[1].Height = freqdata->sfh;
	freqdata->reqgads[2].Height = freqdata->reqgads[3].Height = freqdata->reqgads[6].Height = freqdata->reqgads[7].Height =
		freqdata->sfh + 4;
	freqdata->reqgads[8].Height = a - 18;

	for(b = 2; b < 8; b++)
		IDOpus->Do3DBox(rp, freqdata->reqgads[b].LeftEdge + 2, freqdata->reqgads[b].TopEdge + 1, freqdata->reqgads[b].Width - 4, freqdata->reqgads[b].Height - 2, 2, 1);
	if(!(freqdata->flags & DFRF_DIRREQ))
		IDOpus->Do3DStringBox(rp, freqdata->reqgads[0].LeftEdge, freqdata->reqgads[0].TopEdge, freqdata->reqgads[0].Width, freqdata->sfh, 2, 1);
	IDOpus->Do3DStringBox(rp, freqdata->reqgads[1].LeftEdge, freqdata->reqgads[1].TopEdge, freqdata->reqgads[1].Width, freqdata->sfh, 2, 1);
	for(b = 4; b < 6; b++)
		IDOpus->DoArrow(rp, freqdata->ww + 14, freqdata->reqgads[b].TopEdge + 1, 12, 6, 1, 0, b - 4);
	IDOpus->Do3DBox(rp, 10, freqdata->fy, freqdata->ww, a, 2, 1);
	IDOpus->Do3DBox(rp, freqdata->ww + 14, freqdata->fy, 12, a - 16, 2, 1);

	freqdata->reqtext[0].LeftEdge = freqdata->reqtext[1].LeftEdge = -(freqdata->reqgads[0].LeftEdge - 10);
	for(b = 2; b < 6; b++)
		freqdata->reqtext[b].LeftEdge = (freqdata->reqgads[2].Width - (freqdata->sfw * 6)) / 2;

	IIntuition->AddGList(freqdata->fr_Window, &freqdata->reqgads[(freqdata->flags & DFRF_DIRREQ) ? 1 : 0], -1, -1, NULL);
	IIntuition->RefreshGList(&freqdata->reqgads[(freqdata->flags & DFRF_DIRREQ) ? 1 : 0], freqdata->fr_Window, NULL, -1);
	activatefilegad(freqdata);
	return (1);
}

int _DOpus_FileRequest(struct DOpusIFace *Self, struct DOpusFileReq *freq)
{
	struct Library *AslBase = IExec->OpenLibrary("asl.library", 50L);
	struct AslIFace *IAsl = (struct AslIFace *)IExec->GetInterface(AslBase, "main", 1, NULL);
	IDOpus = Self;

	if(freq->dirbuf[0] && (Self->CheckExist(freq->dirbuf, NULL) < 0))
	{
		char *ptr;
		if(ptr = IDOS->FilePart(freq->dirbuf))
		{
			if(freq->filebuf)
			{
				int a;

				Self->LStrnCpy(freq->filebuf, ptr, FILEBUF_SIZE - 2);
				a = strlen(freq->filebuf);
				if(freq->filebuf[a - 1] == '/')
					freq->filebuf[a - 1] = 0;
			}
			*ptr = 0;
		}
	}

	if(AslBase && IAsl)
	{
		struct TagItem asltags[10];
		struct FileRequester *fr;
		int font, res = FALSE, a;
		char initialfont[40], *ptr;
		APTR request;

		for(a = 0; a < 10; a++)
		{
			asltags[a].ti_Tag = TAG_SKIP;
			asltags[a].ti_Data = 0;
		}
		asltags[9].ti_Tag = TAG_END;

		if(freq->flags & DFRF_FONT)
			font = 1;
		else
			font = 0;

		if(freq->title)
		{
			asltags[0].ti_Tag = ASLFR_TitleText;
			asltags[0].ti_Data = (ULONG) freq->title;
		}

		asltags[1].ti_Tag = ASLFR_Window;
		asltags[1].ti_Data = (ULONG) ((freq->window) ? freq->window : ((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow);

		if(freq->x > 0)
		{
			asltags[2].ti_Tag = ASLFR_InitialLeftEdge;
			asltags[2].ti_Data = freq->x;
		}

		if(freq->y > 0)
		{
			asltags[3].ti_Tag = ASLFR_InitialTopEdge;
			asltags[3].ti_Data = freq->y;
		}

		asltags[4].ti_Tag = ASLFR_InitialHeight;
		asltags[4].ti_Data = freq->lines * 14;

		if(font)
		{
			if(ptr = IDOS->FilePart(freq->dirbuf))
			{
				Self->LStrCpy(initialfont, ptr);
				a = strlen(initialfont);
				if(initialfont[a - 1] == '/')
					initialfont[a - 1] = 0;
			}
			else
				initialfont[0] = 0;
			asltags[5].ti_Tag = ASLFO_InitialName;
			asltags[5].ti_Data = (ULONG) initialfont;
			asltags[6].ti_Tag = ASLFO_InitialSize;
			IDOS->StrToLong(freq->filebuf, &asltags[6].ti_Data);
		}
		else
		{
			if(freq->filebuf)
			{
				asltags[5].ti_Tag = ASLFR_InitialFile;
				asltags[5].ti_Data = (ULONG) freq->filebuf;
			}
			if(freq->dirbuf)
			{
				asltags[6].ti_Tag = ASLFR_InitialDrawer;
				asltags[6].ti_Data = (ULONG) freq->dirbuf;
			}
		}

		if((request = IAsl->AllocAslRequest((font) ? ASL_FontRequest : ASL_FileRequest, asltags)))
		{
			if(res = IAsl->AslRequest(request, NULL))
			{
				if(font)
				{
					if(freq->dirbuf)
					{
						Self->StrCombine(freq->dirbuf, "FONTS:", ((struct FontRequester *)request)->fo_Attr.ta_Name, 256);
						Self->LStrCat(freq->dirbuf, "/");
					}
					if(freq->filebuf)
					{
						LSprintf(freq->filebuf, "%ld", ((struct FontRequester *)request)->fo_Attr.ta_YSize);
					}
				}
				else
				{
					fr = (struct FileRequester *)request;
					if(freq->dirbuf)
						Self->LStrCpy(freq->dirbuf, fr->fr_Drawer);
					if(!(freq->flags & DFRF_DIRREQ))
					{
						if(freq->filebuf)
							Self->LStrCpy(freq->filebuf, fr->fr_File);
					}
					if(freq->flags & DFRF_MULTI
					   && fr->fr_NumArgs > 0)
					{
						if((freq->filearray = Self->LAllocRemember(&freq->filearraykey, (fr->fr_NumArgs + 1) * 4, MEMF_CLEAR)))
						{
							for(a = 0; a < fr->fr_NumArgs; a++)
							{
								if(freq->filearray[a] = Self->LAllocRemember(&freq->filearraykey, strlen(fr->fr_ArgList[a].wa_Name) + 1, MEMF_CLEAR))
								{
									Self->LStrCpy(freq->filearray[a], fr->fr_ArgList[a].wa_Name);
								}
							}
						}
					}
				}
			}
			IAsl->FreeAslRequest(request);
		}
		IExec->DropInterface((struct Interface *)IIntuition);
		IExec->CloseLibrary(IntuitionBase);
		IExec->DropInterface((struct Interface *)IDOS);
		IExec->CloseLibrary(DOSBase);
		IExec->DropInterface((struct Interface *)IAsl);
		IExec->CloseLibrary(AslBase);
		return (res);
	}
	else
	{
		ULONG class;
		USHORT code, gadgetid, qual;
		struct FileReqData *freqdata;
		unsigned char *dirbuf;
		int i, ret = 1;

		if(!(freqdata = IExec->AllocMem(sizeof(struct FileReqData), MEMF_CLEAR)))
			return (ERROR_NOMEM);
		freqdata->freq = freq;
		if(!(initrequester(freqdata)))
		{
			close_req(freqdata);
			return (ERROR_NOMEM);
		}
		dirbuf = freq->dirbuf;

		if((i = get_filenames(freqdata)) == CANCEL)
		{
			close_req(freqdata);
			freemulti(freq);
			return (ERROR_CANCEL);
		}
		if(i != OKAY)
		{
			FOREVER
			{
				IExec->Wait(1 << freqdata->fr_Window->UserPort->mp_SigBit);
				while(getintuimsg(freqdata->fr_Window, &class, &code, &qual, &gadgetid))
				{
					if((i = do_idcmp(freqdata, class, code, qual, gadgetid)) == CANCEL)
					{
						close_req(freqdata);
						freemulti(freq);
						return (ERROR_CANCEL);
					}
					if(i == OKAY)
					{
						close_req(freqdata);
						goto acceptreq;
					}
				}
			}
		}
		else
			close_req(freqdata);
	      acceptreq:
		checkdrawer(dirbuf);
		if(freq->flags & DFRF_DIRREQ)
		{
			if(i == OKAY || dirbuf[0])
				ret = 1;
			else
				ret = 0;
		}
		return (ret);
	}
}
