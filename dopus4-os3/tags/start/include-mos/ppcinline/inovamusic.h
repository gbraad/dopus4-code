/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_INOVAMUSIC_H
#define _PPCINLINE_INOVAMUSIC_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef INOVAMUSIC_BASE_NAME
#define INOVAMUSIC_BASE_NAME MUSICBase
#endif /* !INOVAMUSIC_BASE_NAME */

#define ContModule() \
	LP0NR(0x36, ContModule, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FlushModule() \
	LP0NR(0x30, FlushModule, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define IsModule(arg1) \
	LP1(0x2a, WORD, IsModule, char *, arg1, d0, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define PlayFaster() \
	LP0NR(0x42, PlayFaster, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define PlayModule(arg1, arg2) \
	LP2(0x1e, WORD, PlayModule, char *, arg1, a0, BOOL, arg2, d0, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define PlaySlower() \
	LP0NR(0x48, PlaySlower, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define SetVolume(arg1) \
	LP1NR(0x3c, SetVolume, WORD, arg1, d0, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define StopModule() \
	LP0NR(0x24, StopModule, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define TempoReset() \
	LP0NR(0x4e, TempoReset, \
	, INOVAMUSIC_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#endif /* !_PPCINLINE_INOVAMUSIC_H */
