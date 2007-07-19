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
#include "view.h"

int dofilefunction(int function, int flags, char *sourcedir, char *destdir, int act, int inact, int rexx)
{
	struct InfoData *infodata = NULL;
	struct FileInfoBlock *fileinfo = IDOS->AllocDosObject(DOS_FIB, NULL);
	int a = 0, b = 0, special = 0, candoicon = 1, old = 0, specflags = 0, noshow = 0, err = 0;
	int sourcewild = 0, destwild = 0, firstset = 0, breakout = 0, rexarg = 0, protstuff[2];
	int pt = 1, okayflag = 0, show = 0, lastfile = 0, flag = 0, exist = 0, count = 0, data = 0, mask = 0, temp = 0;
	int globflag, noremove, doicons = 0, value = 0, progtype = 0, blocksize = 0, retval = 0;
	int32 total = -1;
	int64 byte, bb;
	struct Directory *file = NULL, *tempfile, *nextfile, filebuf, dummyfile;
	char *sourcename, *destname, *oldiconname, *newiconname;
	char *buf, *buf1, *buf2, *namebuf, *srename, *drename, *database;
	static char tbuf[256], titlebuf[32];
	struct DateTime datetime;
	void *function_memory_pool;
	struct dopusfiletype *type;
	struct dopusfuncpar par;
	struct DirectoryWindow *swindow, *dwindow;
	struct DOpusArgsList arglist;
	struct ViewData *viewdata = NULL;
	BPTR filelock;
	static int entry_depth;
	char progress_copy = 0, prog_indicator = 0;
	BOOL arcfile;

	if(act > -1)
		swindow = dopus_curwin[act];
	else
		swindow = NULL;
	if(inact > -1)
		dwindow = dopus_curwin[inact];
	else
		dwindow = NULL;

	if(destdir && (dwindow->flags & DWF_ARCHIVE))
	{
		dostatustext(globstring[STR_OPERATION_NOT_SUPPORTED]);
		IDOS->FreeDosObject(DOS_FIB, fileinfo);
		return 0;
	}

	data = rexarg = 0;
	function_memory_pool = IExec->AllocSysObjectTags(ASOT_MEMPOOL, ASOPOOL_MFlags, MEMF_CLEAR, ASOPOOL_Puddle, 16384, ASOPOOL_Threshold, 4096, TAG_DONE);
	specflags = flags & ~255;
	flags &= 255;

	if(config->iconflags & ICONFLAG_DOUNTOICONS || func_external_file[0])
		doicons = 1;

	/* Find the first file to work upon; if only one file specified, use that */

	if(status_flags & STATUS_GLOBALFILE)	/* kludge */
		flags = FUNCFLAGS_FILES;

	if(func_single_entry)
	{
		file = func_single_entry;
		globflag = 1;
	}
	else if(swindow)
	{
		globflag = 0;
		if(flags & FUNCFLAGS_DIRS)
		{
			file = checkdirtot(swindow);	/* First dir */
			total = swindow->dirsel;
			progtype = 1;
		}
		if(flags & FUNCFLAGS_DEVS && !file)
		{
			file = checkdevtot(swindow);	/*    or dev */
			total = swindow->dirsel;
			progtype = 1;
		}
		if(flags & FUNCFLAGS_FILES)
		{
			file = checktot(swindow);	/* First file */
			total = swindow->filesel;
			progtype = 0;
		}
		if(flags == 0 || flags & FUNCFLAGS_ANYTHING)
		{
			file = checkalltot(swindow);
			total = swindow->filesel + swindow->dirsel;	/* Anything   */
			progtype = 2;
		}
		if(!file && flags & FUNCFLAGS_ANYTHING)
		{
			file = checkdevtot(swindow);
			total = swindow->dirsel;
			progtype = 1;
		}

		/* Name may be in ARexx */

		if(rexx && (tempfile = findfile(swindow, rexx_args[0], NULL)))
		{
			++rexarg;
			file = tempfile;
			func_single_entry = file;
			strcpy(func_single_file, file->name);
			globflag = 1;
		}
	}

	if(!file)
	{
		IDOS->FreeDosObject(DOS_FIB, fileinfo);
		return (0);	/* No files selected, return */
	}

	if(!(database = IExec->AllocPooled(function_memory_pool, 3000)))
	{
		IDOS->FreeDosObject(DOS_FIB, fileinfo);
		return (0);
	}

	sourcename = database;
	destname = database + 300;
	oldiconname = database + 600;
	newiconname = database + 900;
	buf = database + 1200;
	buf1 = database + 1500;
	buf2 = database + 1800;
	namebuf = database + 2100;
	srename = database + 2400;
	drename = database + 2700;

	if(swindow)
		scrdata_old_offset_store = swindow->offset;	/* Store directory offset for auto-scroll */
	else
		scrdata_old_offset_store = -1;

	status_justabort = count = 0;
	namebuf[0] = 0;
	autoskip = 0;

	/* Bump entry depth to check for recursion */

	++entry_depth;

	/* Do initial setting up functions */

	switch (function)
	{
	case FUNC_READ:
	case FUNC_HEXREAD:
	case FUNC_ANSIREAD:
	case FUNC_SMARTREAD:
		if(!globflag && !(viewdata = IExec->AllocPooled(function_memory_pool, sizeof(struct ViewData))))
			goto endfunction;
	case FUNC_LOOPPLAY:
	case FUNC_SHOW:
		total = -1;
	case FUNC_PLAY:
		candoicon = 0;
		break;
	case FUNC_DELETE:
		askeach = 1;
		if(config->deleteflags & DELETE_ASK && !globflag)
		{
			/* Ask for confirmation before commencing delete */
			displaydirgiven(act, file, 0);
			if(!(a = simplerequest(globstring[STR_REALLY_DELETE], globstring[STR_DELETE], str_cancelstring, globstring[STR_ALL], NULL)))
			{
				endfollow(act);
				myabort();
				goto endfunction;
			}
			if(a == 2)
			{
				askeach = 0;
			}
		}
		glob_unprotect_all = 0;
		break;
	case FUNC_RENAME:
		if(rexx)
		{
			if(rexx_argcount < 2)
			{
				goto endfunction;
			}
			strcpy(srename, rexx_args[0]);
			strcpy(drename, rexx_args[1]);
			if(strchr(srename, '*'))
			{
				sourcewild = 1;
			}
			if(strchr(drename, '*'))
			{
				destwild = 1;
			}
			else
			{
				strcpy(namebuf, drename);
				firstset = 1;
			}
		}
		else
			while(file)
			{
				if(file->selected)
				{
					strcpy(srename, file->name);
					strcpy(drename, file->name);
					displaydirgiven(act, file, 0);
					if(!(a = getrenamedata(srename, drename)) || !srename[0] || !drename[0])
					{
						endfollow(act);
						myabort();
						goto endfunction;
					}
					if(a == 1)
					{
						if(strchr(srename, '*'))
						{
							sourcewild = 1;
						}
						if(strchr(drename, '*'))
						{
							destwild = 1;
						}
						else
						{
							strcpy(namebuf, drename);
							firstset = 1;
							total = -1;
						}
						break;
					}
				}
				++value;
				file = file->next;
			}
		if(!file)
		{
			endfollow(act);
			goto endfunction;
		}
		askeach = 1;
		break;
	case FUNC_MOVE:
		if(destdir[0] && (checksame(sourcedir, destdir, 1) == LOCK_SAME))
			goto endfunction;
	case FUNC_MOVEAS:
		if(!(checkdest(inact)))
			goto endfunction;
		if(!(config->existflags & REPLACE_ALWAYS))
			askeach = 1;
		else
			askeach = 0;
		if(checksame(sourcedir, destdir, 2) != LOCK_SAME_VOLUME)
			progress_copy = 1;
		break;
	case FUNC_COPY:
		if(destdir[0] && (checksame(sourcedir, destdir, 1) == LOCK_SAME))
			goto endfunction;
//        case FUNC_CLONE:
	case FUNC_COPYAS:
		if(!(checkdest(inact)))
			goto endfunction;
		askeach = (config->existflags & REPLACE_ALWAYS) ? 0 : 1;
		progress_copy = 1;
		if(!(specflags & FUNCFLAGS_COPYISCLONE) && config->copyflags & COPY_CHECK)
		{
			old = scrdata_old_offset_store;
			if(!globflag)
			{
				a = 0;
				tempfile = file;
				while(tempfile)
				{
					if(tempfile->selected && tempfile->type >= ENTRY_DIRECTORY && tempfile->size == -1)
					{
						a = 1;
						break;
					}
					tempfile = tempfile->next;
				}
				if(!a || simplerequest(globstring[STR_SIZES_NOT_KNOWN], globstring[STR_YES], globstring[STR_NO], NULL))
				{
					if(status_justabort || (!(dofilefunction(FUNC_BYTE, FUNCFLAGS_BYTEISCHECKFIT, sourcedir, destdir, act, inact, 0)) && !(simplerequest(globstring[STR_ENTRIES_MAY_NOT_FIT], globstring[STR_CONTINUE], str_cancelstring, NULL))))
					{
						myabort();
						goto endfunction;
					}
					total += dos_global_files;
				}
			}
			scrdata_old_offset_store = old;
		}
		break;
	case FUNC_HUNT:
		if(rexx && rexx_argcount > 0)
		{
			strcpy(str_hunt_name, rexx_args[rexarg]);
		}
		else if(!(whatsit(globstring[STR_ENTER_HUNT_PATTERN], 80, str_hunt_name, NULL)) || !str_hunt_name[0])
		{
			myabort();
			goto endfunction;
		}
		IDOS->ParsePattern(str_hunt_name, str_hunt_name_parsed, 160);
		IExec->CopyMem(str_hunt_name_parsed, buf2, 170);
		candoicon = 0;
		break;
        case FUNC_PRINT:
		if(globflag)
		{
			IDOpus->StrCombine(sourcename, sourcedir, file->name, 256);
			arglist.single_file = sourcename;
			arglist.file_list = NULL;
			arglist.last_select = NULL;
		}
		else
		{
			arglist.single_file = NULL;
			arglist.file_window = act;
			arglist.file_list = (APTR)file;
		}
		dopus_print(rexx, &arglist, 0, str_arexx_portname, NULL);
		goto endfunction;
		break;
	case FUNC_COMMENT:
		candoicon = 0;
	case FUNC_DATESTAMP:
	case FUNC_PROTECT:
		askeach = 1;
		break;
	case FUNC_ENCRYPT:
		if(!(checkdest(inact)) || (checksame(sourcedir, destdir, 1) == LOCK_SAME))
		{
			goto endfunction;
		}
		if(!(config->existflags & REPLACE_ALWAYS))
		{
			askeach = 1;
		}
		else
		{
			askeach = 0;
		}
		if(rexx && rexx_argcount > 0)
		{
			strcpy(buf2, rexx_args[rexarg]);
		}
		else if(!(whatsit(globstring[STR_ENTER_PASSWORD], 20, buf2, NULL)) || !buf2[0])
		{
			myabort();
			goto endfunction;
		}
		if(buf2[0] == '-')
		{
			data = 0;
			strcpy(buf2, &buf2[1]);
		}
		else
		{
			data = 1;
		}
		candoicon = 0;
		progress_copy = 1;
		break;
	case FUNC_EXTRACT:
		dostatustext(globstring[STR_EXTRACT_ARCHIVE]);
		xadflags = 7;
		if(xadflags & XAD_WRITEOVER)
		{
			xadoverwrite = 1;
		}
		else
		{
			xadoverwrite = 0;
		}
		if(xadflags & XAD_NOABS)
		{
			xadnoabs = 1;
		}
		else
		{
			xadnoabs = 0;
		}
		if(xadflags & XAD_MAKETREE)
		{
			xadmaketree = 1;
		}
		else
		{
			xadmaketree = 0;
		}
		progress_copy = 1;
		break;
	case FUNC_SEARCH:
		if(rexx && rexx_argcount > 0)
		{
			strcpy(str_search_string, rexx_args[rexarg]);
		}
		else
		{
			if(!(get_search_data(str_search_string, &search_flags, Window, scr_font[FONT_REQUEST])))
			{
				myabort();
				goto endfunction;
			}
		}
		candoicon = 0;
		askeach = 1;
		break;
	case FUNC_RUN:
		candoicon = 0;
		break;
	case FUNC_ADDICON:
		askeach = 1;
		candoicon = 0;
		break;
	case FUNC_ICONINFO:
		candoicon = 0;
		total = -1;
		break;
	case FUNC_AUTO:
	case FUNC_AUTO2:
	case FUNC_AUTO3:
	case FUNC_AUTO4:
		dostatustext(globstring[STR_INTERROGATING_FILES]);
		candoicon = 0;
		total = -1;
		break;
	case FUNC_BYTE:
		candoicon = 0;
		special = 2;
		count = 1;
		IDOS->SetProcWindow((APTR)-1L);
		infodata = IDOS->AllocDosObject(DOS_INFODATA, NULL);
		if(infodata && destdir && (filelock = IDOS->Lock(destdir, ACCESS_READ)))
		{
			IDOS->Info(filelock, infodata);
			IDOS->UnLock(filelock);
			blocksize = infodata->id_BytesPerBlock;
		}
		else
		{
			blocksize = 512;
		}
		if(config->errorflags & ERROR_ENABLE_DOS)
		{
			IDOS->SetProcWindow(Window);
		}
		total = -1;
		dos_global_files = 0;
		IDOS->FreeDosObject(DOS_INFODATA, infodata);
		break;
	}

	if(!(config->dynamicflags & UPDATE_PROGRESSIND_COPY))
		progress_copy = 0;

	endnotifies();

	if(config->dynamicflags & UPDATE_PROGRESSINDICATOR && (progress_copy || total > 1))
	{
		char *title = NULL;
		int x;

		for(x = 0;; x++)
		{
			if(!commandlist[x].name)
			{
				break;
			}
			if(commandlist[x].function == function && !(commandlist[x].flags & RCL_SYNONYM))
			{
				title = commandlist[x].name;
				break;
			}
		}

		if(title)
		{
			switch (function)
			{
			case FUNC_SEARCH:
			case FUNC_HUNT:
				IDOpus->StrCombine(titlebuf, title, ": ", 32);
				IDOpus->StrConcat(titlebuf, (function == FUNC_SEARCH) ? str_search_string : str_hunt_name, 32);
				break;
			default:
				IDOpus->StrCombine(titlebuf, title, "...", 32);
			}
		}
		else
		{
			strcpy(titlebuf, "Directory Opus");
		}

		if(total == -1)
		{
			total = 1;
		}

		dotaskmsg(hotkeymsg_port, PROGRESS_OPEN, (total > 1) ? value : 1, (total > 1) ? total : 1, titlebuf, progress_copy | ((swindow->dirsel && (!dos_global_files)) ? 0x80 : 0x00));
		prog_indicator = 1;
	}

	while(file)
	{
		if(status_haveaborted)
		{
			myabort();
			break;
		}
		nextfile = file->next;

		if((!file->selected && !globflag) || ((file->type <= ENTRY_FILE) && str_filter_parsed[0] && (IDOS->MatchPatternNoCase(str_filter_parsed, file->name) == FALSE)))
		{
			file = nextfile;
			continue;
		}

		if(doicons && candoicon && !(isicon(file->name)))
		{
			IDOpus->StrCombine(oldiconname, file->name, ".info", 256);
			IDOpus->StrCombine(buf1, sourcedir, oldiconname, 256);
			if(!(IDOpus->CheckExist(buf1, NULL)))
				oldiconname[0] = 0;
		}
		else
		{
			oldiconname[0] = 0;
		}

		if(prog_indicator)
		{
			if((progtype == 1 && file->type >= ENTRY_DEVICE) || (progtype == 0 && file->type <= ENTRY_FILE) || (progtype == 2))
				++value;
			dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, value, total, NULL, 0);
			if(progress_copy)
				dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, -2, 0, file->name, 1);
		}

		lastfile = flag = breakout = 0;

		arcfile = FALSE;
	      functionloop:
		IDOpus->StrCombine(sourcename, sourcedir, file->name, 256);
		if(!special || count == 0 || (special == 2 && file->type >= 0))
		{
			dofilename(sourcename);
			displaydirgiven(act, file, 0);
		}
		if(!firstset && !lastfile && !namebuf[0])
			strcpy(namebuf, file->name);
		else if(lastfile)
			strcpy(namebuf, IDOS->FilePart(newiconname));
		okayflag = a = 0;
		show = -1;

		if(func_external_file[0] || file == &dummyfile)
			noremove = 1;
		else
			noremove = 0;

		switch (function)
		{
		case FUNC_AUTO:
		case FUNC_AUTO2:
		case FUNC_AUTO3:
		case FUNC_AUTO4:
			if(function == FUNC_AUTO)
				a = FTFUNC_AUTOFUNC1;
			else if(function == FUNC_AUTO2)
				a = FTFUNC_AUTOFUNC2;
			else if(function == FUNC_AUTO3)
				a = FTFUNC_AUTOFUNC3;
			else if(function == FUNC_AUTO4)
				a = FTFUNC_AUTOFUNC4;
			okayflag = 1;
			if(file->type <= ENTRY_FILE)
			{
				if((type = checkfiletype(sourcename, a, 0)))
				{
					char title[256];

					par.which = type->which[a];
					par.stack = type->stack[a];
					par.key = par.qual = 0;
					par.pri = type->pri[a];
					par.delay = type->delay[a];
					status_previousabort = 0;
					if(status_iconified && status_flags & STATUS_ISINBUTTONS)
						strcpy(func_external_file, sourcename);
					if(type->actionstring[a][0])
					{
						do_title_string(type->actionstring[a], title, 0, file->name);
						dostatustext(title);
					}
					else
						title[0] = 0;
					dofunctionstring(type->function[a], file->name, title, &par);
					if(status_previousabort)
						status_haveaborted = 1;
				}
				else
				{
					sprintf(buf, globstring[STR_NOT_IDENTIFIED], file->name);
					dostatustext(buf);
				}
			}
			break;

		case FUNC_BYTE:
			if(file->type >= ENTRY_DIRECTORY && (file->size < 0 || (specflags & FUNCFLAGS_BYTEISCHECKFIT && (file->userdata == 0 || file->userdata2 != blocksize))))
			{
				bb = dos_global_files;
				if((a = recursedir(sourcename, NULL, R_GETBYTES, blocksize)) == -10)
				{
					myabort();
					break;
				}
				dos_global_files += bb;
				file->userdata = dos_global_blocksneeded;
				file->userdata2 = blocksize;
				setdirsize(file, dos_global_bytecount, act);
				refreshwindow(act, 0);
			}
			if(file->type >= ENTRY_DIRECTORY)
				data += file->userdata + 1;
			else
			{
				a = (file->size + (blocksize - 1)) / blocksize;
				data += a + (a / 72) + 1;
			}
			if(doicons && !(isicon(file->name)))
			{
				IDOpus->StrCombine(oldiconname, file->name, ".info", 256);
				bb = -1;
				if((file = findfile(swindow, oldiconname, NULL)))
				{
					if(!file->selected)
						bb = file->size;
				}
				else
				{
					IDOpus->StrCombine(tbuf, sourcedir, oldiconname, 256);
					if(lockandexamine(tbuf, fileinfo))
						bb = fileinfo->fib_Size;
				}
				if(bb > -1)
				{
					a = (bb + (blocksize - 1)) / blocksize;
					data += a + (a / 72) + 1;
				}
				oldiconname[0] = 0;
			}
			file = NULL;
			okayflag = 1;
			break;

		case FUNC_RUN:
			if(file->type <= ENTRY_FILE)
			{
				if(checkexec(sourcename))
				{
					if((a = dorun(sourcename, 1, 0)) == -1)
						break;
					else if(a && a != -2)
						okayflag = 1;
				}
				else if(file->protection & FIBF_SCRIPT)
				{
					struct dopusfuncpar par;

					sprintf(buf, "Execute \"%s\"", sourcename);
					defaultpar(&par);
					dofunctionstring(buf, NULL, NULL, &par);
					okayflag = 1;
				}
			}
			break;

		case FUNC_DELETE:
			if(file->type >= ENTRY_DIRECTORY)
			{
				if((a = delfile(sourcename, file->name, globstring[STR_DELETING], glob_unprotect_all, 0)) == -1)
				{
					myabort();
					break;
				}
				else if(a > 0)
				{
					if(a == 2)
						glob_unprotect_all = 1;
					if(!noremove)
						removefile(file, swindow, act, 1);
					file = NULL;
					okayflag = 1;
				}
				else if(a < 0)
				{
					if((a = IDOS->IoErr()) == ERROR_DELETE_PROTECTED && config->deleteflags & DELETE_SET)
					{
						if(!flag)
						{
							flag = 1;
							IDOS->SetProtection(sourcename, 0);
							goto functionloop;
						}
					}
					else if(a == ERROR_OBJECT_NOT_FOUND)
					{
						if(!noremove)
							removefile(file, swindow, act, 1);
						file = NULL;
						okayflag = 1;
					}
					else if(a == ERROR_DIRECTORY_NOT_EMPTY)
					{
						if(config->deleteflags & DELETE_DIRS && askeach)
						{
							sprintf(buf2, globstring[STR_NOT_EMPTY], file->name);
							if(!(a = simplerequest(buf2, globstring[STR_DELETE], globstring[STR_CANCEL], globstring[STR_ALL], globstring[STR_SKIP], NULL)))
							{
								myabort();
								break;
							}
							if(a == 2)
							{
								askeach = 0;
							}
							else if(a == 3)
							{
								okayflag = 0;
								break;
							}
						}
						IDOpus->TackOn(sourcename, NULL, 256);
						recursive_delete(sourcename, NULL, R_DELETE, 0);
						a = recursedir(sourcename, NULL, R_DELETE, 0);
						sourcename[strlen(sourcename) - 1] = 0;
						if(!a)
						{
							if((a = delfile(sourcename, file->name, globstring[STR_DELETING], glob_unprotect_all, 1)) == -1)
							{
								myabort();
								break;
							}
							else if(a == 1 || a == 2)
							{
								if(a == 2)
									glob_unprotect_all = 1;
								if(!noremove)
									removefile(file, swindow, act, 1);
								file = NULL;
								okayflag = 1;
							}
							else
							{
								setdirsize(file, dos_global_bytecount - dos_global_deletedbytes, act);
								okayflag = 1;
							}
						}
						else
						{
							setdirsize(file, file->size - dos_global_deletedbytes, act);
							refreshwindow(act, 0);
						}
					}
					else
					{
						doerror((a = IDOS->IoErr()));
						if((a = checkerror(globstring[STR_DELETING], file->name, a)) == 3)
						{
							myabort();
							break;
						}
						if(a == 1)
							goto functionloop;
					}
				}
				show = act;
				break;
			}
			if(file->type == ENTRY_DEVICE)
			{
				if((!strcmp(file->comment, "<DEV>")) || (!strcmp(file->comment, "<VOL>")))
					break;
			}
			if(config->deleteflags & DELETE_FILES && askeach && !lastfile)
			{
				sprintf(buf2, file->type == ENTRY_DEVICE ? globstring[STR_QUERY_REMOVE_ASSIGN] : globstring[STR_WISH_TO_DELETE], file->name);	// HUX
				a = simplerequest(buf2, globstring[file->type == ENTRY_DEVICE ? STR_REMOVE : STR_DELETE], globstring[STR_CANCEL], globstring[STR_ALL], globstring[STR_SKIP], NULL);
				if(a == 3)
				{
					okayflag = 1;
					break;
				}
				else if(a == 2)
					askeach = 0;
				else if(a == 0)
				{
					myabort();
					break;
				}
			}
			if(file->type == ENTRY_DEVICE)	/* remove assign */
			{
				char buf[FILEBUF_SIZE], *c;

				strcpy(buf, file->name);
				c = strchr(buf, ':');
				if(c)
					*c = 0;
				a = IDOS->AssignLock(buf, 0)/*NULL)*/ ? 1 : 0;
			}
			else if((a = delfile(sourcename, file->name, globstring[STR_DELETING], glob_unprotect_all, 1)) == -1)
			{
				myabort();
				break;
			}
			if(a)
			{
				if(a == 2)
					glob_unprotect_all = 1;
				if(!noremove)
					removefile(file, swindow, act, 1);
				file = NULL;
				show = act;
				okayflag = 1;
			}
			break;

		case FUNC_RENAME:
			if(firstset || lastfile)
			{
				a = 1;
			}
			else if(!destwild)
			{
				if(!(a = whatsit(globstring[STR_ENTER_NEW_NAME], FILEBUF_SIZE - 2, namebuf, globstring[STR_SKIP])))
				{
					myabort();
					break;
				}
				if(a == 2)
				{
					file = NULL;
					okayflag = 1;
					oldiconname[0] = 0;
					break;
				}
			}
			else if(!sourcewild)
			{
				a = getwildrename("*", drename, file->name, namebuf);
			}
			else if(sourcewild)
			{
				a = getwildrename(srename, drename, file->name, namebuf);
			}
			firstset = 0;
			if(a)
			{
				if(lastfile)
				{
					IDOpus->StrCombine(destname, sourcedir, newiconname, 256);
				}
				else
				{
					namebuf[FILEBUF_SIZE - 1] = 0;
					IDOpus->StrCombine(destname, sourcedir, namebuf, 256);
					IDOpus->StrCombine(newiconname, namebuf, ".info", 256);
				}
			      retry_rename:
				if(!(IDOS->Rename(sourcename, destname)))
				{
					if((a = IDOS->IoErr()) == ERROR_OBJECT_EXISTS)
					{
						if(askeach)
						{
							if(!lastfile)
							{
								if((a = checkexistreplace(sourcename, destname, &file->date, destwild, 1)) == REPLACE_ABORT)
								{
									myabort();
									break;
								}
								if(a == REPLACE_ALL)
								{
									if(!destwild)
										goto functionloop;	// TRY AGAIN
									askeach = 0;	// REPLACE ALL
								}
								else if(a == REPLACE_SKIP)
								{
									break;
								}
								else if(a == REPLACE_SKIPALL)
								{
									askeach = 0;
									autoskip = 1;
								}
								else if(a == REPLACE_RENAME)
									goto retry_rename;
							}
						}
						if(autoskip)
							break;
						if((a = delfile(destname, namebuf, globstring[STR_DELETING], 1, 1)) == -2)
						{
							if(!(a = recursedir(destname, NULL, R_DELETE, 0)))
								a = 1;
							else if(a == -10)
								a = -1;
						}
						if(a == -1)
						{
							myabort();
							break;
						}
						if(a)
						{
							if((tempfile = findfile(swindow, namebuf, NULL)))
							{
								if(tempfile == nextfile)
								{
									nextfile = tempfile->next;
								}
								removefile(tempfile, swindow, act, 1);
							}
							firstset = 1;
							goto functionloop;
						}
					}
					doerror(a);
					if((a = checkerror(globstring[STR_RENAMING], file->name, a)) == 3)
					{
						myabort();
						break;
					}
					if(a == 1)
						goto functionloop;
				}
				else
				{
					if(file->type >= ENTRY_DIRECTORY)
					{
						IDOpus->StrCombine(buf, sourcedir, file->name, 256);
						IDOpus->TackOn(buf, NULL, 256);
						IDOpus->StrCombine(buf1, sourcedir, namebuf, 256);
						IDOpus->TackOn(buf1, NULL, 256);
						renamebuffers(buf, buf1);
					}
					if(!noremove)
					{
						if(act > -1 && config->sortmethod[act] == DISPLAY_NAME)
						{
							IExec->CopyMem((char *)file, (char *)&filebuf, sizeof(struct Directory));
							if(file->comment)
								strcpy(buf2, file->comment);
							else
								buf2[0] = 0;
							removefile(file, swindow, act, 0);
							file = NULL;
							addfile(swindow, act, namebuf, filebuf.size, filebuf.type, &filebuf.date, buf2, filebuf.protection, filebuf.subtype, 1, NULL, NULL, filebuf.owner_id, filebuf.group_id);
						}
						else
							strcpy(file->name, namebuf);
						if(lastfile)
							refreshwindow(act, 0);
					}
					okayflag = 1;
				}
			}
			else
				okayflag = 1;
			break;

		case FUNC_MOVEAS:
			if(!lastfile && !flag)
			{
				if(rexx && rexx_argcount > 1)
					strcpy(namebuf, rexx_args[1]);
				else
				{
					if(!(a = whatsit(globstring[STR_ENTER_NEW_NAME_MOVE], FILEBUF_SIZE - 2, namebuf, globstring[STR_SKIP])))
					{
						myabort();
						break;
					}
					if(a == 2)
					{
						file = NULL;
						okayflag = 1;
						oldiconname[0] = 0;
						break;
					}
				}
				namebuf[FILEBUF_SIZE - 1] = 0;
			}
		case FUNC_MOVE:
			if(lastfile)
			{
				strcpy(destname, newiconname);
			}
			else
			{
				IDOpus->StrCombine(destname, destdir, namebuf, 256);
				IDOpus->StrCombine(newiconname, destname, ".info", 256);
			}
			if(checksame(destdir, sourcename, 0) == LOCK_SAME)
			{
				break;
			}
		      retry_move:
			if((exist = IDOpus->CheckExist(destname, NULL)))
			{
				if(askeach)
				{
					if((a = checkexistreplace(sourcename, destname, &file->date, (function == FUNC_MOVE), 1)) == REPLACE_ABORT)
					{
						myabort();
						break;
					}
					if(a == REPLACE_SKIP)
					{
						break;
					}
					if(a == REPLACE_ALL)
					{
						if(function == FUNC_MOVEAS)
							goto functionloop;	// TRY AGAIN
						askeach = 0;	// ALL
					}
					if(a == REPLACE_SKIPALL)
					{
						askeach = 0;
						autoskip = 1;
					}
					else if(a == REPLACE_RENAME)
					{
						IDOpus->StrCombine(newiconname, destname, ".info", 256);
						goto retry_move;
					}
				}
				if(autoskip)
				{
					break;
				}
				if((a = delfile(destname, namebuf, globstring[STR_MOVING], 1, 1)) == -1)
				{
					myabort();
					break;
				}
				if(!a)
				{
					doerror(ERROR_OBJECT_EXISTS);
					break;
				}
				if(a && (tempfile = findfile(dwindow, namebuf, NULL)))
				{
					removefile(tempfile, dwindow, inact, 0);
				}
			}
			if(!(IDOS->Rename(sourcename, destname)))
			{
				if((exist < 0 || file->type <= ENTRY_FILE) && (a = IDOS->IoErr()) != ERROR_RENAME_ACROSS_DEVICES)
				{
					doerror(a);
					if((a = checkerror(globstring[STR_MOVING], file->name, a)) == 3)
					{
						myabort();
						break;
					}
					if(a == 1)
					{
						goto functionloop;
					}
				}
				else
				{
					if(file->type >= ENTRY_DIRECTORY)
					{
						a = recursedir(sourcename, destname, R_COPY | R_DELETE, 0);
						if(a == 0)
						{
							if(!func_external_file[0])
							{
								setdirsize(file, dos_global_bytecount - dos_global_deletedbytes, act);
							}
						}
						else if(status_justabort || a < 0)
						{
							if(a != -10)
							{
								if((a = checkerror(globstring[STR_MOVING], file->name, IDOS->IoErr())) == 3)
									a = -10;
								else if(a == 1)
									goto functionloop;
							}
							if(!func_external_file[0])
							{
								setdirsize(file, file->size - dos_global_deletedbytes, act);
								refreshwindow(act, 0);
							}
							if(IDOpus->CheckExist(destname, NULL))
							{
								if(exist)
								{
									if((tempfile = findfile(dwindow, namebuf, NULL)))
										removefile(tempfile, dwindow, inact, 0);
								}
								addfile(dwindow, inact, namebuf, dos_global_copiedbytes, file->type, &file->date, file->comment, file->protection, file->subtype, 1, NULL, NULL, file->owner_id, file->group_id);
							}
							if(a == -10)
								myabort();
							else
								status_justabort = 0;
							break;
						}
						if(!(tempfile = findfile(dwindow, namebuf, NULL)))
						{
							addfile(dwindow, inact, namebuf, exist ? -1 : dos_global_copiedbytes, file->type, &file->date, file->comment, file->protection, file->subtype, 1, NULL, NULL, file->owner_id, file->group_id);
						}
						else
						{
							setdirsize(tempfile, exist ? -1 : dos_global_copiedbytes, act);
							refreshwindow(inact, 0);
						}
						if((a = delfile(sourcename, file->name, globstring[STR_DELETING], 1, 1)) == -1)
						{
							myabort();
							break;
						}
						if(a == 1)
						{
							if(!noremove)
								removefile(file, swindow, act, 1);
							file = NULL;
						}
						okayflag = 1;
					}
					else
					{
						if(!(a = copyfile(sourcename, destname, &err, NULL, 0)))
						{
							doerror(err);
							if((a = checkerror(globstring[STR_MOVING], file->name, err)) == 3)
							{
								myabort();
								break;
							}
							if(a == 1)
								goto functionloop;
						}
						else if(a == -1)
						{
							myabort();
							break;
						}
						else if(lockandexamine(destname, fileinfo))
						{
							addfile(dwindow, inact, fileinfo->fib_FileName, fileinfo->fib_Size, fileinfo->fib_DirEntryType, &fileinfo->fib_Date, fileinfo->fib_Comment, fileinfo->fib_Protection, file->subtype, 1, NULL, NULL, fileinfo->fib_OwnerUID, fileinfo->fib_OwnerGID);
							if((a = delfile(sourcename, file->name, globstring[STR_DELETING], 1, 1)) == -2)
							{
								if(!(a = recursedir(sourcename, NULL, R_DELETE, 0)))
									a = 1;
								else if(a == -10)
								{
									myabort();
									break;
								}
							}
							if(a == -1)
							{
								myabort();
								break;
							}
							if(a)
							{
								if(!noremove)
									removefile(file, swindow, act, 1);
								file = NULL;
							}
							else
								oldiconname[0] = 0;
							okayflag = 1;
						}
					}
					show = act;
					seename(inact);
				}
			}
			else
			{
				if(progress_copy)
				{
					dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, 100, 100, file->name, 1);
				}
				addfile(dwindow, inact, namebuf, file->size, file->type, &file->date, file->comment, file->protection, file->subtype, 1, NULL, NULL, file->owner_id, file->group_id);
				if(!noremove)
				{
					removefile(file, swindow, act, 1);
				}
				file = NULL;
				okayflag = 1;
			}
			break;

		case FUNC_COPYAS:	/* If flag is set then won't ask for new name */
			if(!lastfile && !flag)
			{
				if(rexx && rexx_argcount > 1)
					strcpy(namebuf, rexx_args[1]);
				else
				{
					if(!(a = whatsit((specflags & FUNCFLAGS_COPYISCLONE) ? globstring[STR_ENTER_NEW_NAME_CLONE] : globstring[STR_ENTER_NEW_NAME], FILEBUF_SIZE - 2, namebuf, globstring[STR_SKIP])))
					{
						myabort();
						break;
					}
					if(a == 2)
					{
						okayflag = 1;
						file = NULL;
						oldiconname[0] = 0;
						break;
					}
					if((strcmp(sourcedir, destdir) == 0) && (strcmp(namebuf, file->name) == 0))
					{
						okayflag = 1;
						oldiconname[0] = 0;
						break;
					}
				}
				namebuf[FILEBUF_SIZE - 1] = 0;
			}
		case FUNC_COPY:
			if(lastfile)
			{
				strcpy(destname, newiconname);
			}
			else
			{
				IDOpus->StrCombine(destname, destdir, namebuf, 256);
				IDOpus->StrCombine(newiconname, destname, ".info", 256);
			}
			arcfile = getsourcefromarc(swindow, sourcename, file->name);
			if(checksame(destdir, sourcename, 0) == LOCK_SAME)
				break;
		      retry_copy:
			if((exist = IDOpus->CheckExist(destname, NULL)))
			{
				if(askeach)
				{
					if((a = checkexistreplace(sourcename, destname, &file->date, (function == FUNC_COPY), 1)) == REPLACE_ABORT)
					{
						myabort();
						break;
					}
					if(a == REPLACE_SKIP)
						break;
					else if(a == REPLACE_ALL)
					{
						if(function != FUNC_COPY /*==FUNC_COPYAS*/)
							goto functionloop;	// TRY AGAIN
						askeach = 0;	// ALL
					}
					else if(a == REPLACE_SKIPALL)
					{
						askeach = 0;
						autoskip = 1;
					}
					else if(a == REPLACE_RENAME)
					{
						IDOpus->StrCombine(newiconname, destname, ".info", 256);
						goto retry_copy;
					}
				}
				if(autoskip)
					break;
			}
			if(exist < 0)
			{
				if((a = delfile(destname, file->name, globstring[STR_COPYING], 1, 1)) == -1)
				{
					myabort();
					break;
				}
				if(!a)
				{
					doerror(ERROR_OBJECT_EXISTS);
					break;
				}
				if(a == 1 && (tempfile = findfile(dwindow, namebuf, NULL)))
				{
					if(!noremove)
						removefile(tempfile, dwindow, inact, (tempfile->type != file->type));
					exist = 0;
				}
			}
			if(file->type >= ENTRY_DIRECTORY)
			{
				a = recursedir(sourcename, destname, R_COPY, 0);
				if(exist)
				{
					byte = -1;
					exist = 0;
				}
				else
				{
					byte = dos_global_copiedbytes;
				}
				if(config->copyflags & COPY_DATE)
					IDOS->SetFileDate(destname, &file->date);

				if(!a && !func_external_file[0])
				{
					setdirsize(file, dos_global_bytecount, act);
					refreshwindow(act, 0);
					filloutcopydata(file);
				}
				else if(a == -1 || a == -10)
				{
					if(a == -1)
					{
						if((a = checkerror(globstring[STR_COPYING], file->name, IDOS->IoErr())) == 1)
							goto functionloop;
						if(a == 3)
							a = -10;
					}
					if(IDOpus->CheckExist(destname, NULL))
					{
						if(exist && (tempfile = findfile(dwindow, namebuf, NULL)))
							removefile(tempfile, dwindow, inact, 0);
						addfile(dwindow, inact, namebuf, byte, file->type, &file->date, file->comment, file->protection, file->subtype, 1, NULL, NULL, file->owner_id, file->group_id);
					}
					if(a == -10)
						myabort();
					show = inact;
					break;
				}
				else if(a < 0)
					break;
			}
			else
			{
				a = copyfile(sourcename, destname, &err, NULL, 0);
				if(a == 0)
				{
					doerror(err);
					if((a = checkerror(globstring[STR_COPYING], file->name, err)) == 3)
					{
						myabort();
						break;
					}
					if(a == 1)
						goto functionloop;
					break;
				}
				else if(a == -1)
				{
					myabort();
					break;
				}
				byte = file->size;
			}
			if(exist && (tempfile = findfile(dwindow, namebuf, NULL)))
				removefile(tempfile, dwindow, inact, 0);
			if(lockandexamine(destname, fileinfo))
			{
				if(fileinfo->fib_DirEntryType < 0)
					byte = fileinfo->fib_Size;
				addfile(dwindow, inact, fileinfo->fib_FileName, byte, fileinfo->fib_DirEntryType, &fileinfo->fib_Date, fileinfo->fib_Comment, fileinfo->fib_Protection, file->subtype, 1, NULL, NULL, fileinfo->fib_OwnerUID, fileinfo->fib_OwnerGID);
				if(config->copyflags & COPY_ARC && !(file->protection & FIBF_ARCHIVE))
				{
					if(IDOS->SetProtection(sourcename, file->protection | FIBF_ARCHIVE))
					{
						file->protection |= FIBF_ARCHIVE;
						getprot(file->protection, file->protbuf);
					}
				}
			}
			okayflag = 1;
			show = inact;
			break;

		case FUNC_HUNT:
			if(file->type <= ENTRY_FILE)
			{
				okayflag = 1;
				break;
			}
			if((a = recursedir(sourcename, NULL, R_HUNT, 0)) == -3)
			{
				wildselect(buf2, 2, 0, WILDSELECT_NAME);
				findfirstsel(act, -2);
				okayflag = breakout = noshow = 1;
				count = -1;
				break;
			}
			if(a > -1)
			{
				okayflag = 1;
				if(a == 0 && !func_external_file[0])
					setdirsize(file, dos_global_bytecount, act);
				if(a > 0)
					count += a;
			}
			else
				count = -1;
			break;

		case FUNC_READ:
		case FUNC_HEXREAD:
		case FUNC_ANSIREAD:
		case FUNC_SMARTREAD:
			if(file->type >= ENTRY_DEVICE)
			{
				okayflag = 1;
				break;
			}
			arcfile = getsourcefromarc(swindow, sourcename, file->name);
			a = viewfile(sourcename, str_arcorgname[0] ? str_arcorgname : file->name, function, NULL, viewdata, (viewdata || str_arcorgname[0]) ? 1 : 0, (entry_depth > 1));
			if(a != -2)
			{
				if(a != -3)
				{
					++count;
				}
				else if(count == 0)
				{
					unselect(act, file);
				}
				if(file->selected)
				{
					file->selected = 0;
					if(swindow)
					{
						--swindow->filesel;
						swindow->bytessel -= file->size;
					}
				}
				if(a == -1)
				{
					status_justabort = 1;
				}
				else
				{
					okayflag = 1;
				}
			}
			else
			{
				if(!count && !(doerror(-1)))
					okayflag = 1;
			}
			break;

		case FUNC_SHOW:
			if(file->type >= ENTRY_DEVICE || file->size < 1)
			{
				okayflag = 1;
				file = NULL;
				break;
			}
			arcfile = getsourcefromarc(swindow, sourcename, file->name);
			if(entry_depth < 2 && checkfiletypefunc(sourcename, FTFUNC_SHOW))
			{
				okayflag = 1;
			}
			else
			{
				if((a = showpic(sourcename, pt)) == -1)
				{
					okayflag = 1;
					myabort();
					break;
				}
				else
				{
					if(a == 1)
					{
						okayflag = 1;
					}
					else
					{
						file = NULL;
					}
				}
			}
			break;

		case FUNC_ICONINFO:
			arcfile = getsourcefromarc(swindow, sourcename, file->name);
			if(strcmp(file->name, ".info") == 0)
			{
				okayflag = 1;
			}
			else
			{
				a = dopus_iconinfo(sourcename);
				switch(a)
				{
					case -1:
						doerror(-1);
						file = NULL;
						break;
					case -2:
						dostatustext(globstring[STR_ERROR_OCCURED]);
						file=NULL;
						break;
					case -3:
						dostatustext(globstring[STR_CANT_FIND_ICON]);
						file=NULL;
						break;
					case -4:
						dostatustext(globstring[STR_NO_CHIP_FOR_ICON]);
						file=NULL;
						break;
					case 1:
					default:
						okayflag = 1;
						show = act;
						break;
				}
			}
			break;
		case FUNC_PLAY8SVX:
		case FUNC_LOOPPLAY8SVX:
			if(file->type >= ENTRY_DEVICE || file->size < 1)
			{
				okayflag = 1;
				file = NULL;
				break;
			}
			a = doplay8svxold(sourcename, (function == FUNC_LOOPPLAY8SVX));
			kill8svx();
			if(a == -1)
			{
				myabort();
				break;
			}
			else
			{
				if(a == 1)
				{
					okayflag = 1;
				}
				else
				{
					handle8svxerror(a);
					file = NULL;
				}
			}
			break;
		case FUNC_PLAYST:
			a = 1;
		case FUNC_PLAY:
		case FUNC_LOOPPLAY:
			if(file->type >= ENTRY_DEVICE || file->size < 1)
			{
				okayflag = 1;
				file = NULL;
				break;
			}
			arcfile = getsourcefromarc(swindow, sourcename, file->name);
			if(entry_depth < 2 && checkfiletypefunc(sourcename, (function == FUNC_PLAY) ? FTFUNC_PLAY : FTFUNC_LOOPPLAY))
			{
				okayflag = 1;
			}
			else
			{
				a = doplay8svx(sourcename, (function == FUNC_LOOPPLAY));
				if(a == -1)
				{
					myabort();
					break;
				}
				else
				{
					if(a == 1)
					{
						okayflag = 1;
					}
				}
			}
			break;

		case FUNC_ADDICON:
			if(strlen(file->name) > (FILEBUF_SIZE - 6) || isicon(file->name))
			{
				okayflag = 1;
				file = NULL;
				break;
			}
			IDOpus->StrCombine(buf2, file->name, ".info", FILEBUF_SIZE - 1);
			IDOpus->StrCombine(destname, sourcename, ".info", 256);
			if((a = IDOpus->CheckExist(sourcename, NULL)) == 0)
			{
				doerror((a = IDOS->IoErr()));
				if((a = checkerror(globstring[STR_ADDING_ICON], file->name, a)) == 1)
					goto functionloop;
				if(a == 3)
				{
					myabort();
					break;
				}
				file = NULL;
				break;
			}
			if(a > 0)
				data = ICONTYPE_DRAWER;
			else if(checkexec(sourcename))
				data = ICONTYPE_TOOL;
			else
				data = ICONTYPE_PROJECT;
			if(IDOpus->CheckExist(destname, NULL))
			{
				if(askeach)
				{
					doerror(ERROR_OBJECT_EXISTS);
					if((a = checkexistreplace(destname, destname, NULL, 1, 1)) == REPLACE_ABORT)
					{
						myabort();
						break;
					}
					else if(a == REPLACE_ALL)
						askeach = 0;
					else if(a == REPLACE_SKIP)
					{
						file = NULL;
						break;
					}
					else if(a == REPLACE_SKIPALL)
					{
						askeach = 0;
						autoskip = 1;
						file = NULL;
					}
					else if(a == REPLACE_RENAME)
					{
						strcpy(buf2, IDOS->FilePart(destname));
					}
				}
				if(autoskip)
					break;
			}
			if((iconwrite(data, destname)) != -1)
			{
				if(lockandexamine(destname, fileinfo))
				{
					addfile(swindow, act, buf2, fileinfo->fib_Size, -1, &fileinfo->fib_Date, fileinfo->fib_Comment, fileinfo->fib_Protection, 0, 1, NULL, NULL, fileinfo->fib_OwnerUID, fileinfo->fib_OwnerGID);
					show = act;
					okayflag = 1;
				}
			}
			else
			{
				myabort();
				break;
			}
			break;

		case FUNC_COMMENT:
			if(askeach)
			{
				if(rexx && rexx_argcount > 0)
					strcpy(buf2, rexx_args[rexarg]);
				else
				{
					if(file->comment)
						strcpy(buf2, file->comment);
					else
						buf2[0] = 0;
					if(!(a = whatsit(globstring[STR_ENTER_COMMENT], 79, buf2, globstring[STR_ALL])))
					{
						myabort();
						break;
					}
					if(a == 2)
						askeach = 0;
				}
			}
			FOREVER
			{
				if(IDOS->SetComment(sourcename, buf2))
					break;
				doerror((a = IDOS->IoErr()));
				if((a = checkerror(globstring[STR_COMMENTING], file->name, a)) == 3)
				{
					myabort();
					break;
				}
				if(a != 1)
				{
					file = NULL;
					break;
				}
			}
			if(status_justabort || !file)
				break;
			okayflag = 1;
			if(file->type >= ENTRY_DIRECTORY)
			{
				if((a = recursedir(sourcename, buf2, R_COMMENT, 0)) > 0 && status_justabort)
					status_justabort = okayflag = 0;
				else if(!a && !func_external_file[0])
					setdirsize(file, dos_global_bytecount, act);
			}
			if(!noremove && file->comment != dos_copy_comment)
			{
				if(act > -1 && config->sortmethod[act] == DISPLAY_COMMENT)
				{
					IExec->CopyMem((char *)file, (char *)&filebuf, sizeof(struct Directory));
					removefile(file, swindow, act, 0);
					file = NULL;
					addfile(swindow, act, namebuf, filebuf.size, filebuf.type, &filebuf.date, buf2, filebuf.protection, filebuf.subtype, 1, NULL, NULL, filebuf.owner_id, filebuf.group_id);
				}
				else
				{
					if(file->comment)
						IExec->FreePooled(dir_memory_pool, file->comment, strlen(file->comment) + 1);
					if((file->comment = IExec->AllocPooled(dir_memory_pool, strlen(buf2) + 1)))
						strcpy(file->comment, buf2);
				}
			}
			break;

		case FUNC_PROTECT:
			if(askeach && !lastfile)
			{
				if(rexx)
				{
					data = rexx_arg_value[0];
					mask = rexx_arg_value[1];
				}
				else
				{
					data = file->protection;
					if(!(a = getprotectdata(&data, &mask)))
					{
						myabort();
						break;
					}
					if(a == 2)
						askeach = 0;
				}
			}
			temp = getnewprot(file->protection, data, mask);
			FOREVER
			{
				if(IDOS->SetProtection(sourcename, temp))
					break;
				doerror((a = IDOS->IoErr()));
				if((a = checkerror(globstring[STR_PROTECTING], file->name, a)) == 3)
				{
					myabort();
					break;
				}
				if(a != 1)
				{
					file = NULL;
					break;
				}
			}
			if(status_justabort || !file)
				break;
			file->protection = temp;
			getprot(temp, file->protbuf);
			if(file->type >= ENTRY_DIRECTORY)
			{
				protstuff[0] = data;
				protstuff[1] = mask;
				if((a = recursedir(sourcename, NULL, R_PROTECT, (int)protstuff)) && status_justabort)
					break;
				if(!a && !func_external_file[0])
					setdirsize(file, dos_global_bytecount, act);
			}
			if(!file->selected)
				refreshwindow(act, 0);
			okayflag = 1;
			break;

		case FUNC_ENCRYPT:
			if(file->type >= ENTRY_DEVICE || file->size < 1)
			{
				okayflag = 1;
				file = NULL;
				break;
			}
			IDOpus->StrCombine(destname, destdir, file->name, 256);
			if(IDOpus->CheckExist(destname, NULL))
			{
				if(askeach)
				{
					if((a = checkexistreplace(sourcename, destname, &file->date, 1, 1)) == REPLACE_ABORT)
					{
						myabort();
						break;
					}
					else if(a == REPLACE_ALL)
						askeach = 0;
					else if(a == REPLACE_SKIP)
					{
						file = NULL;
						break;
					}
					else if(a == REPLACE_SKIPALL)
					{
						askeach = 0;
						autoskip = 1;
						file = NULL;
					}
				}
				if(autoskip)
					break;
				if((a = delfile(destname, file->name, globstring[STR_DATESTAMPING], 1, 1)) == -1)
				{
					myabort();
					break;
				}
			}
			if(!IDOpus->CheckExist(destname, NULL))
			{
				FOREVER
				{
					if(copyfile(sourcename, destname, &err, /*-1*/ buf2 /*NULL*/, data))
					{
						addfile(dwindow, inact, file->name, file->size, file->type, &file->date, file->comment, file->protection, file->subtype, 1, NULL, NULL, file->owner_id, file->group_id);
						okayflag = 1;
						show = inact;
						break;
					}
					doerror(err);
					if((a = checkerror(globstring[STR_ENCRYPTING], file->name, err)) == 3)
					{
						myabort();
						break;
					}
					if(a != 1)
					{
						file = NULL;
						break;
					}
				}
			}
			else
			{
				doerror(ERROR_OBJECT_EXISTS);
			}
			break;

		case FUNC_EXTRACT:
			{
				dostatustext(globstring[STR_EXTRACT_ARCHIVE]);
				if((a = extractarchive(file->name, sourcedir, destdir)) == 0)
				{
					myabort();
				}
				else
				{
					okayflag = 1;
				}
			}
			break;

		case FUNC_DATESTAMP:
			if(askeach && !lastfile)
			{
				if(rexx && rexx_argcount > 0)
				{
					strcpy(buf2, rexx_args[rexarg]);
				}
				else
				{
					seedate(&(file->date), buf2, 0);	// buf2[0]=0;
					if(!(a = whatsit(globstring[STR_ENTER_DATE_AND_TIME], 20, buf2, globstring[STR_ALL])))
					{
						myabort();
						break;
					}
					if(a == 2)
					{
						askeach = 0;
					}
				}
				if(buf2[0])
				{
					parsedatetime(buf2, buf, buf1, &a);
					strtostamp(buf, buf1, &datetime.dat_Stamp);
				}
				else
				{
					IDOS->DateStamp(&datetime.dat_Stamp);
				}
			}
			FOREVER
			{
				if((a = setdate(sourcename, &datetime.dat_Stamp)) == 1)
				{
					if(!noremove)
					{
						if(act > -1 && config->sortmethod[act] == DISPLAY_DATE)
						{
							IExec->CopyMem((char *)file, (char *)&filebuf, sizeof(struct Directory));
							if(file->comment)
							{
								strcpy(buf2, file->comment);
								filebuf.comment = buf2;
							}
							removefile(file, swindow, act, 0);
							file = (struct Directory *)addfile(swindow, act, filebuf.name, filebuf.size, filebuf.type, &datetime.dat_Stamp, filebuf.comment, filebuf.protection, filebuf.subtype, 1, NULL, NULL, filebuf.owner_id, filebuf.group_id);
						}
						else
						{
							file->date.ds_Days = datetime.dat_Stamp.ds_Days;
							file->date.ds_Minute = datetime.dat_Stamp.ds_Minute;
							file->date.ds_Tick = datetime.dat_Stamp.ds_Tick;
							seedate(&file->date, file->datebuf, 1);
						}
					}
					break;
				}
				doerror(a);
				if((a = checkerror(globstring[STR_DATESTAMPING], file->name, a)) == 3)
				{
					myabort();
					break;
				}
				if(a != 1)
				{
					file = NULL;
					break;
				}
			}
			if(status_justabort || !file)
				break;
			if(file->type >= ENTRY_DIRECTORY)
			{
				if(!(recursedir(sourcename, NULL, R_DATESTAMP, (int)&datetime.dat_Stamp)))
				{
					okayflag = 1;
					if(!func_external_file[0])
						setdirsize(file, dos_global_bytecount, act);
				}
			}
			else
				okayflag = 1;
			break;

		case FUNC_SEARCH:
			if(file->type == ENTRY_DEVICE)
			{
				if((a = recursedir(file->name, NULL, R_SEARCH, 0)) == -2)
				{
					okayflag = 1;
					count = -1;
					nextfile = NULL;
					break;
				}
				if(a == -1)
				{
					nextfile = NULL;
					break;
				}
				if(a == -10)
				{
					myabort();
					break;
				}
				if(a > 0)
					count += a;
				okayflag = 1;
			}
			else
			{
				if(file->type <= ENTRY_FILE)
				{
					arcfile = getsourcefromarc(swindow, sourcename, file->name);
					a = filesearch(sourcename, &b, (askeach) ? 1 : -1);
					count += b;
					if(a == 2)
					{
						okayflag = 1;
						file = NULL;
						busy();
						if(act > -1)
						{
							if(swindow)
							{
								if(count > 0)
									b = (swindow->filesel - count) + swindow->dirsel;
								else
									b = swindow->filesel + swindow->dirsel;
								if(!b)
									nextfile = NULL;
							}
						}
						if(nextfile && !simplerequest(globstring[STR_CONTINUE_WITH_SEARCH], globstring[STR_CONTINUE], str_cancelstring, NULL))
							nextfile = NULL;
					}
					if(a == -1)
					{
						myabort();
						break;
					}
					if(a)
					{
						okayflag = 1;
						if(a == 3)
							file = NULL;
						else if(a == 4)
						{
							file = NULL;
							askeach = 0;
						}
					}
					else
						doerror(-1);
				}
				else
				{
					if(!(a = recursedir(sourcename, NULL, R_SEARCH, 0)))
					{
						if(!func_external_file[0])
							setdirsize(file, dos_global_bytecount, act);
						okayflag = 1;
					}
					if(a == -2)
					{
						okayflag = 1;
						breakout = 2;
						count = -1;
						nextfile = NULL;
						break;
					}
					if(a == -1 || a == -10)
					{
						if(a == -10)
						{
							myabort();
							file = NULL;
						}
						else
							nextfile = NULL;
						break;
					}
					if(a > 0)
						count += a;
				}
			}
			break;
		}
		if(arcfile)
			removetemparcfile(sourcename);

		if(config->dynamicflags & UPDATE_FREE && show > -1)
			seename(show);
		if(breakout == 1)
			break;
		if(file && okayflag)
		{
			if(file->selected)
			{
				unselect(act, file);
			}
			else
			{
				refreshwindow(act, 0);
			}
		}
		if(status_justabort || breakout == 2)
			break;

		if(doicons && !lastfile && okayflag && oldiconname[0])
		{
			if((file = findfile(swindow, oldiconname, NULL)))
			{
				if(file == nextfile)
					nextfile = file->next;
				lastfile = 1;
				if(file->selected && prog_indicator)
				{
					if(progtype == 0 || progtype == 2)
					{
						++value;
						dotaskmsg(hotkeymsg_port, PROGRESS_INCREASE, 1, 0, NULL, 0);
					}
				}
				if(progress_copy)
					dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, -2, 0, file->name, 1);
				goto functionloop;
			}
			else
			{
				IDOpus->StrCombine(sourcename, sourcedir, oldiconname, 256);
				if(filloutdummy(sourcename, &dummyfile))
				{
					dummyfile.selected = 0;
					file = &dummyfile;
					lastfile = 1;
					goto functionloop;
				}
			}
		}

		if(globflag)
			break;
		if(!lastfile && file && file->next && file->next != nextfile)
			file = file->next;
		else
			file = nextfile;
		namebuf[0] = 0;
	}

	if(prog_indicator)
	{
		dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, -1, status_justabort, NULL, 0);
		if(progress_copy)
			dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, -1, status_justabort, NULL, 1);
	}

	switch (function)
	{
	case FUNC_HUNT:
		if(count == 0)
		{
			okayflag = 0;
			doerror(ERROR_OBJECT_NOT_FOUND);
			simplerequest(globstring[STR_COULD_NOT_FIND_FILE], globstring[STR_CONTINUE], NULL);
		}
		else if(count > 0)
		{
			sprintf(buf2, globstring[STR_FOUND_MATCHING_FILES], count);
			dostatustext(buf2);
			simplerequest(buf2, globstring[STR_CONTINUE], NULL);
		}
		break;

	case FUNC_READ:
	case FUNC_HEXREAD:
	case FUNC_ANSIREAD:
	case FUNC_SMARTREAD:
		if(act > -1)
			refreshwindow(act, 0);
		if(viewdata)
			cleanupviewfile(viewdata);
		if(status_justabort)
			myabort();
		else if(!okayflag && !(doerror(-1)))
			okayflag = 1;
		break;

	case FUNC_SHOW:
		break;
	case FUNC_SEARCH:
		if(count > -1)
		{
			if(count == 0)
				simplerequest(globstring[STR_STRING_NOT_FOUND], globstring[STR_CONTINUE], NULL);
			else
			{
				sprintf(buf2, globstring[STR_MATCHED_FILES], count);
				simplerequest(buf2, globstring[STR_CONTINUE], NULL);
			}
			okayflag = 1;
		}
		break;

	case FUNC_BYTE:
		if(!status_justabort)
		{
			int64 memvalue;

			IDOS->SetProcWindow((APTR)-1L);
			infodata = IDOS->AllocDosObject(DOS_INFODATA, NULL);
			if(!(destdir && (filelock = IDOS->Lock(destdir, ACCESS_READ))))
			{
				memvalue = 0;
			}
			else
			{
				IDOS->Info(filelock, infodata);
				if(ramdisk_lock && IDOS->SameLock(filelock, ramdisk_lock) != LOCK_DIFFERENT)
				{
					memvalue = IExec->AvailMem(0);
				}
				else
				{
					memvalue = (infodata->id_NumBlocks - infodata->id_NumBlocksUsed) * (int64)blocksize;
				}
				IDOS->UnLock(filelock);
			}
			if(config->errorflags & ERROR_ENABLE_DOS)
			{
				IDOS->SetProcWindow(Window);
			}
			if(specflags & FUNCFLAGS_BYTEISCHECKFIT)
			{
				float64 needed, percent;

				needed = data * blocksize;
				if(memvalue < 1)
				{
					percent = 0;
				}
				else if(memvalue >= needed || needed < 1)
				{
					percent = 100;
					retval = 1;
				}
				else
				{
					percent = 100 / (needed / memvalue);
				}
				sprintf(buf, globstring[STR_CHECKFIT_STRING], data * (int64)blocksize, memvalue, percent);
				dostatustext(buf);
			}
			else
			{
				doselinfo(act);
				strcat(str_select_info, (memvalue <= swindow->bytessel) ? globstring[STR_NO_FIT] : globstring[STR_YES_FIT]);
				dostatustext(str_select_info);
			}
			okayflag = 0;
			IDOS->FreeDosObject(DOS_INFODATA, infodata);
		}
		break;
	}

	if(okayflag && !status_justabort)
		okay();
	if(!noshow)
	{
		endfollow(act);
		displaydir(act);
		seename(act);
	}
	if(inact > -1)
	{
		displaydir(inact);
		seename(inact);
	}
	switch (function)
	{
	case FUNC_MOVEAS:
		if(checksame(sourcedir, destdir, 2) == LOCK_SAME)
			break;
	case FUNC_DELETE:
	case FUNC_RENAME:
	case FUNC_MOVE:
	case FUNC_ADDICON:
//	case FUNC_ICONINFO:  // Directory won't be rescanned if uncommented.
		update_buffer_stamp(act, 1);
		break;
	case FUNC_COPYAS:
		if(specflags & FUNCFLAGS_COPYISCLONE)
			update_buffer_stamp(act, 1);
		break;

	case FUNC_COMMENT:
	case FUNC_PROTECT:
	case FUNC_DATESTAMP:
		update_buffer_stamp(act, 0);
		break;
	}

	switch (function)
	{
	case FUNC_MOVEAS:
	case FUNC_COPYAS:
		if(checksame(sourcedir, destdir, 2) == LOCK_SAME)
			break;
	case FUNC_MOVE:
	case FUNC_COPY:
	case FUNC_ENCRYPT:
		update_buffer_stamp(inact, 1);
		break;
	case FUNC_EXTRACT:
		break;
	}

	switch (function)
	{
	case FUNC_SEARCH:
		if(count > 0)
			rexx_return_value = count;
		break;
	}

	startnotifies();

      endfunction:
	if(prog_indicator)
		dotaskmsg(hotkeymsg_port, PROGRESS_CLOSE, 0, 0, NULL, 0);
	IExec->FreeSysObject(ASOT_MEMPOOL, function_memory_pool);
	--entry_depth;

	IDOS->FreeDosObject(DOS_FIB, fileinfo);

	return (retval);
}
