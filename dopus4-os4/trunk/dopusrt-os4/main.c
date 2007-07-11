/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 distributed under the GPL may be entitled
to upgrade to the latest versions of Directory Opus at a reduced price.
Please see http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include <string.h>

#include <proto/dopus.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <graphics/gfxbase.h>
#include <devices/input.h>
#include <workbench/startup.h>

#include "DOpusRT_rev.h"

struct Library *DOpusBase = NULL;
struct DOpusIFace *IDOpus = NULL;

void WBRun(int, char **);
int setarg(struct WBArg *, char *, BPTR);
BPTR CloneCommandDir(const char *);

static const char version[] __attribute__ ((used)) = NEW_VERSTAG " OS4";

static const char *pathlists[7] =
{
	"Workbench",
	"Initial CLI",
	"Shell Process",
	"New CLI",
	"AmigaShell",
	"New_WShell",
	"Background CLI"
};

struct InputEvent *InputHandler(struct InputEvent *ie, struct Task *task)
{
	if(ie->ie_Class == IECLASS_RAWMOUSE)
	{
		if(ie->ie_Code & IECODE_LBUTTON)
		{
			IExec->Signal(task, (ie->ie_Code & IECODE_UP_PREFIX) ? 2 : 1);
		}
	}
	return(ie);
}

