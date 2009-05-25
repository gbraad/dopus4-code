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
#include <dos/dostags.h>

#define MAXCOMMANDLEN 510

int returntype(struct ExamineData *data)
{
	if(EXD_IS_FILE(data))
		return ST_FILE;
	if(EXD_IS_SOFTLINK(data))
		return ST_SOFTLINK;

	return ST_USERDIR;
}

void defaultpar(struct dopusfuncpar *par)
{
	par->which = FLAG_DOPUSF | FLAG_CDSOURCE;
	par->delay = 2;
	par->key = par->qual = par->type = 0;
	par->pri = config->priority;
	par->stack = 8192;
}

void dofunctionstring(char *func, char *name, char *title, struct dopusfuncpar *pars)
{
	char fbuf[256], *ptr;
	int a, b, c, lb, ls, run = 1, noloop = 0, norm = 1;
	struct Directory *curcf;
	struct function_data *funcdata;

	if(!func || !func[0] || !(funcdata = IExec->AllocPooled(general_memory_pool, sizeof(struct function_data))))
		return;

	a = strlen(func);
	status_flags &= ~STATUS_VERIFYFAIL;
	if(Window && !(status_flags & STATUS_FROMHOTKEY))
		busy();

	funcdata->activewin = data_active_window;
	funcdata->inactivewin = 1 - data_active_window;

	funcdata->file_request.window = Window;
	funcdata->file_request.lines = 15;

	if(func_external_file[0])
	{
		noloop = 1;
		if((ptr = IDOS->FilePart(func_external_file)))
			strcpy(func_single_file, ptr);
		strcpy(funcdata->source_path, func_external_file);
		if((ptr = IDOS->FilePart(funcdata->source_path)))
			*ptr = 0;
		norm = 0;
		if(!status_iconified)
			strcpy(funcdata->dest_path, str_pathbuffer[data_active_window]);
		funcdata->activewin = 1 - data_active_window;
		funcdata->inactivewin = data_active_window;
	}
	else if(func_single_file[0])
		noloop = 1;
	else if(status_iconified)
		norm = 0;
	else
		funcdata->entry_first = checkalltot(dopus_curwin[funcdata->activewin]);

	if(norm)
	{
		strcpy(funcdata->source_path,
		       str_arcorgname[0] ? "T:" : str_pathbuffer[funcdata->activewin]);
		strcpy(funcdata->dest_path,
		       str_pathbuffer[funcdata->inactivewin]);
	}

	status_justabort = status_haveaborted = 0;

	FOREVER
	{
		c = lb = 0;
		ls = -1;

		curcf = funcdata->entry_first;

		for(b = 0; b < a; b++)
		{
			if(c > 0 && func[b] == '|' && lb)
			{
				fbuf[c - 1] = 0;
				if(ls > -1)
					fbuf[ls] = 0;
				c = lb = 0;
				if(handlefunctionss(fbuf, name, title, pars, funcdata) || status_flags & STATUS_VERIFYFAIL)
				{
					run = 0;
					break;
				}
			}
			else
			{
				if(c > 0 || !(isspace(func[b])))
				{
					if(func[b] == '|' && func[b + 1] == '|')
						lb = 1;
					else if(isspace(func[b]))
					{
						if(ls == -1)
							ls = c;
					}
					else
						ls = -1;
					fbuf[c++] = func[b];
				}
			}
		}
		if(c > 0)
		{
			fbuf[c] = 0;
			if(ls > -1)
				fbuf[ls] = 0;
			if(handlefunctionss(fbuf, name, title, pars, funcdata) || status_flags & STATUS_VERIFYFAIL)
				run = 0;
		}
		if(!pars || !(pars->which & FLAG_DOALL) || status_justabort || status_haveaborted || status_flags & STATUS_VERIFYFAIL || !run || func_single_file[0] || noloop || !funcdata->external_flag)
			break;

		if(curcf && curcf == funcdata->entry_first)
			funcdata->entry_first = funcdata->entry_first->next;

		while (funcdata->entry_first)
		{
			if(funcdata->entry_first->selected)
				break;
			funcdata->entry_first = funcdata->entry_first->next;
		}

		if(!funcdata->entry_first)
			break;
		funcdata->arg_use = funcdata->arg_first;
		++funcdata->function_count;
	}
	closescriptfile(pars, run, funcdata);
	func_single_file[0] = 0;
	func_external_file[0] = 0;
	IExec->FreePooled(general_memory_pool, funcdata, sizeof(struct function_data));

	if(norm && Window)
	{
		for(a = 0; a < 2; a++)
			check_old_buffer(a);
	}
}

int handlefunctionss(char *funcbuf, char *name, char *title, struct dopusfuncpar *pars, struct function_data *funcdata)
{
	int function, abort = 0, a, b;
	char *funcptr;
	struct dopusfuncpar par;
	struct Directory *didselect = NULL;

	func_single_entry = NULL;

	if(funcbuf[0] == FC_INTERNAL)
	{
		b = strlen(funcbuf);
		for(a = 0; a < b; a++)
			if(funcbuf[a] == '{')
				break;
		if(a < b)
		{
			char buf2[256], newfunc[MAXCOMMANDLEN];

			parserunline(funcbuf, buf2);
			if(!(buildcustfunc(buf2, strlen(funcbuf), newfunc, NULL, 0, 0, 0, funcdata)))
				return (1);
			funcbuf = newfunc;
		}
		if((function = getfunction(funcbuf, &funcptr)) != -1)
		{
			if(function != FUNC_HELP && status_flags & STATUS_HELP)
			{
				dohelp(name, funcbuf, pars ? pars->key : 0, pars ? pars->qual : 0, NULL);
				abort = 1;
			}
			else if(function == FUNC_ENDFUNCTION)
				closescriptfile(pars, 1, funcdata);
			else
			{
				func_entry_deleted = 0;
				if(func_single_file[0])
				{
					if(func_external_file[0])
					{
						if(filloutdummy(func_external_file, &funcdata->entry_external))
						{
							funcdata->entry_external.selected = 0;
							func_single_entry = &funcdata->entry_external;
						}
					}
					else if((func_single_entry = findfile(dopus_curwin[funcdata->activewin], func_single_file, NULL)))
					{
						func_single_entry->selected = 1;
						updateselectinfo(func_single_entry, funcdata->activewin, 0);
						didselect = func_single_entry;
					}
				}

				global_swap_window = FALSE;

				if(funcptr)
				{
					func_global_function = function;
					rexxdisp(NULL, NULL, funcptr);
					if(func_global_function)
						abort = internal_function(func_global_function, rexx_global_flag, funcdata->source_path, funcdata->dest_path);
					func_global_function = 0;
				}
				else
					abort = internal_function(function, 0, funcdata-> source_path, funcdata->dest_path);

				if(global_swap_window)
				{
					funcdata->activewin = data_active_window;
					funcdata->inactivewin = 1 - data_active_window;

					strcpy(funcdata->source_path, str_pathbuffer[funcdata->activewin]);
					strcpy(funcdata->dest_path, str_pathbuffer[funcdata->inactivewin]);

					funcdata->entry_first = checkalltot(dopus_curwin[funcdata->activewin]);
				}

				if(didselect && !func_entry_deleted && didselect->selected)
				{
					didselect->selected = 0;
					updateselectinfo(didselect, funcdata->activewin, 0);
				}
				func_single_entry = NULL;
				func_entry_deleted = 0;
			}
		}
	}
	else
	{
		funcdata->external_flag = 1;
		if(status_flags & STATUS_HELP)
		{
			dohelp(name, funcbuf, pars ? pars->key : 0, pars ? pars->qual : 0, NULL);
			abort = 1;
		}
		else
		{
			if(pars)
				IExec->CopyMem((char *)pars, (char *)&par, sizeof(struct dopusfuncpar));
			else
				defaultpar(&par);
			switch (funcbuf[0])
			{
			case FC_WORKBENCH:
				par.type = FT_WORKBENCH;
				++funcbuf;
				break;
			case FC_BATCH:
				par.type = FT_BATCH;
				++funcbuf;
				break;
			case FC_AREXX:
				par.type = FT_AREXX;
				++funcbuf;
				break;
			case FC_CHDIR:
				par.type = FT_CHDIR;
				++funcbuf;
				break;
			default:
				par.type = FT_EXECUTABLE;
				break;
			}
			if(!(customthing(name, title, funcbuf, &par, funcdata)))
				abort = 1;
		}
	}
	return (abort);
}

