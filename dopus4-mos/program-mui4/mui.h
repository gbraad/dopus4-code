#ifndef __MUI_H__
#define __MUI_H__

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
	MM_Clock_Update,
};

struct MUIP_Application_DeleteWindow { ULONG MethodID; APTR window; };
struct MUIP_Application_DeleteFontWindow { ULONG MethodID; APTR window; APTR font; };


extern struct MUI_CustomClass *CL_App;
extern struct MUI_CustomClass *CL_FileList;
extern struct MUI_CustomClass *CL_Clock;
extern APTR dopusapp, dopuswin;
extern APTR dopusgads, dopusstatus;
extern APTR *dopusgadarray;
extern APTR dopusdirlist[2];

#endif /* __MUI_H__ */