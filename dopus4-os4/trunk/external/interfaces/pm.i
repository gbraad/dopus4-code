#ifndef PM_INTERFACE_DEF_H
#define PM_INTERFACE_DEF_H
/*
** This file is machine generated from idltool
** Do not edit
*/ 

#include <exec/types.i>
#include <exec/exec.i>
#include <exec/interfaces.i>

STRUCTURE PopupMenuIFace, InterfaceData_SIZE
	    FPTR IPopupMenu_Obtain
	    FPTR IPopupMenu_Release
	    FPTR IPopupMenu_Expunge
	    FPTR IPopupMenu_Clone
	    FPTR IPopupMenu_PM_MakeMenuA
	    FPTR IPopupMenu_PM_MakeMenu
	    FPTR IPopupMenu_PM_MakeItemA
	    FPTR IPopupMenu_PM_MakeItem
	    FPTR IPopupMenu_PM_FreePopupMenu
	    FPTR IPopupMenu_PM_OpenPopupMenuA
	    FPTR IPopupMenu_PM_OpenPopupMenu
	    FPTR IPopupMenu_PM_MakeIDListA
	    FPTR IPopupMenu_PM_MakeIDList
	    FPTR IPopupMenu_PM_ItemChecked
	    FPTR IPopupMenu_PM_GetItemAttrsA
	    FPTR IPopupMenu_PM_GetItemAttrs
	    FPTR IPopupMenu_PM_SetItemAttrsA
	    FPTR IPopupMenu_PM_SetItemAttrs
	    FPTR IPopupMenu_PM_FindItem
	    FPTR IPopupMenu_PM_AlterState
	    FPTR IPopupMenu_Reserved1
	    FPTR IPopupMenu_PM_ExLstA
	    FPTR IPopupMenu_PM_ExLst
	    FPTR IPopupMenu_PM_FilterIMsgA
	    FPTR IPopupMenu_PM_FilterIMsg
	    FPTR IPopupMenu_PM_InsertMenuItemA
	    FPTR IPopupMenu_PM_InsertMenuItem
	    FPTR IPopupMenu_PM_RemoveMenuItem
	    FPTR IPopupMenu_PM_AbortHook
	    FPTR IPopupMenu_PM_GetVersion
	    FPTR IPopupMenu_PM_ReloadPrefs
	    FPTR IPopupMenu_PM_LayoutMenuA
	    FPTR IPopupMenu_PM_LayoutMenu
	    FPTR IPopupMenu_Reserved2
	    FPTR IPopupMenu_PM_FreeIDList
	LABEL PopupMenuIFace_SIZE

#endif
