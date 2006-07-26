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

#include <proto/alib.h>
#include <proto/muimaster.h>

#include "dopus.h"
#include "mui.h"

int LoadPic(CONST_STRPTR name)
{
	APTR app, win;

	app = ApplicationObject,
			MUIA_Application_NoIconify, TRUE,
			MUIA_Application_UseRexx, FALSE,
			SubWindow, win = WindowObject,
				MUIA_Window_UseBottomBorderScroller, TRUE,
				MUIA_Window_UseRightBorderScroller, TRUE,
				MUIA_Window_ShowAbout, FALSE,
				MUIA_Window_ShowIconify, FALSE,
				MUIA_Window_ShowJump, FALSE,
				MUIA_Window_ShowPopup, FALSE,
				MUIA_Window_ShowPrefs, FALSE,
				MUIA_Window_ShowSnapshot, FALSE,
				MUIA_Window_Title, name,

				WindowContents, VGroup,
					Child, ScrollgroupObject,
						MUIA_Scrollgroup_UseWinBorder, TRUE,
						MUIA_Scrollgroup_Contents, VirtgroupObject,
							Child, MUI_NewObject(MUIC_Dtpic, MUIA_Dtpic_Name, name, TAG_DONE),
						End,
					End,
				End,
			End,
		End;

	if (app)
	{
		ULONG ok;

		dostatustext(name);

		DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
		set(win, MUIA_Window_Open, TRUE);

		GetAttr(MUIA_Window_Open, win, &ok);

		if (ok)
		{
			ULONG sigs = 0;

			for (;;)
			{
				ULONG ret = DoMethod(app, MUIM_Application_NewInput, &sigs);

				if (ret == MUIV_Application_ReturnID_Quit)
				{
					break;
				}

				if (sigs)
					sigs = Wait(sigs);
			}
		}

		MUI_DisposeObject(app);
	}

	return (1);
}

int WaitForMouseClick(int tits, struct Window *wind)
{
	struct IntuiMessage *msg;
	ULONG class;
	USHORT code, qual;
	int ticks = 0, waitfor = 0, waitbits, retcode = 1;

	while((msg = (struct IntuiMessage *)GetMsg(wind->UserPort)))
		ReplyMsg((struct Message *)msg);
	waitbits = 1 << wind->UserPort->mp_SigBit;

	for(;;)
	{
		while((msg = (struct IntuiMessage *)GetMsg(wind->UserPort)))
		{
			class = msg->Class;
			code = msg->Code;
			qual = msg->Qualifier;
			ReplyMsg((struct Message *)msg);
			switch (class)
			{
			case IDCMP_INTUITICKS:
				if(waitfor && (++ticks) >= waitfor)
					retcode = 0;
				break;
			case IDCMP_RAWKEY:
				switch (code)
				{
				case 0x45:	// ESC
					retcode = -1;
					break;
				}
				break;
			case IDCMP_ACTIVEWINDOW:
				ModifyIDCMP(wind, (wind->IDCMPFlags & ~IDCMP_ACTIVEWINDOW) | IDCMP_INACTIVEWINDOW);
				break;
			case IDCMP_INACTIVEWINDOW:
				if(IntuitionBase->ActiveScreen != wind->WScreen)
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
		Wait(waitbits);
	}
	return (retcode);
}
