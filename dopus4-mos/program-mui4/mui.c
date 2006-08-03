#include <proto/alib.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

#include "mui.h"

struct MUI_CustomClass *CL_App;

APTR dopusapp, dopuswin;
APTR dopusgads, dopusstatus;

/**********************************************************************
	AppClass
**********************************************************************/

STATIC ULONG mDeleteWindow(APTR obj, struct MUIP_Application_DeleteWindow *msg)
{
	DoMethod(obj, OM_REMMEMBER, msg->window);
	return MUI_DisposeObject(msg->window);
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
		case MM_Application_DeleteWindow	: return mDeleteWindow(obj, (APTR)msg);
	}

	return DoSuperMethodA(cl, obj, msg);
}


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

STATIC struct EmulLibEntry mDispatcherTrap = { TRAP_LIB, 0, (APTR)&mDispatcher };

ULONG init_classes(void)
{
	if ((CL_App = MUI_CreateCustomClass(NULL, MUIC_Application, NULL, 0, (APTR)&mDispatcherTrap)))
	{
		return 1;
	}

	return 0;
}

VOID delete_classes(void)
{
	if (CL_App)
		MUI_DeleteCustomClass(CL_App);
}
