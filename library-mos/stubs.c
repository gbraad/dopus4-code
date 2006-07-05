#include <proto/alib.h>

STATIC APTR DOPUS_FileRequest(void)
{
	return _DOpus_FileRequest(REG_A0);
}
STATIC APTR DOPUS_Do3DBox(void)
{
	return _DOpus_Do3DBox(REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC APTR DOPUS_Do3DStringBox(void)
{
	return _DOpus_Do3DStringBox(REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC APTR DOPUS_Do3DCycleBox(void)
{
	return _DOpus_Do3DCycleBox(REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC APTR DOPUS_DoArrow(void)
{
	return _DOpus_DoArrow(REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5, REG_D6);
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

STATIC APTR DOPUS_LStrCat(void)
{
	return _DOpus_LStrCat(REG_A0, REG_A1);
}
STATIC APTR DOPUS_LStrnCat(void)
{
	return _DOpus_LStrnCat(REG_A0, REG_A1, REG_D0);
}
STATIC APTR DOPUS_LStrCpy(void)
{
	return _DOpus_LStrCpy(REG_A0, REG_A1);
}
STATIC APTR DOPUS_LStrnCpy(void)
{
	return _DOpus_LStrnCpy(REG_A0, REG_A1, REG_D0);
}
STATIC APTR DOPUS_LStrCmp(void)
{
	return _DOpus_LStrCmp(REG_A0, REG_A1);
}
STATIC APTR DOPUS_LStrnCmp(void)
{
	return _DOpus_LStrnCmp(REG_A0, REG_A1, REG_D0);
}
STATIC APTR DOPUS_LStrCmpI(void)
{
	return _DOpus_LStrCmpI(REG_A0, REG_A1);
}
STATIC APTR DOPUS_LStrnCmpI(void)
{
	return _DOpus_LStrnCmpI(REG_A0, REG_A1, REG_D0);
}
STATIC APTR DOPUS_StrCombine(void)
{
	return _DOpus_StrCombine(REG_A0, REG_A1, REG_A2, REG_D0);
}
STATIC APTR DOPUS_StrConcat(void)
{
	return _DOpus_StrConcat(REG_A0, REG_A1, REG_D0);
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

STATIC APTR DOPUS_Assign(void)
{
	return _DOpus_Assign(REG_A0, REG_A1);
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

STATIC APTR DOPUS_PathName(void)
{
	return _DOpus_PathName(REG_A0, REG_A1, REG_D0);
}
STATIC APTR DOPUS_SendPacket(void)
{
//	return _DOpus_SendPacket(REG_A0, REG_D0, REG_A1, REG_D1);
	return NULL;
}

STATIC APTR DOPUS_TackOn(void)
{
	return _DOpus_TackOn(REG_A0, REG_A1, REG_D0);
}
STATIC APTR DOPUS_StampToStr(void)
{
	return _DOpus_StampToStr(REG_A0);
}
STATIC APTR DOPUS_StrToStamp(void)
{
	return _DOpus_StrToStamp(REG_A0);
}
STATIC APTR DOPUS_AddListView(void)
{
	return _DOpus_AddListView(REG_A0, REG_D0);
}
STATIC APTR DOPUS_ListViewIDCMP(void)
{
	return _DOpus_ListViewIDCMP(REG_A0, REG_A1);
}
STATIC APTR DOPUS_RefreshListView(void)
{
	return _DOpus_RefreshListView(REG_A0, REG_D0);
}
STATIC APTR DOPUS_RemoveListView(void)
{
	return _DOpus_RemoveListView(REG_A0, REG_D0);
}
STATIC APTR DOPUS_DrawCheckMark(void)
{
	return _DOpus_DrawCheckMark(REG_A0, REG_D0, REG_D1, REG_D2);
}
STATIC APTR DOPUS_FixSliderBody(void)
{
	return _DOpus_FixSliderBody(REG_A0, REG_A1, REG_D0, REG_D1, REG_D2);
}
STATIC APTR DOPUS_FixSliderPot(void)
{
	return _DOpus_FixSliderPot(REG_A0, REG_A1, REG_D0, REG_D1, REG_D2, REG_D3);
}
STATIC APTR DOPUS_GetSliderPos(void)
{
	return _DOpus_GetSliderPos(REG_A0, REG_D0, REG_D1);
}
STATIC APTR DOPUS_LAllocRemember(void)
{
	return _DOpus_LAllocRemember(REG_A0, REG_D0, REG_D1);
}
STATIC APTR DOPUS_LFreeRemember(void)
{
	return _DOpus_LFreeRemember(REG_A0);
}
STATIC APTR DOPUS_SetBusyPointer(void)
{
	return _DOpus_SetBusyPointer(REG_A0);
}
STATIC APTR DOPUS_GetWBScreen(void)
{
	return _DOpus_GetWBScreen(REG_A0);
}
STATIC APTR DOPUS_SearchPathList(void)
{
	return _DOpus_SearchPathList(REG_A0, REG_A1, REG_D0);
}
STATIC APTR DOPUS_CheckExist(void)
{
	return _DOpus_CheckExist(REG_A0, REG_A1);
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

STATIC APTR DOPUS_StrToUpper(void)
{
	return _DOpus_StrToUpper(REG_A0, REG_A1);
}
STATIC APTR DOPUS_StrToLower(void)
{
	return _DOpus_StrToLower(REG_A0, REG_A1);
}
STATIC APTR DOPUS_RawkeyToStr(void)
{
	return _DOpus_RawkeyToStr(REG_D0, REG_D1, REG_A0, REG_A1, REG_D2);
}
STATIC APTR DOPUS_DoRMBGadget(void)
{
	return _DOpus_DoRMBGadget(REG_A0, REG_A1);
}
STATIC APTR DOPUS_AddGadgets(void)
{
	return _DOpus_AddGadgets(REG_A0, REG_A1, REG_A2, REG_D0, REG_D1, REG_D2, REG_D3);
}
STATIC APTR DOPUS_ActivateStrGad(void)
{
	return _DOpus_ActivateStrGad(REG_A0, REG_A1);
}
STATIC APTR DOPUS_RefreshStrGad(void)
{
	return _DOpus_RefreshStrGad(REG_A0, REG_A1);
}
STATIC APTR DOPUS_CheckNumGad(void)
{
	return _DOpus_CheckNumGad(REG_A0, REG_A1, REG_D0, REG_D1);
}
STATIC APTR DOPUS_CheckHexGad(void)
{
	return _DOpus_CheckHexGad(REG_A0, REG_A1, REG_D0, REG_D1);
}
STATIC APTR DOPUS_Atoh(void)
{
	return _DOpus_Atoh(REG_A0, REG_D0);
}
STATIC APTR DOPUS_HiliteGad(void)
{
	return _DOpus_HiliteGad(REG_A0, REG_A1);
}
STATIC APTR DOPUS_DoSimpleRequest(void)
{
	return _DOpus_DoSimpleRequest(REG_A0, REG_A1);
}

STATIC APTR DOPUS_ReadConfig(void)
{
	return _DOpus_ReadConfig(REG_A0, REG_A1);
}
STATIC APTR DOPUS_SaveConfig(void)
{
	return _DOpus_SaveConfig(REG_A0, REG_A1);
}
STATIC LONG DOPUS_DefaultConfig(void)
{
	return _DOpus_DefaultConfig(REG_A0);
}

STATIC LONG DOPUS_GetDevices(void)
{
	return _DOpus_GetDevices(REG_A0);
}

STATIC APTR DOPUS_AssignGadget(void)
{
	return _DOpus_AssignGadget(REG_A0, REG_D0, REG_D1, REG_A1, REG_A2);
}
STATIC APTR DOPUS_AssignMenu(void)
{
	return _DOpus_AssignMenu(REG_A0, REG_D0, REG_A1, REG_A2);
}
STATIC APTR DOPUS_FindSystemFile(void)
{
	return _DOpus_FindSystemFile(REG_A0, REG_A1, REG_D0, REG_D1);
}
STATIC APTR DOPUS_Do3DFrame(void)
{
	return _DOpus_Do3DFrame(REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_A1, REG_D4, REG_D5);
}
STATIC APTR DOPUS_FreeConfig(void)
{
	return _DOpus_FreeConfig(REG_A0);
}
STATIC APTR DOPUS_DoCycleGadget(void)
{
	return _DOpus_DoCycleGadget(REG_A0, REG_A1, REG_A2, REG_D0);
}

STATIC VOID DOPUS_UScoreText(void)
{
	_DOpus_UScoreText(REG_A0, REG_A1, REG_D0, REG_D1, REG_D2);
}

STATIC APTR DOPUS_DisableGadget(void)
{
	return _DOpus_DisableGadget(REG_A0, REG_A1, REG_D0, REG_D1);
}
STATIC APTR DOPUS_EnableGadget(void)
{
	return _DOpus_EnableGadget(REG_A0, REG_A1, REG_D0, REG_D1);
}
STATIC APTR DOPUS_GhostGadget(void)
{
	return _DOpus_GhostGadget(REG_A0, REG_A1, REG_D0, REG_D1);
}
STATIC APTR DOPUS_DrawRadioButton(void)
{
	return _DOpus_DrawRadioButton(REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5);
}
STATIC APTR DOPUS_GetButtonImage(void)
{
	return _DOpus_GetButtonImage(REG_D0, REG_D1, REG_D2, REG_D3, REG_D4, REG_D5, REG_A0);
}
STATIC APTR DOPUS_ShowSlider(void)
{
	return _DOpus_ShowSlider(REG_A0, REG_A1);
}

STATIC APTR DOPUS_CheckConfig(void)
{
	return _DOpus_CheckConfig(REG_A0);
}

STATIC APTR DOPUS_GetCheckImage(void)
{
	return _DOpus_GetCheckImage(REG_D0, REG_D1, REG_D2, REG_A0);
}
STATIC APTR DOPUS_OpenRequester(void)
{
	return _DOpus_OpenRequester(REG_A0);
}
STATIC APTR DOPUS_CloseRequester(void)
{
	return _DOpus_CloseRequester(REG_A0);
}
STATIC APTR DOPUS_AddRequesterObject(void)
{
	return _DOpus_AddRequesterObject(REG_A0, REG_A1);
}

STATIC VOID DOPUS_RefreshRequesterObject(void)
{
	_DOpus_RefreshRequesterObject(REG_A0, REG_A1);
}

STATIC APTR DOPUS_ObjectText(void)
{
	return _DOpus_ObjectText(REG_A0, REG_D0, REG_D1, REG_D2, REG_D3, REG_A1, REG_D4);
}
STATIC APTR DOPUS_DoGlassImage(void)
{
	return _DOpus_DoGlassImage(REG_A0, REG_A1, REG_D0, REG_D1, REG_D2);
}

STATIC VOID DOPUS_Decode_RLE(void)
{
	_DOpus_Decode_RLE(REG_A0, REG_A1, REG_D0);
}

STATIC APTR DOPUS_ReadStringFile(void)
{
	return _DOpus_ReadStringFile(REG_A0, REG_A1);
}
STATIC APTR DOPUS_FreeStringFile(void)
{
	return _DOpus_FreeStringFile(REG_A0);
}

STATIC APTR DOPUS_LFreeRemEntry(void)
{
	return _DOpus_LFreeRemEntry(REG_A0, REG_A1);
}

STATIC APTR DOPUS_AddGadgetBorders(void)
{
	return _DOpus_AddGadgetBorders(REG_A0, REG_A1, REG_D0, REG_D1, REG_D2);
}

STATIC APTR DOPUS_CreateGadgetBorders(void)
{
	return _DOpus_CreateGadgetBorders(REG_A0, REG_D0, REG_D1, REG_A1, REG_A2, REG_D2, REG_D4, REG_D4);
}

STATIC VOID DOPUS_SelectGadget(void)
{
	_DOpus_SelectGadget(REG_A0, REG_A1);
}

STATIC int DOPUS_FSSetMenuStrip(void)
{
	return _DOpus_FSSetMenuStrip(REG_A0, REG_A1);
}