int getfunction(char *func, char **funcptr)
{
	int a, b;

	if(funcptr)
		*funcptr = NULL;
	if(!func)
		return (-1);
	if(func[0] == FC_INTERNAL)
	{
		b = 0;
		FOREVER
		{
			if(commandlist[b].name == NULL)
				break;
			a = strlen(commandlist[b].name);
			if(IUtility->Strnicmp(commandlist[b].name, &func[1], a) == 0 && (isspace(func[a + 1]) || func[a + 1] == 0))
			{
				if(funcptr && func[a + 1])
					*funcptr = &func[a + 1];
				return ((int)commandlist[b].function);
			}
			++b;
		}
	}
	return (-1);
}

int customthing(char *name, char *title, char *function, struct dopusfuncpar *par, struct function_data *funcdata)
{
	int moretodo, a, b, len, retval, type, count;
	char buf[256], *ptr;
	char buf2[MAXCOMMANDLEN], tbuf[512];
	struct args *usearg, *endarg;

	if((!name || !name[0]) && (!function || !function[0]))
		return (1);
	if(Window && !status_iconified && !(status_flags & STATUS_FROMHOTKEY))
	{
		if(title)
			dostatustext(title);
		else if(name)
			dostatustext(name);
	}

	type = par->type;

	if(type != FT_AREXX)
	{
		if(!(openscriptfile(par, funcdata)))
			return (0);
		if(type == FT_CHDIR)
		{
			b = strlen(function);
			for(a = 1; a < b; a++)
				if(function[a] == '!')
					break;
			strncpy(buf2, &function[1], a - 1);
			sprintf(tbuf, "CD %s\n", buf2);
			IDOS->Write(funcdata->output_file, tbuf, strlen(tbuf));
			return (1);
		}
	}
	retval = 1;

	if(!function[0])
	{
		if(type != FT_AREXX)
		{
			ptr = name;
			while (*ptr)
			{
				if(*ptr == ';')
					*ptr = '\n';
				++ptr;
			}
			sprintf(tbuf, "%s\n", name);
			IDOS->Write(funcdata->output_file, tbuf, strlen(tbuf));
		}
	}
	else
	{
		parserunline(function, (STRPTR)buf);
		ptr = buf;
		while (*ptr)
		{
			if(*ptr == ';')
				*ptr = '\n';
			++ptr;
		}
		len = strlen((STRPTR)buf);
		count = funcdata->function_count;
		endarg = usearg = funcdata->arg_use;

		FOREVER
		{
			if(status_haveaborted)
			{
				if(type != FT_AREXX)
					closescriptfile(NULL, 0, funcdata);
				myabort();
				retval = 0;
				break;
			}
			moretodo = 0;
			if(!(buildcustfunc((STRPTR)buf, len, buf2, &moretodo, par->which & FLAG_RECURSE, par->which & FLAG_RELOAD, ((par->which & FLAG_NOQUOTE) ? 0 : 1), funcdata)))
			{
				if(type != FT_AREXX)
					closescriptfile(NULL, 0, funcdata);
				retval = 0;
				break;
			}
			if(type == FT_AREXX)
				rexx_command(buf2, NULL);
			else
			{
				if(type == FT_BATCH)
					strcpy(tbuf, "Execute ");
				else if(type == FT_WORKBENCH)
					sprintf(tbuf, "\"%s\" -r ", str_dopusrt_path);
				else
					tbuf[0] = 0;
				IDOS->Write(funcdata->output_file, tbuf, strlen(tbuf));

				sprintf(tbuf, "%s\n", buf2);
				IDOS->Write(funcdata->output_file, tbuf, strlen(tbuf));
			}
			if(moretodo && (funcdata->entry_first || funcdata->file_request.filearray))
				goto domorestuff;
			if(!funcdata->recursive_path)
				break;
		      domorestuff:
			funcdata->function_count = -1;
			endarg = funcdata->arg_use;
			if(usearg)
				funcdata->arg_use = usearg->next;
			if(!funcdata->arg_use)
				funcdata->arg_use = usearg ? usearg : funcdata->arg_first;
		}
		funcdata->function_count = count;
		funcdata->arg_use = endarg;
		if(funcdata->activewin > -1)
			funcdata->entry_first = checkalltot(dopus_curwin[funcdata->activewin]);
	}
	if(funcdata->file_request.filearray)
	{
		IDOpus->LFreeRemember(&funcdata->file_request.filearraykey);
		funcdata->file_request.filearray = NULL;
	}
	return (retval);
}

