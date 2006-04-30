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
#include <proto/ahi.h>
#include <workbench/icon.h>

#ifndef SAMPLETYPE_Stereo
#define SAMPLETYPE_Stereo SVX_STEREO
#endif

#define RAWKEY_ESC 0x45
#define RAWKEY_Q   0x10
#define RAWKEY_X   0x32

struct Library *AHIBase;
struct AHIIFace *IAHI;
static struct MsgPort *AHImp;
static struct AHIRequest *AHIio;
static BYTE AHIDevice = -1;
static struct AHIAudioCtrl *actrl;

BOOL useAHI = FALSE;

int showpic(STRPTR fullname, int np)
{
	int res, a;
	char buf[256];

	if(checkexec(fullname) && checkisfont(fullname, buf))
		return ((showfont(buf, atoi(IDOS->FilePart(fullname)), np)));

	a = strlen(fullname);
	if(a > 5 && strcmp(&fullname[a - 5], ".info") == 0)
	{
		if((res = readicon(fullname, np)) == 0)
			return (1);
		if(res == -2)
		{
			doerror(-1);
			return (0);
		}
		if(res == -3)
		{
			dostatustext(globstring[STR_NO_CHIP_FOR_ICON]);
			return (0);
		}
		return (res);
	}

	if(!(res = LoadPic(fullname)))
		doerror(-1);
	else if(res == IFFERR_NOTILBM)
		dostatustext(globstring[STR_FILE_NOT_ILBM]);
	else if(res == IFFERR_BADIFF)
		dostatustext(globstring[STR_ERROR_IN_IFF]);
	else if(res == IFFERR_NOMEMORY)
		dostatustext(globstring[STR_NO_CHIP_FOR_PICTURE]);
	else if(res == IFFERR_BADMODE)
		dostatustext(globstring[STR_SCREEN_MODE_UNAVAILABLE]);
	else if(res == IFFERR_NOSCREEN)
		dostatustext(globstring[STR_UNABLE_TO_OPEN_WINDOW]);
	if(res == -1)
		return (-1);
	else if(!res || res < -1)
		return (0);
	return (1);
}

int checkisfont(STRPTR pathname, STRPTR fontname)
{
	int a;
	char fontsize[36], fontpath[256], *ptr;

	strcpy(fontpath, pathname);
	if((ptr = IDOS->FilePart(fontpath)))
	{
		strcpy(fontsize, ptr);
		*(--ptr) = 0;
		if((ptr = IDOS->FilePart(fontpath)))
		{
			for(a = 0;; a++)
			{
				if(!(_isdigit(fontsize[a])))
					break;
			}
			if(!fontsize[a])
			{
				strcat(fontpath, ".font");
				if(IDOpus->CheckExist(fontpath, NULL))
				{
					strcpy(fontname, fontpath);
					return (1);
				}
			}
		}
	}
	return (0);
}

