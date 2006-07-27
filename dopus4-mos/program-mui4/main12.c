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

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include <math.h>
#include <dos/filehandler.h>
#include <proto/alib.h>
#include <proto/muimaster.h>

#include "dopus.h"
#include "mui.h"

void setupchangestate(void)
{
	int unit;
	struct IOStdReq *diskreq;

	disk_change_state = 0;
	if((diskreq = (struct IOStdReq *)CreateIORequest(general_port, sizeof(struct IOStdReq))))
	{
		for(unit = 0; unit < 4; unit++)
		{
			if(!(OpenDevice("trackdisk.device", unit, (struct IORequest *)diskreq, 0)))
			{
				diskreq->io_Command = TD_CHANGESTATE;
				DoIO((struct IORequest *)diskreq);
				if(!diskreq->io_Actual)
					disk_change_state |= 1 << unit;
				CloseDevice((struct IORequest *)diskreq);
			}
		}
		DeleteIORequest((struct IORequest *)diskreq);
	}
}

#define ITEM_NUM 11

static const short diskinfo_heads[] =
{
	STR_DISKINFO_DEVICE,
	STR_DISKINFO_NAME,
	STR_DISKINFO_SIZE,
	STR_DISKINFO_USED,
	STR_DISKINFO_FREE,
	STR_DISKINFO_PERCENT,
	STR_DISKINFO_DENSITY,
	STR_DISKINFO_ERRORS,
	STR_DISKINFO_STATUS,
	STR_DISKINFO_DATE,
	STR_DISKINFO_FILESYSTEM
};