int buildcustfunc(STRPTR function, int line_len, char *buffer, int *moretodo, int star, int reload, int quote, struct function_data *funcdata)
{
	char buf3[256], *ptr, filebuf[FILEBUF_SIZE], dirbuf[2048] = { 0, }, *spath, defbuf[256], titlebuf[80];
	int a, pos, bufpos = 0, d, f, sblen, buftitpos, h, qad = quote * 2, def, tit;
	struct Directory *cust = NULL, dummy;
	struct args *arg;

	memset(buffer, 0, MAXCOMMANDLEN);
	memset(buf3, 0, 256);

	sblen = strlen(funcdata->source_path);

	for(pos = 0; pos < line_len; pos++)
	{
		switch (function[pos])
		{
		case FUNC_ONEFILE:
		case FUNC_ONEFILE_NO:
			if(!func_single_file[0])
			{
				if(status_iconified || status_flags & STATUS_FROMHOTKEY)
				{
					if(getdummyfile(&dummy, dirbuf))
						cust = &dummy;
					else
						return (0);
				}
				else
					cust = custgetfirst(funcdata);
				if(cust)
				{
					strcpy(funcdata->last_file, funcdata->source_path);
					IDOS->AddPart(funcdata->last_file, cust->name, 256);
					if(!funcdata->recursive_path && cust->type > 0)
					{
						if(star)
						{
							IDOpus->LFreeRemember(&rec_pathkey);
							funcdata->entry_current = cust;
							strncpy(buf3, funcdata->source_path, 256);
							strncat(buf3, cust->name, 256);
							if(recursedir(buf3, NULL, R_STARDIR, strlen(funcdata->source_path)))
								return (0);
							setdirsize(cust, dos_global_bytecount, funcdata->activewin);
							funcdata->recursive_path = rec_firstpath;
							goto addfile1;
						}
						else
						{
							bufpos += addfilename(buffer, cust->name, NULL, quote);
							if(function[pos] != FUNC_ONEFILE_NO)
							{
								custunselect(cust, reload, funcdata);
								custnextsel(&funcdata->entry_first);
							}
						}
					}
					else
					{
					      addfile1:
						if(funcdata->recursive_path)
						{
							bufpos += addfilename(buffer, funcdata->recursive_path->path, NULL, quote);
							if(!(funcdata->recursive_path = funcdata->recursive_path->next) && function[pos] != FUNC_ONEFILE_NO)
								custunselect(funcdata->entry_current, reload, funcdata);
						}
						else
						{
							bufpos += addfilename(buffer, cust->name, NULL, quote);
							if(function[pos] != FUNC_ONEFILE_NO)
							{
								custunselect(cust, reload, funcdata);
								custnextsel(&funcdata->entry_first);
							}
						}
					}
				}
			}
			else
			{
				strcpy(funcdata->last_file, func_single_file);
				bufpos += addfilename(buffer, func_single_file, NULL, quote);
			}
			break;

		case FUNC_ALLFILES:
		case FUNC_ALLFILES_NO:
			if(!func_single_file[0])
			{
				a = d = strlen(buffer);
				if(status_iconified || status_flags & STATUS_FROMHOTKEY)
				{
					if(getdummyfile(&dummy, dirbuf))
						cust = &dummy;
					else
						return (0);
				}
				else
					cust = custgetfirst(funcdata);
				while (a < (MAXCOMMANDLEN - 1) && (cust || funcdata->recursive_path))
				{
					if(funcdata->recursive_path || (cust->selected && cust->type != 0))
					{
						if(!funcdata->recursive_path && cust->type > 0)
						{
							if(star)
							{
								IDOpus->LFreeRemember(&rec_pathkey);
								funcdata->entry_current = cust;
								strncpy(buf3, funcdata->source_path, 256);
								strncat(buf3, cust->name, 256);
								if(recursedir(buf3, NULL, R_STARDIR, strlen(funcdata->source_path)))
									return (0);
								setdirsize(cust, dos_global_bytecount, funcdata->activewin);
								funcdata->recursive_path = rec_firstpath;
								goto addfile2;
							}
							else
							{
								if((a + strlen(cust->name) + qad) > (MAXCOMMANDLEN - 1))
									break;
								addfilename(buffer, cust->name, NULL, quote);
								if(function[pos] != FUNC_ALLFILES_NO)
									custunselect(cust, reload, funcdata);
							}
						}
						else
						{
						      addfile2:
							if(funcdata->recursive_path)
							{
								if((a + strlen(funcdata->recursive_path->path) + qad) > (MAXCOMMANDLEN - 1))
									break;
								addfilename(buffer, funcdata->recursive_path->path, NULL, quote);
								if(!(funcdata->recursive_path = funcdata->recursive_path->next))
								{
									if(function[pos] != FUNC_ALLFILES_NO)
										custunselect(funcdata->entry_current, reload, funcdata);
								}
							}
							else
							{
								if((a + strlen(cust->name) + qad) > (MAXCOMMANDLEN - 1))
									break;
								addfilename(buffer, cust->name, NULL, quote);
								if(function[pos] != FUNC_ALLFILES_NO)
									custunselect(cust, reload, funcdata);
							}
						}
						strncat(buffer, " ", MAXCOMMANDLEN);
						a = strlen(buffer);
					}
					if(!funcdata->recursive_path)
					{
						if(cust)
							cust = cust->next;
					}
				}
				if(!funcdata->recursive_path)
				{
					custnextsel(&funcdata->entry_first);
				}
				bufpos += a - d;
				if(moretodo)
					*moretodo = 1;
			}
			else
			{
				bufpos += addfilename(buffer, func_single_file, NULL, quote);
				strncat(buffer, " ", MAXCOMMANDLEN);
				++bufpos;
			}
			break;

		case FUNC_ONEPATH:
		case FUNC_ONEPATH_NO:
			if(!func_single_file[0])
			{
				if(status_iconified || status_flags & STATUS_FROMHOTKEY)
				{
					if(getdummyfile(&dummy, dirbuf))
						cust = &dummy;
					else
						return (0);
					IDOpus->TackOn(dirbuf, NULL, 256);
					spath = dirbuf;
				}
				else
				{
					cust = custgetfirst(funcdata);
					spath = funcdata->source_path;
				}
				if(cust)
				{
					strcpy(funcdata->last_file, spath);
					IDOS->AddPart(funcdata->last_file, cust->name, 256);
					if(!funcdata->recursive_path && cust->type > 0)
					{
						if(star)
						{
							IDOpus->LFreeRemember(&rec_pathkey);
							funcdata->entry_current = cust;
							strncpy(buf3, spath, 256);
							strncat(buf3, cust->name, 256);
							if(recursedir(buf3, NULL, R_STARDIR, 0))
								return (0);
							setdirsize(cust, dos_global_bytecount, funcdata->activewin);
							funcdata->recursive_path = rec_firstpath;
							goto addfile3;
						}
						else
						{
							bufpos += addfilename(buffer, spath, cust->name, quote);
							if(function[pos] != FUNC_ONEPATH_NO)
							{
								custunselect(cust, reload, funcdata);
								custnextsel(&funcdata->entry_first);
							}
						}
					}
					else
					{
					      addfile3:
						if(funcdata->recursive_path)
						{
							bufpos += addfilename(buffer, funcdata->recursive_path->path, NULL, quote);
							funcdata->recursive_path = funcdata->recursive_path->next;
							if((funcdata->recursive_path == NULL) && (function[pos] != FUNC_ONEPATH_NO))
								custunselect(funcdata->entry_current, reload, funcdata);
						}
						else
						{
							bufpos += addfilename(buffer, spath, cust->name, quote);
							if(function[pos] != FUNC_ONEPATH_NO)
							{
								custunselect(cust, reload, funcdata);
								custnextsel(&funcdata->entry_first);
							}
						}
					}
				}
			}
			else
			{
				bufpos += addfilename(buffer, funcdata->source_path, func_single_file, quote);
				strcpy(funcdata->last_file, funcdata->source_path);
				IDOS->AddPart(funcdata->last_file, func_single_file, 256);
			}
			break;

		case FUNC_ALLPATHS:
		case FUNC_ALLPATHS_NO:
			if(!func_single_file[0])
			{
				a = d = strlen(buffer);
				if(status_iconified || status_flags & STATUS_FROMHOTKEY)
				{
					if(getdummyfile(&dummy, dirbuf))
						cust = &dummy;
					else
						return (0);
					IDOpus->TackOn(dirbuf, NULL, 256);
					spath = dirbuf;
				}
				else
				{
					cust = custgetfirst(funcdata);
					spath = funcdata->source_path;
				}
				while (a < MAXCOMMANDLEN
				       && (cust || funcdata->recursive_path))
				{
					if(funcdata->recursive_path || (cust->selected && cust->type != 0))
					{
						if(!funcdata->recursive_path && cust->type > 0)
						{
							if(star)
							{
								IDOpus->LFreeRemember(&rec_pathkey);
								funcdata->entry_current = cust;
								strncpy(buf3, spath, 256);
								strncat(buf3, cust->name, 256);
								if(recursedir(buf3, NULL, R_STARDIR, 0))
									return (0);
								setdirsize(cust, dos_global_bytecount, funcdata->activewin);
								funcdata->recursive_path = rec_firstpath;
								goto addfile4;
							}
							else
							{
								if((a + strlen(cust->name) + strlen(spath) + qad) > (MAXCOMMANDLEN - 1))
									break;
								addfilename(buffer, spath, cust->name, quote);
								if(function[pos] != FUNC_ALLPATHS_NO)
									custunselect(cust, reload, funcdata);
							}
						}
						else
						{
						      addfile4:
							if(funcdata->recursive_path)
							{
								if((a + strlen(funcdata->recursive_path->path) + qad) > (MAXCOMMANDLEN - 1))
									break;
								addfilename(buffer, funcdata->recursive_path->path, NULL, quote);
								if(!(funcdata->recursive_path = funcdata->recursive_path->next))
								{
									if(function[pos] != FUNC_ALLPATHS_NO)
										custunselect(funcdata->entry_current, reload, funcdata);
								}
							}
							else
							{
								if((a + strlen(cust->name) + strlen(spath) + qad) > (MAXCOMMANDLEN - 1))
									break;
								addfilename(buffer, spath, cust->name, quote);
								if(function[pos] != FUNC_ALLPATHS_NO)
									custunselect(cust, reload, funcdata);
							}
						}
						strncat(buffer, " ", MAXCOMMANDLEN);
						a = strlen(buffer);
					}
					if(!funcdata->recursive_path)
					{
						if(cust)
							cust = cust->next;
					}
				}
				if(!funcdata->recursive_path)
				{
					custnextsel(&funcdata->entry_first);
				}
				bufpos += a - d;
				if(moretodo)
					*moretodo = 1;
			}
			else
			{
				bufpos += addfilename(buffer, funcdata->source_path, func_single_file, quote);
				strncat(buffer, " ", MAXCOMMANDLEN);
				++bufpos;
			}
			break;

		case FUNC_SOURCE:
		case FUNC_SOURCE_RR:
			strcpy(buf3, funcdata->source_path);
			if((status_iconified || status_flags & STATUS_FROMHOTKEY))
			{
				if(getdummypath(buf3, STR_SELECT_SOURCE_DIR))
				{
					myabort();
					return (0);
				}
				strncat(buffer, buf3, MAXCOMMANDLEN);
				bufpos = strlen(buffer);
			}
			else
			{
				strncat(buffer, funcdata->source_path, MAXCOMMANDLEN);
				bufpos += sblen;
			}
		case FUNC_NOSOURCE_RR:
			if(function[pos] != FUNC_SOURCE)
				funcdata->rereadsource = 1;
			break;

		case FUNC_DEST:
		case FUNC_DEST_RR:
			if(!(check_dest_path(funcdata)))
				return (0);
			strncat(buffer, funcdata->dest_path, MAXCOMMANDLEN);
			bufpos += strlen(funcdata->dest_path);
		case FUNC_NODEST_RR:
			if(function[pos] != FUNC_DEST)
				funcdata->rereaddest = 1;
			break;

		case FUNC_SCREENNAME:
			strncat(buffer, str_arexx_portname, MAXCOMMANDLEN);
			bufpos += strlen(str_arexx_portname);
			break;

		case FUNC_QUERYINFO:
			++pos;
			{
				char *data = NULL;

				switch (function[pos])
				{

					/* Public screen name */
				case 's':
					data = get_our_pubscreen();
					break;

					/* ARexx port name */
				case 'p':
					data = str_arexx_portname;
					break;

					/* Last result */
				case 'r':
					data = str_last_rexx_result;
					break;
				}

				if(data && data[0])
				{
					strncat(buffer, data, MAXCOMMANDLEN);
					bufpos += strlen(data);
				}
			}
			break;

		case FUNC_VARIABLE:
			tit = 0;

			for(pos++; pos < line_len; pos++)
			{
				if(function[pos] == FUNC_ENDARG)
					break;
				if(tit < 79)
					titlebuf[tit++] = function[pos];
			}
			titlebuf[tit] = 0;

			if((a = IDOS->GetVar(titlebuf, buf3, 256, 0)) > 0)
			{
				strncat(buffer, buf3, MAXCOMMANDLEN);
				bufpos += a;
			}
			break;

		case FUNC_GETARG:
			if(funcdata->function_count && funcdata->arg_use)
			{
			      use_old_arg:
				strncat(buffer, funcdata->arg_use->argstring, MAXCOMMANDLEN);
				bufpos += strlen(funcdata->arg_use->argstring);
				funcdata->arg_use = funcdata->arg_use->next;
				for(f = pos + 1; f < line_len && function[f] != FUNC_ENDARG; f++);
				pos = f;
				break;
			}
		case FUNC_STDARG:
			if(funcdata->function_count == -1 && funcdata->arg_use)
				goto use_old_arg;
			buftitpos = -1;
			def = 0;
			tit = 0;

			for(f = pos + 1; f < line_len; f++)
			{
				if(function[f] == FUNC_ENDARG)
					break;
				else if(buftitpos == -1)
				{
					if(function[f] == ':')
						buftitpos = f + 1;
					else if(tit < 79)
						titlebuf[tit++] = function[f];
				}
				else if(def < 255)
					defbuf[def++] = function[f];
			}
			titlebuf[tit] = 0;
			defbuf[def] = 0;

			if(!titlebuf[0])
				ptr = globstring[STR_ENTER_ARGUMENTS];
			else
				ptr = titlebuf;

			buf3[0] = 0;
			if(buftitpos > -1)
				build_default_string(defbuf, buf3, funcdata->last_file, funcdata->source_path, funcdata->dest_path);

			if(!(whatsit(ptr, 256, buf3, NULL)))
			{
				myabort();
				return (0);
			}

			if(function[pos] == FUNC_GETARG)
			{
				if(!(arg = IDOpus->LAllocRemember(&funcdata->arg_memkey, sizeof(struct args), MEMF_CLEAR)))
					return (0);
				if(!funcdata->arg_first)
					funcdata->arg_first = arg;
				else
					funcdata->arg_current->next = arg;
				funcdata->arg_current = arg;
				strcpy(funcdata->arg_current->argstring, buf3);
			}
			strncat(buffer, buf3, MAXCOMMANDLEN);
			bufpos += strlen(buf3);
			pos = f;
			break;

		case FUNC_REQUESTER:
			if(funcdata->file_request.filearray)
			{
			      domultifiles:
				while (bufpos < MAXCOMMANDLEN)
				{
					if(funcdata->file_request.filearray[funcdata->fileargpos][0])
					{
						if((bufpos + strlen(funcdata->file_request.filearray[funcdata->fileargpos]) + qad) < MAXCOMMANDLEN)
						{
							bufpos += addfilename(buffer, funcdata->file_request.filearray[funcdata->fileargpos], NULL, quote);
							strncat(buffer, " ", MAXCOMMANDLEN);
							++bufpos;
							++funcdata->fileargpos;
						}
						else
						{
							if(moretodo)
								*moretodo = 1;
							break;
						}
					}
					if(!funcdata->file_request.filearray[funcdata->fileargpos][0])
					{
						IDOpus->LFreeRemember(&funcdata->file_request.filearraykey);
						funcdata->file_request.filearray = NULL;
						break;
					}
				}
				for(f = pos + 1; f < line_len; f++)
					if(function[f] == FUNC_ENDARG || function[f] == 0)
						break;
				pos = f;
				break;
			}
			h = -1;
			switch (function[pos + 1])
			{
			case 'f':
				h = 0;
				break;
			case 'F':
				h = DFRF_MULTI;
				break;
			case 'd':
				h = DFRF_DIRREQ;
				break;
			case 'o':
				h = DFRF_FONT;
				break;
			}
			if(h == -1)
				break;
			++pos;
			buftitpos = -1;
			for(f = pos + 1; f < line_len; f++)
			{
				if(function[f] == ':' && buftitpos == -1)
				{
					function[f] = 0;
					buftitpos = f + 1;
				}
				if(function[f] == FUNC_ENDARG)
				{
					function[f] = 0;
					break;
				}
			}
			if(!function[(pos + 1)])
				funcdata->file_request.title = globstring[STR_FILE_REQUEST];
			else
				funcdata->file_request.title = (char *)function + pos + 1;
			buf3[0] = 0;
			if(buftitpos > -1)
				build_default_string(&function[buftitpos], buf3, funcdata->last_file, funcdata->source_path, funcdata->dest_path);
			else if(h & DFRF_FONT)
				strcpy(buf3, "FONTS:");
			funcdata->file_request.dirbuf = buf3;
			funcdata->file_request.filebuf = filebuf;
			filebuf[0] = 0;
			if(!(h & DFRF_DIRREQ))
			{
				ptr = IDOS->FilePart(buf3);
				if(ptr > buf3)
				{
					if(ptr[(strlen(ptr) - 1)] != '/')
					{
						strcpy(filebuf, ptr);
						*ptr = 0;
					}
				}
			}
			funcdata->file_request.flags = h;
			if(IDOpus->FileRequest(&funcdata->file_request))
			{
				if(h & DFRF_MULTI)
				{
					funcdata->fileargpos = 0;
					goto domultifiles;
				}
				if(!funcdata->file_request.dirbuf[0])
					expand_path("", funcdata->file_request.dirbuf);
				if(!(h & DFRF_DIRREQ))
				{
					if(quote)
						strncat(buffer, "\"", MAXCOMMANDLEN);
					strncat(buffer, funcdata->file_request.dirbuf, MAXCOMMANDLEN);
					IDOS->AddPart(buffer, filebuf, MAXCOMMANDLEN);
					if(quote)
						strncat(buffer, "\"", MAXCOMMANDLEN);
				}
				else
					strncat(buffer, funcdata->file_request.dirbuf, MAXCOMMANDLEN);
				bufpos = strlen(buffer);
			}
			else
			{
				myabort();
				return (0);
			}
			pos = f;
			break;
		default:
			buffer[bufpos++] = function[pos];
			break;
		}
		if(bufpos == MAXCOMMANDLEN)
			break;
	}
	buffer[MAXCOMMANDLEN - 1] = 0;
	return (1);
}