int readicon(STRPTR name, int np)
{
	struct DiskObject *dobj;
	struct Gadget *gad;
	struct Image *icon_image[2];
	UWORD coltab[256];
	int fred, ret, width, height, x, y, x1, y1, imagenum, depth;

	if(!IconBase)
		return (0);

	name[(strlen(name) - 5)] = 0;
	dobj = IIcon->GetIconTags(name, ICONGETA_Screen, NULL, ICONGETA_GenerateImageMasks, FALSE, TAG_END);

	if(dobj == NULL)
		return (-2);

	gad = &(dobj->do_Gadget);

	icon_image[0] = (struct Image *)1;
	depth = 8;

	if(!icon_image[0] || !(setupfontdisplay(depth, coltab)))
	{
		IIcon->FreeDiskObject(dobj);
		return (-3);
	}

	IGraphics->LoadRGB4(&fontscreen->ViewPort, coltab, 256);
	IIcon->LayoutIconA(dobj, fontscreen, NULL);
	IGraphics->LoadRGB4(&fontscreen->ViewPort, nullpalette, 256);
	icon_image[0] = (struct Image *)(gad->GadgetRender);

	if(gad->Flags & GFLG_GADGHIMAGE)
		icon_image[1] = (struct Image *)gad->SelectRender;
	else
		icon_image[1] = icon_image[0];

	for(x = 0; x < 2; x++)
		icon_image[x]->NextImage = NULL;

	imagenum = 0;

	IIntuition->ScreenToFront(fontscreen);
	IIntuition->ActivateWindow(fontwindow);

	width = fontscreen->Width;
	height = fontscreen->Height;

	IIntuition->DrawImage(fontwindow->RPort, icon_image[0], ((width - icon_image[0]->Width) / 2), ((height - icon_image[0]->Height) / 2));

	FadeRGB4(fontscreen, coltab, (1 << depth), 1, config->fadetime);
	show_global_icon = dobj;
	show_global_icon_name = IDOS->FilePart(name);

	FOREVER
	{
		if((fred = WaitForMouseClick(2, fontwindow)) == -2)
		{
			imagenum = 1 - imagenum;
			x = (width - icon_image[imagenum]->Width) / 2;
			y = (height - icon_image[imagenum]->Height) / 2;
			x1 = x + icon_image[imagenum]->Width;
			y1 = y + icon_image[imagenum]->Height;
			IIntuition->DrawImage(fontwindow->RPort, icon_image[imagenum], x, y);
			drawrecaround(fontwindow->RPort, x, y, x1, y1, width, height);
		}
		else
		{
			if(fred == 0 || fred == -3)
				ret = TRUE;
			else
				ret = -1;
			break;
		}
	}
	show_global_icon = NULL;
	if(fred != -3)
		FadeRGB4(fontscreen, coltab, (1 << depth), -1, config->fadetime);
	IIcon->FreeDiskObject(dobj);
	cleanup_fontdisplay();
	return (ret);
}

void drawrecaround(struct RastPort *r, int x, int y, int x1, int y1, int width, int height)
{
	UBYTE o;

	o = IGraphics->GetAPen(r);
	IGraphics->SetAPen(r, 0);
	if(x > 0)
		IGraphics->RectFill(r, 0, 0, x - 1, height - 1);
	if(x1 < width)
		IGraphics->RectFill(r, x1, 0, width - 1, height - 1);
	if(y > 0)
		IGraphics->RectFill(r, 0, 0, width - 1, y - 1);
	if(y1 < height)
		IGraphics->RectFill(r, 0, y1, width - 1, height - 1);
	IGraphics->SetAPen(r, o);
}

/******************************************************************************
**** OpenAHI ******************************************************************
******************************************************************************/

/* Open the device for low-level usage */

