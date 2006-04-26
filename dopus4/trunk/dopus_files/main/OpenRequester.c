/*
 *  $VER: OpenRequester.c $Revision$ (01-Feb-2006)
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


#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/OpenRequester ******************************************
*
*   NAME
*      OpenRequester -- Description
*
*   SYNOPSIS
*      struct Window * OpenRequester(struct RequesterBase * reqbase);
*
*   FUNCTION
*
*   INPUTS
*       reqbase - 
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

struct PrivateData
{
	struct Image *check_off_image;
	struct Image *check_on_image;
	struct Image *button_off_image;
	struct Image *button_on_image;
};

const struct TextAttr topazfont = { (STRPTR)"topaz.font", 8, 0, 0 };

struct Window *_DOpus_OpenRequester(struct DOpusIFace *Self, struct RequesterBase *reqbase)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *IntuitionBase = IExec->OpenLibrary("intuition.library", 50L);
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
	struct Library *GfxBase = IExec->OpenLibrary("graphics.library", 50L);
	struct GraphicsIFace *IGraphics = (struct GraphicsIFace *)IExec->GetInterface(GfxBase, "main", 1, NULL);
	struct NewWindow newwin;
	struct Screen *screen = NULL, screendata;
	struct Window *window = NULL, *center_window = NULL;
	short width, height, attempt;

	/* Forbid to prevent changes to Screen lists */

	IExec->Forbid();

	newwin.Type = 0;

	if(reqbase->rb_flags & RBF_WINDOWCENTER)
		center_window = reqbase->rb_window;
	reqbase->rb_window = NULL;

	/* rb_screen overrides rb_screenname; if this is initialised then try
	   to open the window on the CUSTOMSCREEN given */

	if(reqbase->rb_screen)
	{
		newwin.Type = CUSTOMSCREEN;
		newwin.Screen = reqbase->rb_screen;
		screen = reqbase->rb_screen;
	}

	/* Otherwise, if rb_screenname is given, search screen lists for that
	   screen */

	else if(reqbase->rb_screenname)
	{

		/* Look for it as a public screen first (2.0 only) */

		if((screen = IIntuition->LockPubScreen(reqbase->rb_screenname)))
		{
			newwin.Type = PUBLICSCREEN;
			newwin.Screen = screen;
		}

	}

	/* If the screen type is still unset, just open on the Workbench screen
	   (or default public screen) */

	if(!newwin.Type)
	{
		newwin.Type = WBENCHSCREEN;
		Self->GetWBScreen(&screendata);
		screen = &screendata;
	}

	/* Check font pointer; if not set, use screen's font */

	if(!reqbase->rb_font)
		reqbase->rb_font = screen->RastPort.Font;

	/* If font is proportional, use system default font */

	if(reqbase->rb_font->tf_Flags & FPF_PROPORTIONAL)
		reqbase->rb_font = ((struct GfxBase *)(IGraphics->Data.LibBase))->DefaultFont;

	/* Attempt this twice; first with the selected font, and if
	   the window is too large, again with topaz/8 */

	for(attempt = 0; attempt < 2; attempt++)
	{

		/* Setup window size for selected font */

		width = (reqbase->rb_width * reqbase->rb_font->tf_XSize) + reqbase->rb_widthfine;
		height = (reqbase->rb_height * reqbase->rb_font->tf_YSize) + reqbase->rb_heightfine;

		if(reqbase->rb_flags & RBF_BORDERS)
		{
			width += screen->WBorLeft + screen->WBorRight;
			height += screen->WBorTop + screen->WBorBottom + screen->Font->ta_YSize + 1;
		}

		/* If it's not going to fit on the screen, don't even attempt
		   to open it. Otherwise, fill in NewWindow structure */

		if(width <= screen->Width && height <= screen->Height)
		{

			if((reqbase->rb_flags & RBF_WINDOWCENTER) && center_window)
			{
				newwin.LeftEdge = center_window->LeftEdge + ((center_window->Width - width) / 2);
				newwin.TopEdge = center_window->TopEdge + ((center_window->Height - height) / 2);
			}
			else
			{
				newwin.LeftEdge = (screen->Width - width) / 2;
				newwin.TopEdge = (screen->Height - height) / 2;
			}

			if(newwin.LeftEdge < 0)
				newwin.LeftEdge = 0;
			else if(newwin.LeftEdge + width > screen->Width)
				newwin.LeftEdge = screen->Width - width;
			if(newwin.TopEdge < 0)
				newwin.TopEdge = 0;
			else if(newwin.TopEdge + height > screen->Height)
				newwin.TopEdge = screen->Height - height;

			newwin.Width = width;
			newwin.Height = height;
			newwin.DetailPen = 255;
			newwin.BlockPen = 255;

			if(reqbase->rb_idcmpflags)
				newwin.IDCMPFlags = reqbase->rb_idcmpflags;
			else
				newwin.IDCMPFlags = IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_RAWKEY|IDCMP_VANILLAKEY|IDCMP_DISKREMOVED|IDCMP_DISKINSERTED;

			if(reqbase->rb_flags & RBF_BORDERS)
			{
				newwin.Flags = WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_RMBTRAP|WFLG_ACTIVATE|WFLG_NEWLOOKMENUS;
				if(reqbase->rb_flags & RBF_CLOSEGAD)
				{
					newwin.Flags |= WFLG_CLOSEGADGET;
					newwin.IDCMPFlags |= IDCMP_CLOSEWINDOW;
				}
			}
			else
			{
				newwin.Flags = WFLG_BORDERLESS|WFLG_RMBTRAP|WFLG_ACTIVATE | WFLG_NEWLOOKMENUS;
			}

			newwin.FirstGadget = NULL;
			newwin.CheckMark = NULL;
			newwin.Title = (UBYTE *)reqbase->rb_title;

			/* Try to open window; if we succeed, then render border and fill
			   in background */

			if((window = IIntuition->OpenWindow(&newwin)))
			{
				IGraphics->SetFont(window->RPort, reqbase->rb_font);
				if(!(reqbase->rb_flags & RBF_BORDERS))
				{
					Self->Do3DBox(window->RPort, 2, 1, window->Width - 4, window->Height - 2, reqbase->rb_shine, reqbase->rb_shadow);
				}
				IGraphics->SetAPen(window->RPort, reqbase->rb_bg);
				IGraphics->RectFill(window->RPort, window->BorderLeft + 2, window->BorderTop + 1, window->Width - 3 - window->BorderRight, window->Height - 2 - window->BorderBottom);
				IGraphics->SetAPen(window->RPort, reqbase->rb_fg);
				IGraphics->SetBPen(window->RPort, reqbase->rb_bg);
				IGraphics->SetDrMd(window->RPort, JAM2);
				break;
			}
		}

		/* If we failed with the custom font, try again with topaz/8 */

		if(attempt == 0)
		{
			if(!(reqbase->rb_font = IGraphics->OpenFont(&topazfont)))
				break;
			reqbase->rb_privateflags |= RBP_CLOSEFONT;
		}
	}

	/* Unlock the public screen we may have locked earlier */

	if(screen && (newwin.Type == PUBLICSCREEN))
		IIntuition->UnlockPubScreen(NULL, screen);

	/* Fix StringExtend function for current font */

	if(reqbase->rb_extend && window)
		reqbase->rb_extend->Font = reqbase->rb_font;

	/* Restore multitasking */

	IExec->Permit();

	/* Initialise other data */

	reqbase->rb_objects = NULL;
	reqbase->rb_memory = NULL;
	reqbase->rb_privatedata = Self->LAllocRemember(&reqbase->rb_memory, sizeof(struct PrivateData), MEMF_CLEAR);

	/* Fill out rb_window pointer in RequesterBase structure. We also
	   return the window pointer to indicate success or failure */

	reqbase->rb_window = window;

	IExec->DropInterface((struct Interface *)IIntuition);
	IExec->CloseLibrary(IntuitionBase);
	IExec->DropInterface((struct Interface *)IGraphics);
	IExec->CloseLibrary(GfxBase);
	return (window);
}
