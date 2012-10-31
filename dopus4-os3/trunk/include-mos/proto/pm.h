#ifndef _PROTO_PM_H
#define _PROTO_PM_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#if !defined(CLIB_PM_PROTOS_H)
#include <clib/pm_protos.h>
#endif

#ifndef __NOLIBBASE__
extern struct PopupMenuBase *PopupMenuBase;
#endif

#ifdef __GNUC__
#include <inline/pm.h>
#elif !defined(__VBCC__)
#include <pragma/pm_lib.h>
#endif

#endif	/*  _PROTO_PM_H  */
