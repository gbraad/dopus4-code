/*
 *  $VER: FileRequest.c $Revision$ (01-Feb-2006)
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
#include <proto/dos.h>
#include <dos/dos.h>
#include <proto/asl.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <graphics/gfxbase.h>
#include <libraries/dopus.h>
#include <libraries/asl.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <dopus/filereq.h>
#include <stdarg.h>

#include "extras.h"
#include "FileRequest.h"

/****** dopus/main/FileRequest ******************************************
*
*   NAME
*      FileRequest -- Description
*
*   SYNOPSIS
*      int FileRequest(struct DOpusFileReq * freq);
*
*   FUNCTION
*
*   INPUTS
*       freq - 
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

int _DOpus_FileRequest(struct DOpusIFace *Self, struct DOpusFileReq *freq)
{
	struct ExecIFace *IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
	struct Library *DOSBase = IExec->OpenLibrary("dos.library", 50L);
	struct DOSIFace *IDOS = (struct DOSIFace *)IExec->GetInterface(DOSBase, "main", 1, NULL);
	struct Library *AslBase = IExec->OpenLibrary("asl.library", 50L);
	struct AslIFace *IAsl = (struct AslIFace *)IExec->GetInterface(AslBase, "main", 1, NULL);
	struct Library *IntuitionBase = IExec->OpenLibrary("intuition.library", 50L);
	struct IntuitionIFace *IIntuition = (struct IntuitionIFace *)IExec->GetInterface(IntuitionBase, "main", 1, NULL);
//	struct Library *AslBase;

	if(freq->dirbuf[0] && (Self->CheckExist(freq->dirbuf, NULL) < 0))
	{
		char *ptr;
		if(ptr = IDOS->FilePart(freq->dirbuf))
		{
			if(freq->filebuf)
			{
				int a;

				Self->LStrnCpy(freq->filebuf, ptr, FILEBUF_SIZE - 2);
				a = strlen(freq->filebuf);
				if(freq->filebuf[a - 1] == '/')
					freq->filebuf[a - 1] = 0;
			}
			*ptr = 0;
		}
	}

	if(AslBase && IAsl)
	{
		struct TagItem asltags[10];
		struct FileRequester *fr;
		int font, res = FALSE, a;
		char initialfont[40], *ptr;
		APTR request;

		for(a = 0; a < 10; a++)
		{
			asltags[a].ti_Tag = TAG_SKIP;
			asltags[a].ti_Data = 0;
		}
		asltags[9].ti_Tag = TAG_END;

		if(freq->flags & DFRF_FONT)
			font = 1;
		else
			font = 0;

		if(freq->title)
		{
			asltags[0].ti_Tag = ASLFR_TitleText;
			asltags[0].ti_Data = (ULONG) freq->title;
		}

		asltags[1].ti_Tag = ASLFR_Window;
		asltags[1].ti_Data = (ULONG) ((freq->window) ? freq->window : ((struct IntuitionBase *)(IIntuition->Data.LibBase))->ActiveWindow);

		if(freq->x > 0)
		{
			asltags[2].ti_Tag = ASLFR_InitialLeftEdge;
			asltags[2].ti_Data = freq->x;
		}

		if(freq->y > 0)
		{
			asltags[3].ti_Tag = ASLFR_InitialTopEdge;
			asltags[3].ti_Data = freq->y;
		}

		asltags[4].ti_Tag = ASLFR_InitialHeight;
		asltags[4].ti_Data = freq->lines * 14;

		if(font)
		{
			if(ptr = IDOS->FilePart(freq->dirbuf))
			{
				Self->LStrCpy(initialfont, ptr);
				a = strlen(initialfont);
				if(initialfont[a - 1] == '/')
					initialfont[a - 1] = 0;
			}
			else
				initialfont[0] = 0;
			asltags[5].ti_Tag = ASLFO_InitialName;
			asltags[5].ti_Data = (ULONG) initialfont;
			asltags[6].ti_Tag = ASLFO_InitialSize;
			asltags[6].ti_Data = atoi(freq->filebuf);
		}
		else
		{
			if(freq->filebuf)
			{
				asltags[5].ti_Tag = ASLFR_InitialFile;
				asltags[5].ti_Data = (ULONG) freq->filebuf;
			}
			if(freq->dirbuf)
			{
//				asltags[6].ti_Tag = ASL_Dir;
				asltags[6].ti_Tag = ASLFR_InitialDrawer;
				asltags[6].ti_Data = (ULONG) freq->dirbuf;
			}
		}

//		asltags[7].ti_Tag = ASL_FuncFlags;
/*		asltags[7].ti_Tag = ASLFR_Flags1;

		if(font)
			asltags[7].ti_Data = FONF_NEWIDCMP;
		else
		{
			if(freq->flags & DFRF_SAVE)
				asltags[7].ti_Data = FILF_SAVE | FILF_NEWIDCMP;
			else if(freq->flags & DFRF_MULTI)
				asltags[7].ti_Data = FILF_MULTISELECT | FILF_NEWIDCMP;
			else
				asltags[7].ti_Data = FILF_NEWIDCMP;

			if(freq->flags & DFRF_DIRREQ)
			{
				asltags[8].ti_Tag = ASL_ExtFlags1;
				asltags[8].ti_Data |= FIL1F_NOFILES;
			}
		}
*/
		if((request = IAsl->AllocAslRequest((font) ? ASL_FontRequest : ASL_FileRequest, asltags)))
		{
			if(res = IAsl->AslRequest(request, NULL))
			{
				if(font)
				{
					if(freq->dirbuf)
					{
						Self->StrCombine(freq->dirbuf, "FONTS:", ((struct FontRequester *)request)->fo_Attr.ta_Name, 256);
						Self->LStrCat(freq->dirbuf, "/");
					}
					if(freq->filebuf)
					{
						LSprintf(freq->filebuf, "%ld", ((struct FontRequester *)request)->fo_Attr.ta_YSize);
					}
				}
				else
				{
					fr = (struct FileRequester *)request;
					if(freq->dirbuf)
						Self->LStrCpy(freq->dirbuf, fr->fr_Drawer);
					if(!(freq->flags & DFRF_DIRREQ))
					{
						if(freq->filebuf)
							Self->LStrCpy(freq->filebuf, fr->fr_File);
					}
					if(freq->flags & DFRF_MULTI
					   && fr->fr_NumArgs > 0)
					{
						if((freq->filearray = Self->LAllocRemember(&freq->filearraykey, (fr->fr_NumArgs + 1) * 4, MEMF_CLEAR)))
						{
							for(a = 0; a < fr->fr_NumArgs; a++)
							{
								if(freq->filearray[a] = Self->LAllocRemember(&freq->filearraykey, strlen(fr->fr_ArgList[a].wa_Name) + 1, MEMF_CLEAR))
								{
									Self->LStrCpy(freq->filearray[a], fr->fr_ArgList[a].wa_Name);
								}
							}
						}
					}
				}
			}
			IAsl->FreeAslRequest(request);
		}
