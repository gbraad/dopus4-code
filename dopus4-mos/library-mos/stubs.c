#include <proto/alib.h>

#include "functions.h"

STATIC int DOPUS_FileRequest(void)
{
	return _DOpus_FileRequest((struct DOpusFileReq *)REG_A0);
}
STATIC void DOPUS_Do3DBox(void)
{
	_DOpus_Do3DBox((struct RastPort *)REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC void DOPUS_Do3DStringBox(void)
{
	_DOpus_Do3DStringBox((struct RastPort *)REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC void DOPUS_Do3DCycleBox(void)
{
	_DOpus_Do3DCycleBox((struct RastPort *)REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC void DOPUS_DoArrow(void)
{
	_DOpus_DoArrow((struct RastPort *)REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5, REG_D6);
}

STATIC VOID DOPUS_LSprintf(void)
{
//	return _DOpus_LSprintfA();
}

STATIC APTR DOPUS_LCreateExtIO(void)
{
	APTR port = (APTR)REG_A0;
	ULONG size = REG_D0;

	return CreateIORequest(port, size);
}

STATIC APTR DOPUS_LCreatePort(void)
{
	APTR name = (APTR)REG_A0;
	ULONG pri = REG_D0;

	return CreatePort(name, pri);
}

STATIC VOID DOPUS_LDeleteExtIO(void)
{
	APTR io = (APTR)REG_A0;
	DeleteIORequest(io);
}

STATIC VOID DOPUS_LDeletePort(void)
{
	APTR port = (APTR)REG_A0;
	DeletePort(port);
}

STATIC ULONG DOPUS_LToUpper(void)
{
	return toupper(REG_D0);
}

STATIC ULONG DOPUS_LToLower(void)
{
	return tolower(REG_D0);
}

STATIC void DOPUS_LStrCat(void)
{
	_DOpus_LStrCat((char *)REG_A0, (char *)REG_A1);
}
STATIC void DOPUS_LStrnCat(void)
{
	_DOpus_LStrnCat((char *)REG_A0, (char *)REG_A1, REG_D0);
}
STATIC void DOPUS_LStrCpy(void)
{
	_DOpus_LStrCpy((char *)REG_A0, (char *)REG_A1);
}
STATIC void DOPUS_LStrnCpy(void)
{
	_DOpus_LStrnCpy((char *)REG_A0, (char *)REG_A1, REG_D0);
}
STATIC int DOPUS_LStrCmp(void)
{
	return _DOpus_LStrCmp((char *)REG_A0, (char *)REG_A1);
}
STATIC int DOPUS_LStrnCmp(void)
{
	return _DOpus_LStrnCmp((char *)REG_A0, (char *)REG_A1, REG_D0);
}
STATIC int DOPUS_LStrCmpI(void)
{
	return _DOpus_LStrCmpI((char *)REG_A0, (char *)REG_A1);
}
STATIC int DOPUS_LStrnCmpI(void)
{
	return _DOpus_LStrnCmpI((char *)REG_A0, (char *)REG_A1, REG_D0);
}
STATIC int DOPUS_StrCombine(void)
{
	return _DOpus_StrCombine((char *)REG_A0, (char *)REG_A1, (char *)REG_A2, REG_D0);
}
STATIC int DOPUS_StrConcat(void)
{
	return _DOpus_StrConcat((char *)REG_A0, (char *)REG_A1, REG_D0);
}
STATIC APTR DOPUS_LParsePattern(void)
{
//	return _DOpus_LParsePattern(REG_A0, REG_A1);
	return NULL;
}

STATIC APTR DOPUS_LMatchPattern(void)
{
//	return _DOpus_LMatchPattern(REG_A0, REG_A1);
	return NULL;
}

STATIC APTR DOPUS_LParsePatternI(void)
{
//	return _DOpus_LParsePatternI(REG_A0, REG_A1);
	return NULL;
}

STATIC APTR DOPUS_LMatchPatternI(void)
{
//	return _DOpus_LMatchPatternI(REG_A0, REG_A1);
	return NULL;
}

STATIC APTR DOPUS_BtoCStr(void)
{
//	return _DOpus_BtoCStr(REG_A0, REG_A1, REG_D0);
	return NULL;
}

STATIC int DOPUS_Assign(void)
{
	return _DOpus_Assign((char *)REG_A0, (char *)REG_A1);
}

STATIC APTR DOPUS_BaseName(void)
{
//	return _DOpus_BaseName(REG_A0);
	return NULL;
}

STATIC APTR DOPUS_CompareLock(void)
{
//	return _DOpus_CompareLock(REG_A0, REG_A1);
	return NULL;
}

STATIC int DOPUS_PathName(void)
{
	return _DOpus_PathName(REG_A0, (char *)REG_A1, REG_D0);
}
STATIC APTR DOPUS_SendPacket(void)
{
//	return _DOpus_SendPacket(REG_A0, REG_D0, REG_A1, REG_D1);
	return NULL;
}

STATIC int DOPUS_TackOn(void)
{
	return _DOpus_TackOn((char *)REG_A0, (char *)REG_A1, REG_D0);
}
STATIC void DOPUS_StampToStr(void)
{
	_DOpus_StampToStr((struct DateTime *)REG_A0);
}
STATIC int DOPUS_StrToStamp(void)
{
	return _DOpus_StrToStamp((struct DateTime *)REG_A0);
}
STATIC int DOPUS_AddListView(void)
{
	return _DOpus_AddListView((struct DOpusListView *)REG_A0, REG_D0);
}
STATIC struct DOpusListView *DOPUS_ListViewIDCMP(void)
{
	return _DOpus_ListViewIDCMP((struct DOpusListView *)REG_A0, (struct IntuiMessage *)REG_A1);
}
STATIC int DOPUS_RefreshListView(void)
{
	return _DOpus_RefreshListView((struct DOpusListView *)REG_A0, REG_D0);
}
STATIC int DOPUS_RemoveListView(void)
{
	return _DOpus_RemoveListView((struct DOpusListView *)REG_A0, REG_D0);
}
STATIC void DOPUS_DrawCheckMark(void)
{
	_DOpus_DrawCheckMark((struct RastPort *)REG_A0, REG_D0, REG_D1, REG_D2);
}
STATIC void DOPUS_FixSliderBody(void)
{
	_DOpus_FixSliderBody((struct Window *)REG_A0, (struct Gadget *)REG_A1, REG_D0, REG_D1, REG_D2);
}
STATIC void DOPUS_FixSliderPot(void)
{
	_DOpus_FixSliderPot((struct Window *)REG_A0, (struct Gadget *)REG_A1, REG_D0, REG_D1, REG_D2, REG_D3);
}
STATIC int DOPUS_GetSliderPos(void)
{
	return _DOpus_GetSliderPos((struct Gadget *)REG_A0, REG_D0, REG_D1);
}
STATIC APTR DOPUS_LAllocRemember(void)
{
	return _DOpus_LAllocRemember((struct DOpusRemember **)REG_A0, REG_D0, REG_D1);
}
STATIC void DOPUS_LFreeRemember(void)
{
	_DOpus_LFreeRemember((struct DOpusRemember **)REG_A0);
}
STATIC void DOPUS_SetBusyPointer(void)
{
	_DOpus_SetBusyPointer((struct Window *)REG_A0);
}
STATIC void DOPUS_GetWBScreen(void)
{
	_DOpus_GetWBScreen((struct Screen *)REG_A0);
}
STATIC int DOPUS_SearchPathList(void)
{
	return _DOpus_SearchPathList((char *)REG_A0, (char *)REG_A1, REG_D0);
}
STATIC int DOPUS_CheckExist(void)
{
	return _DOpus_CheckExist((char *)REG_A0, (int *)REG_A1);
}

STATIC APTR DOPUS_CompareDate(void)
{
//	return _DOpus_CompareDate(REG_A0, REG_A1);
	return NULL;
}

STATIC APTR DOPUS_Seed(void)
{
//	return _DOpus_Seed(REG_D0);
	return NULL;
}

STATIC APTR DOPUS_Random(void)
{
//	return _DOpus_Random(REG_D0);
	return NULL;
}

STATIC void DOPUS_StrToUpper(void)
{
	_DOpus_StrToUpper((char *)REG_A0, (char *)REG_A1);
}
STATIC void DOPUS_StrToLower(void)
{
	_DOpus_StrToLower((char *)REG_A0, (char *)REG_A1);
}
STATIC int DOPUS_RawkeyToStr(void)
{
	return _DOpus_RawkeyToStr(REG_D0, REG_D1, (char *)REG_A0, (char *)REG_A1, REG_D2);
}
STATIC int DOPUS_DoRMBGadget(void)
{
	return _DOpus_DoRMBGadget((struct RMBGadget *)REG_A0, (struct Window *)REG_A1);
}
STATIC int DOPUS_AddGadgets(void)
{
	return _DOpus_AddGadgets((struct Window *)REG_A0, (struct Gadget *)REG_A1, (char **)REG_A2, REG_D0, REG_D1, REG_D2, REG_D3);
}
STATIC void DOPUS_ActivateStrGad(void)
{
	_DOpus_ActivateStrGad((struct Gadget *)REG_A0, (struct Window *)REG_A1);
}
STATIC void DOPUS_RefreshStrGad(void)
{
	_DOpus_RefreshStrGad((struct Gadget *)REG_A0, (struct Window *)REG_A1);
}
STATIC int DOPUS_CheckNumGad(void)
{
	return _DOpus_CheckNumGad((struct Gadget *)REG_A0, (struct Window *)REG_A1, REG_D0, REG_D1);
}
STATIC int DOPUS_CheckHexGad(void)
{
	return _DOpus_CheckHexGad((struct Gadget *)REG_A0, (struct Window *)REG_A1, REG_D0, REG_D1);
}
STATIC int DOPUS_Atoh(void)
{
	return _DOpus_Atoh((char *)REG_A0, REG_D0);
}
STATIC VOID DOPUS_HiliteGad(void)
{
	_DOpus_HiliteGad((struct Gadget *)REG_A0, (struct RastPort *)REG_A1);
}
STATIC int DOPUS_DoSimpleRequest(void)
{
	return _DOpus_DoSimpleRequest((struct Window *)REG_A0, (struct DOpusSimpleRequest *)REG_A1);
}

STATIC int DOPUS_ReadConfig(void)
{
	return _DOpus_ReadConfig((STRPTR)REG_A0, (struct ConfigStuff *)REG_A1);
}
STATIC int DOPUS_SaveConfig(void)
{
	return _DOpus_SaveConfig((char *)REG_A0, (struct ConfigStuff *)REG_A1);
}
STATIC LONG DOPUS_DefaultConfig(void)
{
	return _DOpus_DefaultConfig((struct ConfigStuff *)REG_A0);
}

STATIC LONG DOPUS_GetDevices(void)
{
	return _DOpus_GetDevices((struct ConfigStuff *)REG_A0);
}

STATIC int DOPUS_AssignGadget(void)
{
	return _DOpus_AssignGadget((struct ConfigStuff *)REG_A0, REG_D0, REG_D1, (char *)REG_A1, (char *)REG_A2);
}
STATIC int DOPUS_AssignMenu(void)
{
	return _DOpus_AssignMenu((struct ConfigStuff *)REG_A0, REG_D0, (char *)REG_A1, (char *)REG_A2);
}
STATIC int DOPUS_FindSystemFile(void)
{
	return _DOpus_FindSystemFile((char *)REG_A0, (char *)REG_A1, REG_D0, REG_D1);
}
STATIC VOID DOPUS_Do3DFrame(void)
{
	_DOpus_Do3DFrame((struct RastPort *)REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, (char *)REG_A1, REG_D4, REG_D5);
}
STATIC int DOPUS_FreeConfig(void)
{
	return _DOpus_FreeConfig((struct ConfigStuff *)REG_A0);
}
STATIC VOID DOPUS_DoCycleGadget(void)
{
	_DOpus_DoCycleGadget((struct Gadget *)REG_A0, (struct Window *)REG_A1, (char **)REG_A2, REG_D0);
}

STATIC VOID DOPUS_UScoreText(void)
{
	_DOpus_UScoreText((struct RastPort *)REG_A0, (char *)REG_A1, REG_D0, REG_D1, REG_D2);
}

STATIC VOID DOPUS_DisableGadget(void)
{
	_DOpus_DisableGadget((struct Gadget *)REG_A0, (struct RastPort *)REG_A1, REG_D0, REG_D1);
}
STATIC VOID DOPUS_EnableGadget(void)
{
	_DOpus_EnableGadget((struct Gadget *)REG_A0, (struct RastPort *)REG_A1, REG_D0, REG_D1);
}
STATIC VOID DOPUS_GhostGadget(void)
{
	_DOpus_GhostGadget((struct Gadget *)REG_A0, (struct RastPort *)REG_A1, REG_D0, REG_D1);
}
STATIC VOID DOPUS_DrawRadioButton(void)
{
	_DOpus_DrawRadioButton((struct RastPort *)REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC APTR DOPUS_GetButtonImage(void)
{
	return _DOpus_GetButtonImage(REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5, (struct DOpusRemember **)REG_A0);
}
STATIC VOID DOPUS_ShowSlider(void)
{
	_DOpus_ShowSlider((struct Window *)REG_A0, (struct Gadget*)REG_A1);
}

STATIC int DOPUS_CheckConfig(void)
{
	return _DOpus_CheckConfig((struct ConfigStuff *)REG_A0);
}

STATIC APTR DOPUS_GetCheckImage(void)
{
	return _DOpus_GetCheckImage(REG_D0, REG_D1, REG_D2, (struct DOpusRemember **)REG_A0);
}
STATIC APTR DOPUS_OpenRequester(void)
{
	return _DOpus_OpenRequester((struct RequesterBase *)REG_A0);
}
STATIC VOID DOPUS_CloseRequester(void)
{
	return _DOpus_CloseRequester((struct RequesterBase *)REG_A0);
}
STATIC VOID DOPUS_AddRequesterObject(void)
{
	_DOpus_AddRequesterObject((struct RequesterBase *)REG_A0, (struct TagItem *)REG_A1);
}

STATIC VOID DOPUS_RefreshRequesterObject(void)
{
	_DOpus_RefreshRequesterObject((struct RequesterBase *)REG_A0, (struct RequesterObject *)REG_A1);
}

STATIC void DOPUS_ObjectText(void)
{
	_DOpus_ObjectText((struct RequesterBase *)REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, (char *)REG_A1, REG_D4);
}
STATIC void DOPUS_DoGlassImage(void)
{
	_DOpus_DoGlassImage((struct RastPort *)REG_A0, (struct Gadget *)REG_A1, REG_D0, REG_D1, REG_D2);
}

STATIC VOID DOPUS_Decode_RLE(void)
{
	_DOpus_Decode_RLE((char *)REG_A0, (char *)REG_A1, REG_D0);
}

STATIC int DOPUS_ReadStringFile(void)
{
	return _DOpus_ReadStringFile((struct StringData *)REG_A0, (char *)REG_A1);
}
STATIC int DOPUS_FreeStringFile(void)
{
	return _DOpus_FreeStringFile((struct StringData *)REG_A0);
}

STATIC void DOPUS_LFreeRemEntry(void)
{
	_DOpus_LFreeRemEntry((struct DOpusRemember **)REG_A0, (char *)REG_A1);
}

STATIC void DOPUS_AddGadgetBorders(void)
{
	_DOpus_AddGadgetBorders((struct DOpusRemember **)REG_A0, (struct Gadget *)REG_A1, REG_D0, REG_D1, REG_D2);
}

STATIC void DOPUS_CreateGadgetBorders(void)
{
	_DOpus_CreateGadgetBorders((struct DOpusRemember **)REG_A0, REG_D0, REG_D1, (struct Border **)REG_A1, (struct Border **)REG_A2, REG_D2, REG_D3, REG_D4);
}

STATIC VOID DOPUS_SelectGadget(void)
{
	_DOpus_SelectGadget((struct Window *)REG_A0, (struct Gadget *)REG_A1);
}

STATIC int DOPUS_FSSetMenuStrip(void)
{
	return _DOpus_FSSetMenuStrip((struct Window *)REG_A0, (struct Menu *)REG_A1);
}
