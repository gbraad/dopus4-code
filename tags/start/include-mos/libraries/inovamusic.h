

#ifndef INOVAMUSIC_H
#define INOVAMUSIC_H

#include <exec/types.h>
#include <libraries/dos.h>
#include <exec/resident.h>
#include <exec/memory.h>
#include <exec/libraries.h>
struct MusicBase
{
	struct Library MLib;
	BPTR __attribute__((aligned(2))) ml_SegList;
	APTR __attribute__((aligned(2))) ml_Module;
	LONG __attribute__((aligned(2))) ml_Len;
	WORD ml_Playing, ml_ModType, ml_Volume, ml_Tempo, ml_Tracks, ml_SongPos;
	struct Library* __attribute__((aligned(2))) ml_DOSBase, *ml_PPBase;
	char * __attribute__((aligned(2))) ml_SongName;
} __attribute__((packed));


#define MOD_STNTPT 1 
#define MOD_MED 2 
#define MOD_OKTA 3 
#define MOD_OLDST 4 



#define ML_NOMEM 101 
#define ML_BADMOD 102 
#define ML_NOMOD 103 
#define ML_NOPLAYER 104 
#define ML_LIBLOCKED 105 
#define ML_INVALIDFNAME 106 
#define ML_NOAUDIO 107 

#endif 