int addfilename(char *buf, char *part1, char *part2, int quote)
{
	int c = 0, d;

	d = strlen(buf);
	if(d > 0 && !(isspace(buf[d - 1])))
		quote = 0;

	if(quote)
	{
		strncat(buf, "\"", MAXCOMMANDLEN);
		++c;
	}
	if(part1)
	{
		strncat(buf, part1, MAXCOMMANDLEN);
		c += strlen(part1);
	}
	if(part2)
	{
		strncat(buf, part2, MAXCOMMANDLEN);
		c += strlen(part2);
	}
	if(quote)
	{
		strncat(buf, "\"", MAXCOMMANDLEN);
		++c;
	}
	return (c);
}

void parserunline(STRPTR buf, STRPTR buf1)
{
	int a, b, c, d;

	a = strlen(buf);
	memset(buf1, 0, 256);
	c = 0;
	for(b = 0; b < a; b++)
	{
		if(c == 256)
			break;
		if(buf[b] == '{')
		{
			if((b + 2) >= a)
			{
				buf1[c] = buf[b];
				++c;
				continue;
			}
			if(buf[(b + 1)] == '{')
			{
				buf1[c] = buf[b];
				++b;
				++c;
				continue;
			}

			switch (buf[(b + 1)])
			{
			case 'a':
				buf1[c] = FUNC_GETARG;
			case 'A':
				if(buf[(b + 1)] == 'A')
					buf1[c] = FUNC_STDARG;
			case 'v':
				if(buf[(b + 1)] == 'v')
					buf1[c] = FUNC_VARIABLE;
			case 'R':
				if(buf[(b + 1)] == 'R')
				{
					if(buf[(b + 2)] == 's')
					{
						buf1[c] = FUNC_GETARG;
						++b;
					}
					else if(buf[(b + 2)] == 'S')
					{
						buf1[c] = FUNC_STDARG;
						++b;
					}
					else
						buf1[c] = FUNC_REQUESTER;
				}
				++c;
				for(d = (b + 2); d < a; d++)
				{
					if(buf[d] == '}')
						break;
					buf1[c] = buf[d];
					++c;
					if(c == 255)
						break;
				}
				buf1[c++] = FUNC_ENDARG;
				b = d;
				continue;

			case 'Q':
				buf1[c++] = FUNC_QUERYINFO;
				buf1[c++] = buf[b + 2];
				b += 3;
				continue;
			}

			switch (buf[(b + 2)])
			{
			case 'r':
				if(buf[(b + 1)] == 'd')
					buf1[c] = FUNC_DEST_RR;
				else if(buf[(b + 1)] == 's')
					buf1[c] = FUNC_SOURCE_RR;
				else if(buf[(b + 1)] == 'S')
					buf1[c] = FUNC_NOSOURCE_RR;
				else if(buf[(b + 1)] == 'D')
					buf1[c] = FUNC_NODEST_RR;
				b += 3;
				++c;
				continue;

			case 'u':
				if(buf[(b + 1)] == 'o')
					buf1[c] = FUNC_ONEFILE_NO;
				else if(buf[(b + 1)] == 'O')
					buf1[c] = FUNC_ALLFILES_NO;
				else if(buf[(b + 1)] == 'f')
					buf1[c] = FUNC_ONEPATH_NO;
				else if(buf[(b + 1)] == 'F')
					buf1[c] = FUNC_ALLPATHS_NO;
				b += 3;
				++c;
				continue;

			case '}':
				switch (buf[(b + 1)])
				{
				case 'o':
					buf1[c] = FUNC_ONEFILE;
					break;
				case 'O':
					buf1[c] = FUNC_ALLFILES;
					break;
				case 's':
					buf1[c] = FUNC_SOURCE;
					break;
				case 'd':
					buf1[c] = FUNC_DEST;
					break;
				case 'f':
					buf1[c] = FUNC_ONEPATH;
					break;
				case 'F':
					buf1[c] = FUNC_ALLPATHS;
					break;
				case 'p':
					buf1[c] = FUNC_SCREENNAME;
					break;
				}
				b += 2;
				++c;
				continue;
			}
		}
		buf1[c] = buf[b];
		++c;
		if(c == 256)
			break;
	}
}

