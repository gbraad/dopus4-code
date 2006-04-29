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

#include "dopus.h"

struct makedirlist
{
	struct makedirlist *last, *next;
	char *path;
};

struct recurse
{
	struct recurse *last;
	char *dir, *dest;
	int data;
	APTR data2;
	APTR data3;
	BPTR lock;
	struct FileInfoBlock info;
	struct Directory *entry;
	struct DirectoryWindow lister;
};

int copymakedir(struct DOpusRemember **, struct makedirlist **, char *, struct FileInfoBlock *);

struct recurse *current_recurse;

int recursedir(STRPTR fdir, STRPTR fdest, int dowhat, int fdata)
{
	struct FileInfoBlock *myfinfo = IDOS->AllocDosObject(DOS_FIB, NULL);
	struct FileInfoBlock *enfinfo = IExec->AllocVec(sizeof(struct FileInfoBlock), MEMF_CLEAR); //IDOS->AllocDosObject(DOS_FIB, NULL);
	BPTR mylock;
	char *name, *dir, *dest, *dname, *ddir, *adir, *adest, *ndir, *ndest;
	int suc, to_do, ret = 0, a, err, adata, depth = 0, b, rtry, data = fdata, *pstuff, blocks;
	struct recpath *crec = NULL, *trec;
	struct RecursiveDirectory *cur_recurse, *addparent_recurse, *new_rec, *pos_rec, *cur_parent, *cur_lastparent = NULL;
	APTR data2 = NULL, adata2 = NULL, data3 = NULL, adata3 = NULL;
	struct DOpusRemember *memkey = NULL;
	struct makedirlist *first_makedir = NULL;
	struct DirectoryWindow lister;
	struct Directory *entry = NULL;

	if(dowhat & R_STARDIR)
	{
		rec_firstpath = NULL;
		rec_pathkey = NULL;
	}

	if(dowhat & R_GETNAMES)
	{
		first_recurse = NULL;
		recurse_dir_key = NULL;
		cur_recurse = NULL;
		addparent_recurse = NULL;
	}

	current_recurse = NULL;

	recurse_max_depth = 0;

	dos_global_bytecount = 0;
	dos_global_copiedbytes = 0;
	dos_global_deletedbytes = 0;
	dos_global_blocksneeded = 0;
	dos_global_files = 0;

	if(dopus_curwin[data_active_window]->xai)
	{
		lister = *(dopus_curwin[data_active_window]);
		for(entry = lister.firstentry; entry && (!(entry->selected));)
			entry = entry->next;
		arcfillfib(myfinfo, entry);

		mylock = 0; //NULL;
	}
	else
	{
		if(!(mylock = IDOS->Lock(fdir, ACCESS_READ)))
		{
			doerror(-1);
			return (-1);
		}
		IDOS->Examine(mylock, myfinfo);
	}
	if(!(name = IDOpus->LAllocRemember(&memkey, 5 * 512, MEMF_CLEAR)))
	{
		doerror(-1);
		return (-1);
	}
	dir = name + 512;
	dest = dir + 512;
	dname = dest + 512;
	ddir = dname + 512;
	if(fdir)
		strcpy(dir, fdir);
	if(fdest)
		strcpy(dest, fdest);

	if(dowhat & R_COPY)
	{
		strcpy(ddir, dest);
		if(!(a = copymakedir(&memkey, &first_makedir, dest, myfinfo)) || a == -1)
		{
			IDOS->UnLock(mylock);
			ret = a ? -10 : -3;
			goto goaway;
		}
	}
	if(mylock)
	{
		to_do = IDOS->ExNext(mylock, myfinfo);
	}
	else
	{
		lister.firstentry = lister.firstfile = lister.firstdir = NULL;
		lister.total = lister.filesel = lister.dirsel = lister.dirtot = lister.filetot = 0;
		lister.bytessel = 0;
		lister.bytestot = 0;
		strcpy(lister.directory, dir);
		strcat(lister.directory, "/");
		readarchive(&lister, 0);
		arcfillfib(myfinfo, entry = lister.firstentry);
		to_do = lister.total;
	}

	for(;;)
	{
		if(status_haveaborted)
		{
			myabort();
			ret = -10;
			break;
		}
		if(!to_do)
		{
			if(current_recurse)
			{
				IDOS->UnLock(mylock);
				strcpy(dname, dir);
				mylock = current_recurse->lock;
				if((entry = current_recurse->entry))
				{
					struct Directory *t_entry;

					lister = current_recurse->lister;
					for(to_do = lister.total, t_entry = lister.firstentry; t_entry != entry; to_do--, t_entry = t_entry->next);
				}
				myfinfo = &current_recurse->info;
				strcpy(dir, current_recurse->dir);
				strcpy(dest, current_recurse->dest);
				data = current_recurse->data;
				data2 = current_recurse->data2;
				data3 = current_recurse->data3;
				current_recurse = current_recurse->last;

				strcpy(name, dir);
				IDOpus->TackOn(name, myfinfo->fib_FileName, 512);

				if(dowhat & R_GETNAMES)
				{
					cur_recurse = (struct RecursiveDirectory *)data2;
					cur_lastparent = (struct RecursiveDirectory *)data3;
					addparent_recurse = NULL;
				}

				else if(dowhat & R_COPY)
				{
					strcpy(ddir, dest);
					if(config->copyflags & COPY_DATE)
					{
						IDOpus->TackOn(ddir, myfinfo->fib_FileName, 512);
						setdate(ddir, &myfinfo->fib_Date);
						strcpy(ddir, dest);
					}
				}

				a = 0;
				if(dowhat & R_COMMENT)
				{
					FOREVER
					{
						if(!(IDOS->SetComment(name, dest)))
						{
							doerror(err = IDOS->IoErr());
							a = checkerror(globstring[STR_COMMENTING], myfinfo->fib_FileName, err);
							if(a == 1)
								continue;
						}
						break;
					}
					if(a == 3)
						break;
				}

				else if(dowhat & R_PROTECT)
				{
					pstuff = (int *)data;
					b = getnewprot(myfinfo->fib_Protection, pstuff[0], pstuff[1]);
					FOREVER
					{
						if(!(IDOS->SetProtection(name, b)))
						{
							doerror(err = IDOS->IoErr());
							a = checkerror(globstring[STR_PROTECTING], myfinfo->fib_FileName, err);
							if(a == 1)
								continue;
						}
						break;
					}
					if(a == 3)
						break;
				}

				else if(dowhat & R_DATESTAMP)
				{
					FOREVER
					{
						if((err = setdate(name, (struct DateStamp *)data)) != 1)
						{
							doerror(err);
							a = checkerror(globstring[STR_DATESTAMPING], myfinfo->fib_FileName, err);
							if(a == 1)
								continue;
						}
						break;
					}
					if(a == 3)
						break;
				}

				if(mylock)
					to_do = IDOS->ExNext(mylock, myfinfo);
				else
				{
					arcfillfib(myfinfo, entry = entry->next);
					to_do--;
				}
				if(dowhat & R_DELETE && depth > 0)
				{
					a = strlen(dname);
					if(a > 0 && dname[a - 1] == '/')
						dname[a - 1] = 0;
					if((a = delfile(dname, IDOS->FilePart(dname), globstring[STR_DELETING], glob_unprotect_all, 1)) == -1)
					{
						myabort();
						ret = -10;
						break;
					}
					if(a == 2)
						glob_unprotect_all = 1;
				}
				--depth;
				continue;
			}
			else
			{
				break;
			}
		}
		IExec->CopyMemQuick((char *)myfinfo, (char *)enfinfo, sizeof(struct FileInfoBlock));

		strcpy(name, dir);
		IDOpus->TackOn(name, enfinfo->fib_FileName, 512);

		if(FIB_IS_DRAWER(enfinfo))
		{
			++dos_global_blocksneeded;
			if(!FIB_IS_LINKDIR(enfinfo))
			{
				adir = NULL;
				dofilename(name);
				if(dowhat & R_GETNAMES)
				{
					if((new_rec = IDOpus->LAllocRemember(&recurse_dir_key, sizeof(struct RecursiveDirectory), MEMF_CLEAR)))
					{
						strcpy(new_rec->name, enfinfo->fib_FileName);
						IExec->CopyMem((char *)&enfinfo->fib_Date, (char *)&new_rec->date, sizeof(struct DateStamp));
						if(addparent_recurse)
						{
							addparent_recurse->child = new_rec;
							cur_lastparent = addparent_recurse;
							cur_recurse = new_rec;
						}
						else if(!first_recurse)
						{
							first_recurse = new_rec;
							cur_recurse = new_rec;
						}
						else if(cur_recurse)
						{
							if(strcmp(new_rec->name, cur_recurse->name) < 0)
							{
								if(first_recurse == cur_recurse)
									first_recurse = new_rec;
								if(cur_lastparent && cur_lastparent->child == cur_recurse)
									cur_lastparent->child = new_rec;
								new_rec->next = cur_recurse;
								cur_recurse = new_rec;
							}
							else
							{
								pos_rec = cur_recurse;
								FOREVER
								{
									if(!pos_rec->next || (strcmp(new_rec->name, pos_rec->next->name) < 0))
									{
										new_rec->next = pos_rec->next;
										pos_rec->next = new_rec;
										break;
									}
									pos_rec = pos_rec->next;
								}
							}
						}
						else
							cur_recurse = new_rec;
						cur_parent = new_rec;
					}
					adir = dir;
					adest = dest;
					ndir = name;
					ndest = dest;
					adata2 = (APTR) cur_recurse;
					adata3 = (APTR) cur_lastparent;
					addparent_recurse = cur_parent;
				}
				strcpy(dname, dest);
				if(dowhat & R_COPY)
				{

					dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, -2, 0, enfinfo->fib_FileName, 1);

					IDOpus->TackOn(dname, enfinfo->fib_FileName, 512);
					adir = dir;
					adest = dest;
					adata = data;
					ndir = name;
					ndest = dname;
					strcpy(ddir, dname);
					if((a = copymakedir(&memkey, &first_makedir, ddir, enfinfo)) == -1)
					{
						ret = -10;
						break;
					}
					else if(a == 0)
						adir = NULL;
				}
				if(dowhat & R_DELETE)
				{
					rtry = 1;
				      delloop:
					if(!IDOS->DeleteFile(name))
					{
						if(config->deleteflags & 8 && rtry == 1 && IDOS->IoErr() == 222)
						{
							rtry = 0;
							IDOS->SetProtection(name, 0);
							goto delloop;
						}
						adir = dir;
						adest = dest;
						adata = data;
						ndir = name;
						ndest = dname;
					}
					else
					{
						if(config->dynamicflags & UPDATE_FREE)
							seename(data_active_window);
						adir = NULL;
					}
				}
				else if(dowhat & (R_HUNT | R_SEARCH | R_COMMENT | R_PROTECT | R_DATESTAMP | R_GETBYTES | R_STARDIR))
				{
					adir = dir;
					adest = dest;
					adata = data;
					ndir = name;
					ndest = dest;
					adata2 = data2;
					adata3 = data3;
				}
				if(adir)
				{
					if(!(addrecurse(&memkey, adir, adest, adata, adata2, adata3, mylock, enfinfo, mylock ? NULL : entry, &lister)))
					{
						if(mylock)
							to_do = 0;
						continue;
					}
					strcpy(dir, ndir);
					strcpy(dest, ndest);
					if(mylock)
					{
						if(!(mylock = IDOS->Lock(dir, ACCESS_READ)))
						{
							to_do = 0;
							continue;
						}
						IDOS->Examine(mylock, myfinfo);
						to_do = IDOS->ExNext(mylock, myfinfo);
					}
					else
					{
						lister.firstentry = lister.firstfile = lister.firstdir = NULL;
						lister.total = lister.filesel = lister.dirsel = lister.dirtot = lister.filetot = 0;
						lister.bytessel = 0;
						lister.bytestot = 0;
						strcpy(lister.directory, dir);
						strcat(lister.directory, "/");
						readarchive(&lister, 0);
						arcfillfib(myfinfo, entry = lister.firstentry);
						to_do = lister.total;
					}
					++depth;
					if(depth > recurse_max_depth)
						recurse_max_depth = depth;
					continue;
				}
			}
		}
		if(FIB_IS_FILE(enfinfo))
		{
			a = 0;
			dos_global_bytecount += enfinfo->fib_Size;
			dos_global_files++;
			if(dowhat & R_GETBYTES && data)
			{
				blocks = (enfinfo->fib_Size + (data - 1)) / data;
				dos_global_blocksneeded += blocks + (blocks / 72) + 1;
			}

			if(dowhat & R_GETNAMES)
				goto skipgetnam;
			else if(dowhat & R_STARDIR)
			{
				if((trec = IDOpus->LAllocRemember(&rec_pathkey, sizeof(struct recpath), MEMF_CLEAR)) && (trec->path = IDOpus->LAllocRemember(&rec_pathkey, (strlen(name) + 1) - data, MEMF_CLEAR)))
				{
					trec->next = NULL;
					strcpy(trec->path, &name[data]);
					if(crec)
						crec->next = trec;
					crec = trec;
					if(!rec_firstpath)
						rec_firstpath = trec;
				}
			}
			if(!str_filter_parsed[0] || IDOS->MatchPatternNoCase(str_filter_parsed, enfinfo->fib_FileName))
			{
				if(dowhat & R_COPY)
				{
					strcpy(dname, ddir);
					IDOpus->TackOn(dname, enfinfo->fib_FileName, 512);

					dotaskmsg(hotkeymsg_port, PROGRESS_UPDATE, -2, 0, enfinfo->fib_FileName, 1);
					if(!mylock)
					{
						char tempname[FILEBUF_SIZE];

						strcpy(name, "T:");
						if(!unarcfiledir(&lister, name, tempname, enfinfo->fib_FileName))
							continue;
						IDOS->AddPart(name, tempname, 512);
					}
					a = 0;
					if(askeach)
					{
						if((a = checkexistreplace(name, dname, &enfinfo->fib_Date, 1, 1)) == REPLACE_ABORT)
						{
							myabort();
							ret = -10;
							break;
						}
						if(a == REPLACE_ALL)
						{
							askeach = 0;
						}
						else if(a == REPLACE_SKIPALL)
						{
							askeach = 0;
							autoskip = 1;
						}
					}
					if(!autoskip && (a != REPLACE_SKIP))
					{
						a = 0;
						FOREVER
						{
							if(!(a = copyfile(name, dname, &err, NULL, 0)))
							{
								doerror(err);
								a = checkerror(globstring[STR_COPYING], enfinfo->fib_FileName, err);
								if(a == 1)
									continue;
								if(a != 3)
									seename(data_active_window);
							}
							else if(a == -1)
							{
								myabort();
								a = 3;
								break;
							}
							else
							{
								if(dowhat == R_COPY)
								{
									if(config->copyflags & COPY_ARC && !(enfinfo->fib_Protection & FIBF_ARCHIVE))
										IDOS->SetProtection(name, enfinfo->fib_Protection | FIBF_ARCHIVE);
								}
								dos_global_copiedbytes += enfinfo->fib_Size;
							}
							if(config->dynamicflags & UPDATE_FREE)
								seename(data_active_window);
							break;
						}
						dotaskmsg(hotkeymsg_port, PROGRESS_INCREASE, 1, 0, NULL, 0);
						if(!mylock)
							removetemparcfile(name);
						if(a == 3)
						{
							ret = -10;
							break;
						}
						else if(config->dynamicflags & UPDATE_FREE)
						{
							seename(data_active_window);
							seename(1 - data_active_window);
						}
					}
					else if(!mylock)
						removetemparcfile(name);
				}
				if(dowhat & R_DELETE)
				{
					if(!((dowhat & R_COPY) && (a == 2)))
					{
						if((a = delfile(name, enfinfo->fib_FileName, globstring[STR_DELETING], glob_unprotect_all, 1)) == -1)
						{
							myabort();
							ret = -10;
							break;
						}
						if(a == 2)
							glob_unprotect_all = 1;
						if(config->dynamicflags & UPDATE_FREE)
							seename(data_active_window);
						dos_global_deletedbytes += enfinfo->fib_Size;
					}
				}
				else if(dowhat & R_COMMENT)
				{
					FOREVER
					{
						if(!(IDOS->SetComment(name, dest)))
						{
							doerror(err = IDOS->IoErr());
							a = checkerror(globstring[STR_COMMENTING], enfinfo->fib_FileName, err);
							if(a == 1)
								continue;
						}
						break;
					}
					if(a == 3)
					{
						ret = -10;
						break;
					}
				}
				else if(dowhat & R_PROTECT)
				{
					pstuff = (int *)data;
					b = getnewprot(enfinfo->fib_Protection, pstuff[0], pstuff[1]);
					FOREVER
					{
						if(!(IDOS->SetProtection(name, b)))
						{
							doerror(err = IDOS->IoErr());
							a = checkerror(globstring[STR_PROTECTING], enfinfo->fib_FileName, err);
							if(a == 1)
								continue;
						}
						break;
					}
					if(a == 3)
					{
						ret = -10;
						break;
					}
				}
				else if(dowhat & R_DATESTAMP)
				{
					for(;;)
					{
						if((err = setdate(name, (struct DateStamp *)data)) != 1)
						{
							doerror(err = IDOS->IoErr());
							a = checkerror(globstring[STR_DATESTAMPING], enfinfo->fib_FileName, err);
							if(a == 1)
								continue;
						}
						break;
					}
					if(a == 3)
					{
						ret = -10;
						break;
					}
				}
				else if(dowhat & R_HUNT)
				{
					suc = huntfile(enfinfo->fib_FileName, name, &a);
					ret += a;
					if(suc)
					{
						if(suc == -1)
						{
							myabort();
							ret = -10;
						}
						else
							ret = suc;
						break;
					}
				}
				else if(dowhat & R_SEARCH)
				{
					if(!mylock)
					{
						char tempname[FILEBUF_SIZE];

						strcpy(name, "T:");
						if(!unarcfiledir(&lister, name, tempname, enfinfo->fib_FileName))
							continue;
						IDOS->AddPart(name, tempname, 256);
					}
					suc = filesearch(name, &a, 0);
					if(!mylock)
						removetemparcfile(name);
					ret += a;
					busy();
					if(suc == 2)
					{
						if(!simplerequest(globstring[STR_CONTINUE_WITH_SEARCH], globstring[STR_CONTINUE], str_cancelstring, NULL))
						{
							okay();
							ret = -2;
							break;
						}
					}
				      skipgetnam:
					if(suc == -1)
					{
						myabort();
						ret = -10;
						break;
					}
				}
			}
		}
		if(mylock)
			to_do = IDOS->ExNext(mylock, myfinfo);
		else
		{
			arcfillfib(myfinfo, entry = entry->next);
			to_do--;
		}

	}
	IDOS->UnLock(mylock);
	while(current_recurse != NULL)
	{
		IDOS->UnLock(current_recurse->lock);
		current_recurse = current_recurse->last;
	}
	if(first_makedir && dowhat == R_COPY && str_filter_parsed[0])
	{
		struct makedirlist *last = first_makedir;

		while(last->next)
			last = last->next;

		while(last)
		{
			IDOS->DeleteFile(last->path);
			last = last->last;
		}
	}
      goaway:
	if(memkey)
		IDOpus->LFreeRemember(&memkey);

