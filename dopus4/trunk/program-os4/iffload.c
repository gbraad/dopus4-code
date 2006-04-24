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

enum
{
	PRINT_ASPECT,
	PRINT_IMAGE,
	PRINT_SHADE,
	PRINT_PLACE,
	PRINT_FORMFD,
	PRINT_TITLE,
	PRINT_OKAY,
	PRINT_CANCEL,
	PRINT_ABORT
};

#define DIMBUFSIZE sizeof(struct DimensionInfo)

struct AnimFrame
{
	struct AnimFrame *next;
	struct AnimHeader *animheader;
	/*unsigned*/ char *delta;
	/*unsigned*/ char *cmap;
	int cmapsize;
};

struct DOpusAnim
{
	struct AnimFrame initial_frame;
	struct AnimFrame *first_frame;
	struct AnimFrame *current_frame;
	struct AnimFrame *last_frame;
	struct BitMap *frame_bm[2], *initialframe_bm;
	struct Image frameimage;
	int framecount, framenum, framedisp, speed, framespersec, type;
};

static struct NewWindow iffwin =
{
	0, 0, 0, 0,
	255, 255,
	IDCMP_MOUSEBUTTONS | IDCMP_EXTENDEDMOUSE | IDCMP_RAWKEY | IDCMP_INTUITICKS | IDCMP_INACTIVEWINDOW,
	WFLG_RMBTRAP | WFLG_BORDERLESS | WFLG_SIMPLE_REFRESH | WFLG_NOCAREREFRESH,
	NULL, NULL, NULL, NULL, NULL,
	0, 0, 0, 0, CUSTOMSCREEN
};

static char *picbuffer, *picturename;
static char *bodyptr;
static ULONG buffersize, bufferpos;

static uint32 *colourtable_8;
static UWORD *copperlist, *colourtable_4;

static struct ViewPort *ivp;

static int numcolours, currange, copperheight, screenwidth, screenheight, bitmapwidth, bitmapheight, viewflags;
static int palette32, got_dpan;
static char doublebuffer;
static char specialformat;

static struct BitMapHeader bmhd;

static struct BitMap *iffbm[2];
static struct Window *iffwindow;

static char iffscreenname[34];

static struct DOpusAnim anim;

static inline void dotitle(struct BitMapHeader *bmhd)
{
	char title[100], modes[140], cols[30];

	getcolstring(cols);
	getviewmodes(modes);
	IUtility->SNPrintf(title, 100, "%ld x %ld x %ld (page %ld x %ld) %s cols (%s)", bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth, bmhd->bmh_PageWidth, bmhd->bmh_PageHeight, cols, modes);
	dostatustext(title);
}

