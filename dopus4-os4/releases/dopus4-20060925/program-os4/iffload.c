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

long min(long a, long b)
{
	return (a < b ? a : b);
}

int LoadPic(char *name)
{
	int retcode = 1;
	APTR dto = NULL;
	struct Screen *dtscreen = NULL;

	if(MainScreen)
	{
		dtscreen = MainScreen;
	}
	else
	{
		dtscreen = IIntuition->LockPubScreen(NULL);
	}

	if(dtscreen)
	{

		if(DataTypesBase && IDataTypes)
		{
			if((dto = IDataTypes->NewDTObject(name, DTA_GroupID, GID_PICTURE, PDTA_Remap, TRUE, PDTA_Screen, dtscreen, PDTA_DestMode,PMODE_V43, TAG_END)))
			{
				struct BitMapHeader *bmhd = NULL;
				struct BitMap *bm = NULL;
				int32 winw, winh, winx, winy;
				struct Window *dtwin = NULL;
				BOOL cont;
				struct IntuiMessage *mess;

				IDataTypes->DoDTMethod(dto, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);

				IDataTypes->GetDTAttrs(dto, PDTA_BitMapHeader, &bmhd, PDTA_DestBitMap, &bm, TAG_DONE);

				winw = min(dtscreen->Width, bmhd->bmh_Width);
				winh = min(dtscreen->Height, bmhd->bmh_Height);
				winx = (dtscreen->Width - winw) >> 1;
				winy = (dtscreen->Height - winh) >> 1;
				
				dostatustext(name);
				
				if((dtwin = IIntuition->OpenWindowTags(NULL, WA_CustomScreen, dtscreen, WA_Left, winx, WA_Top, winy, WA_InnerWidth, winw, WA_InnerHeight, winh, WA_Flags, WFLG_GIMMEZEROZERO | WFLG_ACTIVATE, WA_IDCMP, IDCMP_MOUSEBUTTONS | IDCMP_VANILLAKEY, TAG_END)))
				{
					IGraphics->BltBitMapRastPort(bm, 0, 0, dtwin->RPort, 0, 0, winw, winh, 0xC0);

					cont = TRUE;
					do
					{
						IExec->WaitPort(dtwin->UserPort);
						while((mess = (struct IntuiMessage *)IExec->GetMsg(dtwin->UserPort)))
						{
							switch (mess->Class)
							{
							case IDCMP_MOUSEBUTTONS:
								if(mess->Code == IECODE_LBUTTON)
									cont = FALSE;
								if(mess->Code == IECODE_RBUTTON)
									cont = FALSE;
								break;
							case IDCMP_VANILLAKEY:
								if(mess->Code == 0x1b)
									cont = FALSE;
								break;
							}
							IExec->ReplyMsg((struct Message *)mess);
						}
					}
					while(cont);

					IIntuition->CloseWindow(dtwin);
				}
				IDataTypes->DisposeDTObject(dto);
			}
		}
		if(dtscreen != MainScreen)
			IIntuition->UnlockPubScreen(NULL, dtscreen);
	}

	return (retcode);
}

int WaitForMouseClick(struct Window *wind)
{
	struct IntuiMessage *msg;
	ULONG class;
	USHORT code, qual;
	int ticks = 0, waitfor = 0, waitbits, retcode = 1;

	while((msg = (struct IntuiMessage *)IExec->GetMsg(wind->UserPort)))
	{
		IExec->ReplyMsg((struct Message *)msg);
	}
	waitbits = 1 << wind->UserPort->mp_SigBit;

	for(;;)
	{
		while((msg = (struct IntuiMessage *)IExec->GetMsg(wind->UserPort)))
		{
			class = msg->Class;
			code = msg->Code;
			qual = msg->Qualifier;
			IExec->ReplyMsg((struct Message *)msg);
			switch (class)
			{
			case IDCMP_INTUITICKS:
				if(waitfor && (++ticks) >= waitfor)
					retcode = 0;
				break;
			case IDCMP_RAWKEY:
				switch (code)
				{
				case RAWKEY_ESC:
					retcode = -1;
					break;
				}
				break;
			case IDCMP_ACTIVEWINDOW:
				IIntuition->ModifyIDCMP(wind, (wind->IDCMPFlags & ~IDCMP_ACTIVEWINDOW) | IDCMP_INACTIVEWINDOW);
				break;
			case IDCMP_INACTIVEWINDOW:
				if(((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveScreen != wind->WScreen)
				{
					retcode = -3;
					break;
				}
				code = SELECTDOWN;
			case IDCMP_MOUSEBUTTONS:
				if(code == SELECTDOWN)
					retcode = 0;
				else if(code == MENUDOWN)
					retcode = -1;
				break;
			}
			if(retcode != 1)
				break;
		}
		if(retcode != 1)
			break;
		IExec->Wait(waitbits);
	}
	return (retcode);
}