void custunselect(struct Directory *dir, int rel, struct function_data *funcdata)
{
	struct recpath *temp;
	int win;

	win = funcdata->activewin;

	if(!status_iconified && !(status_flags & STATUS_FROMHOTKEY) && !func_external_file[0])
	{
		if(rel && dir->name[0])
		{
			if((temp = IDOpus->LAllocRemember(&funcdata->reload_memkey, sizeof(struct recpath), MEMF_CLEAR)) && (temp->path = IDOpus->LAllocRemember(&funcdata->reload_memkey, FILEBUF_SIZE, MEMF_CLEAR)))
			{
				strcpy(temp->path, dir->name);
				if(funcdata->reload_current)
					funcdata->reload_current->next = temp;
				funcdata->reload_current = temp;
				if(!funcdata->reload_first)
					funcdata->reload_first = temp;
				temp->next = NULL;
			}
		}
		unselect(win, dir);
	}
}

void doreloadfiles(struct function_data *funcdata)
{
	char buf[256];
	struct recpath *rel;
	int off;

	rel = funcdata->reload_first;
	off = dopus_curwin[funcdata->activewin]->offset;
	while (rel)
	{
		strncpy(buf, funcdata->source_path, 256);
		strncat(buf, rel->path, 256);
		reload_file(funcdata->activewin, buf);
		rel = rel->next;
	}
	dopus_curwin[funcdata->activewin]->offset = off;
	refreshwindow(funcdata->activewin, 0);
}

