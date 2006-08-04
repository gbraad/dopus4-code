#include <string.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include "libraries/dopus.h"
#include	"dopus/dopusmessage.h"
#include "dopus.h"
#include "mui.h"

struct MUI_CustomClass *CL_App;
struct MUI_CustomClass *CL_FileList;
struct MUI_CustomClass *CL_Clock;

APTR dopusapp, dopuswin;
APTR dopusgads, dopusstatus;
APTR *dopusgadarray;
APTR dopusdirlist[2];

/**********************************************************************
	ListClass
**********************************************************************/

STATIC ULONG mListNew(struct IClass *cl, APTR obj, struct opSet *msg)
{
	return (ULONG)DoSuperNew(cl, obj,
		InputListFrame,
		MUIA_Font, MUIV_Font_Fixed,
		MUIA_List_Format, ",PREPARSE=\33r,PREPARSE=\33c,,",
		MUIA_List_Title, TRUE,
		TAG_MORE, msg->ops_AttrList);
}

STATIC ULONG mListConstruct(APTR obj, struct MUIP_List_Construct *msg)
{
	struct Directory *newentry;

	newentry = AllocVecPooled(msg->pool, sizeof(*newentry));

	if (newentry)
	{
		CopyMem(msg->entry, newentry, sizeof(*newentry));
	}

	return (ULONG)newentry;
}

STATIC ULONG mListDestruct(APTR obj, struct MUIP_List_Construct *msg)
{
	FreeVecPooled(msg->pool, msg->entry);

	return 0;
}

STATIC ULONG mListCompare(APTR obj, struct MUIP_List_Compare *msg)
{
	struct Directory *e1, *e2;
	LONG rc = 0;

	e1 = msg->entry1;
	e2 = msg->entry2;

	rc = e2->type - e1->type;

	if (!rc)
	{
		rc = stricmp(e1->name, e2->name);
	}

	return rc;
}

STATIC ULONG mListDisplay(APTR obj, struct MUIP_List_Display *msg)
{
	CONST_STRPTR *array = (CONST_STRPTR *)msg->array;
	struct Directory *entry;

	entry = (struct Directory *)msg->entry;
	array = (CONST_STRPTR *)msg->array;

	if (entry)
	{
		static TEXT sizetxt[20];

		sizetxt[0] = '\0';

		if (entry->size >= 0)
			NewRawDoFmt("%lld", NULL, sizetxt, entry->size);

		array[0]	= entry->name;
		array[1] = sizetxt;
		array[2] = entry->protbuf;
		array[3] = entry->datebuf;
		array[4] = entry->comment ? entry->comment : "";
	}
	else
	{
		array[0] = "Name";
		array[1] = "Size";
		array[2] = "Protection";
		array[3] = "Date";
		array[4] = "Comment";
	}

	return 0;
}

STATIC ULONG mListDispatcher(void)
{
	struct IClass *cl;
	APTR obj;
	Msg msg;

	cl = (APTR)REG_A0;
	msg = (APTR)REG_A1;
	obj = (APTR)REG_A2;

	switch (msg->MethodID)
	{
		case OM_NEW					: return mListNew			(cl, obj, (APTR)msg);
		case MUIM_List_Compare	: return mListCompare	(obj, (APTR)msg);
		case MUIM_List_Construct: return mListConstruct	(obj, (APTR)msg);
		case MUIM_List_Destruct	: return mListDestruct	(obj, (APTR)msg);
		case MUIM_List_Display	: return mListDisplay	(obj, (APTR)msg);
	}

	return DoSuperMethodA(cl, obj, msg);
}

/**********************************************************************
	ClockClass
**********************************************************************/

struct Clock_Data
{
	struct MUI_InputHandlerNode	ihnode;
	ULONG	chipnum;
	ULONG fastnum;
	ULONG totalnum;
};

static int getmaxmem(ULONG type)
{
	ULONG size = 0;
	int a;

	size = AvailMem(type | MEMF_TOTAL);
	size = (size + 1023) / 1024;
	for(a = 1;; a++)
		if(!(size /= 10))
			break;
	return (a);
}