int LoadPic(char *name)
{
	struct DOpusRemember *iffkey;
	int retcode, a, gotbody = 0, brush = 0, scroll = 1, mp = 1, isanim = 0, extflag = 0, colourptr = -1, coloursize, imagewidth, imageheight, minwidth, minheight, maxwidth, maxheight, depth, coppersize, viewmode, maxframes;
	UWORD colourlist[4];
	uint32 chunkid, chunksize, chunkbuf[3];
	struct DimensionInfo *dimension;
	DisplayInfoHandle *handle;
	char dimbuf[DIMBUFSIZE], *ptr;
	DPAnimChunk dpan;
	struct AnimFrame *cur_frame = NULL, *frame;
	CRange cyclerange[6];
	APTR dto = NULL;
	BOOL dt_ok = 0;

	picturename = name;
	viewmode = -1;
	anim.speed = 50000;
	anim.framespersec = 20;
	maxframes = 65535;
	copperlist = NULL;
	iffwindow = NULL;
	iffscreen = NULL;
	iffscreenname[0] = 0;
	copperheight = 0;
	currange = 0;
	doublebuffer = 0;
	bodyptr = NULL;
	bufferpos = 0;
	iffkey = NULL;
	anim.framecount = 0;
	specialformat = 0;
	anim.initialframe_bm = NULL;
	palette32 = 0;
	got_dpan = 0;
	anim.type = 0;
	anim.first_frame = NULL;
	anim.last_frame = NULL;
	anim.initial_frame.cmap = NULL;

	for(a = 0; a < 2; a++)
		iffbm[a] = anim.frame_bm[a] = NULL;

	if(readfile(name, &picbuffer, &buffersize))
		return (0);

	if(!chunkread(chunkbuf, sizeof(ULONG) * 3) || chunkbuf[0] != ID_FORM || ((chunkbuf[2] != ID_ANIM) && (chunkbuf[2] != ID_ILBM)))
	{
	      DTload:
		if(DataTypesBase && IDataTypes)
		{
			if((dto = IDataTypes->NewDTObject(name, DTA_GroupID, GID_PICTURE, PDTA_FreeSourceBitMap, TRUE, TAG_END)))
			{
				struct BitMapHeader *dto_bmhd;

				if(IDataTypes->GetDTAttrs(dto, PDTA_ModeID, (Tag) & viewmode, PDTA_BitMapHeader, (Tag) & dto_bmhd, PDTA_ColorRegisters, (Tag) & colourptr, PDTA_NumColors, (Tag) & coloursize, TAG_DONE) == 4)
				{
					bmhd = *dto_bmhd;
					coloursize *= 3;
					picbuffer = NULL;

					dt_ok = TRUE;
				}
			}
		}
		if(!dt_ok)
		{
			retcode = IFFERR_NOTILBM;
			goto endiff;
		}
	}
	if(!dt_ok)
	{
		if(chunkbuf[2] == ID_ANIM)
			isanim = 1;

		while(bufferpos <= buffersize)
		{
			if(!chunkread(&chunkid, sizeof(int)) || !chunkread(&chunksize, sizeof(int)) || ((bufferpos + chunksize) > buffersize))
				break;
			switch (chunkid)
			{
			case ID_FORM:
				bufferpos += 4;
				continue;
			case ID_DPAN:
				if(!isanim)
					break;
				if(!(chunkread(&dpan, sizeof(DPAnimChunk))))
				{
					retcode = IFFERR_BADIFF;
					goto endiff;
				}
				chunksize = 0;
				maxframes = dpan.nframes;
				if((anim.framespersec = dpan.framespersecond) > 0)
					anim.speed = 1000000 / dpan.framespersecond;
				got_dpan = 1;
				break;
			case ID_ANHD:
				if(isanim && anim.framecount < maxframes && (frame = IDOpus->LAllocRemember(&iffkey, sizeof(struct AnimFrame), MEMF_CLEAR)))
				{
					frame->animheader = (struct AnimHeader *)(picbuffer + bufferpos);
					if(frame->animheader->ah_Interleave != 1)
						doublebuffer = 1;
					if(!anim.type)
						anim.type = frame->animheader->ah_Operation;

					if(cur_frame)
						cur_frame->next = frame;
					else
						anim.first_frame = frame;
					cur_frame = frame;
				}
				break;
			case ID_DLTA:
				if(isanim && anim.framecount < maxframes && cur_frame)
				{
					cur_frame->delta = picbuffer + bufferpos;
					++anim.framecount;
				}
				break;
			case ID_BMHD:
				if(!chunkread(&bmhd, sizeof(struct BitMapHeader)))
				{
					retcode = IFFERR_BADIFF;
					goto endiff;
				}
				if((!isanim) && (bmhd.bmh_Depth > 8))
					goto DTload;
				chunksize = 0;
				break;
			case ID_CMAP:
				if(colourptr == -1)
				{
					colourptr = bufferpos;
					coloursize = chunksize;
					anim.initial_frame.cmap = (char *)&picbuffer[bufferpos];
					anim.initial_frame.cmapsize = chunksize;
				}
				else if(isanim && anim.framecount < maxframes && cur_frame)
				{
					cur_frame->cmap = (char *)&picbuffer[bufferpos];
					cur_frame->cmapsize = chunksize;
				}
				break;
			case ID_GRAB:
				brush = 1;
				break;
			case ID_CAMG:
				if(!chunkread(&viewmode, sizeof(int)))
				{
					retcode = IFFERR_BADIFF;
					goto endiff;
				}
				chunksize = 0;
				break;
			case ID_CTBL:
				if((copperlist = IDOpus->LAllocRemember(&iffkey, chunksize, MEMF_CLEAR)))
				{
					coppersize = chunksize;
					copperheight = chunksize / 32;
					IExec->CopyMem((char *)&picbuffer[bufferpos], (char *)copperlist, chunksize);
					IExec->CopyMem((char *)&picbuffer[bufferpos], (char *)colourlist, 8);
				}
				break;
			case ID_DYCP:
				mp = 0;
				break;
			case ID_CRNG:
				if(currange < 6)
				{
					IExec->CopyMem((char *)&picbuffer[bufferpos], (char *)&cyclerange[currange], sizeof(CRange));
					if(cyclerange[currange].rate <= CRNG_NORATE || cyclerange[currange].low == cyclerange[currange].high)
						cyclerange[currange].active = 0;
					if(cyclerange[currange].rate > CRNG_NORATE)
						cyclerange[currange].rate = 16384 / cyclerange[currange].rate;
					else
						cyclerange[currange].rate = 0;
					++currange;
				}
				break;
			case ID_BODY:
				if(!gotbody)
				{
					gotbody = 1;
					bodyptr = picbuffer + bufferpos;
				}
				break;
			}
			if(chunksize)
			{
				bufferpos += chunksize;
				if(chunksize & 1)
					++bufferpos;
			}
		}

		if(!gotbody)
		{
			retcode = IFFERR_BADIFF;
			goto endiff;
		}

		if(copperlist)
			specialformat = 1;
	}

	depth = bmhd.bmh_Depth;

	if(colourptr > -1)
	{
		numcolours = coloursize / 3;

		if(!specialformat)
			colourtable_8 = IDOpus->LAllocRemember(&iffkey, (coloursize + 2) * 4, MEMF_CLEAR);

		colourtable_4 = IDOpus->LAllocRemember(&iffkey, numcolours * sizeof(UWORD), MEMF_CLEAR);
	}

	if(!colourtable_8 && !colourtable_4)
	{
		retcode = IFFERR_BADIFF;
		goto endiff;
	}

	if((config->viewbits & VIEWBITS_8BITSPERGUN || bmhd.bmh_Pad & BMHF_CMAPOK) && colourtable_8)
		palette32 = 1;

	build_palettes(&picbuffer[colourptr], coloursize, colourtable_4, colourtable_8);

	imagewidth = bmhd.bmh_Width;
	screenwidth = bmhd.bmh_Width;
	imageheight = bmhd.bmh_Height;
	screenheight = bmhd.bmh_Height;

	minwidth = 640; //320;
	minheight = 480; //200;

	maxwidth = 1280; //368;
	maxheight = 1024; //(scrdata_is_pal ? 281 : 241);

	if(viewmode == -1 || brush)
	{
		viewflags = viewmode;
		viewmode = 0;
		if(bmhd.bmh_PageWidth > maxwidth)
			viewmode |= HIRES;
		if(bmhd.bmh_PageHeight > maxheight)
			viewmode |= LACE;
		viewflags = 0;
	}

	if(!(IGraphics->ModeNotAvailable(viewmode)))
		extflag = 1;
	if(config->viewbits & VIEWBITS_BESTMODEID)
	{
		ULONG newmode;

		if(!(IGraphics->ModeNotAvailable((newmode = IGraphics->BestModeID(BIDTAG_DesiredWidth, imagewidth, BIDTAG_DesiredHeight, imageheight, BIDTAG_Depth, depth, BIDTAG_SourceID, viewmode, BIDTAG_MonitorID, IGraphics->GetVPModeID(&Window->WScreen->ViewPort) & MONITOR_ID_MASK, TAG_END)))))
			viewmode = newmode;
	}

	if((handle = IGraphics->FindDisplayInfo(viewmode)))
	{
		if(!(a = IGraphics->GetDisplayInfoData(handle, dimbuf, DIMBUFSIZE, DTAG_NAME, 0)))
		{
			DisplayInfoHandle *temphandle;
			ULONG tempviewmode;

			tempviewmode = viewmode;
			tempviewmode &= ~(HAM_KEY | EXTRAHALFBRITE_KEY);

			if((temphandle = IGraphics->FindDisplayInfo(tempviewmode)))
				a = IGraphics->GetDisplayInfoData(temphandle, dimbuf, DIMBUFSIZE, DTAG_NAME, 0);
		}
		if(a && (((struct NameInfo *)dimbuf)->Name))
		{
			strcpy(iffscreenname, ((struct NameInfo *)dimbuf)->Name);
			if((ptr = strchr(iffscreenname, ':')))
				strcpy(iffscreenname, ptr + 1);
		}
		IGraphics->GetDisplayInfoData(handle, dimbuf, DIMBUFSIZE, DTAG_DIMS, 0);
		dimension = (struct DimensionInfo *)dimbuf;
		maxwidth = (dimension->StdOScan.MaxX - dimension->StdOScan.MinX) + 1;
		maxheight = (dimension->StdOScan.MaxY - dimension->StdOScan.MinY) + 1;
		if(depth > dimension->MaxDepth)
		{
			retcode = IFFERR_BADMODE;
			goto endiff;
		}
	}
	else
		extflag = 0;

	viewflags = 0;
	if((viewmode & SUPER_KEY) == SUPER_KEY)
		viewflags |= SUPERHIRES | HIRES;
	else if(viewmode & HIRES_KEY)
		viewflags |= HIRES;
	if(viewmode & LORESLACE_KEY)
		viewflags |= LACE;
	if(viewmode & HAM_KEY)
		viewflags |= HAM;
	if(viewmode & EXTRAHALFBRITE_KEY)
		viewflags |= EXTRA_HALFBRITE;

	if(!extflag)
	{
		viewmode = viewflags;
		viewmode &= 0x0000ffff;
		viewmode &= ~(SPRITES | GENLOCK_AUDIO | GENLOCK_VIDEO | VP_HIDE | DUALPF | PFBA | EXTENDED_MODE);
		viewflags = viewmode;

		if(viewflags & HIRES)
		{
			maxwidth *= (viewflags & SUPERHIRES) ? 4 : 2;
			minwidth *= 2;
		}
	}

	if(screenwidth < minwidth)
		screenwidth = minwidth;
	else if(screenwidth > maxwidth)
		screenwidth = maxwidth;

	if(!extflag && viewflags & LACE)
	{
		maxheight *= 2;
		minheight *= 2;
	}
	if(screenheight < minheight)
		screenheight = minheight;
	else if(screenheight > maxheight)
		screenheight = maxheight;

	bitmapwidth = (imagewidth < screenwidth) ? screenwidth : imagewidth;
	bitmapheight = (imageheight < screenheight) ? screenheight : imageheight;

	if((bitmapwidth > screenwidth) || (bitmapheight > screenheight))
		scroll = 3;

	dotitle(&bmhd);

	if(isanim && anim.first_frame)
	{
		if(!(anim.frame_bm[0] = IGraphics->AllocBitMap(bitmapwidth, bitmapheight, bmhd.bmh_Depth, BMF_DISPLAYABLE | BMF_CLEAR, NULL)))
		{
			retcode = IFFERR_NOMEM;
			goto endiff;
		}
		if(doublebuffer)
		{
			if((anim.frame_bm[1] = IGraphics->AllocBitMap(bitmapwidth, bitmapheight, bmhd.bmh_Depth, BMF_DISPLAYABLE | BMF_CLEAR, NULL)))
			{
				anim.initialframe_bm = IGraphics->AllocBitMap(bitmapwidth, bitmapheight, bmhd.bmh_Depth, BMF_DISPLAYABLE | BMF_CLEAR, NULL);
			}
		}
	}

	if(doublebuffer)
	{
		for(a = 0; a < 2; a++)
		{
			if(anim.frame_bm[a])
			{
				if(!(iffbm[a] = IDOpus->LAllocRemember(&iffkey, sizeof(struct BitMap), MEMF_CLEAR)))
				{
					retcode = IFFERR_NOMEMORY;
					goto endiff;
				}
				IExec->CopyMem((char *)anim.frame_bm[a], (char *)iffbm[a], sizeof(struct BitMap));
			}
		}
	}
	else
	{
		if(!dt_ok)
		{
			iffbm[0] = IGraphics->AllocBitMap(bitmapwidth, bitmapheight, depth, BMF_DISPLAYABLE | BMF_CLEAR, NULL);

			if(iffbm[0] == NULL)
			{
				retcode = IFFERR_NOMEMORY;
				goto endiff;
			}
		}
	}

	if(!(iffscreen = IIntuition->OpenScreenTags(NULL, SA_Width, bitmapwidth, SA_Height, bitmapheight, SA_Depth, depth, dt_ok ? SA_Type : SA_BitMap, dt_ok ? CUSTOMSCREEN : (Tag) iffbm[0], SA_Behind, TRUE, SA_DisplayID, viewmode, SA_AutoScroll, TRUE, SA_Overscan, OSCAN_MAX, SA_SharePens, TRUE, SA_ShowTitle, FALSE, SA_Colors32, (Tag) colourtable_8, TAG_END)))
	{
		retcode = IFFERR_NOSCREEN;
		goto endiff;
	}

	ivp = &(iffscreen->ViewPort);
	if(!dt_ok)
		IGraphics->LoadRGB4(ivp, nullpalette, 1 << (depth > 8 ? 8 : depth));

	iffwin.Screen = iffscreen;
	iffwin.Width = iffscreen->Width;
	iffwin.Height = iffscreen->Height;

	if(!(iffwindow = IIntuition->OpenWindow(&iffwin)))
	{
		retcode = IFFERR_NOSCREEN;
		goto endiff;
	}

	if(dt_ok)
	{
		IDataTypes->SetDTAttrs(dto, NULL, NULL, PDTA_Screen, (Tag) iffscreen, TAG_END);
		IDataTypes->DoDTMethod(dto, iffwindow, NULL, DTM_PROCLAYOUT, NULL, 1);
		IDataTypes->GetDTAttrs(dto, PDTA_DestBitMap, (Tag) & iffbm[0], TAG_END);
		if(!(iffbm[0]))
		{
			IDataTypes->GetDTAttrs(dto, PDTA_BitMap, (Tag) & iffbm[0], TAG_END);
		}
		if(!(iffbm[0]))
		{
			retcode = IFFERR_NOMEMORY;
			goto endiff;
		}
		IGraphics->BltBitMapRastPort(iffbm[0], 0, 0, iffwindow->RPort, 0, 0, bitmapwidth, bitmapheight, 0xC0);
	}

	setnullpointer(iffwindow);

	if(isanim && anim.first_frame)
	{
		anim.framenum = 1;
		anim.framedisp = 0;
		anim.current_frame = anim.first_frame;

		rletobuffer(bodyptr, bmhd.bmh_Width, bmhd.bmh_Height, anim.frame_bm[0], bmhd.bmh_Masking, bmhd.bmh_Compression);

		if(doublebuffer)
		{
			if(anim.frame_bm[1])
			{
				IGraphics->BltBitMap(anim.frame_bm[0], 0, 0, anim.frame_bm[1], 0, 0, bmhd.bmh_Width, bmhd.bmh_Height, 0xc0, 0xff, NULL);
				if(anim.initialframe_bm)
					IGraphics->BltBitMap(anim.frame_bm[0], 0, 0, anim.initialframe_bm, 0, 0, bmhd.bmh_Width, bmhd.bmh_Height, 0xc0, 0xff, NULL);
				IGraphics->WaitBlit();
			}
		}
		else
		{
			anim.frameimage.LeftEdge = (bitmapwidth - bmhd.bmh_Width) / 2;
			anim.frameimage.TopEdge = (bitmapheight - bmhd.bmh_Height) / 2;
			doanimframe();
		}
	}
	else if(!dt_ok)
		readpic(&bmhd, bodyptr, iffbm[0]);

	if(numcolours > ivp->ColorMap->Count)
		numcolours = ivp->ColorMap->Count;

	if(!copperlist)
	{
		if(viewflags & HAM)
		{
			if(palette32)
				IGraphics->LoadRGB32(ivp, colourtable_8);
			else
				IGraphics->LoadRGB4(ivp, colourtable_4, numcolours);
		}
	}

	IIntuition->ActivateWindow(iffwindow);
	IIntuition->ScreenToFront(iffscreen);

	if(copperlist)
	{
		struct View *view = IIntuition->ViewAddress();

		if((copperheight >> 1) > scrdata_norm_height)
			view->DyOffset = 14 + 8;
		if(mp)
			IGraphics->LoadRGB4(ivp, colourlist, 4);
		if(!InitDHIRES(mp))
		{
		}
	}
	else if((depth <= 8) && !specialformat && !(viewflags & HAM))
	{
		if(palette32)
			FadeRGB32(iffscreen, &colourtable_8[1], numcolours, 1, config->fadetime);
		else
			FadeRGB4(iffscreen, colourtable_4, numcolours, 1, config->fadetime);
	}

	if(currange)
		initcycle(ivp, colourtable_4, numcolours, cyclerange, currange);

	retcode = (WaitForMouseClick(scroll, iffwindow) ? -1 : 1);

	if(checkcycling())
		togglecycle();
	stopcycle();

	if(specialformat)
	{
		IIntuition->ScreenToBack(iffscreen);
		IGraphics->FreeVPortCopLists(ivp);
	}
	else if((depth <= 8) && a != -3 && !(viewflags & HAM))
	{
		if(palette32)
			FadeRGB32(iffscreen, &colourtable_8[1], numcolours, -1, config->fadetime);
		else
			FadeRGB4(iffscreen, colourtable_4, numcolours, -1, config->fadetime);
	}

      endiff:
	IIntuition->ScreenToFront(blankscreen ? blankscreen : Window->WScreen);

	if(iffwindow)
	{
		IIntuition->CloseWindow(iffwindow);
		iffwindow = NULL;
	}
	if(iffscreen)
	{
		IIntuition->CloseScreen(iffscreen);
		iffscreen = NULL;
	}
	if(dt_ok)
	{
		IDataTypes->DisposeDTObject(dto);
		dto = NULL;
	}
	else
	{
		for(a = 0; a < 2; a++)
		{
			IGraphics->FreeBitMap(iffbm[a]);
			IGraphics->FreeBitMap(anim.frame_bm[a]);
		}
		IGraphics->FreeBitMap(anim.initialframe_bm);
	}

	IIntuition->ActivateWindow(Window);

	IDOpus->LFreeRemember(&iffkey);
	IExec->FreeVec(picbuffer);

	anim.first_frame = NULL;
	colourtable_8 = NULL;
	colourtable_4 = NULL;

	return (retcode);
}