//	if(myfinfo)
//		IDOS->FreeDosObject(DOS_FIB, myfinfo);
	if(enfinfo)
		IExec->FreeVec(enfinfo);

	return (ret);
}

int addrecurse(struct DOpusRemember **key, char *dir, char *dest, int data, APTR data2, APTR data3, BPTR lock, struct FileInfoBlock *info, struct Directory *entry, struct DirectoryWindow *lister)
{
	struct recurse *rec;

	if(!(rec = IDOpus->LAllocRemember(key, sizeof(struct recurse), MEMF_ANY | MEMF_CLEAR)))
		return (0);
	rec->last = current_recurse;
	if(!(rec->dir = IDOpus->LAllocRemember(key, strlen(dir) + 1, MEMF_ANY | MEMF_CLEAR)) || !(rec->dest = IDOpus->LAllocRemember(key, strlen(dest) + 1, MEMF_ANY | MEMF_CLEAR)))
		return (0);
	strcpy(rec->dir, dir);
	strcpy(rec->dest, dest);
	rec->data = data;
	rec->data2 = data2;
	rec->data3 = data3;
	rec->lock = lock;
	rec->info = *info;
	rec->entry = entry;
	if(entry)
		rec->lister = *lister;
	current_recurse = rec;
	return (1);
}