STATIC ULONG mClockNew(struct IClass *cl, APTR obj, struct opSet *msg)
{
	obj = DoSuperNew(cl, obj,
		TextFrame,
		MUIA_Background, MUII_TextBack,
		MUIA_DoubleBuffer, TRUE,
		MUIA_Text_PreParse, "\33c",
		TAG_MORE, msg->ops_AttrList);

	if (obj)
	{
		struct Clock_Data *data = (struct Clock_Data *)INST_DATA(cl, obj);

		data->ihnode.ihn_Object	= obj;
		data->ihnode.ihn_Millis	= 1000;
		data->ihnode.ihn_Flags	= MUIIHNF_TIMER;
		data->ihnode.ihn_Method	= MM_Clock_Update;

		data->chipnum = getmaxmem(MEMF_CHIP);
		data->fastnum = getmaxmem(MEMF_FAST);
		data->totalnum = getmaxmem(MEMF_ANY);
	}

	return (ULONG)obj;
}

STATIC ULONG mClockSetup(struct IClass *cl, APTR obj, Msg msg)
{
	ULONG rc = DoSuperMethodA(cl, obj, msg);

	if (rc)
	{
		struct Clock_Data *data = (struct Clock_Data *)INST_DATA(cl, obj);
		DoMethod(_app(obj), MUIM_Application_AddInputHandler, &data->ihnode);
	}

	return rc;
}

STATIC VOID mClockCleanup(struct IClass *cl, APTR obj)
{
	struct Clock_Data *data = (struct Clock_Data *)INST_DATA(cl, obj);

	DoMethod(_app(obj), MUIM_Application_RemInputHandler, &data->ihnode);
}

STATIC ULONG mClockUpdate(struct IClass *cl, APTR obj)
{
	struct Clock_Data *data = (struct Clock_Data *)INST_DATA(cl, obj);
	TEXT formstring[160];

	formstring[0] = 0;

	if (config->scrclktype & (SCRCLOCK_MEMORY | SCRCLOCK_CPU | SCRCLOCK_DATE | SCRCLOCK_TIME))
	{
		TEXT buf[160], memstring[160];
		ULONG m, s;

		m = (config->scrclktype & SCRCLOCK_BYTES) ? 3 : 0;
		s = (config->scrclktype & SCRCLOCK_BYTES) ? 1 : 0;


		if (config->scrclktype & SCRCLOCK_MEMORY)
		{
			STATIC CONST TEXT Kstr[] = "K  ";
			ULONG chipc, fast;

			chipc = AvailMem(MEMF_CHIP);
			fast = AvailMem(MEMF_FAST);
			
			if (!(config->scrclktype & SCRCLOCK_BYTES))
			{
				chipc /= 1024;
				fast /= 1024;
			}

			if(config->scrclktype & SCRCLOCK_C_AND_F)
			{
				sprintf(memstring, "%lc:%%-%ldld%s", globstring[STR_CLOCK_CHIP][0], data->chipnum + m, Kstr + s);
				sprintf(memstring + strlen(memstring), "%lc:%%-%ldld%s", globstring[STR_CLOCK_FAST][0], data->fastnum + m, Kstr + s);
				sprintf(memstring + strlen(memstring), "%lc:%%-%ldld%s", globstring[STR_CLOCK_TOTAL][0], data->totalnum + m, Kstr + s);
			}
			else
			{
				sprintf(memstring, "%s%%-%ldld%s", globstring[STR_CLOCK_CHIP], data->chipnum + m, Kstr + s);
				sprintf(memstring + strlen(memstring), "%s%%-%ldld%s", globstring[STR_CLOCK_FAST], data->fastnum + m, Kstr + s);
				sprintf(memstring + strlen(memstring), "%s%%-%ldld%s", globstring[STR_CLOCK_TOTAL], data->totalnum + m, Kstr + s);
			}

			sprintf(buf, memstring, chipc, fast, chipc + fast);
			strcat(formstring, buf);
		}

		if (config->scrclktype & SCRCLOCK_CPU)
		{
			ULONG usage = getusage();

			sprintf(buf, "CPU:%3ld%%  ", usage);
			strcat(formstring, buf);
		}

		if (config->scrclktype & (SCRCLOCK_DATE | SCRCLOCK_TIME))
		{
			struct DateTime datetime;
			TEXT time[20], date[20];

			DateStamp(&(datetime.dat_Stamp));
			initdatetime(&datetime, date, time, 0);

			if(config->scrclktype & SCRCLOCK_DATE)
			{
				sprintf(buf, "%-9s  ", date);
				strcat(formstring, buf);
			}

			if (config->scrclktype & SCRCLOCK_TIME)
			{
				if (config->dateformat & DATE_12HOUR)
				{
					ULONG h, m, s, ampm;

					h = datetime.dat_Stamp.ds_Minute / 60;
					m = datetime.dat_Stamp.ds_Minute % 60;
					s = datetime.dat_Stamp.ds_Tick / TICKS_PER_SECOND;

					if (h > 11)
					{
						ampm = 'P';
						h -= 12;
					}
					else
					{
						ampm = 'A';
					}

					if (h == 0)
					{
						h = 12;
					}

					sprintf(time, "%02ld:%02ld:%02ld%lc", h, m, s, ampm);
				}

				strcat(formstring, time);
			}
		}
	}

	set(obj, MUIA_Text_Contents, formstring);

	return 0;
}

