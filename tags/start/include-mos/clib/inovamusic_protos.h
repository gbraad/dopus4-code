#ifndef  CLIB_INOVAMUSIC_PROTOS_H
#define  CLIB_INOVAMUSIC_PROTOS_H

#ifndef  INOVAMUSIC_H
#include <libraries/inovamusic.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


WORD PlayModule(char *,BOOL);
VOID StopModule(void);
WORD IsModule(char *);
VOID FlushModule(void);
VOID ContModule(void);
VOID SetVolume(WORD);
VOID PlayFaster(void);
VOID PlaySlower(void);
VOID TempoReset(void);

#ifdef __cplusplus
}
#endif

#endif   /* CLIB_INOVAMUSIC_PROTOS_H */