void rletobuffer(STRPTR source, int sourcewidth, int sourceheight, struct BitMap *dest, char mask, char comp)
{
	struct RLEinfo picinfo;

	picinfo.sourceptr = source;
	picinfo.destplanes = dest->Planes;
	picinfo.imagebpr = ((sourcewidth + 15) / 16) * 2;
	picinfo.imageheight = sourceheight;
	picinfo.imagedepth = dest->Depth;
	picinfo.destbpr = dest->BytesPerRow;
	picinfo.destheight = dest->Rows;
	picinfo.destdepth = dest->Depth;
	picinfo.masking = mask;
	picinfo.compression = comp;
	picinfo.offset = 0;

	decoderle(&picinfo);
}

void readpic(struct BitMapHeader *bmhd, STRPTR source, struct BitMap *bmap)
{
	int rowbytes, bmrbytes, rows;
	int byteoff = 0;
	struct RLEinfo picinfo;

	rowbytes = ((bmhd->bmh_Width + 15) / 16) * 2;
	bmrbytes = ((IGraphics->GetBitMapAttr(bmap, BMA_WIDTH) + 15) / 16) * 2;

	if((rows = bmhd->bmh_Height) > bmap->Rows)
		rows = bmap->Rows;

	picinfo.sourceptr = source;
	picinfo.destplanes = bmap->Planes;
	picinfo.imagebpr = rowbytes;
	picinfo.imageheight = rows;
	picinfo.imagedepth = bmhd->bmh_Depth;
	picinfo.destbpr = bmrbytes;
	picinfo.destheight = bmap->Rows;
	picinfo.destdepth = bmap->Depth;
	picinfo.masking = bmhd->bmh_Masking;
	picinfo.compression = bmhd->bmh_Compression;
	picinfo.offset = byteoff;

	decoderle(&picinfo);
}

