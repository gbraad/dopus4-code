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

#include "print.h"

#define PD_SIZE 1
#define PD_DATE 2
#define PD_PROT 4
#define PD_COMM 8
#define PD_TYPE 16

#define ENTRY_FILE              -1
#define ENTRY_DEVICE             0
#define ENTRY_DIRECTORY          1
#define ENTRY_CUSTOM           999
#define CUSTOMENTRY_DIRTREE      1
#define CUSTOMENTRY_BUFFERLIST   2
#define CUSTOMENTRY_USER         3

enum
{
	PRINTDIR_SIZE,
	PRINTDIR_DATE,
	PRINTDIR_PROTECTION,
	PRINTDIR_COMMENT,
	PRINTDIR_FILETYPE,
	PRINTDIR_PRINTER,
	PRINTDIR_FILE,
	PRINTDIR_FILEREQUESTER,
	PRINTDIR_FILESTRING,
	PRINTDIR_PRINT,
	PRINTDIR_EXIT,

	PRINTDIR_NUM_GADGETS
};

struct TagItem printdir_size_gadget[] = {
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_CHECK},
	{RO_GadgetID, PRINTDIR_SIZE},
	{RO_Top, 1},
	{RO_TopFine, 4},
	{RO_LeftFine, 20},
	{RO_TextNum, STR_SIZE},
	{RO_TextPos, TEXTPOS_RIGHT},
	{RO_BoolOn, TRUE},
	{RO_ChkCenter, TRUE},
	{TAG_END, 0}
}, printdir_date_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_CHECK},
	{RO_GadgetID, PRINTDIR_DATE},
	{RO_Top, 2},
	{RO_TopFine, 9},
	{RO_LeftFine, 20},
	{RO_TextNum, STR_DATE},
	{RO_TextPos, TEXTPOS_RIGHT},
	{RO_BoolOn, TRUE},
	{RO_ChkCenter, TRUE},
	{TAG_END, 0}
}, printdir_protection_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_CHECK},
	{RO_GadgetID, PRINTDIR_PROTECTION},
	{RO_Top, 1},
	{RO_TopFine, 4},
	{RO_Left, 10},
	{RO_LeftFine, 71},
	{RO_TextNum, STR_PROTECTION},
	{RO_TextPos, TEXTPOS_RIGHT},
	{RO_BoolOn, TRUE},
	{RO_ChkCenter, TRUE},
	{TAG_END, 0}
}, printdir_comment_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_CHECK},
	{RO_GadgetID, PRINTDIR_COMMENT},
	{RO_Top, 2},
	{RO_TopFine, 9},
	{RO_Left, 10},
	{RO_LeftFine, 71},
	{RO_TextNum, STR_COMMENT},
	{RO_TextPos, TEXTPOS_RIGHT},
	{RO_ChkCenter, TRUE},
	{TAG_END, 0}
}, printdir_filetype_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_CHECK},
	{RO_GadgetID, PRINTDIR_FILETYPE},
	{RO_Top, 3},
	{RO_TopFine, 14},
	{RO_LeftFine, 20},
	{RO_TextNum, STR_FILETYPE},
	{RO_TextPos, TEXTPOS_RIGHT},
	{RO_ChkCenter, TRUE},
	{TAG_END, 0}
}, printdir_printer_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_RADIO},
	{RO_GadgetID, PRINTDIR_PRINTER},
	{RO_Top, 5},
	{RO_TopFine, 24},
	{RO_LeftFine, 22},
	{RO_TextNum, STR_PRINTER},
	{RO_TextPos, TEXTPOS_RIGHT},
	{RO_BoolOn, TRUE},
	{RO_ChkCenter, TRUE},
	{TAG_END, 0}
}, printdir_file_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_RADIO},
	{RO_GadgetID, PRINTDIR_FILE},
	{RO_Top, 6},
	{RO_TopFine, 28},
	{RO_LeftFine, 22},
	{RO_TextNum, STR_FILE},
	{RO_TextPos, TEXTPOS_RIGHT},
	{RO_ChkCenter, TRUE},
	{TAG_END, 0}
}, printdir_filerequester_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_TINYGLASS},
	{RO_GadgetID, PRINTDIR_FILEREQUESTER},
	{RO_Left, 8},
	{RO_LeftFine, 45},
	{RO_Top, 6},
	{RO_TopFine, 26},
	{RO_Height, 1},
	{RO_HeightFine, 4},
	{RO_WidthFine, 24},
	{RO_HighRecess, TRUE},
	{TAG_END, 0}
}, printdir_filestring_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_STRING},
	{RO_GadgetID, PRINTDIR_FILESTRING},
	{RO_Left, 8},
	{RO_LeftFine, 75},
	{RO_Top, 6},
	{RO_TopFine, 28},
	{RO_Width, 32},
	{RO_WidthFine, -59},
	{RO_Height, 1},
	{RO_StringLen, PATHBUF_SIZE},
	{TAG_END, 0}
}, printdir_print_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_BOOLEAN},
	{RO_GadgetID, PRINTDIR_PRINT},
	{RO_Top, 7},
	{RO_TopFine, 34},
	{RO_Width, 12},
	{RO_Height, 1},
	{RO_HeightFine, 4},
	{RO_TextNum, STR_PRINT},
	{RO_TextPos, TEXTPOS_CENTER},
	{RO_HighRecess, TRUE},
	{TAG_END, 0}
}, printdir_exit_gadget[] =
{
	{RO_Type, OBJECT_GADGET},
	{RO_GadgetType, GADGET_BOOLEAN},
	{RO_GadgetID, PRINTDIR_EXIT},
	{RO_Left, 28},
	{RO_LeftFine, 20},
	{RO_Top, 7},
	{RO_TopFine, 34},
	{RO_Width, 12},
	{RO_Height, 1},
	{RO_HeightFine, 4},
	{RO_TextNum, STR_EXIT},
	{RO_TextPos, TEXTPOS_CENTER},
	{RO_HighRecess, TRUE},
	{TAG_END, 0}
}, printdir_print_text[] =
{
	{RO_Type, OBJECT_TEXT},
	{RO_TopFine, 2},
	{RO_TextNum, STR_PRINT_DOT},
	{TAG_END, 0}
}, printdir_output_text[] =
{
	{RO_Type, OBJECT_TEXT},
	{RO_Top, 4},
	{RO_TopFine, 21},
	{RO_TextNum, STR_OUTPUT_TO},
	{TAG_END, 0}
}, *printdir_gadgets[] =
{
	printdir_size_gadget, printdir_date_gadget, printdir_protection_gadget, printdir_comment_gadget, printdir_filetype_gadget, printdir_printer_gadget, printdir_file_gadget, printdir_filerequester_gadget, printdir_filestring_gadget, printdir_print_gadget, printdir_exit_gadget, NULL
};

