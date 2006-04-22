/*
 *  $VER: FSSetMenuStrip.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 *  (C) Copyright 2006 Hyperion Entertainment
 *      All rights reserved
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <string.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/FSSetMenuStrip ******************************************
*
*   NAME
*      FSSetMenuStrip -- Description
*
*   SYNOPSIS
*      int FSSetMenuStrip(struct Window * window, struct Menu * menu);
*
*   FUNCTION
*
*   INPUTS
*       window - 
*       menu - 
*
*   RESULT
*       The result ...
*
*   EXAMPLE
*
*   NOTES
*
*   BUGS
*
*   SEE ALSO
*
*****************************************************************************
*
*/

int _DOpus_FSSetMenuStrip(struct DOpusIFace *Self, struct Window *window, struct Menu *firstmenu)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *IntuitionBase = IExec->OpenLibrary("intuition.library", 50L);
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	struct Menu *menu;
	struct MenuItem *item;
	struct DrawInfo *drinfo;
	int fpen, bpen;
	int y, offset, xpos = 0, iwidth, comwidth, checkwidth;
	struct IntuiText testtext;
	struct TextAttr testattr;

	comwidth = COMMWIDTH;
	checkwidth = CHECKWIDTH;

	IGraphics->AskFont(&window->WScreen->RastPort, &testattr);
	testtext.FrontPen = 1;
	testtext.DrawMode = JAM1;
	testtext.NextText = NULL;

	if((drinfo = IIntuition->GetScreenDrawInfo(window->WScreen)))
	{
		fpen = drinfo->dri_Pens[BARDETAILPEN];
		bpen = drinfo->dri_Pens[BARBLOCKPEN];
		if(drinfo->dri_AmigaKey)
			comwidth = drinfo->dri_AmigaKey->Width;
		if(drinfo->dri_CheckMark)
			checkwidth = drinfo->dri_CheckMark->Width;
		IIntuition->FreeScreenDrawInfo(window->WScreen,drinfo);
	}
	else
	{
		fpen = 0;
		bpen = 1;
	}

	offset = window->WScreen->Font->ta_YSize + 3;
	menu = firstmenu;
	while(menu)
	{
		if(xpos == 0)
			xpos = menu->LeftEdge;
		else
			menu->LeftEdge = xpos;
		menu->Width = IGraphics->TextLength(&window->WScreen->RastPort, menu->MenuName, strlen(menu->MenuName)) + window->WScreen->RastPort. Font->tf_XSize;
		xpos += menu->Width + 16;

		y = 0;
		iwidth = 0;
		item = menu->FirstItem;

		while(item)
		{
  			item->TopEdge = y;

  			if(item->Flags & ITEMTEXT)
			{
				struct IntuiText *text;
				int wid, mwidth = 0, first = 0;

				item->Height = offset - 1;

				text = (struct IntuiText *)item->ItemFill;
				while(text)
				{
					text->FrontPen = fpen;
					text->BackPen = bpen;

					if(first == 0)
					{
						if(Self->LStrnCmp((char *)text->IText, "   ", 3) == 0)
							text->IText = &text->IText[3];
						if(item->Flags & CHECKIT)
							text->LeftEdge = checkwidth;
						else
							text->LeftEdge = 0;
					}

					if(text->ITextFont)
						testtext.ITextFont = text->ITextFont;
					else
						testtext.ITextFont = &testattr;
					testtext.IText = text->IText;

					wid = text->LeftEdge + IIntuition->IntuiTextLength(&testtext) + window->WScreen->RastPort.Font->tf_XSize;
					if(wid > mwidth)
						mwidth = wid;

					if((text = text->NextText))
					{
						y += offset;
						text->TopEdge = offset + 3;
					}
					++first;
				}
				if(item->Flags & COMMSEQ)
					mwidth += comwidth + IGraphics->TextLength(&window->WScreen->RastPort, (char *)&item->Command, 1);
				if(mwidth > iwidth)
					iwidth = mwidth;

				y += offset;
			}
			else
			{
				struct Image *image;
				int wid;

				++item->TopEdge;

				image = (struct Image *)item->ItemFill;
				while(image)
				{
					if(image->ImageData == NULL)
					{
						image->Height = 2;
						image->PlaneOnOff = fpen;
					}
					else
					{
						wid = image->LeftEdge + image->Width;
						if(wid > iwidth)
							iwidth = wid;
					}

					y += image->TopEdge + image->Height;

					image = image->NextImage;
				}
				y += 3;
			}
			item = item->NextItem;
		}

		item = menu->FirstItem;
		while(item)
		{
			item->Width = iwidth;
			if(!(item->Flags & ITEMTEXT))
			{
				struct Image *image;

				image = (struct Image *)item->ItemFill;
				while(image)
				{
					if(image->ImageData == NULL)
						image->Width = iwidth;
					image = image->NextImage;
				}
			}
			item = item->NextItem;
		}
		menu = menu->NextMenu;
	}
	return((int)IIntuition->SetMenuStrip(window, firstmenu));
}