void decoderle(struct RLEinfo *rleinfo)
{
	STRPTR source, dest;
	int copy, col;
	int8 count;
	int plane, row, bmoffset, planes;

	planes = rleinfo->imagedepth;
	if(rleinfo->masking == mskHasMask)
		++planes;
	source = rleinfo->sourceptr;

	switch (rleinfo->compression)
	{
	case cmpByteRun1:
		bmoffset = rleinfo->offset;
		for(row = 0; row < rleinfo->imageheight; row++)
		{
			for(plane = 0; plane < planes; plane++)
			{
				if(plane < rleinfo->destdepth && rleinfo->destplanes[plane])
					dest = (char *)(rleinfo->destplanes[plane] + bmoffset);
				else
					dest = NULL;

				for(col = 0; col < rleinfo->imagebpr;)
				{
					if(!((count = *source++) >= 0))
					{
						copy = count + 1;
						col += copy;
						if(dest)
						{
							while(copy--)
								*dest++ = *source++;
						}
						else
							source += copy;
					}
					else if(count != -128)
					{
						copy = 1 - count;
						col += copy;
						if(dest)
						{
							while(copy--)
								*dest++ = *source;
						}
						++source;
					}
				}
			}
			bmoffset += rleinfo->destbpr;
		}
		break;
	case cmpNone:
		bmoffset = rleinfo->offset;
		for(row = 0; row < rleinfo->imageheight; row++)
		{
			for(plane = 0; plane < planes; plane++)
			{
				if(plane < rleinfo->destdepth)
					IExec->CopyMem(source, (char *)rleinfo->destplanes[plane] + bmoffset, rleinfo->imagebpr);
				source += rleinfo->imagebpr;
			}
			bmoffset += rleinfo->destbpr;
		}
		break;
	}
}