int main(int argc, char **argv)
{
	int32 a, out, x, y, flag;
	struct MsgPort *port, *port1, *cont;
	struct Process *myproc;
	struct Message msg;
	struct Window *win = NULL;
	struct CommandLineInterface *cli;
	struct Interrupt *interrupt;
	struct IOStdReq *inputreq;

	/* Attempt to open the DOPUS.LIBRARY. Look first in default search path, and then look for it on the distribution disk. If we can't find it exit */
	if(!(DOpusBase = IExec->OpenLibrary("dopus.library", 0)))
	{
		if(!(DOpusBase = IExec->OpenLibrary("PROGDIR:/libs/dopus.library", 0)))
		{
			DOpusBase = NULL;
		}
	}
	if(DOpusBase && !(IDOpus = (struct DOpusIFace *)IExec->GetInterface(DOpusBase, "main", 1, NULL)))
	{
		IDOpus = NULL;
	}
	if(!DOpusBase || !IDOpus)
	{
		IDOS->Printf("Can't Open dopus.library and get IDOpus interface\n");
		IExec->DropInterface((struct Interface *)IDOpus);
		IExec->CloseLibrary(DOpusBase);
		return 5;
	}
	/* OpenLibrary() GetInterface() above  */

	myproc = (struct Process *)IExec->FindTask(NULL);
	cli = IDOS->Cli(); //BADDR(myproc->pr_CLI);

	if((cont = IDOS->GetConsoleTask()))
	{
		struct InfoData *ind = IDOS->AllocDosObject(DOS_INFODATA, NULL);

		if(IDOS->DoPkt(cont, ACTION_DISK_INFO, MKBADDR(ind), 0, 0, 0, 0))
		{
			win = (struct Window *)ind->id_VolumeNode;
		}
		IDOS->FreeDosObject(DOS_INFODATA, ind);
	}
	out = IDOS->Output();

	if(argv[1][0] == '-')
	{
		switch (IUtility->ToLower(argv[1][1]))
		{
		case 's':
			if(win)
			{
				if((win->WScreen->Flags & SCREENTYPE) != WBENCHSCREEN)
				{
					IDOS->SetProcWindow(win);
					IGraphics->SetFont(win->RPort, ((struct GfxBase *)(IGraphics->Data.LibBase))->DefaultFont);
					IDOS->Write(out, "\x1b\x63", 2);
				}
			}
			if(cli && !cli->cli_CommandDir)
			{
				BPTR path = 0L;

				for(a = 0; a < 7; a++)
					if((path = CloneCommandDir(pathlists[a])))
						break;
				cli->cli_CommandDir = path;
			}
			break;
		case 'w':
			if(IDOS->StrToLong(argv[2], &a))
			{
				if(a > 60)
				{
					a = 60;
				}
			}
			if(a > 0)
			{
				IDOS->Delay(a * 50);
			}
			else if(a == -1)
			{
				if(out)
				{
					if((port = IExec->CreatePort(NULL, 0)))
					{
						if((inputreq = (struct IOStdReq *)IExec->CreateIORequest(port, sizeof(struct IOStdReq))))
						{
							if(!(IExec->OpenDevice("input.device", 0L, (struct IORequest *)inputreq, 0L)))
							{
								if((interrupt = IExec->AllocMem(sizeof(struct Interrupt), MEMF_CLEAR|MEMF_PUBLIC)))
								{
									interrupt->is_Code = (APTR)InputHandler;
									interrupt->is_Node.ln_Pri = 51;
									interrupt->is_Node.ln_Name = "DOpusRT";
									interrupt->is_Data = myproc;
									if(argc > 3)
									{
										IDOS->Write(out, "\n", 1);
										IDOS->Write(out, argv[3], strlen(argv[3]));
									}
									else
									{
										IDOS->Write(out, "\nPress left mouse button to continue...", 39);
									}
									inputreq->io_Data = (APTR)interrupt;
									inputreq->io_Command = IND_ADDHANDLER;
									IExec->DoIO((struct IORequest *)inputreq);
									flag = 0;
									FOREVER
									{
										a = IExec->Wait(3);
										if(!win)
											break;
										if(((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow == win)
										{
											x = win->MouseX;
											y = win->MouseY;
											if(a & 1)
											{
												if((x <= win->BorderLeft) || (x >= win->Width - win->BorderRight) || (y <= win->BorderTop) || (y >= win->Height - win->BorderBottom))
													flag = 0;
												else
													flag = 1;
											}
											else if(a & 2 && flag)
											{
												flag = 0;
												if(x > win->BorderLeft && x < win->Width - win->BorderRight && y > win->BorderTop && y < win->Height - win->BorderBottom)
													break;
											}
										}
									}
									inputreq->io_Command = IND_REMHANDLER;
									IExec->DoIO((struct IORequest *)inputreq);
									IExec->FreeMem(interrupt, sizeof(struct Interrupt));
								}
								IExec->CloseDevice((struct IORequest *)inputreq);
							}
							IExec->DeleteIORequest((struct IORequest *)inputreq);
						}
						IExec->DeletePort(port);
					}
				}
			}
			break;
		case 'p':
			IExec->Forbid();
			if((port = IExec->FindPort(argv[2])))
			{
				IExec->Permit();
				port1 = IExec->CreatePort(0, 0);
				msg.mn_Node.ln_Type = NT_MESSAGE;
				msg.mn_ReplyPort = port1;
				msg.mn_Length = sizeof(struct Message);
				IExec->PutMsg(port, &msg);
				IExec->WaitPort(port1);
				IExec->DeletePort(port1);
			}
			else
			{
				IExec->Permit();
			}
			break;
		case 'r':
			WBRun(argc - 2, &argv[2]);
			break;
		}
	}

	if(IDOpus)
	{
		IExec->DropInterface((struct Interface *)IDOpus);
	}
	if(DOpusBase)
	{
		IExec->CloseLibrary(DOpusBase);
	}

	return(0);
}

struct PathList
{
	BPTR nextPath;
	BPTR pathLock;
};

BPTR CloneCommandDir(const char *taskname)
{
	struct Process *teacher;
	struct CommandLineInterface *teachcli;
	struct PathList *wext, *mext, *lastmext = NULL;
	BPTR newpath = 0L;

	IExec->Forbid();
	if((teacher = (struct Process *)IExec->FindTask(taskname)))
	{
		if((teachcli = (struct CommandLineInterface *)BADDR(teacher->pr_CLI)))
		{
			for(wext = BADDR(teachcli->cli_CommandDir); wext; wext = BADDR(wext->nextPath))
			{
				if(!(mext = IExec->AllocVec(sizeof(struct PathList), MEMF_PUBLIC)))
					break;

				mext->nextPath = 0L;
				mext->pathLock = IDOS->DupLock(wext->pathLock);
				if(!newpath)
					newpath = MKBADDR(mext);

				if(lastmext)
					lastmext->nextPath = MKBADDR(mext);
				lastmext = mext;
			}
		}
	}
	IExec->Permit();
	return(newpath);
}

void WBRun(int argc, char **argv)
{
	struct WBStartup *WBStartup = NULL;
	struct DiskObject *diskobj = NULL;
	char namebuf[256];
	int stacksize, i, ok = 1;
	struct Process *ourtask;
	struct MsgPort *replyport = NULL;
	BPTR olddir = -1;
//	struct DOpusRemember *key = NULL;
	struct CommandLineInterface *cli;
	void *key;

	if(argc < 1)
		return;

	key = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_CLEAR, ASOPOOL_Puddle, 16384, ASOPOOL_Threshold, 4096, TAG_DONE);

	ourtask = (struct Process *)IExec->FindTask(NULL);

//	if((WBStartup = IDOpus->LAllocRemember(&key, sizeof(struct WBStartup), MEMF_CLEAR)) && (WBStartup->sm_ArgList = IDOpus->LAllocRemember(&key, sizeof(struct WBArg) * (argc + 1), MEMF_CLEAR)) && (replyport = IExec->CreatePort(NULL, 0)))
	if(key && (WBStartup = IExec->AllocPooled(key, sizeof(struct WBStartup))) && (WBStartup->sm_ArgList = IExec->AllocPooled(key, sizeof(struct WBArg) * (argc + 1))) && (replyport = IExec->CreatePort(NULL, 0)))
	{
		WBStartup->sm_Message.mn_ReplyPort = replyport;
		WBStartup->sm_NumArgs = argc;

		for(i = 0; i < argc; i++)
			if(setarg(&WBStartup->sm_ArgList[i], argv[i], ourtask->pr_CurrentDir))
				break;

		if(i == argc)
		{
			olddir = IDOS->CurrentDir(WBStartup->sm_ArgList[0].wa_Lock);

			if(!(diskobj = IIcon->GetDiskObject(WBStartup->sm_ArgList[0].wa_Name)))
			{
				if(!(IDOpus->SearchPathList(argv[0], namebuf, 256)))
					ok = 0;
			}
			else if(diskobj->do_Type == WBTOOL)
				IDOpus->LStrCpy(namebuf, argv[0]);
			else if(diskobj->do_Type == WBPROJECT)
			{
				if(IDOpus->SearchPathList(diskobj->do_DefaultTool, namebuf, 256))
				{
					for(i = argc - 1; i >= 0; i--)
						IExec->CopyMem(&WBStartup->sm_ArgList[i], &WBStartup->sm_ArgList[i + 1], sizeof(struct WBArg));
					if(setarg(&WBStartup->sm_ArgList[0], namebuf, WBStartup->sm_ArgList[0].wa_Lock))
						ok = 0;
					else
						++WBStartup->sm_NumArgs;
				}
				else
				{
					ok = 0;
				}
			}
			else
			{
				ok = 0;
			}

			if(ok)
			{
				if(diskobj)
				{
					stacksize = diskobj->do_StackSize;
				}
				else
				{
					if((cli = BADDR(ourtask->pr_CLI)))
						stacksize = cli->cli_DefaultStack * 4;
					else
						stacksize = 8192;
				}

				stacksize = (stacksize + 3) & (~3);
				if(stacksize < 8192)
					stacksize = 8192;

				WBStartup->sm_ToolWindow = (diskobj) ? diskobj->do_ToolWindow : NULL;

				if((WBStartup->sm_Segment = IDOS->LoadSeg(namebuf)))
				{
					char *ptr = IDOS->FilePart(namebuf);

					IExec->Forbid();
					if((WBStartup->sm_Process = IDOS->CreateProc(ptr, ourtask->pr_Task. tc_Node.ln_Pri, WBStartup->sm_Segment, stacksize)))
					{
						if(ptr)
							*ptr = 0;
						((struct Process *)(WBStartup->sm_Process->mp_SigTask))->pr_HomeDir = IDOS->Lock(ptr, ACCESS_READ);

						IExec->PutMsg(WBStartup->sm_Process, (struct Message *)WBStartup);

						IExec->Permit();
						IExec->WaitPort(replyport);
					}
					else
					{
						IExec->Permit();
					}
				}
			}
		}
	}
	if(replyport)
		IExec->DeletePort(replyport);
	if(diskobj)
		IIcon->FreeDiskObject(diskobj);
	if(olddir != -1)
		IDOS->CurrentDir(olddir);
	if(WBStartup)
	{
		IDOS->UnLoadSeg(WBStartup->sm_Segment);
		if(WBStartup->sm_ArgList)
		{
			for(i = 0; i < WBStartup->sm_NumArgs; i++)
			{
				IDOS->UnLock(WBStartup->sm_ArgList[i].wa_Lock);
			}
		}
	}
//	IDOpus->LFreeRemember(&key);
	IExec->FreeSysObject(ASOT_MEMPOOL, key);
}

int setarg(struct WBArg *WBArg, char *name, BPTR curdir)
{
	char *p, *lastc;
	unsigned char c;

	if(!name || !name[0])
		return (1);

	lastc = NULL;
	for(p = name; *p; p++)
		if(*p == ':' || *p == '/')
			lastc = p + 1;

	if(!lastc)
	{
		WBArg->wa_Lock = IDOS->DupLock(curdir);
		WBArg->wa_Name = name;
	}
	else
	{
		if(!lastc[0])
			return (1);
		WBArg->wa_Name = lastc;
		c = lastc[0];
		lastc[0] = 0;
		if(!(WBArg->wa_Lock = IDOS->Lock(name, ACCESS_READ)))
			return (1);
		*lastc = c;
	}
	return (0);
}