struct Directory *reload_file(int win, char *name)
{
	struct ExamineData *data = NULL;
	struct Directory *cust, *ret = NULL;

	if((data = IDOS->ExamineObjectTags(EX_StringName, name, TAG_END)))
	{
		if((cust = findfile(dopus_curwin[win], data->Name, NULL)))
		{
			removefile(cust, dopus_curwin[win], win, FALSE);
		}
		ret = (struct Directory *)addfile(dopus_curwin[win], win, data->Name, data->FileSize, returntype(data), &(data->Date), data->Comment, data->Protection, 0, FALSE, NULL, NULL, data->OwnerUID, data->OwnerGID);
		IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
	}
	else
	{
		if((cust = findfile(dopus_curwin[win], IDOS->FilePart(name), NULL)))
			removefile(cust, dopus_curwin[win], win, FALSE);
	}

	return (ret);
}

int openscriptfile(struct dopusfuncpar *par, struct function_data *funcdata)
{
	char buf[2048], buf2[2048];
	int a;
	struct ExamineData *data = NULL;
	APTR context = NULL;
	BPTR tlock = 0;

	if(funcdata->output_file)
	{
		return (1);
	}

	if(par->which & FLAG_CDSOURCE && !funcdata->source_path[0])
	{
		if(!(simplerequest(globstring[STR_NO_SOURCE_SELECTED], globstring[STR_CONTINUE], str_cancelstring, NULL)))
		{
			myabort();
			return (0);
		}
	}
	else if(par->which & FLAG_CDDEST && !funcdata->dest_path[0])
	{
		if(!(simplerequest(globstring[STR_NO_DESTINATION_SELECTED], globstring[STR_CONTINUE], str_cancelstring, NULL)))
		{
			myabort();
			return (0);
		}
	}

	makereselect(&func_reselection, funcdata->activewin);

	IDOS->SetProcWindow((APTR)-1L);

	if((tlock = IDOS->Lock("T:", SHARED_LOCK)))
	{
		IUtility->Strlcpy(buf, "T:", 2048);
		IDOS->UnLock(tlock);
	}
	else
	{
		IUtility->Strlcpy(buf, "RAM:", 2048);
	}

	if((context = IDOS->ObtainDirContextTags(EX_StringName, buf, TAG_END)))
	{
		while((data = IDOS->ExamineDir(context)))
		{
			if(EXD_IS_FILE(data))
			{
				if(IUtility->Strnicmp(data->Name, "dopustemp", 9) == 0)
				{
					IDOS->DeleteFile(data->Name);
				}
			}
		}
		IDOS->ReleaseDirContext(context);
	}

	if(config->errorflags & ERROR_ENABLE_DOS)
	{
		IDOS->SetProcWindow(Window);
	}

	for(a = 0; a < 100; a++)
	{
		sprintf(funcdata->scriptname, "%sdopustemp.tmp%d", buf, a);
		if((funcdata->output_file = IDOS->Open(funcdata->scriptname, MODE_NEWFILE)))
		{
			break;
		}
	}
	if(!funcdata->output_file)
	{
		doerror(-1);
		return (0);
	}

	sprintf(funcdata->tempfile, "%sdopusout.tmp%d", buf, a);

	rec_pathkey = NULL;

	sprintf(buf, "\"%s\" -s\n", str_dopusrt_path);
	IDOS->Write(funcdata->output_file, buf, strlen(buf));

	if(par->which & FLAG_SHELLUP)
	{
		if(config->shellstartup[0])
		{
			strncpy(buf, "S:", 40);
			strncat(buf, config->shellstartup, 40);
		}
		else
		{
			strcpy(buf, "S:Shell-Startup");
		}

		if((data = IDOS->ExamineObjectTags(EX_StringName, buf, TAG_END)))
		{
			sprintf(buf2, "Execute %s\n", buf);
			IDOS->Write(funcdata->output_file, buf2, strlen(buf2));
			IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		}
	}
	if(par->which & FLAG_DOPUSUP)
	{
		if((data = IDOS->ExamineObjectTags(EX_StringName, "S:DOpus-Startup", TAG_END)))
		{
			IDOS->Write(funcdata->output_file, "Execute S:DOpus-Startup\n", 24);
			IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		}
		else if((data = IDOS->ExamineObjectTags(EX_StringName, "S:DOpusShell-Startup", TAG_END)))
		{
			IDOS->Write(funcdata->output_file, "Execute S:DOpusShell-Startup\n", 29);
			IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		}
	}
	IDOS->Write(funcdata->output_file, "FailAt 999999\n", 14);

	buf[0] = 0;
	if(status_flags & STATUS_FROMHOTKEY)
	{
		expand_path("", buf2);
		sprintf(buf, "CD \"%s\"\n", buf2);
	}
	else
	{
		if(par->which & FLAG_CDSOURCE && funcdata->source_path[0])
			sprintf(buf, "CD \"%s\"\n", funcdata->source_path);
		else if(par->which & FLAG_CDDEST)
		{
			if(!(check_dest_path(funcdata)))
			{
				return (0);
			}
			sprintf(buf, "CD \"%s\"\n", funcdata->dest_path);
		}
	}
	if(buf[0])
		IDOS->Write(funcdata->output_file, buf, strlen(buf));

	sprintf(buf, "Stack %d\n", (par->stack < 4000) ? 4000 : par->stack);
	IDOS->Write(funcdata->output_file, buf, strlen(buf));

	if(par->pri != 0)
	{
		sprintf(buf, "ChangeTaskPri %d\n", par->pri);
		IDOS->Write(funcdata->output_file, buf, strlen(buf));
	}
	return (1);
}