int printdir_string(struct RequesterBase *, int, char *);
void printdirectory(struct RequesterBase *reqbase, char *portname, int flags, char *output, int wind);
void save_printdir_env(int flags, int printer, char *filename);
void read_printdir_env(int *flags, int *printer, char *filename);

void do_printdir(struct VisInfo *vis, char *portname, int wind)
{
	struct RequesterBase printdirreq;
	struct IntuiMessage *msg;
	struct Window *window;
	struct Gadget *gadgets[PRINTDIR_NUM_GADGETS];
	ULONG class;
	USHORT code, gadgetid;
	APTR iaddress;
	struct StringExtend stringex;
	struct Requester busyreq;
	char *printdir_filestring_buffer;
	int a, printflags;

	printdirreq.rb_width = 40;
	printdirreq.rb_height = 8;
	printdirreq.rb_flags = 0;

	fill_out_req(&printdirreq, vis);

	printdirreq.rb_privateflags = 0;
	printdirreq.rb_screenname = NULL;

	if(printdirreq.rb_screen && !(vis->vi_flags & VISF_BORDERS))
	{
		printdirreq.rb_flags |= RBF_STRINGS;
		printdirreq.rb_title = NULL;
	}
	else
	{
		printdirreq.rb_flags |= RBF_BORDERS | RBF_CLOSEGAD | RBF_STRINGS;
		printdirreq.rb_title = string_table[STR_PRINTDIR];
	}

	printdirreq.rb_widthfine = 45;
	printdirreq.rb_heightfine = 45;
	printdirreq.rb_leftoffset = 13;
	printdirreq.rb_topoffset = 3;

	printdirreq.rb_extend = &stringex;
	printdirreq.rb_idcmpflags = 0;
	printdirreq.rb_string_table = string_table;

	for(a = 0; a < 2; a++)
	{
		stringex.Pens[a] = vis->vi_stringcol[a];
		stringex.ActivePens[a] = vis->vi_activestringcol[a];
	}
	stringex.InitialModes = 0;
	stringex.EditHook = NULL;
	stringex.WorkBuffer = NULL;
	for(a = 0; a < 4; a++)
		stringex.Reserved[a] = 0;

	if(!(window = IDOpus->OpenDORequester(&printdirreq)) ||
	   !(addreqgadgets(&printdirreq, printdir_gadgets, gadgets)) ||
	   !(IDOpus->AddRequesterObject(&printdirreq, printdir_print_text)) ||
	   !(IDOpus->AddRequesterObject(&printdirreq, printdir_output_text)))
	{
		IDOpus->CloseRequester(&printdirreq);
		return;
	}

	IDOpus->RefreshRequesterObject(&printdirreq, NULL);

	printdir_filestring_buffer = ((struct StringInfo *)gadgets[PRINTDIR_FILESTRING]->SpecialInfo)->Buffer;

	read_printdir_env(&printflags, &a, printdir_filestring_buffer);
	if(!a)
	{
		gadgets[PRINTDIR_PRINTER]->Flags &= ~GFLG_SELECTED;
		gadgets[PRINTDIR_FILE]->Flags |= GFLG_SELECTED;
	}
	for(a = 0; a < 5; a++)
	{
		if(printflags & (1 << a))
			gadgets[a]->Flags |= GFLG_SELECTED;
		else
			gadgets[a]->Flags &= ~GFLG_SELECTED;
	}

	IIntuition->RefreshGList(gadgets[0], window, NULL, -1);

	for(;;)
	{
		while((msg = (struct IntuiMessage *)IExec->GetMsg(window->UserPort)))
		{
			class = msg->Class;
			code = msg->Code;
			iaddress = msg->IAddress;
			IExec->ReplyMsg((struct Message *)msg);

			switch (class)
			{
			case IDCMP_GADGETDOWN:
				gadgetid = ((struct Gadget *)iaddress)->GadgetID;

				switch (gadgetid)
				{

				case PRINTDIR_PRINTER:
					gadgets[PRINTDIR_PRINTER]->Flags |= GFLG_SELECTED;
					gadgets[PRINTDIR_FILE]->Flags &= ~GFLG_SELECTED;
					IIntuition->RefreshGList(gadgets[PRINTDIR_PRINTER], window, NULL, 2);
					break;

				case PRINTDIR_FILE:
					gadgets[PRINTDIR_PRINTER]->Flags &= ~GFLG_SELECTED;
					gadgets[PRINTDIR_FILE]->Flags |= GFLG_SELECTED;
					IIntuition->RefreshGList(gadgets[PRINTDIR_PRINTER], window, NULL, 2);
					break;
				}
				break;

			case IDCMP_GADGETUP:
				gadgetid = ((struct Gadget *)iaddress)->GadgetID;

				switch (gadgetid)
				{

				case PRINTDIR_FILEREQUESTER:
					if(!(get_file_byrequest(gadgets[PRINTDIR_FILESTRING], window, 1)))
						break;
				case PRINTDIR_FILESTRING:
					IDOpus->RefreshStrGad(gadgets[PRINTDIR_FILESTRING], window);
					if(gadgets[PRINTDIR_PRINTER]->Flags & GFLG_SELECTED)
					{
						gadgets[PRINTDIR_PRINTER]->Flags &= ~GFLG_SELECTED;
						gadgets[PRINTDIR_FILE]->Flags |= GFLG_SELECTED;
						IIntuition->RefreshGList(gadgets[PRINTDIR_PRINTER], window, NULL, 2);
					}
					break;

				      startprint:
				case PRINTDIR_PRINT:
					printflags = 0;

					for(a = 0; a < 5; a++)
					{
						if(gadgets[a]->Flags & GFLG_SELECTED)
							printflags |= 1 << a;
					}

					IIntuition->InitRequester(&busyreq);
					busyreq.Flags = NOISYREQ;
					IIntuition->Request(&busyreq, window);
					IDOpus->SetBusyPointer(window);

					printdirectory(&printdirreq, portname, printflags, (gadgets[PRINTDIR_PRINTER]->Flags & GFLG_SELECTED) ? "PRT:" : printdir_filestring_buffer, wind);
					gadgetid = PRINTDIR_EXIT;

					IIntuition->EndRequest(&busyreq, window);

				case PRINTDIR_EXIT:

					code = 0x1b;
					break;
				}
				if(gadgetid != PRINTDIR_EXIT)
					break;

			case IDCMP_VANILLAKEY:
				if(code != 0x1b)
				{
					if(code == '\r')
						goto startprint;
					else
						break;
				}
			case IDCMP_CLOSEWINDOW:

				IDOpus->SetBusyPointer(window);
				printflags = 0;
				for(a = 0; a < 5; a++)
				{
					if(gadgets[a]->Flags & GFLG_SELECTED)
						printflags |= 1 << a;
				}
				save_printdir_env(printflags, (gadgets[PRINTDIR_PRINTER]->Flags & GFLG_SELECTED) ? 1 : 0, printdir_filestring_buffer);

				IDOpus->CloseRequester(&printdirreq);
				return;
			}
		}
		IExec->Wait(1 << window->UserPort->mp_SigBit);
	}
}