int copymakedir(struct DOpusRemember **key, struct makedirlist **first, char *dirname, struct FileInfoBlock *finfo)
{
	int exist, a, err;
	BPTR mylock;

	exist = IDOpus->CheckExist(dirname, NULL);
	if(exist <= 0)
	{
	      loop:
		if(exist < 0 || !(mylock = IDOS->CreateDir(dirname)))
		{
			err = (exist < 0) ? 203 : IDOS->IoErr();
			doerror(err);
			a = checkerror(globstring[STR_CREATING], finfo->fib_FileName, err);
			if(a == 1)
				goto loop;
			if(a == 2)
				return (0);
			return (-1);
		}
		if(mylock)
		{
			struct makedirlist *list, *pos;

			IDOS->UnLock(mylock);

			if(config->copyflags & COPY_DATE)
				setdate(dirname, &finfo->fib_Date);
			if(config->copyflags & COPY_PROT)
				IDOS->SetProtection(dirname, finfo->fib_Protection & (~FIBF_ARCHIVE));
			if(config->copyflags & COPY_NOTE)
				IDOS->SetComment(dirname, finfo->fib_Comment);

			if((list = IDOpus->LAllocRemember(key, sizeof(struct makedirlist), MEMF_CLEAR)) && (list->path = IDOpus->LAllocRemember(key, strlen(dirname) + 1, 0)))
			{
				strcpy(list->path, dirname);
				if(!(pos = *first))
					*first = list;
				else
				{
					while(pos->next)
						pos = pos->next;
					pos->next = list;
					list->last = pos;
				}
			}
		}
	}
	return (1);
}

