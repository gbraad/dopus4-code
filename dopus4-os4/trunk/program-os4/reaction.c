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

/* ReAction Requester */
int ra_simplerequest(CONST_STRPTR format, CONST_STRPTR gadgets, uint32 type)
{
	Object *requester;
	uint32 result = 0;
	struct Screen *reqscreen;

	IExec->SetSignal(0L,SIGBREAKF_CTRL_C);

	requester = RequesterObject, REQ_Type, REQTYPE_INFO, REQ_Image, type, REQ_TitleText, globstring[STR_DIRECTORY_OPUS_REQUEST], REQ_BodyText, format, REQ_GadgetText, gadgets, End;
	if(requester)
	{
		result = OpenRequester(requester, Window);
		IIntuition->DisposeObject(requester);
	}

	return result;
}

/* ReAction Progress Window */
enum
{
	OBJ_WINDOW,
	OBJ_MAINGROUP,
	OBJ_GROUP,
	OBJ_FUELGAUGE_ONE,
	OBJ_FUELGAUGE_TWO,
	OBJ_ABORT,

	OBJ_NUM
};

Object *Objects[OBJ_NUM];
#define OBJ(x) Objects[x]
#define GAD(x) (struct Gadget *)Objects[x]

struct Window *progwindow;
int32 fuelargs[] = { 0, 0 };

void abortfunction(Object *obj, struct IntuiMessage *imsg)
{
	status_haveaborted = status_rexxabort = 1;
	IExec->Signal((struct Task *)main_proc, INPUTSIG_ABORT);

	return;
}


Object *ra_progresswindow_build(STRPTR title, int32 totalfiles, char copy_file_indicator)
{
	fuelargs[0] = 0;
	fuelargs[1] = totalfiles;

	OBJ(OBJ_WINDOW) = WindowObject,
		WA_DragBar, TRUE,
		WA_Title, title,
		WA_SmartRefresh, TRUE,
		WA_CloseGadget, FALSE,
		WA_SizeGadget, FALSE,
		WA_DepthGadget, FALSE,
		WA_PubScreen, MainScreen,
		WA_InnerWidth, Window->Width / 2,
		WA_Activate, TRUE,
		WINDOW_Position, /*WPOS_TOPLEFT,*/ WPOS_CENTERSCREEN,
		WINDOW_ParentGroup, OBJ(OBJ_MAINGROUP) = HLayoutObject,
			LAYOUT_AddChild, SpaceObject, End,
			CHILD_WeightedWidth, 0,
			LAYOUT_AddChild, OBJ(OBJ_GROUP) = VLayoutObject,
//			LAYOUT_SpaceOuter, TRUE,
//			LAYOUT_SpaceInner, TRUE,
				LAYOUT_AddChild, SpaceObject, End,
				LAYOUT_AddChild, OBJ(OBJ_FUELGAUGE_ONE) = FuelGaugeObject,
					FUELGAUGE_Min, 0,
					FUELGAUGE_Max, 100,
					FUELGAUGE_Level, 0,
					FUELGAUGE_Ticks, FALSE,
					FUELGAUGE_ShortTicks, FALSE,
					FUELGAUGE_Percent, FALSE,
					FUELGAUGE_Justification, FGJ_CENTER,
				End,
				LAYOUT_AddChild, SpaceObject, End,
				LAYOUT_AddChild, OBJ(OBJ_FUELGAUGE_TWO) = FuelGaugeObject,
					GA_Text, "%ld / %ld",
					FUELGAUGE_Min, 0,
					FUELGAUGE_Max, totalfiles,
					FUELGAUGE_Level, 1,
					FUELGAUGE_Ticks, FALSE,
					FUELGAUGE_ShortTicks, FALSE,
					FUELGAUGE_Percent, FALSE,
					FUELGAUGE_Justification, FGJ_CENTER,
					FUELGAUGE_VarArgs, &fuelargs,
				End,
				LAYOUT_AddChild, SpaceObject, End,
				LAYOUT_AddChild, HLayoutObject,
					LAYOUT_AddChild, SpaceObject, End,
					LAYOUT_AddChild, OBJ(OBJ_ABORT) = ButtonObject,
						GA_ID, OBJ_ABORT,
						GA_RelVerify, TRUE,
						GA_Text, globstring[STR_ABORT],
					End,
					CHILD_WeightedWidth, 0,
					LAYOUT_AddChild, SpaceObject,
					End,
				End,
				LAYOUT_AddChild, SpaceObject, End,
			End,
			CHILD_WeightedWidth, 100,
			LAYOUT_AddChild, SpaceObject, End,
			CHILD_WeightedWidth, 0,
		End,
	End;

	if(!(copy_file_indicator))
	{
		IIntuition->SetAttrs(OBJ(OBJ_GROUP), LAYOUT_RemoveChild, OBJ(OBJ_FUELGAUGE_ONE), TAG_DONE);
	}

	return OBJ(OBJ_WINDOW);
}

struct Window *ra_progresswindow_open(Object *obj)
{
	if(obj)
	{
		progwindow = RA_OpenWindow(obj);
	}

	return progwindow;
}

int ra_progresswindow_close()
{
	RA_CloseWindow(OBJ(OBJ_WINDOW));
	IIntuition->DisposeObject(OBJ(OBJ_WINDOW));

	return 0;
}

int ra_progresswindow_update_one(int64 readwrite, int64 total, STRPTR curname)
{
	if((config->dynamicflags & UPDATE_PROGRESSIND_COPY))
	{
		IIntuition->RefreshSetGadgetAttrs(GAD(OBJ_FUELGAUGE_ONE), progwindow, NULL, GA_Text, curname, FUELGAUGE_Level, (int32)((readwrite * 100) / total), TAG_END);
	}

	return 0;
}

int ra_progresswindow_update_two(int32 value)
{
	fuelargs[0] += value;
	IIntuition->RefreshSetGadgetAttrs(GAD(OBJ_FUELGAUGE_TWO), progwindow, NULL, FUELGAUGE_Level, fuelargs[0], FUELGAUGE_VarArgs, &fuelargs, TAG_END);

	return 0;
}