void doanimframe()
{
	struct AnimFrame *frame;

	if(anim.last_frame)
	{
		frame = anim.last_frame;
		anim.last_frame = anim.last_frame->next;
	}
	else
	{
		frame = &anim.initial_frame;
		anim.last_frame = anim.first_frame;
	}

	if(frame->cmap)
	{
		build_palettes(frame->cmap, frame->cmapsize, colourtable_4, colourtable_8);
		if(palette32)
			IGraphics->LoadRGB32(ivp, colourtable_8);
		else
			IGraphics->LoadRGB4(ivp, colourtable_4, numcolours);
	}

	if(doublebuffer && iffbm[1])
	{
		iffscreen->RastPort.BitMap = iffbm[anim.framedisp];
		iffscreen->ViewPort.RasInfo->BitMap = iffbm[anim.framedisp];
		IIntuition->MakeScreen(iffscreen);
		IIntuition->RethinkDisplay();
	}
	else
	{
		IGraphics->BltBitMap(anim.frame_bm[anim.framedisp], 0, 0, iffwindow->RPort->BitMap, anim.frameimage.LeftEdge, anim.frameimage.TopEdge, bmhd.bmh_Width, bmhd.bmh_Height, 0xc0, 0xff, NULL);
		IGraphics->WaitBlit();
	}
	if(anim.frame_bm[1])
		anim.framedisp = 1 - anim.framedisp;

	if(!anim.current_frame)
	{
		if(anim.frame_bm[1])
		{
			if(anim.initialframe_bm)
			{
				IGraphics->BltBitMap(anim.initialframe_bm, 0, 0, anim.frame_bm[anim.framedisp], 0, 0, bmhd.bmh_Width, bmhd.bmh_Height, 0xc0, 0xff, NULL);
				IGraphics->WaitBlit();
			}
			else
				rletobuffer(bodyptr, bmhd.bmh_Width, bmhd.bmh_Height, anim.frame_bm[anim.framedisp], bmhd.bmh_Masking, bmhd.bmh_Compression);
		}
		if(got_dpan || anim.framenum == 0)
		{
			anim.current_frame = anim.first_frame;
			anim.last_frame = anim.first_frame;
			anim.framenum = 1;
		}
		else
			anim.framenum = 0;
	}

	if(anim.framenum > 0)
	{
		switch (anim.current_frame->animheader->ah_Operation)
		{
		case 0:
			if(anim.current_frame->delta)
				rletobuffer(anim.current_frame->delta, bmhd.bmh_Width, bmhd.bmh_Height, anim.frame_bm[anim.framedisp], bmhd.bmh_Masking, bmhd.bmh_Compression);
			break;
		case 5:
			doriff(anim.current_frame->delta, anim.frame_bm[anim.framedisp], (anim.current_frame->animheader->ah_Flags & 2 || !anim.frame_bm[1]), bmhd.bmh_Width, 0);
			break;
		case 7:
			doriff7(anim.current_frame->delta, anim.frame_bm[anim.framedisp], bmhd.bmh_Width, (anim.current_frame->animheader->ah_Flags & 1) ? 2 : 1);
			break;
		case 8:
			doriff(anim.current_frame->delta, anim.frame_bm[anim.framedisp], 1, bmhd.bmh_Width, (anim.current_frame->animheader->ah_Flags & 1) ? 2 : 1);
			break;
		}
		++anim.framenum;
		anim.current_frame = anim.current_frame->next;
	}
}

