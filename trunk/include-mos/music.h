


#include <exec/types.h>
#include <libraries/dos.h>
#include <exec/resident.h>
#include <exec/memory.h>
#include <exec/libraries.h>
struct MusicBase {
    struct Library MLib;
    BPTR __attribute__((aligned(2)))    ml_SegList;
    APTR __attribute__((aligned(2)))    ml_Module;
    LONG __attribute__((aligned(2)))    ml_Len;
    WORD    ml_Playing,
            ml_ModType,
            ml_Volume,
            ml_Tempo,
            ml_Tracks,
            ml_SongPos;
    struct Library* __attribute__((aligned(2))) ml_DOSBase, *ml_PPBase;
    char   * __attribute__((aligned(2))) ml_SongName;
} __attribute__((packed));



#define MOD_STNTPT   1               
#define MOD_MED      2               
#define MOD_OKTA     3               
#define MOD_OLDST    4               




#define ML_NOMEM        101       
#define ML_BADMOD       102       
#define ML_NOMOD        103       
#define ML_NOPLAYER     104       
#define ML_LIBLOCKED    105       
#define ML_INVALIDFNAME 106       
#define ML_NOAUDIO      107       

WORD PlayModule(char *,BOOL);
VOID StopModule(void);
WORD IsModule(char *);
VOID FlushModule(void);
VOID ContModule(void);
VOID SetVolume(WORD);
VOID PlayFaster(void);
VOID PlaySlower(void);
VOID TempoReset(void);

#pragma libcall MUSICBase PlayModule   1e 0802  
#pragma libcall MUSICBase StopModule   24 0
#pragma libcall MUSICBase IsModule     2a 801   
#pragma libcall MUSICBase FlushModule  30 0
#pragma libcall MUSICBase ContModule   36 0
#pragma libcall MUSICBase SetVolume    3c 801   
#pragma libcall MUSICBase PlayFaster   42 0
#pragma libcall MUSICBase PlaySlower   48 0
#pragma libcall MUSICBase TempoReset   4e 0



