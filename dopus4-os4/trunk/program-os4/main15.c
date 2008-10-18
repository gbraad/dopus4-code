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

#include "dopus.h"
#include <proto/xadmaster.h>

void ftype_doubleclick(char *path, char *name, int state)
{
	int a, b, dodef = 0;
	int64 size;
	ULONG threelongs[3];
	char buf[256], buf2[256];
	struct dopusfiletype *type;
	struct dopusfuncpar par;
	struct Directory *file;
	APTR dto = NULL;

	strcpy(buf, path);
	IDOS->AddPart(buf, name, 256);
	threelongs[0] = 0;

	if(IDOpus->CheckExist(buf, &size) >= 0)
	{
		doerror(ERROR_OBJECT_NOT_FOUND);
		return;
	}
	if(!size)
	{
		doselinfo(data_active_window);
		return;
	}
	dostatustext(globstring[STR_INTERROGATING_FILES]);
	file = findfile(dopus_curwin[data_active_window], name, NULL);
	busy();
	if((type = checkfiletype(buf, FTFUNC_DOUBLECLICK, 0)))
	{
		par.which = type->which[FTFUNC_DOUBLECLICK];
		par.stack = type->stack[FTFUNC_DOUBLECLICK];
		par.pri = type->pri[FTFUNC_DOUBLECLICK];
		par.delay = type->delay[FTFUNC_DOUBLECLICK];
		par.key = par.qual = 0;
		par.type = 3;

		if((strcmp(type->type, "Default") == 0) || (strcmp(type->type, globstring[STR_FTYPE_DEFAULT]) == 0))
		{
			dodef = 1;
		}
		else
		{
			char title[256];

			if(type->actionstring[FTFUNC_DOUBLECLICK][0])
			{
				do_title_string(type->actionstring[FTFUNC_DOUBLECLICK], title, 0, name);
				dostatustext(title);
			}
			else
			{
				title[0] = 0;
			}
			strcpy(func_single_file, name);
			dofunctionstring(type->function[FTFUNC_DOUBLECLICK], name, title, &par);
			if(file && file->selected)
			{
				unselect(data_active_window, file);
			}
			func_single_file[0] = 0;
			return;
		}
	}

	if(file)
	{
		int old;

		old = file->selected;
		file->selected = 1;
		makereselect(&func_reselection, data_active_window);
		file->selected = old;
	}

	if(IxadMaster)
	{
		BPTR fh;
		int len;
		APTR mem;
		struct xadClient *cl;

		if((fh = IDOS->Open(buf, MODE_OLDFILE)))
		{
			len = ((struct xadMasterBase *)(IxadMaster->Data.LibBase))->xmb_RecogSize;
			mem = IExec->AllocMem(len, MEMF_ANY);
			IDOS->Read(fh, mem, len);
			IDOS->Close(fh);
			cl = IxadMaster->xadRecogFile(len, mem, NULL);
			IExec->FreeMem(mem, len);
			if(cl)
			{
				strcpy(str_pathbuffer[data_active_window], buf);
				startgetdir(data_active_window, SGDFLAGS_CANMOVEEMPTY | SGDFLAGS_CANCHECKBUFS);
				return;
			}
		}
	}
	if(checkexec(buf))
	{
		if(checkisfont(buf, buf2))
		{
			dostatustext(globstring[STR_SHOWING_FONT]);
			if(showfont(buf2, atoi(name), 1))
				okay();
			return;
		}
		dostatustext(globstring[STR_RUNNING_FILE]);
		IDOpus->StrCombine(buf2, buf, ".info", 256);
		if(IDOpus->CheckExist(buf2, NULL) < 0)
		{
			a = 1;
		}
		else
		{
			a = 0;
		}
		if((a = dorun(buf, state, a)) > 0 || a == -2)
		{
			okay();
		}
		return;
	}

	if((dto = IDataTypes->NewDTObject(buf, DTA_GroupID, GID_PICTURE, TAG_END)))
	{
		dostatustext(globstring[STR_SHOWING_FILE]);
		strcpy(func_single_file, name);
		a = LoadPic(buf);
		if(a)
		{
			okay();
		}
		func_single_file[0] = 0;
		IDataTypes->DisposeDTObject(dto);
		return;
	}
	if((dto = IDataTypes->NewDTObject(buf, DTA_GroupID, GID_SOUND, TAG_END)))
	{
		dostatustext(globstring[STR_PLAYING_FILE]);
		strcpy(func_single_file, name);
		a = doplay8svx(buf, (config->viewbits & VIEWBITS_PLAYLOOP) ? 1 : 0);
		if(a)
		{
			okay();
		}
		func_single_file[0] = 0;
		IDataTypes->DisposeDTObject(dto);
		return;
	}

	a = strlen(name);
	if(a > 5 && strcmp(&name[a - 5], ".info") == 0)
	{
		struct Screen *infoscreen = NULL;
		BPTR plock, flock = IDOS->Lock(buf, ACCESS_READ);
		char buffer[108] = { 0, };

		if(MainScreen)
		{
			infoscreen = MainScreen;
		}
		else
		{
			infoscreen = IIntuition->LockPubScreen(NULL);
		}

		strcpy(buffer, buf);
		b = strlen(buffer);
		buffer[b - 5] = '\0';
		plock = IDOS->ParentDir(flock);
		dostatustext(globstring[STR_SHOWING_FILE]);
		if(infoscreen && plock)
		{
			IWorkbench->WBInfo(plock, buffer, infoscreen);
		}
		if(!MainScreen)
		{
			IIntuition->UnlockPubScreen(NULL, infoscreen);
		}
		IDOS->UnLock(plock);
		IDOS->UnLock(flock);
		return;
	}

	if(dodef)
	{
		char title[256];

		if(type->actionstring[FTFUNC_DOUBLECLICK][0])
		{
			do_title_string(type->actionstring[FTFUNC_DOUBLECLICK], title, 0, name);
			dostatustext(title);
		}
		else
		{
			title[0] = 0;
		}

		if(!status_iconified)
		{
			strcpy(func_single_file, name);
		}
		dofunctionstring(type->function[FTFUNC_DOUBLECLICK], name, title, &par);
	}
	else
	{
		dostatustext(globstring[STR_READING_SELECTED_FILE]);
		strcpy(func_single_file, name);
		if(viewfile(buf, str_arcorgname[0] ? str_arcorgname : name, FUNC_SMARTREAD, NULL, NULL, str_arcorgname[0] ? 1 : 0, 1))
			okay();
		func_single_file[0] = 0;
	}

	if(strcmp(str_last_statustext, globstring[STR_INTERROGATING_FILES]) == 0)
	{
		okay();
	}

	return;
}