void doriff(STRPTR delta, struct BitMap *image, int xor, int sourcewidth, int size)
{
	int plane, *dptr;
	unsigned char *data;

	sourcewidth = ((sourcewidth + 15) / 16) * 2;

	if((dptr = (int *)delta))
	{
		for(plane = 0; plane < image->Depth; plane++)
		{
			if(dptr[plane])
			{
				data = (unsigned char *)delta + dptr[plane];
				if(size == 0)
				{
					if(xor)
						decode_riff_xor(data, image->Planes[plane], image->BytesPerRow, sourcewidth);
					else
						decode_riff_set(data, image->Planes[plane], image->BytesPerRow, sourcewidth);
				}
				else if(size == 1)
					decode_riff_short((USHORT *) data, image->Planes[plane], image->BytesPerRow, sourcewidth);
				else
					decode_riff_long((ULONG *) data, image->Planes[plane], image->BytesPerRow, sourcewidth);
			}
		}
	}
}

void doriff7(STRPTR delta, struct BitMap *image, int sourcewidth, int size)
{
	int plane, *dptr;
	unsigned char *data;
	unsigned char *opcode;

	sourcewidth = ((sourcewidth + 15) / 16) * 2;

	if((dptr = (int *)delta))
	{
		for(plane = 0; plane < image->Depth; plane++)
		{
			if(dptr[plane] && dptr[plane + 8])
			{
				opcode = (unsigned char *)delta + dptr[plane];
				data = (unsigned char *)delta + dptr[plane + 8];
				if(size == 1)
					decode_riff7_short((USHORT *) data, opcode, image->Planes[plane], image->BytesPerRow, sourcewidth);
				else
					decode_riff7_long((ULONG *) data, opcode, image->Planes[plane], image->BytesPerRow, sourcewidth);
			}
		}
	}
}

