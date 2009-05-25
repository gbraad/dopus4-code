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

enum
{
	MAKELINK_CANCEL,
	MAKELINK_OKAY,
	MAKELINK_NAME,
	MAKELINK_DESTNAME,
	MAKELINK_DESTSELECT,
	MAKELINK_TYPE
};

static struct RequesterBase makelink_req =
{
	46, 7, 48, 27, 8, 6, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, IDCMP_MOUSEBUTTONS | IDCMP_GADGETUP, NULL
};

static struct TagItem makelink_type_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_CYCLE },
	{ RO_GadgetID, MAKELINK_TYPE },
	{ RO_Top, 1 },
	{ RO_TopFine, 2 },
	{ RO_Width, 8 },
	{ RO_WidthFine, 24 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 6 },
	{ RO_HighRecess, TRUE },
	{ TAG_END, 0 }
}, makelink_name_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_STRING },
	{ RO_GadgetID, MAKELINK_NAME },
	{ RO_Top, 1 },
	{ RO_TopFine, 4 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 2 },
	{ RO_Left, 8 },
	{ RO_LeftFine, 28 },
	{ RO_Width, 38 },
	{ RO_StringBuf, 0 },
	{ RO_StringLen, 107 },
	{ RO_StringUndo, (ULONG) str_undobuffer },
	{ TAG_END, 0 }
}, makelink_destname_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_STRING },
	{ RO_GadgetID, MAKELINK_DESTNAME },
	{ RO_Top, 4 },
	{ RO_TopFine, 8 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 2 },
//	{ RO_Left,8 },
	{ RO_LeftFine, 4 },
	{ RO_Width, 46 },
	{ RO_WidthFine, 2 },
	{ RO_StringBuf, 0 },
	{ RO_StringLen, 255 },
	{ RO_StringUndo, (ULONG) str_undobuffer },
	{ TAG_END, 0 }
}, makelink_glass_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_TINYGLASS },
	{ RO_GadgetID, MAKELINK_DESTSELECT },
	{ RO_Top, 4 },
	{ RO_TopFine, 6 },
	{ RO_Left, 47 },
	{ RO_LeftFine, 2 },
//	{ RO_Width,2 },
	{ RO_WidthFine, 24 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 6 },
	{ RO_HighRecess, TRUE },
	{ TAG_END, 0 }
}, makelink_okay_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_BOOLEAN },
	{ RO_GadgetID, MAKELINK_OKAY },
	{ RO_Top, 6 },
	{ RO_TopFine, 11 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 6 },
	{ RO_Width, 10 },
	{ RO_TextNum, STR_OKAY },
	{ RO_TextPos, TEXTPOS_CENTER },
	{ RO_HighRecess, TRUE },
	{ TAG_END, 0 }
}, makelink_cancel_gadget[] =
{
	{ RO_Type, OBJECT_GADGET },
	{ RO_GadgetType, GADGET_BOOLEAN },
	{ RO_GadgetID, MAKELINK_CANCEL },
	{ RO_Top, 6 },
	{ RO_TopFine, 11 },
	{ RO_Height, 1 },
	{ RO_HeightFine, 6 },
	{ RO_Width, 10 },
	{ RO_Left, 36 },
	{ RO_LeftFine, 32 },
	{ RO_TextNum, STR_CANCEL },
	{ RO_TextPos, TEXTPOS_CENTER },
	{ RO_HighRecess, TRUE },
	{ TAG_END, 0}
}, makelink_type_text[] =
{
	{ RO_Type, OBJECT_BORDER },
	{ RO_BorderType, BORDER_NONE },
	{ RO_Width, 8 },
	{ RO_WidthFine, 24 },
	{ RO_TopFine, -1 },
	{ RO_Height, 1 },
	{ RO_TextNum, STR_MAKELINK_TYPE },
	{ RO_TextPos, TEXTPOS_CENTER },
	{ TAG_END, 0 }
}, makelink_name_text[] =
{
	{ RO_Type, OBJECT_BORDER },
	{ RO_BorderType, BORDER_NONE },
	{ RO_Left, 8 },
	{ RO_LeftFine, 28 },
	{ RO_Width, 38 },
	{ RO_TopFine, -1 },
	{ RO_Height, 1 },
	{ RO_TextNum, STR_MAKELINK_NAME },
	{ RO_TextPos, TEXTPOS_CENTER },
	{ TAG_END, 0 }
}, makelink_dest_text[] =
{
	{ RO_Type, OBJECT_BORDER },
	{ RO_BorderType, BORDER_NONE },
	{ RO_Width, 46 },
	{ RO_WidthFine, 26 },
	{ RO_Top, 3 },
	{ RO_TopFine, 3 },
	{ RO_Height, 1 },
	{ RO_TextNum, STR_MAKELINK_DESTINATION },
	{ RO_TextPos, TEXTPOS_CENTER },
	{ TAG_END, 0 }
}, *makelink_gadgets[] =
{
	makelink_name_gadget, makelink_type_gadget, makelink_destname_gadget, makelink_glass_gadget, makelink_okay_gadget, makelink_cancel_gadget, NULL
};

