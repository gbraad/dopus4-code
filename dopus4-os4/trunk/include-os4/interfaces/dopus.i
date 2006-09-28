#ifndef DOPUS_INTERFACE_DEF_H
#define DOPUS_INTERFACE_DEF_H
/*
** This file is machine generated from idltool
** Do not edit
*/ 

#include <exec/types.i>
#include <exec/exec.i>
#include <exec/interfaces.i>

STRUCTURE DOpusIFace, InterfaceData_SIZE
	    FPTR IDOpus_Obtain
	    FPTR IDOpus_Release
	    FPTR IDOpus_Expunge
	    FPTR IDOpus_Clone
	    FPTR IDOpus_FileRequest
	    FPTR IDOpus_Do3DBox
	    FPTR IDOpus_Do3DStringBox
	    FPTR IDOpus_Do3DCycleBox
	    FPTR IDOpus_DoArrow
	    FPTR IDOpus_Reserved1
	    FPTR IDOpus_LStrCat
	    FPTR IDOpus_LStrnCat
	    FPTR IDOpus_LStrCpy
	    FPTR IDOpus_LStrnCpy
	    FPTR IDOpus_LStrCmp
	    FPTR IDOpus_LStrnCmp
	    FPTR IDOpus_LStrCmpI
	    FPTR IDOpus_LStrnCmpI
	    FPTR IDOpus_StrCombine
	    FPTR IDOpus_StrConcat
	    FPTR IDOpus_TackOn
	    FPTR IDOpus_StampToStr
	    FPTR IDOpus_StrToStamp
	    FPTR IDOpus_AddListView
	    FPTR IDOpus_ListViewIDCMP
	    FPTR IDOpus_RefreshListView
	    FPTR IDOpus_RemoveListView
	    FPTR IDOpus_DrawCheckMark
	    FPTR IDOpus_FixSliderBody
	    FPTR IDOpus_FixSliderPot
	    FPTR IDOpus_GetSliderPos
	    FPTR IDOpus_LAllocRemember
	    FPTR IDOpus_LFreeRemember
	    FPTR IDOpus_SetBusyPointer
	    FPTR IDOpus_GetWBScreen
	    FPTR IDOpus_SearchPathList
	    FPTR IDOpus_CheckExist
	    FPTR IDOpus_StrToUpper
	    FPTR IDOpus_StrToLower
	    FPTR IDOpus_RawkeyToStr
	    FPTR IDOpus_DoRMBGadget
	    FPTR IDOpus_AddGadgets
	    FPTR IDOpus_ActivateStrGad
	    FPTR IDOpus_RefreshStrGad
	    FPTR IDOpus_CheckNumGad
	    FPTR IDOpus_CheckHexGad
	    FPTR IDOpus_Atoh
	    FPTR IDOpus_HiliteGad
	    FPTR IDOpus_DoSimpleRequest
	    FPTR IDOpus_ReadConfig
	    FPTR IDOpus_SaveConfig
	    FPTR IDOpus_DefaultConfig
	    FPTR IDOpus_GetDevices
	    FPTR IDOpus_AssignGadget
	    FPTR IDOpus_AssignMenu
	    FPTR IDOpus_FindSystemFile
	    FPTR IDOpus_Do3DFrame
	    FPTR IDOpus_FreeConfig
	    FPTR IDOpus_DoCycleGadget
	    FPTR IDOpus_UScoreText
	    FPTR IDOpus_DisableGadget
	    FPTR IDOpus_EnableGadget
	    FPTR IDOpus_GhostGadget
	    FPTR IDOpus_DrawRadioButton
	    FPTR IDOpus_GetButtonImage
	    FPTR IDOpus_ShowSlider
	    FPTR IDOpus_CheckConfig
	    FPTR IDOpus_GetCheckImage
	    FPTR IDOpus_OpenRequester
	    FPTR IDOpus_CloseRequester
	    FPTR IDOpus_AddRequesterObject
	    FPTR IDOpus_RefreshRequesterObject
	    FPTR IDOpus_ObjectText
	    FPTR IDOpus_DoGlassImage
	    FPTR IDOpus_Decode_RLE
	    FPTR IDOpus_ReadStringFile
	    FPTR IDOpus_FreeStringFile
	    FPTR IDOpus_LFreeRemEntry
	    FPTR IDOpus_AddGadgetBorders
	    FPTR IDOpus_CreateGadgetBorders
	    FPTR IDOpus_SelectGadget
	    FPTR IDOpus_FSSetMenuStrip
	LABEL DOpusIFace_SIZE

#endif