int closescriptfile(struct dopusfuncpar *par, int run, struct function_data *funcdata)
{
	struct MsgPort *msgport;
	struct Message *msg;
	char buf[256], buf2[512], portname[50], pubname[140];
	int wb2f, setcust = 0, tnil = 0, otemp = 0, oldmodes = 0, okayflag = 0, bit, flags = 0;

	if(run > 0 && par && funcdata->output_file)
	{
		flags = par->which;
		if(flags & FLAG_OUTFILE && status_flags & STATUS_FROMHOTKEY)
		{
			flags &= ~FLAG_OUTFILE;
			flags |= FLAG_OUTWIND;
		}

		okayflag = 1;
		wb2f = flags & FLAG_WB2F;
		msgport = NULL;
		endnotifies();

		if(flags & FLAG_OUTWIND)
		{
			sprintf(buf2, "%s \"%s\" from %s", config->outputcmd, config->output, funcdata->scriptname);
			if(!(flags & FLAG_ASYNC))
			{
				sprintf(portname, "dopus_run%d", system_dopus_runcount);
				if(!(msgport = IExec->CreatePort(portname, 0)))
					goto freeargs;
			}
			else
				wb2f = 1;
		}
		else
			sprintf(buf2, "%sExecute %s", (flags & FLAG_ASYNC) ? "Run " : "", funcdata->scriptname);

		if(flags & FLAG_OUTWIND)
		{
			if(par->delay != 0)
			{
				sprintf(buf, "\"%s\" -w %d \"%s\"\n", str_dopusrt_path, par->delay, globstring[STR_PRESS_MOUSE_BUTTON]);
				IDOS->Write(funcdata->output_file, buf, strlen(buf));
			}
			if(msgport)
			{
				sprintf(buf, "\"%s\" -p %s\n", str_dopusrt_path, portname);
				IDOS->Write(funcdata->output_file, buf, strlen(buf));
			}
			IDOS->Write(funcdata->output_file, "EndCLI >NIL:\n", 13);
		}

		if(str_arcorgname[0])
		{
			sprintf(buf, "Delete \"%s\"\n", funcdata->last_file);

			IDOS->Write(funcdata->output_file, buf, strlen(buf));
			str_arcorgname[0] = 0;
		}

		IDOS->Close(funcdata->output_file);
		funcdata->output_file = 0;

		if(wb2f && (!(flags & FLAG_ASYNC) || !(flags & FLAG_DOPUSF)))
			IIntuition->WBenchToFront();
		if(flags & FLAG_OUTWIND && !wb2f && MainScreen)
		{
			oldmodes = IIntuition->SetPubScreenModes(SHANGHAI);
			IIntuition->GetDefaultPubScreen(pubname);
			IIntuition->SetDefaultPubScreen(str_arexx_portname);
			setcust = 1;
		}

		if(flags & FLAG_OUTFILE && (tnil = IDOS->Open(funcdata->tempfile, MODE_NEWFILE)))
			otemp = 1;
		if(!tnil)
			tnil = IDOS->Open("NIL:", MODE_NEWFILE);

		IDOS->SystemTags(buf2, SYS_Input, IDOS->Open("NIL:", MODE_OLDFILE), SYS_Output, tnil, SYS_Error, 0, SYS_Asynch, flags & FLAG_ASYNC, TAG_END);

		if(flags & FLAG_OUTWIND && !wb2f && MainScreen)
		{
			IIntuition->SetDefaultPubScreen(pubname);
			IIntuition->SetPubScreenModes(oldmodes);
			setcust = 0;
		}

		if(msgport && Window)
		{
			IExec->SetSignal(0, INPUTSIG_ABORT);
			FOREVER
			{
				if((bit = IExec->Wait(1 << msgport->mp_SigBit | rexx_signalbit | INPUTSIG_ABORT)) & INPUTSIG_ABORT)
				{
					status_justabort = status_haveaborted = 0;
					break;
				}
				if(bit & rexx_signalbit)
				{
					rexx_dispatch(0);
					continue;
				}
				if((msg = IExec->GetMsg(msgport)))
				{
					IExec->ReplyMsg(msg);
					break;
				}
			}
			IExec->DeletePort(msgport);
		}

		if(!(flags & FLAG_ASYNC) && tnil)
			IDOS->Close(tnil);

		if(setcust)
		{
			IIntuition->SetDefaultPubScreen(pubname);
			IIntuition->SetPubScreenModes(oldmodes);
		}

		if(flags & FLAG_DOPUSF)
		{
			if(MainScreen)
				IIntuition->ScreenToFront(MainScreen);
			if(Window && !Window->Flags & WFLG_BACKDROP)
				IIntuition->WindowToFront(Window);
		}

		if(!status_iconified && !(status_flags & STATUS_FROMHOTKEY))
		{
			if(funcdata->rereadsource || flags & FLAG_SCANSRCE)
			{
				startgetdir(funcdata->activewin, 0);
				funcdata->reload_first = NULL;
			}
			if(funcdata->rereaddest || flags & FLAG_SCANDEST)
				startgetdir(funcdata->inactivewin, 0);
			if(flags & FLAG_RELOAD && funcdata->reload_first)
				doreloadfiles(funcdata);
		}
	}

      freeargs:
	if(funcdata->output_file)
		IDOS->Close(funcdata->output_file);
	if((funcdata->output_file || okayflag) && funcdata->scriptname[0] && (!par || !(flags & FLAG_ASYNC)))
		IDOS->DeleteFile(funcdata->scriptname);
	funcdata->output_file = funcdata->scriptname[0] = 0;
	funcdata->rereaddest = funcdata->rereadsource = 0;

	IDOpus->LFreeRemember(&funcdata->arg_memkey);
	if(otemp)
	{
		viewfile(funcdata->tempfile, globstring[STR_TEMPORARY_OUTPUT_FILE], FUNC_SMARTREAD, NULL, /*NULL,*/ 1, 0);
		IDOS->DeleteFile(funcdata->tempfile);
	}
	IDOpus->LFreeRemember(&rec_pathkey);
	IDOpus->LFreeRemember(&funcdata->reload_memkey);

	if(Window && !(status_flags & STATUS_FROMHOTKEY))
		unbusy();
	if(status_flags & STATUS_VERIFYFAIL)
		myabort();
	else if(okayflag)
	{
		if(Window && !status_iconified && !(status_flags & STATUS_FROMHOTKEY))
			okay();
		if(flags & FLAG_ICONIFY && checkwindowquit())
			iconify(2, 0, 0);
		else
			startnotifies();
	}
	return (run);
}

