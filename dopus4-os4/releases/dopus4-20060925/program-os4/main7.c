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

static struct DOpusRemember *audio_key;	/* Memory key for 8SVX player */

static struct MsgPort *audio_port[2];	/* 8SVX Sound player Ports */

static struct IOAudio *audio_req1[2];	/* Audio IO Request block #1 */
static struct IOAudio *audio_req2[2];	/* Audio IO Request block #2 */

static STRPTR audiodata;	/* Audio data to play */
static uint32 audio_size;	/* Size of audio data */

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

	for(;;)
	{
		if((fred = WaitForMouseClick(fontwindow)) == -2)
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

static struct Hook AHISoundHook;

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

int doplay8svxold(STRPTR fname, int loop)
{
	struct VoiceHeader *vhdr = NULL;
	STRPTR p8data, psample = NULL, ry, compressbuf;
	ULONG size, class, rsize, chan = 0, pan = Unity / 2, waitbits, sigs;
	USHORT code;
	ChunkHeader *p8chunk;
	uint32 a, b, stereo, *vxcheck, finish, playsize = 0;
	struct IOAudio *audioptr[2];
	UBYTE *playdata[2];

	status_flags &= ~STATUS_AUDIOLED;

	if((a = readfile(fname, (char **)&audiodata, &audio_size)))
	{
		if(a == -1)
			return (0);
		return (-2);
	}
	vxcheck = (uint32 *)audiodata;
	if(audio_size < 12 || vxcheck[0] != ID_FORM || vxcheck[2] != ID_8SVX)
	{			// Raw data
		size = audio_size;
		psample = p8data = audiodata;
	}
	else
	{			// 8SVX sample
		p8data = audiodata + 12;
		size = 0;
		while(p8data < audiodata + vxcheck[1])
		{
			p8chunk = (ChunkHeader *) p8data;
			p8data += sizeof(ChunkHeader);
			switch (p8chunk->ckID)
			{
			case ID_VHDR:
				vhdr = (struct VoiceHeader *)p8data;
				break;
			case ID_BODY:
				psample = p8data;
				size = p8chunk->ckSize;
				break;
			case ID_CHAN:
				chan = *((ULONG *) p8data);
				break;
			case ID_PAN:
				pan = *((ULONG *) p8data);
				break;
			}
			p8data += p8chunk->ckSize;
			if(p8chunk->ckSize & 1)
				++p8data;
		}
	}

	if(size > vxcheck[1])
	{
		size /= 2;
		chan = 0;
	}

	if(vhdr)
	{
		if(vhdr->vh_Compression == CMP_FIBDELTA)
		{
			size -= 2;
			if(!(compressbuf = IDOpus->LAllocRemember(&audio_key, size * 2, MEMF_ANY)))
				return (-2);
			DUnpack(psample + 2, size, compressbuf, psample[1]);
			psample = compressbuf;
			size *= 2;
		}
	}

	if(chan == SAMPLETYPE_Stereo)
	{
		size /= 2;
		stereo = size;
	}
	else
		stereo = 0;

	if(useAHI)
	{
		if(OpenAHI())
		{
			AHISoundHook.h_Entry = AHISoundFunc;
			if((actrl = IAHI->AHI_AllocAudio(AHIA_AudioID, AHI_DEFAULT_ID, AHIA_MixFreq, AHI_DEFAULT_FREQ, AHIA_Channels, stereo ? 2 : 1, AHIA_Sounds, stereo ? 2 : 1, loop ? TAG_IGNORE : AHIA_SoundFunc, (Tag)&AHISoundHook, AHIA_UserData, (Tag) IExec->FindTask(NULL), TAG_DONE)))
			{
				struct AHISampleInfo sample = { AHIST_M8S, psample, size };
				UWORD snd0, snd1 = 1;

				snd0 = IAHI->AHI_LoadSound(0, AHIST_SAMPLE, &sample, actrl);
				if(stereo)
				{
					sample.ahisi_Address = psample + stereo;
					snd1 = IAHI->AHI_LoadSound(1, AHIST_SAMPLE, &sample, actrl);
				}
				if((snd0 != AHI_NOSOUND) && (snd1 != AHI_NOSOUND))
				{
					if((AHIsignal = IExec->AllocSignal(-1)) != -1)
					{
						struct TagItem ahitags2[] =
						{
							{ AHIP_BeginChannel, 1 },
							{ AHIP_Freq, vhdr ? vhdr->vh_SamplesPerSec : 10000 },
							{ AHIP_Vol, vhdr ? vhdr->vh_Volume : Unity },
							{ AHIP_Pan, 0L },
							{ AHIP_Sound, 1 },
							{ AHIP_EndChannel, 0L },
							{ TAG_END, 0 }
						};

						IAHI->AHI_ControlAudio(actrl, AHIC_Play, TRUE, TAG_END);

						IAHI->AHI_Play(actrl, AHIP_BeginChannel, 0, AHIP_Freq, vhdr ? vhdr->vh_SamplesPerSec : 10000, AHIP_Vol, vhdr ? vhdr->vh_Volume : Unity, AHIP_Pan, stereo ? Unity : pan, AHIP_Sound, 0, AHIP_EndChannel, NULL, stereo ? TAG_MORE : TAG_END, (Tag) ahitags2);
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
		else
			CloseAHI();
	}
	if(actrl == NULL)
	{
		static UBYTE audiochannels[2][4] = { {8 + 1, 1, 8, 4}, {4 + 2, 2, 4, 8} };

		for(a = 0; a < 2; a++)
		{
			if(!(audio_req1[a] = IDOpus->LAllocRemember(&audio_key, sizeof(struct IOAudio), MEMF_CLEAR)) || !(audio_req2[a] = IDOpus->LAllocRemember(&audio_key, sizeof(struct IOAudio), MEMF_CLEAR)) || !(audio_port[a] = IExec->CreatePort(NULL, 0)))
				return (-2);
		}

		for(a = 0; a < 2; a++)
		{
			audio_req1[a]->ioa_Request.io_Message.mn_ReplyPort = audio_port[a];
			audio_req1[a]->ioa_Request.io_Message.mn_Node.ln_Pri = 75;
			audio_req1[a]->ioa_Data = audiochannels[a];
			audio_req1[a]->ioa_Length = sizeof(audiochannels[a]);
			if(IExec->OpenDevice("audio.device", 0, (struct IORequest *)audio_req1[a], 0))
				return (-6);
		}

		playsize = (size < 25600) ? size : 25600;
		for(a = 0; a < 2; a++)
		{
			if(!(playdata[a] = IDOpus->LAllocRemember(&audio_key, playsize, MEMF_CHIP)))
				return (-2);
		}

		for(a = 0; a < 2; a++)
		{
			audio_req1[a]->ioa_Request.io_Command = CMD_WRITE;
			audio_req1[a]->ioa_Request.io_Flags = ADIOF_PERVOL;
			audio_req1[a]->ioa_Volume = 64 * (a ? Unity - pan : pan) / Unity;
			audio_req1[a]->ioa_Period = data_colorclock / (vhdr ? vhdr->vh_SamplesPerSec : 10000);
			audio_req1[a]->ioa_Cycles = 1;
			IExec->CopyMem((char *)audio_req1[a], (char *)audio_req2[a], sizeof(struct IOAudio));
			audio_req1[a]->ioa_Data = (UBYTE *) playdata[a];
			audio_req2[a]->ioa_Data = (UBYTE *) playdata[a] + playsize / 2;
			audioptr[a] = audio_req2[a];
		}
	}

	waitbits = 1 << Window->UserPort->mp_SigBit;
	if(actrl == NULL)
		waitbits |= 1 << audio_port[0]->mp_SigBit | 1 << audio_port[1]->mp_SigBit;
	else if(!loop)
		waitbits |= 1 << AHIsignal;

	{
		ry = psample;
		rsize = size;
		finish = 0;
		FOREVER
		{
			if(actrl == NULL)
			{
				size = rsize;
				psample = ry;
				for(a = 0; a < 2; a++)
				{
					IExec->CopyMem(psample + (a * stereo), (char *)playdata[a], playsize);
					audio_req1[a]->ioa_Length = audio_req2[a]->ioa_Length = playsize / 2;
					audioptr[a] = audio_req2[a];
				}
				psample += playsize;
				size -= playsize;
				for(a = 0; a < 2; a++)
					IExec->BeginIO((struct IORequest *)audio_req1[a]);
				for(a = 0; a < 2; a++)
					IExec->BeginIO((struct IORequest *)audio_req2[a]);
				a = 0;
			}
			FOREVER
			{
				sigs = IExec->Wait(waitbits);
				while(getintuimsg())
				{
					class = IMsg->Class;
					code = IMsg->Code;
					IExec->ReplyMsg((struct Message *)IMsg);
					if(class == IDCMP_MOUSEBUTTONS)
					{
						if(code == SELECTDOWN)
							return (1);
						else if(code == MENUDOWN)
							return (-1);
					}
					else if(class == IDCMP_RAWKEY)
					{
						if(code == RAWKEY_ESC)
							return (-1);
						else if(code == RAWKEY_Q || code == RAWKEY_X)
							return (1);
					}
				}
				if(actrl == NULL)
				{
					for(b = 0; b < 2; b++)
					{
						if(IExec->GetMsg(audio_port[b]) && !(a & (1 << b)))
						{
							if(size > 0)
							{
								if(audioptr[b] == audio_req1[b])
									audioptr[b] = audio_req2[b];
								else
									audioptr[b] = audio_req1[b];
								audioptr[b]->ioa_Length = (size < (playsize / 2)? size : (playsize / 2));
								IExec->CopyMem(psample + (b * stereo), (char *)audioptr[b]->ioa_Data, audioptr[b]->ioa_Length);
							}
							a |= 1 << b;
						}
					}
					if(a == 3)
					{
						if(size <= 0)
						{
							if(finish)
							{
								finish = 0;
								break;
							}
							finish = 1;
						}
						else
						{
							size -= audioptr[0]->ioa_Length;
							psample += playsize / 2;
							for(b = 0; b < 2; b++)
								IExec->BeginIO((struct IORequest *)audioptr[b]);
						}
						a = 0;
					}
				}
				else if(sigs & (1 << AHIsignal))
				{
					break;
				}
			}
			if(!loop)
				return (1);
		}
	}
}

void kill8svx()
{
	int a;

	if(actrl)
	{
		IAHI->AHI_ControlAudio(actrl, AHIC_Play, FALSE, TAG_DONE);
		IExec->FreeSignal(AHIsignal);
		CloseAHI();
	}
	else
	{
		for(a = 0; a < 2; a++)
		{
			if(audio_req2[a] && audio_req2[a]->ioa_Request.io_Device)
			{
				audio_req2[a]->ioa_Request.io_Command = CMD_FLUSH;
				IExec->DoIO((struct IORequest *)audio_req2[a]);
			}
			if(audio_req1[a] && audio_req1[a]->ioa_Request.io_Device)
			{
				audio_req1[a]->ioa_Request.io_Command = CMD_FLUSH;
				IExec->DoIO((struct IORequest *)audio_req1[a]);
				IExec->CloseDevice((struct IORequest *)audio_req1[a]);
			}
			audio_req1[a] = NULL;
			audio_req2[a] = NULL;
		}
		for(a = 0; a < 2; a++)
		{
			if(audio_port[a])
			{
				while(IExec->GetMsg(audio_port[a]));
				IExec->DeletePort(audio_port[a]);
				audio_port[a] = NULL;
			}
		}
	}
	if(audiodata && audio_size)
		IExec->FreeVec(audiodata);
	IDOpus->LFreeRemember(&audio_key);
	audiodata = NULL;
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

static char codetodelta[16] = { -34, -21, -13, -8, -5, -3, -2, -1, 0, 1, 2, 3, 5, 8, 13, 21 };

char DUnpack(STRPTR source, int n, STRPTR dest, char x)
{
	unsigned char d;
	int i, lim;

	lim = n << 1;
	for(i = 0; i < lim; ++i)
	{
		d = source[i >> 1];
		if(i & 1)
			d &= 0xf;
		else
			d >>= 4;
		x += codetodelta[d];
		dest[i] = x;
	}
	return (x);
}