STATIC ULONG mClockDispatcher(void)
{
	struct IClass *cl;
	APTR obj;
	Msg msg;

	cl = (APTR)REG_A0;
	msg = (APTR)REG_A1;
	obj = (APTR)REG_A2;

	switch (msg->MethodID)
	{
		case OM_NEW					: return mClockNew		(cl, obj, (APTR)msg);
		case MUIM_Setup			: return mClockSetup		(cl, obj, msg);
		case MUIM_Cleanup			:			mClockCleanup	(cl, obj); break;
		case MM_Clock_Update		: return mClockUpdate	(cl, obj);
	}

	return DoSuperMethodA(cl, obj, msg);
}

/**********************************************************************
	AppClass
**********************************************************************/

STATIC ULONG mDeleteWindow(APTR obj, struct MUIP_Application_DeleteWindow *msg)
{
	DoMethod(obj, OM_REMMEMBER, msg->window);
	return MUI_DisposeObject(msg->window);
}

STATIC ULONG mDeleteFontWindow(APTR obj, struct MUIP_Application_DeleteFontWindow *msg)
{
	DoMethod(obj, OM_REMMEMBER, msg->window);
	MUI_DisposeObject(msg->window);
	CloseFont(msg->font);
	return 0;
}

STATIC ULONG mDispatcher(void)
{
	struct IClass *cl;
	APTR obj;
	Msg msg;

	cl = (APTR)REG_A0;
	msg = (APTR)REG_A1;
	obj = (APTR)REG_A2;

	switch (msg->MethodID)
	{
		case MM_Application_DeleteWindow			: return mDeleteWindow(obj, (APTR)msg);
		case MM_Application_DeleteFontWindow	: return mDeleteFontWindow(obj, (APTR)msg);
	}

	return DoSuperMethodA(cl, obj, msg);
}

/**********************************************************************
	MUI
**********************************************************************/

APTR MakeButton(CONST_STRPTR str)
{
	APTR	obj;

	if ((obj = MUI_MakeObject(MUIO_Button, (IPTR)str)))
		SetAttrs(obj, MUIA_CycleChain, TRUE, TAG_DONE);

	return obj;
}

APTR MakeLLabel(CONST_STRPTR str)
{
    return MUI_MakeObject(MUIO_Label, (IPTR)str, MUIO_Label_LeftAligned | MUIO_Label_DontCopy);
}

APTR MakeText(void)
{
	return TextObject, MUIA_Frame, MUIV_Frame_Text, MUIA_Background, MUII_TextBack, End;
}

APTR MakeString(ULONG maxlen)
{
	APTR obj;

	if ((obj = MUI_MakeObject(MUIO_String, NULL, maxlen)))
		SetAttrs(obj, MUIA_CycleChain, TRUE, TAG_DONE);

	return obj;
}

/**********************************************************************
	MUI Classes
**********************************************************************/

STATIC struct EmulLibEntry mDispatcherTrap      = { TRAP_LIB, 0, (APTR)&mDispatcher };
STATIC struct EmulLibEntry mListDispatcherTrap  = { TRAP_LIB, 0, (APTR)&mListDispatcher };
STATIC struct EmulLibEntry mClockDispatcherTrap = { TRAP_LIB, 0, (APTR)&mClockDispatcher };

ULONG init_classes(void)
{
	if ((CL_App = MUI_CreateCustomClass(NULL, MUIC_Application, NULL, 0, (APTR)&mDispatcherTrap)))
	{
		if ((CL_FileList = MUI_CreateCustomClass(NULL, MUIC_List, NULL, 0, (APTR)&mListDispatcherTrap)))
		{
			if ((CL_Clock = MUI_CreateCustomClass(NULL, MUIC_Text, NULL, sizeof(struct Clock_Data), (APTR)&mClockDispatcherTrap)))
			{
				return 1;
			}
		}
	}

	return 0;
}

VOID delete_classes(void)
{
	if (CL_Clock)
		MUI_DeleteCustomClass(CL_Clock);

	if (CL_FileList)
		MUI_DeleteCustomClass(CL_FileList);

	if (CL_App)
		MUI_DeleteCustomClass(CL_App);
}