BOOL OpenAHI(void)
{
	if((AHImp = IExec->CreateMsgPort()))
	{
		if((AHIio = (struct AHIRequest *)IExec->CreateIORequest(AHImp, sizeof(struct AHIRequest))))
		{
			AHIio->ahir_Version = 4;
			if(!(AHIDevice = IExec->OpenDevice(AHINAME, AHI_NO_UNIT, (struct IORequest *)AHIio, 0)))
			{
				AHIBase = (struct Library *)AHIio->ahir_Std.io_Device;
				if((IAHI = (struct AHIIFace *)IExec->GetInterface(AHIBase, "main", 1, NULL)))
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


/******************************************************************************
**** CloseAHI *****************************************************************
******************************************************************************/

/* Close the device */

void CloseAHI(void)
{
	IAHI->AHI_FreeAudio(actrl);
	actrl = NULL;
	if(!AHIDevice)
		IExec->CloseDevice((struct IORequest *)AHIio);
	AHIDevice = -1;
	IExec->DeleteIORequest((struct IORequest *)AHIio);
	AHIio = NULL;
	IExec->DeleteMsgPort(AHImp);
	AHImp = NULL;
	IExec->DropInterface((struct Interface *)IAHI);
	IAHI = NULL;
}

BYTE AHIsignal = -1;
static int AHIloops;

ULONG AHISoundFunc(struct AHIAudioCtrl *actrl, struct AHISoundMessage *smsg)
{
	if(AHIloops)
	{
		IAHI->AHI_SetSound(smsg->ahism_Channel, AHI_NOSOUND, 0, 0, actrl, 0);
		IExec->Signal(actrl->ahiac_UserData, 1L << AHIsignal);
	}
	else
	{
		AHIloops++;
	}
	return 0;
}

//static struct Hook AHISoundHook;

int doplay8svx(STRPTR fname, int loop)
{
	Object *o;
	BYTE sig;

	if((o = IDataTypes->NewDTObject(fname, DTA_GroupID, GID_SOUND, TAG_END)))
	{
		if((sig = IExec->AllocSignal(-1)) != -1)
		{
			IIntuition->SetAttrs(o, SDTA_SignalTask, IExec->FindTask(NULL), SDTA_SignalBit, 1L << sig, DTA_Repeat, loop ? TRUE : FALSE, TAG_END);
			IDataTypes->DoDTMethod(o, NULL, NULL, DTM_TRIGGER, NULL, STM_PLAY, NULL);
			IExec->Wait((1L << sig) | SIGBREAKF_CTRL_C);
			IExec->FreeSignal(sig);
		}
		IDataTypes->DisposeDTObject(o);
	}
	else
	{
		return 0;
	}
		
	return 1;
}

void handle8svxerror(int res)
{
	switch (res)
	{
	case 0:
		doerror(-1);
		break;
	case -2:
		doerror(ERROR_NO_FREE_STORE);
		break;
	case -3:
	case -4:
		dostatustext(globstring[STR_ERROR_IN_IFF]);
		break;
	case -6:
		dostatustext(globstring[STR_CANT_ALLOCATE_AUDIO]);
		break;
	}
}

void dosound(int type)
{
	if(type)
	{
		struct IOAudio audio;
		static UBYTE achannels[8] = { 1 + 2, 1 + 4, 2 + 8, 4 + 8, 1, 2, 4, 8 };
		int a;

		if(useAHI)
		{
			if(OpenAHI())
			{
				if((actrl = IAHI->AHI_AllocAudio(AHIA_AudioID, AHI_DEFAULT_ID, AHIA_MixFreq, AHI_DEFAULT_FREQ, AHIA_Channels, 1, AHIA_Sounds, 1, TAG_DONE)))
				{
					struct AHISampleInfo sample = { AHIST_M8S, beepwave, 16 };

					if(IAHI->AHI_LoadSound(0, AHIST_SAMPLE, &sample, actrl) != AHI_NOSOUND)
					{
						IAHI->AHI_ControlAudio(actrl, AHIC_Play, TRUE, TAG_END);

						for(a = 0; a < 11; a++)
						{
							IAHI->AHI_Play(actrl, AHIP_BeginChannel, 0, AHIP_Freq, (a % 2) ? 6000 : 9000, AHIP_Vol, 0x10000L, AHIP_Pan, 0x8000L, AHIP_Sound, 0, AHIP_EndChannel, NULL, TAG_END);
							IDOS->Delay(6);
						}

						IAHI->AHI_ControlAudio(actrl, AHIC_Play, FALSE, TAG_DONE);

						CloseAHI();

						return;
					}
					else
						CloseAHI();
				}
				else
					CloseAHI();
			}
			else
				CloseAHI();
		}
		audio.ioa_Request.io_Message.mn_ReplyPort = general_port;
		audio.ioa_Request.io_Message.mn_Node.ln_Pri = 90;
		audio.ioa_Data = achannels;
		audio.ioa_Length = sizeof(achannels);
		if(IExec->OpenDevice("audio.device", 0, (struct IORequest *)&audio, 0) == 0)
		{
			audio.ioa_Request.io_Command = CMD_WRITE;
			audio.ioa_Request.io_Flags = ADIOF_PERVOL;
			audio.ioa_Volume = 64;
			audio.ioa_Data = (UBYTE *) beepwave;
			audio.ioa_Length = 16;
			audio.ioa_Cycles = 60;

			for(a = 0; a < 11; a++)
			{
				audio.ioa_Period = (a % 2) ? 600 : 400;
				IExec->BeginIO((struct IORequest *)&audio);
				IExec->WaitIO((struct IORequest *)&audio);
			}

			IExec->CloseDevice((struct IORequest *)&audio);
		}
		else
		{
			dostatustext(globstring[STR_CANT_ALLOCATE_AUDIO]);
		}
	}
	else
	{
		IIntuition->DisplayBeep(NULL);
	}
}

int playmod(STRPTR name)
{
	return (1);
}

int check_is_module(STRPTR name)
{
	return (0);
}