int getmakelinkdata(char *namebuf, char *destbuf, int *type)
{
	ULONG class;
	USHORT gadgetid = 0, code;
	struct Window *swindow;
	struct Gadget *name_gad, *makelink_type_gad, *makelink_destname_gad;
	char *makelink_type_array[3] =
	{
		globstring[STR_MAKELINK_TYPE_SOFT],
		globstring[STR_MAKELINK_TYPE_HARD],
		NULL
	};
	char makelink_namebuf[108], makelink_destbuf[256];
	int makelink_type;
	int ret = 0;

	strcpy(makelink_namebuf, namebuf);
	strcpy(makelink_destbuf, destbuf);
	makelink_type = type ? 1 : 0;

	fix_requester(&makelink_req, globstring[STR_ENTER_MAKELINK_PARAMS]);

	set_reqobject(makelink_name_gadget, RO_StringBuf, (ULONG) makelink_namebuf);
	set_reqobject(makelink_destname_gadget, RO_StringBuf, (ULONG) makelink_destbuf);

	if(!(swindow = IDOpus->OpenDORequester(&makelink_req)) || !(name_gad = addreqgadgets(&makelink_req, makelink_gadgets, 0, NULL)) || !(IDOpus->AddRequesterObject(&makelink_req, makelink_type_text)) || !(IDOpus->AddRequesterObject(&makelink_req, makelink_name_text)) || !(IDOpus->AddRequesterObject(&makelink_req, makelink_dest_text)))
	{
		IDOpus->CloseRequester(&makelink_req);
		return (0);
	}
	IDOpus->RefreshRequesterObject(&makelink_req, NULL);

	makelink_type_gad = name_gad->NextGadget;
	makelink_destname_gad = makelink_type_gad->NextGadget;
	IDOpus->DoCycleGadget(makelink_type_gad, swindow, makelink_type_array, makelink_type);

	IDOpus->ActivateStrGad(name_gad, swindow);

	FOREVER
	{
		IExec->Wait(1 << swindow->UserPort->mp_SigBit);
		while((IMsg = (struct IntuiMessage *)IExec->GetMsg(swindow->UserPort)))
		{
			class = IMsg->Class;
			code = IMsg->Code;
			switch (class)
			{
			case IDCMP_GADGETUP:
				gadgetid = ((struct Gadget *)IMsg->IAddress)->GadgetID;
				break;
			}
			IExec->ReplyMsg((struct Message *)IMsg);

			switch (class)
			{
			case IDCMP_MOUSEBUTTONS:
				IDOpus->ActivateStrGad(name_gad, swindow);
				break;

			case IDCMP_GADGETUP:
				switch (gadgetid)
				{
				case MAKELINK_TYPE:
					if(++makelink_type > 1)
						makelink_type = 0;
					IDOpus->DoCycleGadget(makelink_type_gad, swindow, makelink_type_array, makelink_type);
					IDOpus->ActivateStrGad(name_gad, swindow);
					break;
				case MAKELINK_DESTSELECT:
					{
						struct FileRequester *aslreq;
						char dirbuf[1024], filebuf[FILEBUF_SIZE], *ptr;

//						IDOpus->LStrCpy(dirbuf, makelink_destbuf);
						strncpy(dirbuf, makelink_destbuf, 1024);
						if((ptr = IDOS->FilePart(dirbuf)) > dirbuf)
						{
//							IDOpus->LStrCpy(filebuf, ptr);
							strncpy(filebuf, ptr, FILEBUF_SIZE);
							*ptr = 0;
						}
						else if(IDOpus->CheckExist(dirbuf, NULL) < 1)
						{
							dirbuf[0] = 0;
//							IDOpus->LStrCpy(filebuf, makelink_destbuf);
							strncpy(filebuf, makelink_destbuf, FILEBUF_SIZE);
						}
						else
						{
							filebuf[0] = 0;
						}
						if((aslreq = IAsl->AllocAslRequestTags(ASL_FileRequest, ASLFR_Window, swindow, ASLFR_TitleText, globstring[STR_FILE_REQUEST], ASLFR_InitialFile, filebuf, ASLFR_InitialDrawer, dirbuf, TAG_END)))
						{
							if(IAsl->AslRequest(aslreq, NULL))
							{
//								IDOpus->LStrCpy(makelink_destbuf, aslreq->fr_Drawer);
								strncpy(makelink_destbuf, aslreq->fr_Drawer, 256);
								IDOS->AddPart(makelink_destbuf, aslreq->fr_File, 256);
							}
							IDOpus->RefreshStrGad(makelink_destname_gad, swindow);
							IDOpus->ActivateStrGad(makelink_destname_gad, swindow);
							IAsl->FreeAslRequest(aslreq);
						}
					}
					break;
				case MAKELINK_OKAY:
					ret = 1;
					strcpy(namebuf, str_pathbuffer[data_active_window]);
					IDOS->AddPart(namebuf, makelink_namebuf, 256);
					strcpy(destbuf, makelink_destbuf);
					*type = makelink_type;
				case MAKELINK_CANCEL:
					IDOpus->CloseRequester(&makelink_req);
					return (ret);
				}
				break;
			}
		}
	}
}

int makelink(int rexx)
{
	char name[256], path[2048];
	int mode;

	name[0] = path[0] = mode = 0;

	if(rexx)
	{
		strcpy(name, rexx_args[0]);
		strcpy(path, rexx_args[1]);
		if(atoi(rexx_args[2]))
			mode = 1;
	}
	else if(!(getmakelinkdata(name, path, &mode)))
	{
		return 0;
	}

	if(name[0] && path[0])
	{
		BPTR lock;

		if(mode)
		{
			if((lock = IDOS->Lock(path, ACCESS_READ)))
			{
				if(IDOS->MakeLink(name, lock, TRUE))
				{
					dostatustext(str_okaystring);
					return 1;
				}
			}
			doerror(-1);
			return 0;
		}
		else
		{
			if((lock = IDOS->Lock(path, ACCESS_READ)))
			{
				if(IDOS->MakeLink(name, lock, FALSE))
				{
					dostatustext(str_okaystring);
					return 1;
				}
			}
			doerror(-1);
			return 0;
		}
	}
	doerror(ERROR_REQUIRED_ARG_MISSING);
	return 0;
}