void decode_riff_xor(UBYTE *delta, UBYTE *image, int rowbytes, int sourcebytes)
{
	int column, opcount;
	unsigned char *data, ch;

	for(column = 0; column < sourcebytes; column++)
	{
		data = image + column;
		opcount = *delta++;
		while(opcount--)
		{
			ch = *delta++;
			if(ch)
			{
				if(!(ch & 128))
					data += rowbytes * ch;
				else
				{
					ch &= 127;
					while(ch--)
					{
						*data ^= *delta++;
						data += rowbytes;
					}
				}
			}
			else
			{
				ch = *delta++;
				while(ch--)
				{
					*data ^= *delta;
					data += rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff_set(UBYTE *delta, UBYTE *image, int rowbytes, int sourcebytes)
{
	int column, opcount;
	unsigned char *data, ch;

	for(column = 0; column < sourcebytes; column++)
	{
		data = image + column;
		opcount = *delta++;
		while(opcount--)
		{
			ch = *delta++;
			if(ch)
			{
				if(!(ch & 128))
					data += rowbytes * ch;
				else
				{
					ch &= 127;
					while(ch--)
					{
						*data = *delta++;
						data += rowbytes;
					}
				}
			}
			else
			{
				ch = *delta++;
				while(ch--)
				{
					*data = *delta;
					data += rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff_short(USHORT *delta, UBYTE *image, int rowbytes, int sourcebytes)
{
	int column, opcount;
	unsigned char *data;
	USHORT ch;

	sourcebytes /= 2;

	for(column = 0; column < sourcebytes; column++)
	{
		data = (unsigned char *)(image + (column * 2));
		opcount = *delta++;
		while(opcount--)
		{
			ch = *delta++;
			if(ch)
			{
				if(!(ch & 0x8000))
					data += rowbytes * ch;
				else
				{
					ch &= 0x7fff;
					while(ch--)
					{
						*(USHORT *) data = *delta++;
						data += rowbytes;
					}
				}
			}
			else
			{
				ch = *delta++;
				while(ch--)
				{
					*(USHORT *) data = *delta;
					data += rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff_long(ULONG *delta, UBYTE *image, int rowbytes, int sourcebytes)
{
	int column, opcount;
	unsigned char *data;
	ULONG ch;

	sourcebytes /= 4;

	for(column = 0; column < sourcebytes; column++)
	{
		data = (unsigned char *)(image + (column * 4));
		opcount = *delta++;
		while(opcount--)
		{
			ch = *delta++;
			if(ch)
			{
				if(!(ch & 0x80000000))
					data += rowbytes * ch;
				else
				{
					ch &= 0x7fffffff;
					while(ch--)
					{
						*(ULONG *) data = *delta++;
						data += rowbytes;
					}
				}
			}
			else
			{
				ch = *delta++;
				while(ch--)
				{
					*(ULONG *) data = *delta;
					data += rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff7_short(USHORT *delta, UBYTE *opcode, UBYTE *image, int rowbytes, int sourcebytes)
{
	int column, opcount;
	unsigned char *data, ch;

	sourcebytes /= 2;

	for(column = 0; column < sourcebytes; column++)
	{
		data = (unsigned char *)(image + (column * 2));
		opcount = *opcode++;
		while(opcount--)
		{
			ch = *opcode++;
			if(ch)
			{
				if(!(ch & 0x80))
					data += rowbytes * ch;
				else
				{
					ch &= 0x7f;
					while(ch--)
					{
						*(USHORT *) data = *delta++;
						data += rowbytes;
					}
				}
			}
			else
			{
				ch = *opcode++;
				while(ch--)
				{
					*(USHORT *) data = *delta;
					data += rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff7_long(ULONG *delta, UBYTE *opcode, UBYTE *image, int rowbytes, int sourcebytes)
{
	int column, opcount;
	unsigned char *data, ch;

	sourcebytes /= 4;

	for(column = 0; column < sourcebytes; column++)
	{
		data = (unsigned char *)(image + (column * 4));
		opcount = *opcode++;
		while(opcount--)
		{
			ch = *opcode++;
			if(ch)
			{
				if(!(ch & 0x80))
					data += rowbytes * ch;
				else
				{
					ch &= 0x7f;
					while(ch--)
					{
						*(ULONG *) data = *delta++;
						data += rowbytes;
					}
				}
			}
			else
			{
				ch = *opcode++;
				while(ch--)
				{
					*(ULONG *) data = *delta;
					data += rowbytes;
				}
				++delta;
			}
		}
	}
}

int WaitForMouseClick(int tits, struct Window *wind)
{
	struct IntuiMessage *msg;
	ULONG class;
	USHORT code, qual;
	int ticks = 0, waitfor = 0, pon = 0, waitbits, retcode = 1, gottimer = 0, origspeed, startanim = 0, delta;
	struct timerequest treq;

	while((msg = (struct IntuiMessage *)IExec->GetMsg(wind->UserPort)))
		IExec->ReplyMsg((struct Message *)msg);
	if(!anim.first_frame && config->showdelay > 0)
	{
		waitfor = config->showdelay * 10;
		ticks = 0;
	}

	origspeed = anim.speed;

	waitbits = 1 << wind->UserPort->mp_SigBit;
	if(anim.first_frame)
	{
		if(!(IExec->OpenDevice(TIMERNAME, UNIT_VBLANK, &treq.tr_node, 0)))
		{
			treq.tr_node.io_Message.mn_ReplyPort = general_port;
			treq.tr_node.io_Command = TR_ADDREQUEST;
			treq.tr_node.io_Flags = 0;
			treq.tr_time.tv_secs = 0;
			treq.tr_time.tv_micro = 10;
			IExec->SendIO(&treq.tr_node);
			waitbits |= 1 << general_port->mp_SigBit;
			gottimer = 1;
		}
		if(!(startanim = (!(config->viewbits & VIEWBITS_PAUSEANIMS))) && doublebuffer)
			doanimframe();
	}

	FOREVER
	{
		if(anim.first_frame)
		{
			while(IExec->GetMsg(general_port));
			if(IExec->CheckIO(&treq.tr_node))
			{
				IExec->WaitIO(&treq.tr_node);
				treq.tr_time.tv_secs = 0;
				treq.tr_time.tv_micro = anim.speed;
				IExec->SendIO(&treq.tr_node);
				if(startanim)
					doanimframe();
			}
		}
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
				if(code >= 0x50 && code <= 0x59)
				{	// F1-F10
					code -= 0x50;
					anim.speed = (code * 10000) + 10;
					break;
				}
				switch (code)
				{
				case 0x0b:	// -
					if((anim.speed += 10000) > 1000000)
						anim.speed = 1000000;
					break;
				case 0x0c:	// =
					if((anim.speed -= 10000) < 10)
						anim.speed = 10;
					break;
				case 0x0d:	// '\'
					anim.speed = origspeed;
					break;
				case 0x21:	// S
					startanim = 1 - startanim;
					break;
				case 0x36:	// N
					if(anim.first_frame)
					{
						startanim = 0;
						doanimframe();
					}
					break;
				case CURSOR_LEFT:
				case 0x2d:	// NUM-4
				case CURSOR_RIGHT:
				case 0x2f:	// NUM-6
					if(tits != 3)
						break;
					if(qual & IEQUALIFIER_CONTROL)
						delta = bitmapwidth;
					else if(qual & IEQUALIFIER_ANYSHIFT)
						delta = screenwidth - 20;
					else
						delta = 2;
					if(code == CURSOR_LEFT || code == 0x2d)
						delta = -delta;

					IIntuition->MoveScreen(wind->WScreen, -delta, 0);
					break;
				case CURSOR_UP:
				case 0x3e:	// NUM-8
				case CURSOR_DOWN:
				case 0x1e:	// NUM-2
					if(tits != 3)
						break;
					if(qual & IEQUALIFIER_CONTROL)
						delta = bitmapheight;
					else if(qual & IEQUALIFIER_ANYSHIFT)
						delta = screenheight - 20;
					else
						delta = 2;
					if(code == CURSOR_UP/* || code == NM_WHEEL_UP*/ || code == 0x3e)
					{
						delta = -delta;
						if(wind->WScreen->TopEdge > delta)
							delta = wind->WScreen->TopEdge;
					}
					IIntuition->MoveScreen(wind->WScreen, 0, -delta);
					break;
				case 0x44:	// ENTER
					retcode = ((tits == 2) ? -2 : 0);
					break;
				case 0x45:	// ESC
					retcode = -1;
					break;
				case 0x10:	// Q
					if(!(qual & IEQUALIFIER_REPEAT))
						retcode = 0;
					break;
				case 0x42:	// TAB
					if(tits & 1 && currange)
						togglecycle();
					break;
				case 0x5f:	// HELP
				case 0x19:	// P
				case 0x40:	// SPACE
					break;
				case 0x39:	// .
					if(pon)
						setnullpointer(wind);
					else
						IIntuition->ClearPointer(wind);
					pon ^= 1;
					break;
				case 0x1a:	// [
					if(colourtable_4 && !specialformat)
						IGraphics->LoadRGB4(ivp, colourtable_4, numcolours);
					break;
				case 0x1b:	// ]
					if(colourtable_8 && !specialformat)
						IGraphics->LoadRGB32(ivp, colourtable_8);
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
	if(anim.first_frame && gottimer)
	{
		if(!(IExec->CheckIO(&treq.tr_node)))
			IExec->AbortIO(&treq.tr_node);
		IExec->WaitIO(&treq.tr_node);
		IExec->CloseDevice(&treq.tr_node);
		while(IExec->GetMsg(general_port));
	}
	return (retcode);
}

int chunkread(void *buf, ULONG nbytes)
{
	if(bufferpos + nbytes > buffersize)
		return (0);
	IExec->CopyMem((char *)&picbuffer[bufferpos], (char *)buf, nbytes);
	bufferpos += nbytes;
	return (1);
}

void getcolstring(char *str)
{
	if(viewflags & HAM || specialformat)
	{
		if(bmhd.bmh_Depth == 8)
			strcpy(str, "256K");
		else
			strcpy(str, "4096");
	}
	else
		sprintf(str, "%d", 1 << bmhd.bmh_Depth);
}

int InitDHIRES(int mp)
{
	int line, creg, top, oscan, lace;
	struct UCopList *ucop;

	if(!(ucop = (struct UCopList *)IExec->AllocMem(sizeof(struct UCopList), MEMF_CLEAR)))
		return (0);

	lace = (viewflags & LACE) ? 1 : 0;
	top = copperheight;
	if(lace)
		top /= 2;

	if(mp)
	{
		if(screenwidth > 640)
			oscan = 14;
		else
			oscan = 16;
		IGraphics->UCopperListInit(ucop, top * 13);
		for(line = 0; line < top; line++)
		{
			IGraphics->CWait(ucop, (line - 1) << lace, 112);
			IGraphics->CBump(ucop);
			for(creg = 4; creg < oscan; creg++)
			{
				IGraphics->CMove(ucop, (0xdff180 + (creg * 2)), copperlist[(line << (4 + lace)) + creg]);
				IGraphics->CBump(ucop);
			}
		}
	}
	else
	{
		IGraphics->UCopperListInit(ucop, copperheight * 17);
		for(line = 0; line < copperheight; line++)
		{
			IGraphics->CWait(ucop, line - 1, 122);
			IGraphics->CBump(ucop);
			for(creg = 4; creg < 16; creg++)
			{
				IGraphics->CMove(ucop, (0xdff180 + (creg * 2)), copperlist[(line << 4) + creg]);
				IGraphics->CBump(ucop);
			}
		}
	}

	IGraphics->CWait(ucop, 10000, 255);
	IGraphics->CBump(ucop);
	IExec->Forbid();
	ivp->UCopIns = ucop;
	IExec->Permit();
	IIntuition->RethinkDisplay();
	return (1);
}

void getviewmodes(char *modes)
{
	if(iffscreenname[0])
		strcpy(modes, iffscreenname);
	else
	{
		if(viewflags & SUPERHIRES)
		{
			strcpy(modes, "SUPERHIRES");
			if(bmhd.bmh_Width > 1280)
				strcat(modes, " OSCAN");
		}
		else if(viewflags & HIRES)
		{
			strcpy(modes, "HIRES");
			if(bmhd.bmh_Width > 640)
				strcat(modes, " OSCAN");
		}
		else
		{
			strcpy(modes, "LORES");
			if(bmhd.bmh_Width > 320)
				strcat(modes, " OSCAN");
		}
		if(viewflags & LACE)
			strcat(modes, " LACE");
	}
	if(copperlist)
	{
		strcat(modes, " DHIRES");
	}
	else
	{
		if(viewflags & HAM)
			strcat(modes, " HAM");
		else if(viewflags & EXTRA_HALFBRITE)
			strcat(modes, " EHB");
	}
}

void iffinfotxt(struct RastPort *r, char *buf, int x, int y)
{
	int a, l;

	a = strlen(buf);
	l = (316 - x) / 8;
	IGraphics->Move(r, x, y);
	IGraphics->Text(r, buf, (a > l) ? l : a);
}

void build_palettes(STRPTR colourdata, int coloursize, UWORD *ctable4, ULONG *ctable8)
{
	int a, b;
	struct ColorRegister rgb;

	if(ctable8)
	{
		for(a = 0, b = 1; a < coloursize; a++, b++)
		{
			ctable8[b] = ((colourdata[a] << 24) & 0xff000000) | ((colourdata[a] << 16) & 0x00ff0000) | ((colourdata[a] << 8) & 0x0000ff00) | ((colourdata[a] & 0x000000ff));
		}
		ctable8[0] = numcolours << 16;
		ctable8[(numcolours * 3) + 1] = 0;
	}

	if(ctable4)
	{
		for(a = 0; a < numcolours; a++)
		{
			rgb.red = *(colourdata++) >> 4;
			rgb.green = *(colourdata++) >> 4;
			rgb.blue = *(colourdata++) >> 4;
			ctable4[a] = (rgb.red << 8) + (rgb.green << 4) + rgb.blue;
		}
	}
}