int getdircontentsinfo(STRPTR path, uint64 *size, uint32 *files)
{
	char *buf, *c;
	struct ExAllControl *eac;
	struct ExAllData *ead;
	BPTR lock;
	int more, ret = 1;

	if((lock = IDOS->Lock(path, ACCESS_READ)))
	{
		if((buf = IExec->AllocMem(1024, MEMF_ANY)))
		{
			if((eac = IDOS->AllocDosObject(DOS_EXALLCONTROL, NULL)))
			{
				eac->eac_LastKey = 0; //NULL;

				do
				{
					ead = (struct ExAllData *)buf;
					more = IDOS->ExAll(lock, ead, 1024, ED_SIZE, eac);
					if((!more) && (IDOS->IoErr() != ERROR_NO_MORE_ENTRIES))
					{
						ret = 0;
						break;
					}
					if(eac->eac_Entries == 0)
						continue;

					for(; ead; ead = ead->ed_Next)
					{
						if(ead->ed_Type > 0)
						{
							IDOS->AddPart(path, ead->ed_Name, 256);

							ret = getdircontentsinfo(path, size, files);

							c = IDOS->PathPart(path);
							if(c)
								*c = 0;
						}
						else
						{
							*size += ead->ed_Size;
							(*files)++;
						}
					}
				}
				while(more);

				IDOS->FreeDosObject(DOS_EXALLCONTROL, eac);
			}
			else
				ret = 0;
			IExec->FreeMem(buf, 1024);
		}
		else
			ret = 0;
		IDOS->UnLock(lock);
	}
	else
		ret = 0;
	return ret;
}