int getdummyfile(struct Directory *fbuf,  STRPTR dirbuf)
{
	struct FileRequester *aslfreq;
	char file[FILEBUF_SIZE], buf[2048];

	if(!dirbuf[0])
	{
		expand_path("", dirbuf);
	}

	if((aslfreq = IAsl->AllocAslRequestTags(ASL_FileRequest, ASLFR_Window, Window, ASLFR_TitleText, globstring[STR_SELECT_A_FILE], ASLFR_InitialFile, file, ASLFR_InitialDrawer, dirbuf, TAG_END)))
	{
		if((IAsl->AslRequestTags(aslfreq, TAG_END)) == FALSE)
		{
			IAsl->FreeAslRequest(aslfreq);
			myabort();
			return (0);
		}
	}
	else
	{
		myabort();
		return (0);
	}

	if(!aslfreq->fr_Drawer[0])
	{
		expand_path("", dirbuf);
	}
	else
	{
		strncpy(dirbuf, aslfreq->fr_Drawer, 2048);
	}

	if(aslfreq->fr_File)
	{
		strncpy(buf, dirbuf, 2048);
		IDOS->AddPart(buf, aslfreq->fr_File, 2048);
		IAsl->FreeAslRequest(aslfreq);
		return (filloutdummy(buf, fbuf));
	}
	strcpy(func_single_file, aslfreq->fr_File);
	IAsl->FreeAslRequest(aslfreq);
	return (1);
}

int filloutdummy(char *name, struct Directory *fbuf)
{
	struct ExamineData *data = NULL;

	if((data = IDOS->ExamineObjectTags(EX_StringName, name, TAG_END)))
	{
		fbuf->last = fbuf->next = NULL;
		strcpy(fbuf->name, data->Name);
		fbuf->type = returntype(data);
		if(EXD_IS_FILE(data))
		{
			fbuf->size = data->FileSize;
		}
		else
		{
			fbuf->size = -1;
		}
		fbuf->subtype = 0;
		fbuf->protection = data->Protection;
		fbuf->comment = fbuf->dispstr = NULL;
		getprot(fbuf->protection, fbuf->protbuf);
		seedate(&data->Date, fbuf->datebuf, 1);
		IExec->CopyMem(&data->Date, &fbuf->date, sizeof(struct DateStamp));
		fbuf->selected = 1;

		IDOS->FreeDosObject(DOS_EXAMINEDATA, data);
		return 1;
	}

	return 0;
}

void do_title_string(char *string, char *buf, int ml, char *name)
{
	int a, b, c;

	a = strlen(string);
	c = 0;
	for(b = 0; b < a; b++)
	{
		if(string[b] == '%')
		{
			buf[c] = 0;
			if(name)
				strcat(buf, name);
			else
				addreqfilename(buf, data_active_window);
			c = strlen(buf);
		}
		else
		{
			if(ml && string[b] == '\\')
				buf[c] = '\n';
			else
				buf[c] = string[b];
			++c;
		}
		if(c == (256 + (ml * 262)))
			break;
	}
	buf[c] = 0;
}

void addreqfilename(char *buf, int win)
{
	struct Directory *dir;

	if(func_single_file[0])
		strcat(buf, func_single_file);
	else
	{
		dir = dopus_curwin[win]->firstentry;
		while (dir)
		{
			if(dir->selected && dir->type < 0)
				break;
			dir = dir->next;
		}
		if(dir && dir->selected && dir->type < 0)
			strcat(buf, dir->name);
	}
}

struct Directory *custgetfirst(struct function_data *funcdata)
{
	struct Directory *file;

	file = funcdata->entry_first;
	while(file)
	{
		if((file->type != 0) && (file->selected) && (file->type != ENTRY_CUSTOM))
			return (file);
		file = file->next;
	}
	return (NULL);
}

void custnextsel(struct Directory **dir)
{
	while(*dir)
	{
		if(!(*dir = (*dir)->next))
			break;
		if((*dir)->selected)
			break;
	}
}

int check_dest_path(struct function_data *funcdata)
{
	if(funcdata->dest_path[0])
		return (1);
	if((status_iconified || status_flags & STATUS_FROMHOTKEY) && ((getdummypath(funcdata->dest_path, STR_SELECT_DESTINATION_DIR)) || !funcdata->dest_path[0]))
	{
		myabort();
		return (0);
	}
	if(!funcdata->dest_path[0])
	{
		dostatustext(globstring[STR_NO_DESTINATION_SELECTED]);
		simplerequest(globstring[STR_NO_DESTINATION_SELECTED], globstring[STR_CONTINUE], NULL);
		return (0);
	}
	return (1);
}

int32 getdummypath(STRPTR dir, int title)
{
	struct FileRequester *asldreq;
	BOOL ret = FALSE;

	if(!dir[0])
	{
		expand_path("", dir);
	}

	if((asldreq = IAsl->AllocAslRequestTags(ASL_FileRequest, ASLFR_Window, Window, ASLFR_TitleText, globstring[title], ASLFR_InitialDrawer, dir, ASLFR_DrawersOnly, TRUE, TAG_END)))
	{
		if((ret = IAsl->AslRequestTags(asldreq, TAG_END)))
		{
			if(!asldreq->fr_Drawer[0])
			{
				expand_path("", dir);
			}
			else
			{
				strcpy(dir, asldreq->fr_Drawer);
			}
		}
		IAsl->FreeAslRequest(asldreq);
	}

	return ret;
}

void build_default_string(char *string, char *buffer, char *filename, char *sourcepath, char *destpath)
{
	int len, a, pos, flag;
	char *ptr;

	len = strlen(string);
	pos = 0;
	for(a = 0; a < len && pos < 255; a++)
	{
		if(string[a] == '[' && string[a + 2] == ']')
		{
			flag = 1;
			switch (string[a + 1])
			{
			case 'f':
				strncpy(&buffer[pos], filename, 255 - pos);
				break;
			case 'o':
				if((ptr = IDOS->FilePart(filename)))
					strncpy(&buffer[pos], ptr, 255 - pos);
				break;
			case 's':
				strncpy(&buffer[pos], sourcepath, 255 - pos);
				break;
			case 'd':
				strncpy(&buffer[pos], destpath, 255 - pos);
				break;
			default:
				flag = 0;
				break;
			}
			if(flag)
			{
				pos = strlen(buffer);
				a += 2;
				continue;
			}
		}
		buffer[pos++] = string[a];
	}
}