void dodiskinfo(STRPTR path)
{
	struct InfoData infodata;
	long i;
	UQUAD a;
	int b, c, isd = -1, cx, cy, fail = 0, titwidth;
	static char disktxt[11][60], buf[256], formstr[80], diskname[256], dname[10], buf1[40];
	float f;
	struct DateStamp ds;
	struct MsgPort *prt;
	BPTR lock;
//	UBYTE old_change_state;
	struct DOpusRemember *key = NULL;
	APTR app, win, cont, group;
	ULONG ok;
	LONG reqwin;

	if(!path || !path[0])
		return;

	reqwin = config->generalscreenflags & SCR_GENERAL_REQDRAG;

	cx = scr_font[FONT_REQUEST]->tf_XSize;
	cy = scr_font[FONT_REQUEST]->tf_YSize;

	for(i = 0; i < ITEM_NUM; i++)
	{
		disktxt[i][0] = '-';
		disktxt[i][1] = 0;
	}
	main_proc->pr_WindowPtr = (APTR) - 1;

getnewdisk:
	strcpy(diskname, path);
	if(getroot(diskname, &ds))
	{
		strcat(diskname, ":");
		if((prt = (struct MsgPort *)DeviceProc(diskname)))
		{
			if((lock = Lock(diskname, ACCESS_READ)))
			{
				Forbid();
				get_device_task(lock, disktxt[0], prt);

				for(i = 0; i < 4; i++)
				{
					sprintf(dname, "DF%ld:", i);
					if(strcmp(disktxt[0], dname) == 0)
					{
						isd = i;
						break;
					}
				}
				Permit();
				Info(lock, &infodata);
				UnLock(lock);
				switch (infodata.id_DiskType)
				{
				case ID_UNREADABLE_DISK:
					strcpy(disktxt[1], globstring[STR_UNREADABLE_DISK]);
					break;
				case ID_NOT_REALLY_DOS:
					strcpy(disktxt[1], globstring[STR_NOT_A_DOS_DISK]);
					break;
				case ID_KICKSTART_DISK:
					strcpy(disktxt[1], globstring[STR_KICKSTART_DISK]);
					break;
				default:
					strcpy(buf, path);
					strcpy(disktxt[1], diskname);
					disktxt[1][(strlen(disktxt[1]) - 1)] = 0;
					a = ((QUAD)infodata.id_NumBlocks) * infodata.id_BytesPerBlock;
					sprintf(buf1, "%lld", a);
					b = strlen(buf1);
					sprintf(buf, "%ld", infodata.id_NumBlocks);
					sprintf(formstr, "%%%ds %s; %%%dld %s; %%s", b, globstring[STR_DISKINFO_BYTES], strlen(buf), globstring[STR_DISKINFO_BLOCKS]);
					getsizestring(buf, a);
					sprintf(disktxt[2], formstr, buf1, infodata.id_NumBlocks, buf);
					a = ((QUAD)infodata.id_NumBlocksUsed) * infodata.id_BytesPerBlock;
					getsizestring(buf, a);
					sprintf(buf1, "%lld", a);
					sprintf(disktxt[3], formstr, buf1, infodata.id_NumBlocksUsed, buf);
					a = (infodata.id_NumBlocks - infodata.id_NumBlocksUsed) * ((QUAD)infodata.id_BytesPerBlock);
					getsizestring(buf, a);
					sprintf(buf1, "%lld", a);
					sprintf(disktxt[4], formstr, buf1, (infodata.id_NumBlocks - infodata.id_NumBlocksUsed), buf);

					if(infodata.id_NumBlocks)
						f = ((infodata.id_NumBlocksUsed * 100.0) / infodata.id_NumBlocks);
					else
						f = 100.0;
					sprintf(buf, "%.1f", f);
					sprintf(buf1, "%.1f", 100.0 - f);
					sprintf(disktxt[5], "%s%% %s; %s%% %s", buf, globstring[STR_DISKINFO_FULL], buf1, globstring[STR_DISKINFO_SFREE]);
					sprintf(disktxt[6], "%ld %s", infodata.id_BytesPerBlock, globstring[STR_DISKINFO_BYTESPERBLOCK]);
					sprintf(disktxt[7], "%ld", infodata.id_NumSoftErrors);
					switch (infodata.id_DiskState)
					{
					case ID_VALIDATING:
						strcpy(disktxt[8], globstring[STR_VALIDATING]);
						break;
					case ID_WRITE_PROTECTED:
						strcpy(disktxt[8], globstring[STR_WRITE_PROTECTED]);
						break;
					default:
						strcpy(disktxt[8], globstring[STR_READ_WRITE]);
						break;
					}
					seedate(&ds, disktxt[9], 0);
// DosType fix
					{
						struct DosList *dl;
						struct FileSysStartupMsg *fssm;
						struct DosEnvec *de;
						char buf[32];

						if((dl = LockDosList(LDF_DEVICES | LDF_READ)))
						{
							strcpy(buf, disktxt[0]);
							buf[strlen(buf) - 1] = 0;
							if((dl = FindDosEntry(dl, buf, LDF_DEVICES)))
							{
								if((fssm = BADDR(((struct DeviceNode *)dl)->dn_Startup)))
								{
									if((de = BADDR(fssm->fssm_Environ)))
										infodata.id_DiskType = de->de_DosType;
								}
								else
									infodata.id_DiskType = 0x3F3F3F3F;
							}
							UnLockDosList(LDF_DEVICES | LDF_READ);
						}
					}
// end
// new code
					*((ULONG *) disktxt[10]) = infodata.id_DiskType;
					if(disktxt[10][3] < 32)
						disktxt[10][3] += '0';
// end
					disktxt[10][4] = 0;
					switch (infodata.id_DiskType)
					{
					case ID_DOS_DISK:
						strcat(disktxt[10], " (OFS)");
						break;
					case ID_FFS_DISK:
						strcat(disktxt[10], " (FFS)");
						break;
					case ID_INTER_DOS_DISK:
						strcat(disktxt[10], " (I-OFS)");
						break;
					case ID_INTER_FFS_DISK:
						strcat(disktxt[10], " (I-FFS)");
						break;
					case ID_FASTDIR_DOS_DISK:
						strcat(disktxt[10], " (DC-OFS)");
						break;
					case ID_FASTDIR_FFS_DISK:
						strcat(disktxt[10], " (DC-FFS)");
						break;
					case ID_MSDOS_DISK:
						strcat(disktxt[10], " (MS-DOS)");
						break;
					}
					break;
				}
			}
			else
			{
				i = IoErr();
				doerror(i);
				switch (i)
				{
				case ERROR_DEVICE_NOT_MOUNTED:
					strcpy(disktxt[1], globstring[STR_DEVICE_NOT_MOUNTED]);
					break;
				case ERROR_NOT_A_DOS_DISK:
					strcpy(disktxt[1], globstring[STR_NOT_A_DOS_DISK]);
					break;
				case ERROR_NO_DISK:
					strcpy(disktxt[1], globstring[STR_NO_DISK_IN_DRIVE]);
					break;
				default:
					sprintf(disktxt[1], globstring[STR_DOS_ERROR_CODE], i); // a);
					break;
				}
			}
		}
		else
			fail = 1;
	}
	else
		fail = 1;

	if(config->errorflags & ERROR_ENABLE_DOS)
		main_proc->pr_WindowPtr = (APTR) Window;
	if(fail)
	{
		doerror(-1);
		return;
	}

	b = 0;
	titwidth = 0;
	for(i = 0; i < ITEM_NUM; i++)
	{
		if((c = strlen(disktxt[i])) > b)
			b = c;
		if((c = strlen(globstring[diskinfo_heads[i]])) > titwidth)
			titwidth = c;
	}

	app = ApplicationObject,
			MUIA_Application_NoIconify, TRUE,
			MUIA_Application_UseRexx, FALSE,
			SubWindow, win = WindowObject,
				MUIA_Window_ShowAbout, FALSE,
				MUIA_Window_ShowIconify, FALSE,
				MUIA_Window_ShowJump, FALSE,
				MUIA_Window_ShowPopup, FALSE,
				MUIA_Window_ShowPrefs, FALSE,
				MUIA_Window_ShowSnapshot, FALSE,

				reqwin ? MUIA_Window_Title : TAG_IGNORE, "DiskInfo",
				!reqwin ? MUIA_Window_DragBar : TAG_IGNORE, FALSE,
				!reqwin ? MUIA_Window_DepthGadget : TAG_IGNORE, FALSE,

				WindowContents, VGroup,
					Child, group = ColGroup(3), End,
					Child, HCenter(cont = MakeButton(globstring[STR_CONTINUE])),
				End,
			End,
		End;

	if (!app)
		return;

//	centerwindow(&disk_win);

	DoMethod(group, MUIM_Group_InitChange);

	for(i = 0; i < ITEM_NUM; i++)
	{
		APTR obj1, obj2, obj3;

		obj1 = MakeLLabel(globstring[diskinfo_heads[i]]);

		if (obj1)
		{
			obj2 = TextObject, MUIA_Text_Contents, ":", MUIA_Weight, 0, End;

			if (obj2)
			{
				obj3 = TextObject, MUIA_Text_Contents, disktxt[i], End;

				if (obj3)
				{
					DoMethod(group, OM_ADDMEMBER, obj1);
					DoMethod(group, OM_ADDMEMBER, obj2);
					DoMethod(group, OM_ADDMEMBER, obj3);
					continue;
				}

				MUI_DisposeObject(obj2);
			}

			MUI_DisposeObject(obj1);
		}
	}

	DoMethod(group, MUIM_Group_ExitChange);

	DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(cont, MUIM_Notify, MUIA_Pressed, FALSE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	set(win, MUIA_Window_Open, TRUE);
	GetAttr(MUIA_Window_Open, win, &ok);

	if (ok)
	{
		ULONG sigs = 0;

		i = 0;

		for (;;)
		{
			ULONG ret = DoMethod(app, MUIM_Application_NewInput, &sigs);

			if (ret == MUIV_Application_ReturnID_Quit)
			{
				break;
			}

			if (sigs)
				sigs = Wait(sigs);

			#if 0
			case IDCMP_DISKREMOVED:
				setupchangestate();
				break;
			case IDCMP_DISKINSERTED:
				if(isd != -1)
				{
					old_change_state = disk_change_state;
					setupchangestate();
					if((old_change_state & (1 << isd)) != (disk_change_state & (1 << isd)))
					{
						i = 2;
						break;
					}
				}
				break;
			#endif
		}
	}

	MUI_DisposeObject(app);

	LFreeRemember(&key);
	if(i == 2)
		goto getnewdisk;
}

void get_device_task(BPTR lock, STRPTR buffer, struct MsgPort *port)
{
	struct DosList *dl;
	struct DeviceList *devlist;
	struct FileLock *lock2;

	lock2 = (struct FileLock *)BADDR(lock);
	devlist = (struct DeviceList *)BADDR(lock2->fl_Volume);

	
	dl = LockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS | LDF_READ);
	while((dl = NextDosEntry(dl, LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS)))
	{
		if(dl->dol_Type == DLT_DEVICE && dl->dol_Task == devlist->dl_Task)
			break;
	}

	if(dl)
	{
		STRPTR s = (STRPTR)BADDR(dl->dol_Name);
		ULONG len = *s + 1;
		stccpy(buffer, s+1, len > 32 ? 32 : len);
	}
	else
	{
		strcpy(buffer, ((struct Task *)port->mp_SigTask)->tc_Node.ln_Name);
	}
	strcat(buffer, ":");

	UnLockDosList(LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS | LDF_READ);
}

void getsizestring(STRPTR buf, UQUAD a)
{
	a /= 1024;
	if(a > 1073741824)
		sprintf(buf, "HUGE");
	else if(a > 1048576)
		sprintf(buf, "%.1f G", (double)((double)a / 1048576));
	else if(a > 1024)
		sprintf(buf, "%.1f M", (double)((double)a / 1024));
	else
		sprintf(buf, "%ld K", (long)a);
}
