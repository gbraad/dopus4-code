#include <proto/intuition.h>
#include <proto/muimaster.h>

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