int filesearch(char *name, int *found, int skipall)
{
	char *message;
	int rec, in, suc;

	*found = 0;
	if(!(in = IDOS->Open(name, MODE_OLDFILE)))
		return (0);
	suc = typesearch(in, str_search_string, search_flags, NULL, 0);
	IDOS->Close(in);
	if(suc > -1)
	{
		char buf[350];

		*found = 1;
		if(skipall > -1)
		{
			sprintf(buf, globstring[STR_FOUND_A_MATCH_READ], name);
			if((rec = simplerequest(buf, str_okaystring, globstring[STR_ABORT], globstring[STR_SKIP], (skipall) ? globstring[STR_SKIP_ALL] : NULL, NULL)) == 1)
			{
				message = IDOS->FilePart(name);
				busy();
				rec = viewfile(name, message, 0, str_search_string, NULL, 1, 0);
				unbusy();
				if(rec == -1)
					return (-1);
				return (2);
			}
			if(rec == 0)
				return (-1);
			if(rec == 3)
				return (4);
		}
		return (3);
	}
	if(status_justabort)
		return (-1);
	return (1);
}

void initclock()
{
	if(!clock_task)
	{
		clockmsg_port = NULL;
		clock_task = (struct Task *)IExec->CreateTask("dopus_clock", config->priority + 1, &clocktask, 8192, NULL);
	}
}

