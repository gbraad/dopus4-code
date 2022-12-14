#ifndef __MUI_H__
#define __MUI_H__

ULONG getv(APTR obj, ULONG attr);
APTR MakeButton(CONST_STRPTR str);
APTR MakeLLabel(CONST_STRPTR str);
APTR MakeText(void);
APTR MakeString(ULONG maxlen);

ULONG init_classes(void);
VOID delete_classes(void);

#ifndef MUIA_Application_NoIconify
#define MUIA_Application_NoIconify 0x80426a3b
#endif
#ifndef MUIA_Window_ShowIconify
#define MUIA_Window_ShowIconify 0x8042bc26
#endif
#ifndef MUIA_Window_ShowAbout
#define MUIA_Window_ShowAbout 0x80429c1e
#endif
#ifndef MUIA_Window_ShowPrefs
#define MUIA_Window_ShowPrefs 0x8042e262
#endif
#ifndef MUIA_Window_ShowJump
#define MUIA_Window_ShowJump 0x80422f40
#endif
#ifndef MUIA_Window_ShowSnapshot
#define MUIA_Window_ShowSnapshot 0x80423c55
#endif
#ifndef MUIA_Window_ShowPopup
#define MUIA_Window_ShowPopup 0x8042324e
#endif

#define ITIX_BASE (0xfece0000)

enum
{
	MM_Application_DeleteWindow = ITIX_BASE,
	MM_Application_DeleteFontWindow,
	MM_Application_AppMessage,
	MM_Clock_Update,
	MA_FileList_WindowNumber,
	MM_FileList_DoubleClick,
	MM_FileList_SelectChange,
	MA_FileArea_WindowNumber,
	MM_FileArea_SetDiskName,
	MM_FileArea_Activate,
	MM_ColorButton_SetText,
};

enum
{
	MUIV_AppMsg_Lister0 = 0,
	MUIV_AppMsg_Lister1,
	MUIV_AppMsg_Gadgets,
	MUIV_AppMsg_Status,
};

struct MUIP_Application_DeleteWindow { ULONG MethodID; APTR window; };
struct MUIP_Application_DeleteFontWindow { ULONG MethodID; APTR window; APTR font; };
struct MUIP_Application_AppMessage	{ ULONG MethodID; struct AppMessage *appmsg; ULONG which; };
struct MUIP_FileArea_SetDiskName	{ ULONG MethodID; struct ColourTable *colour; CONST_STRPTR name; CONST_STRPTR space; };
struct MUIP_FileArea_Activate		{ ULONG MethodID; ULONG activate; };
struct MUIP_ColorButton_SetText	{ ULONG MethodID; struct ColourTable *bgcol; struct ColourTable *fgcol; CONST_STRPTR text; };


extern struct MUI_CustomClass *CL_App;
extern struct MUI_CustomClass *CL_FileArea;
extern struct MUI_CustomClass *CL_ColorButton;
extern struct MUI_CustomClass *CL_Clock;
extern APTR dopusapp, dopuswin;
extern APTR dopusgads, dopusstatus;
extern APTR *dopusgadarray;
extern APTR dopusdirlist[2];

#endif /* __MUI_H__ */