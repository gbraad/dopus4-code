/* Automatically generated header! Do not edit! */

#ifndef _INLINE_INOVAMUSIC_H
#define _INLINE_INOVAMUSIC_H

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif /* !__INLINE_MACROS_H */

#ifndef INOVAMUSIC_BASE_NAME
#define INOVAMUSIC_BASE_NAME MUSICBase
#endif /* !INOVAMUSIC_BASE_NAME */

#define ContModule() \
	LP0NR(0x36, ContModule, \
	, INOVAMUSIC_BASE_NAME)

#define FlushModule() \
	LP0NR(0x30, FlushModule, \
	, INOVAMUSIC_BASE_NAME)

#define IsModule(arg1) \
	LP1(0x2a, WORD, IsModule, char *, arg1, d0, \
	, INOVAMUSIC_BASE_NAME)

#define PlayFaster() \
	LP0NR(0x42, PlayFaster, \
	, INOVAMUSIC_BASE_NAME)

#define PlayModule(arg1, arg2) \
	LP2(0x1e, WORD, PlayModule, char *, arg1, a0, BOOL, arg2, d0, \
	, INOVAMUSIC_BASE_NAME)

#define PlaySlower() \
	LP0NR(0x48, PlaySlower, \
	, INOVAMUSIC_BASE_NAME)

#define SetVolume(arg1) \
	LP1NR(0x3c, SetVolume, WORD, arg1, d0, \
	, INOVAMUSIC_BASE_NAME)

#define StopModule() \
	LP0NR(0x24, StopModule, \
	, INOVAMUSIC_BASE_NAME)

#define TempoReset() \
	LP0NR(0x4e, TempoReset, \
	, INOVAMUSIC_BASE_NAME)

#endif /* !_INLINE_INOVAMUSIC_H */