void printdirectory(struct RequesterBase *reqbase, char *portname, int flags, char *output, int wind)
{
	struct PrintDirData pddata;
	int bufsize = 256, pbufsize = 128;
	char buf[bufsize], pbuf[pbufsize], *s, *d;
	int out = 0, abort = 0, a;

	pddata.win = wind;
	if(!(a = dopus_message(DOPUSMSG_GETPRINTDIR, (APTR)&pddata, portname)))
		return;

	while(!out)
	{
		if((out = IDOS->Open(output, MODE_NEWFILE)))
			break;
		if(!(check_error(reqbase, string_table[STR_UNABLE_TO_OPEN_OUTPUT], 0)))
			return;
	}

	if(pddata.entry)
	{
		if(!(printdir_string(reqbase, out, pddata.titlebuf)))
		{
			IDOS->Close(out);
			return;
		}

		while(pddata.entry)
		{
			if(check_print_abort(reqbase))
			{
				abort = 1;
				break;
			}
			if(!pddata.onlysel || pddata.entry->selected)
			{
				switch (pddata.entry->type)
				{
				case ENTRY_CUSTOM:
					if(pddata.entry->comment && !(printdir_string(reqbase, out, pddata.entry->comment)))
						abort = 1;
					break;
				case ENTRY_DEVICE:
					snprintf(buf, bufsize, "%-32s", pddata.entry->name);
					if(pddata.entry->comment)
						strlcat(buf, pddata.entry->comment, bufsize);
					if(!(printdir_string(reqbase, out, buf)))
						abort = 1;
					break;
				default:
					a = pddata.namelen;
					s = pddata.entry->name;
					d = buf;
					while(*s && a)
					{
						*d++ = *s++;
						a--;
					}
					while(a--)
						*d++ = ' ';
					*d = 0;

					if(flags & PD_SIZE)
					{
						if(pddata.entry->type <= ENTRY_FILE)
						{
							snprintf(pbuf, pbufsize, "%9qd ", pddata.entry->size);
							strlcat(buf, pbuf, bufsize);
						}
						else if(pddata.entry->size != -1)
						{
							snprintf(pbuf, pbufsize, "[%8qd]", pddata.entry->size);
							strlcat(buf, pbuf, bufsize);
						}
						else
							strlcat(buf, "[     Dir]", bufsize);
					}
					if(flags & PD_PROT)
					{
						strlcat(buf, pddata.entry->protbuf, bufsize);
						strlcat(buf, " ", bufsize);
					}
					if(flags & PD_DATE)
					{
						strlcat(buf, pddata.entry->datebuf, bufsize);
						strlcat(buf, " ", bufsize);
					}
					if(flags & PD_TYPE && pddata.entry->description)
					{
						strlcat(buf, "(", bufsize);
						strlcat(buf, pddata.entry->description, bufsize);
						strlcat(buf, ")", bufsize);
					}
					if(!(printdir_string(reqbase, out, buf)))
					{
						abort = 1;
						break;
					}
					if(flags & PD_COMM && pddata.entry->comment)
					{
						IDOpus->StrCombine(buf, ": ", pddata.entry->comment, bufsize);
						if(!(printdir_string(reqbase, out, buf)))
							abort = 1;
					}
					break;
				}
			}
			if(abort)
				break;
			pddata.entry = pddata.entry->next;
		}
		if(!abort)
			IDOS->Write(out, "\n", 1);
	}
	IDOS->Close(out);
}

int printdir_string(struct RequesterBase *reqbase, int file, char *string)
{
	int len;

	for(;;)
	{
		if((IDOS->Write(file, string, (len = strlen(string)))) == len)
		{
			IDOS->Write(file, "\n", 1);
			return (1);
		}
		if(!(check_error(reqbase, string_table[STR_ERROR_PRINTING_DIR], 0)))
			return (0);
	}
}

void save_printdir_env(int flags, int printer, char *filename)
{
	int file;

	if((file = IDOS->Open("env:DOpus_printdir.prefs", MODE_NEWFILE)))
	{
		IDOS->Write(file, (char *)&flags, sizeof(int));
		IDOS->Write(file, (char *)&printer, sizeof(int));
		IDOS->Write(file, filename, strlen(filename) + 1);
		IDOS->Close(file);
	}
}

void read_printdir_env(int *flags, int *printer, char *filename)
{
	int file;

	if((file = IDOS->Open("env:DOpus_printdir.prefs", MODE_OLDFILE)))
	{
		IDOS->Read(file, (char *)flags, sizeof(int));
		IDOS->Read(file, (char *)printer, sizeof(int));
		IDOS->Read(file, filename, FILEBUF_SIZE);
		IDOS->Close(file);
	}
}
