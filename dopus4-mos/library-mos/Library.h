#ifndef	__LIBRARY_H__
#define	__LIBRARY_H__

#ifndef	DOS_DOS_H
#include	<dos/dos.h>
#endif

#ifndef	EXEC_LIBRARIES_H
#include	<exec/libraries.h>
#endif

#ifndef	UTILITY_UTILITY_H
#include	<utility/utility.h>
#endif

/**********************************************************************
	Structures
**********************************************************************/

#pragma pack(2)
struct MyInitData
{
	UBYTE ln_Type_Init[4];
	UBYTE ln_Pri_Init[4];
	UBYTE ln_Name_Init[2];
	ULONG ln_Name_Content;
	UBYTE lib_Flags_Init[4];
	UBYTE lib_Version_Init[2]; UWORD lib_Version_Content;
	UBYTE lib_Revision_Init[2]; UWORD lib_Revision_Content;
	UBYTE lib_IdString_Init[2];
	ULONG lib_IdString_Content;
	UWORD EndMark;
};
#pragma pack()

#endif	/* __LIBRARY_H__ */