//		CloseLibrary(AslBase);
		return (res);
	}
	else
	{
		ULONG class;
		USHORT code, gadgetid, qual;
		struct FileReqData *freqdata;
		unsigned char *dirbuf;
		int i, ret = 1;

		if(!(freqdata = IExec->AllocMem(sizeof(struct FileReqData), MEMF_CLEAR)))
			return (ERROR_NOMEM);
		freqdata->freq = freq;
		if(!(initrequester(freqdata)))
		{
			close_req(freqdata);
			return (ERROR_NOMEM);
		}
		dirbuf = freq->dirbuf;

		if((i = get_filenames(freqdata)) == CANCEL)
		{
			close_req(freqdata);
			freemulti(freq);
			return (ERROR_CANCEL);
		}
		if(i != OKAY)
		{
			FOREVER
			{
				IExec->Wait(1 << freqdata->fr_Window->UserPort->mp_SigBit);
				while(getintuimsg(freqdata->fr_Window, &class, &code, &qual, &gadgetid))
				{
					if((i = do_idcmp(freqdata, class, code, qual, gadgetid)) == CANCEL)
					{
						close_req(freqdata);
						freemulti(freq);
						return (ERROR_CANCEL);
					}
					if(i == OKAY)
					{
						close_req(freqdata);
						goto acceptreq;
					}
				}
			}
		}
		else
			close_req(freqdata);
	      acceptreq:
		checkdrawer(dirbuf);
		if(freq->flags & DFRF_DIRREQ)
		{
			if(i == OKAY || dirbuf[0])
				ret = 1;
			else
				ret = 0;
		}
		return (ret);
	}
}