int internal_function(int function, int rexx, char *source, char *dest)
{
	int a, b, actwin = -1, inactwin = -1, flag = 0;
	char buf[256], buf2[256], *spath = NULL, *dpath = NULL;
	struct Directory dummy_entry;
	struct CommandList *command = NULL;

	for(a = 0;; a++)
	{
		if(!commandlist[a].name)
			break;
		if(commandlist[a].function == function)
		{
			command = &commandlist[a];
			break;
		}
	}
	if(status_iconified && command && command->flags & RCL_NOBUTTON)
		return (0);
	status_justabort = status_haveaborted = 0;
	if(function == FUNC_HELP && !dopus_firsthelp)
		return (0);

	if(!rexx && function != FUNC_HELP && function != FUNC_ABOUT)
		busy();

	if(function != FUNC_RESELECT && !(status_flags & STATUS_HELP))
		makereselect(&func_reselection, data_active_window);

	rexx_pathbuffer[0][0] = rexx_pathbuffer[1][0] = 0;
	dos_global_entry.name[0] = 0;

	if(status_flags & STATUS_HELP && function != FUNC_HELP)
	{
		if(function >= DRIVE1 && function <= DRIVE6)
		{
			a = data_drive_offset + (function - DRIVE1);
			if(config->drive[a].name[0])
				dohelp(NULL, "*drivegadgets", config->drive[a].key, config->drive[a].qual, NULL);
		}
		else if(command)
		{
			strcpy(buf, "*");
			strcat(buf, command->name);
			dohelp(NULL, buf, 0, 0, NULL);
		}
	}
	else
	{
		if(function >= DRIVE1 && function <= DRIVE6)
		{
			a = (function - DRIVE1) + data_drive_offset;
			if(config->drive[a].name[0] && config->drive[a].function)
			{
				strcpy(str_pathbuffer[data_active_window], config->drive[a].function);
				startgetdir(data_active_window, SGDFLAGS_CANMOVEEMPTY | SGDFLAGS_CANCHECKBUFS);
			}
		}
		else
			switch (function)
			{
			case FUNC_SETCURDIR:
				setcurdir(rexx);
				break;
			case FUNC_ICONIFYBUTTONS:
				if(rexx)
				{
					a = atoi(rexx_args[0]) + 1;
					b = atoi(rexx_args[1]) - 1;
				}
				else
				{
					a = 1;
					b = 0;
				}
				if(!status_iconified && checkwindowquit())
					iconify(2, a, b);
				break;
			case FUNC_ERRORHELP:
				geterrorhelp(rexx);
				break;
			case FUNC_PRINTDIR:
				dopus_print(rexx, NULL, 1, str_arexx_portname, NULL);
				break;
			case FUNC_NEWCLI:
				newcli((rexx && rexx_args[0][0]) ? rexx_args[0] : NULL);
				break;
			case FUNC_SELECT:
				doselect(rexx);
				break;
			case FUNC_AREXX:
				doarexx(rexx);
				break;
			case FUNC_ASSIGN:
				doassign(rexx);
				break;
			case FUNC_MAKEDIR:
				makedir(rexx);
				break;
			case FUNC_MAKELINK:
				makelink(rexx);
				break;
			case FUNC_RESCAN:
				if(rexx)
					a = atoi(rexx_args[0]);
				else
					a = data_active_window;
				if(a == -1)
					a = data_active_window;
				if(a >= 0 && a <= 1)
				{
					struct DirWindowPars rescanpars;

					rescanpars.reselection_list = NULL;
					makereselect(&rescanpars, a);
					startgetdir(a, 0);
					doreselect(&rescanpars, 0);
					makereselect(&rescanpars, -1);
				}
				break;
			case FUNC_ICONIFY:
				if(!status_iconified && checkwindowquit())
					iconify(2, 0, 0);
				break;
			case FUNC_ABOUT:
				about();
				break;
			case FUNC_VERSION:
				break;
			case FUNC_CONFIGURE:
				doconfig();
				break;
			case FUNC_QUIT:
				if(!(config->generalflags & GENERAL_FORCEQUIT) && (rexx_argcount < 1 || (strcmp(rexx_args[0], "force")) != 0) && !(simplerequest(globstring[STR_REALLY_QUIT], globstring[STR_QUIT], str_cancelstring, NULL)))
				{
					break;
				}

				if(!checkwindowquit())
				{
					break;
				}
				if(config_changed && !(a = simplerequest(globstring[STR_CONFIG_CHANGED_QUIT], globstring[STR_QUIT], str_cancelstring, globstring[STR_SAVE_QUIT], NULL)))
				{
					break;
				}
				if(a == 2)
				{
					dostatustext(globstring[STR_SAVING_CONFIG]);
					busy();
					if(!(savesetup(NULL)))
					{
						doerror(-1);
						unbusy();
						break;
					}
					unbusy();
				}
				quit();
				break;
			case FUNC_SAVECONFIG:
				dostatustext(globstring[STR_SAVING_CONFIG]);
				if(!(savesetup((rexx && rexx_args[0][0]) ? rexx_args[0] : NULL)))
					doerror(-1);
				else
					okay();
				break;
			case FUNC_LASTSAVEDCONFIG:
				if(checklastsaved())
				{
					if(!(checkwindowquit()))
						break;
					shutthingsdown(0);
					read_configuration(0);
					SetUp(0);
					okay();
				}
				break;
			case FUNC_DEFAULTCONFIG:
				a = checkdefaults();
				if(a == 0)
					break;
				if(a == 1)
				{
					dostatustext(globstring[STR_SAVING_CONFIG]);
					if(!(savesetup(NULL)))
					{
						doerror(-1);
						break;
					}
				}
				unbusy();
				if(!(checkwindowquit()))
					break;
				shutthingsdown(0);
				read_configuration(1);
				SetUp(0);
				okay();
				break;
			case FUNC_REDRAW:
				if(!(checkwindowquit()))
					break;
				shutthingsdown(0);
				SetUp(0);
				okay();
				break;
			case FUNC_ALL:
				globalselect(data_active_window, 1);
				refreshwindow(data_active_window, 0);
				break;
			case FUNC_NONE:
				globalselect(data_active_window, 0);
				globalselect(data_active_window, 0);
				refreshwindow(data_active_window, 0);
				break;
			case FUNC_TOGGLE:
				globaltoggle(data_active_window);
				refreshwindow(data_active_window, 0);
				break;
			case FUNC_PARENT:
				if(!rexx || !rexx_argcount > 0 || (a = atoi(rexx_args[0])) < 0 || a > 1)
					a = data_active_window;
				if(dopus_curwin[a]->firstentry && (dopus_curwin[a]->firstentry->type == ENTRY_CUSTOM) && (dopus_curwin[a]->firstentry->subtype == CUSTOMENTRY_DIRTREE))
				{
					advancebuf(a, -1);
					refreshwindow(a, 3);
					break;
				}
				if(doparent(str_pathbuffer[a]))
					do_parent_root(a);
				else
					dodevicelist(a);
				break;
			case FUNC_ROOT:
				if(!rexx || rexx_argcount < 1 || (a = atoi(rexx_args[0])) < 0 || a > 1)
					a = data_active_window;
				if(doroot(str_pathbuffer[a]))
					do_parent_root(a);
				break;
			case FUNC_PARENTLIST:
				if(!rexx || !rexx_argcount > 0 || (a = atoi(rexx_args[0])) < 0 || a > 1)
					a = data_active_window;
				if(str_pathbuffer[a][0] && do_parent_multi(str_pathbuffer[a]))
					startgetdir(a, SGDFLAGS_CANMOVEEMPTY | SGDFLAGS_CANCHECKBUFS);
				break;
			case FUNC_HELP:
				status_flags ^= STATUS_HELP;
				if((status_flags & STATUS_HELP && !(screen_gadgets[SCRGAD_TINYHELP].Flags & GFLG_SELECTED)) || (!(status_flags & STATUS_HELP) && screen_gadgets[SCRGAD_TINYHELP].Flags & GFLG_SELECTED))
				{
					screen_gadgets[SCRGAD_TINYHELP].Flags ^= GFLG_SELECTED;
					if(config->generalscreenflags & SCR_GENERAL_TINYGADS)
						dolittlegads(&screen_gadgets[SCRGAD_TINYHELP], &globstring[STR_TINY_BUTTONS][4], 1);
				}
				if(status_flags & STATUS_HELP)
					dostatustext(globstring[STR_SELECT_FUNCTION]);
				else
					okay();
				break;
			case FUNC_RESELECT:
				doreselect(&func_reselection, 1);
				break;
			case FUNC_BUFFERLIST:
				if(!rexx || rexx_argcount < 1 || (a = atoi(rexx_args[0])) < 0 || a > 1)
					a = data_active_window;
				dolistbuffers(a);
				break;
			case FUNC_CLEARBUFFERS:
				clearbuffers();
				break;
			case FUNC_REMEMBER:
				do_remember_config(remember_data);
				break;
			case FUNC_RESTORE:
				do_restore_config(remember_data);
				break;
			case FUNC_BEEP:
				dosound(0);
				break;
			case FUNC_ALARM:
				dosound(1);
				break;
			default:
				if(!command)
					return (0);
				if(func_external_file[0])
				{
					spath = source;
					dpath = dest;
					if(!status_iconified)
					{
						actwin = 0;
						inactwin = data_active_window;
					}
				}
				else if(!status_iconified)
				{
					actwin = ((spath = getarexxpath(rexx, data_active_window, 0, 0)) != str_pathbuffer[data_active_window]) ? -1 : data_active_window;
					inactwin = ((dpath = getarexxpath(rexx, 1 - data_active_window, 1, (actwin != -1))) != str_pathbuffer[1 - data_active_window]) ? -1 : 1 - data_active_window;
				}
				if(status_iconified)
				{
					if(rexx && rexx_argcount > 0 && (IDOpus->CheckExist(rexx_args[0], NULL)))
					{
						char *ptr;

						strcpy(func_external_file, rexx_args[0]);
						strcpy(buf2, func_external_file);
						if((ptr = IDOS->FilePart(buf2)))
						{
							strcpy(func_single_file, ptr);
							*ptr = 0;
						}
						spath = buf2;
						if(!(filloutdummy(func_external_file, &dummy_entry)))
							return (0);
						func_single_entry = &dummy_entry;
					}
					else if(!func_external_file[0])
					{
						struct DOpusFileReq freq;

						buf2[0] = 0;
						if(command->flags & RCL_NOFILE)
						{
							if(!(getdummypath(buf2, STR_SELECT_A_DIRECTORY)))
								return (0);
							spath = buf2;
						}
						else
						{
							if(!(getdummyfile(&dummy_entry, buf2, &freq)))
								return (0);
							IDOpus->TackOn(buf2, NULL, 256);
							spath = buf2;
							func_single_entry = &dummy_entry;
							strcpy(func_external_file, buf2);
							strcpy(func_single_file, dummy_entry.name);
						}
					}
					else
					{
						strcpy(buf2, source);
						IDOS->AddPart(buf2, func_single_file, 256);
						if(!(filloutdummy(buf2, &dummy_entry)))
							return (0);
						func_single_entry = &dummy_entry;
					}
					if(command->flags & RCL_NEEDDEST && (!dpath || !dpath[0]))
					{
						buf[0] = 0;
						if(!(getdummypath(buf, STR_SELECT_DESTINATION_DIR)))
							return (0);
						dpath = buf;
					}
				}
				switch (function)
				{
				case FUNC_RELABEL:
					if(dopus_curwin[actwin] == dopus_specialwin[actwin])
					{
						struct Directory *entry = dopus_curwin[actwin]->firstentry;

						for(; entry; entry = entry->next)
						{
							if(entry->selected)
								if(entry->type == ENTRY_DEVICE)
									if(!strcmp(entry->comment, "<VOL>"))
									{
										relabel_disk(0, entry->name);
										unselect(actwin, entry);
									}
						}
						dodevicelist(actwin);
					}
					else
						relabel_disk(rexx, spath);
					break;
				case FUNC_SEARCH:
					dofilefunction(FUNC_SEARCH, FUNCFLAGS_ANYTHING, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_PROTECT:
					dofilefunction(FUNC_PROTECT, 0, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_PRINT:
					dofilefunction(FUNC_PRINT, FUNCFLAGS_FILES, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_HUNT:
					dofilefunction(FUNC_HUNT, FUNCFLAGS_DIRS | FUNCFLAGS_DEVS, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_ENCRYPT:
					dofilefunction(FUNC_ENCRYPT, FUNCFLAGS_FILES, spath, dpath, actwin, inactwin, rexx);
					break;
				case FUNC_EXTRACT:
					dofilefunction(function, FUNCFLAGS_FILES, spath, dpath, actwin, inactwin, rexx);
					break;
				case FUNC_DATESTAMP:
					dofilefunction(FUNC_DATESTAMP, 0, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_COMMENT:
					dofilefunction(FUNC_COMMENT, 0, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_ADDICON:
					dofilefunction(FUNC_ADDICON, 0, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_DELETE:
					dofilefunction(FUNC_DELETE, dopus_curwin[actwin] == dopus_specialwin[actwin] ? FUNCFLAGS_DEVS : 0, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_CLONE:
					dofilefunction(FUNC_COPYAS, FUNCFLAGS_COPYISCLONE, spath, spath, actwin, actwin, rexx);
					break;
				case FUNC_COPY:
				case FUNC_COPYAS:
					dofilefunction(function, 0, spath, dpath, actwin, inactwin, rexx);
					break;
				case FUNC_RENAME:
					dofilefunction(FUNC_RENAME, 0, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_MOVE:
				case FUNC_MOVEAS:
					dofilefunction(function, 0, spath, dpath, actwin, inactwin, rexx);
					break;
				case FUNC_RUN:
					dofilefunction(function, 0, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_CHECKFIT:
					flag = FUNCFLAGS_BYTEISCHECKFIT;
				case FUNC_BYTE:
					if(!rexx || rexx_argcount < 1 || (a = atoi(rexx_args[0])) < 0 || a > 1)
						a = actwin;
					if(a != actwin)
					{
						char *temp;

						temp = spath;
						spath = dpath;
						dpath = temp;
					}
					dofilefunction(FUNC_BYTE, flag, spath, (function == FUNC_CHECKFIT) ? dpath : NULL, a, 1 - a, rexx);
					break;
				case FUNC_UNBYTE:
					if(!rexx || rexx_argcount < 1 || (a = atoi(rexx_args[0])) < 0 || a > 1)
						a = data_active_window;
					unbyte(a);
					break;
				case FUNC_PLAY:
				case FUNC_PLAY8SVX:
				case FUNC_LOOPPLAY:
				case FUNC_LOOPPLAY8SVX:
				case FUNC_PLAYST:
					dofilefunction(function, FUNCFLAGS_FILES, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_SHOW:
					dofilefunction(FUNC_SHOW, FUNCFLAGS_FILES, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_READ:
				case FUNC_HEXREAD:
				case FUNC_ANSIREAD:
				case FUNC_SMARTREAD:
					dofilefunction(function, FUNCFLAGS_FILES, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_AUTO:
				case FUNC_AUTO2:
				case FUNC_AUTO3:
				case FUNC_AUTO4:
					dofilefunction(function, FUNCFLAGS_FILES, spath, NULL, actwin, -1, rexx);
					break;
				case FUNC_ICONINFO:
					dofilefunction(FUNC_ICONINFO, 0, spath, NULL, actwin, -1, rexx);
					break;
				default:
					func_global_function = function;
					rexxdisp(NULL, NULL, NULL);
					break;
				}
				break;
			}
	}
	func_global_function = 0;
	func_single_entry = NULL;
	func_single_file[0] = 0;
	func_external_file[0] = 0;

	if(!rexx)
		unbusy();
	a = status_previousabort = status_justabort;
	status_justabort = 0;
	return (a);
}

void do_parent_root(int win)
{
	if(!(config->dirflags & DIRFLAGS_SMARTPARENT) || !(findbuffer(str_pathbuffer[win], win, 0, 0)))
		startgetdir(win, SGDFLAGS_CANMOVEEMPTY);
}

int checklastsaved()
{
	if(config_changed && !(simplerequest(globstring[STR_CONFIG_CHANGED_LASTSAVED], globstring[STR_LOAD], str_cancelstring, NULL)))
		return (0);
	return (1);
}

int checkdefaults()
{
	if(!config_changed)
		return (2);
	return ((simplerequest(globstring[STR_CONFIG_CHANGED_DEFAULTS], globstring[STR_SAVE], str_cancelstring, globstring[STR_DEFAULTS], NULL)));
}

int checknewconfig()
{
	if(config_changed && !(simplerequest(globstring[STR_CONFIG_CHANGED_LOAD], globstring[STR_LOAD], str_cancelstring, NULL)))
		return (0);
	return (1);
}

void dragcompgad(int gad, int state)
{
	if(main_gadgets[gad].GadgetRender)
	{
		if(state)
		{
			IIntuition->DrawBorder(main_rp, main_gadgets[gad].SelectRender, main_gadgets[gad].LeftEdge, main_gadgets[gad].TopEdge);
		}
		else
		{
			IIntuition->DrawBorder(main_rp, main_gadgets[gad].GadgetRender, main_gadgets[gad].LeftEdge, main_gadgets[gad].TopEdge);
		}
	}
	else
	{
		IGraphics->SetDrMd(main_rp, COMPLEMENT);
		rectfill(main_rp, scrdata_gadget_xpos + ((gad % 7) * scrdata_gadget_width), scrdata_gadget_ypos + ((gad / 7) * scrdata_gadget_height) - 1, scrdata_gadget_width, scrdata_gadget_height);
		IGraphics->SetDrMd(main_rp, JAM2);
	}
}
