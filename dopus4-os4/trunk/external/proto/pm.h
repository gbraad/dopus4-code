#ifndef PROTO_PM_H
#define PROTO_PM_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2004 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef LIBRARIES_PM_H
#include <libraries/pm.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * PopupMenuBase;
 #else
  extern struct Library * PopupMenuBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/pm.h>
 #ifdef __USE_INLINE__
  #include <inline4/pm.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_PM_PROTOS_H
  #define CLIB_PM_PROTOS_H 1
 #endif /* CLIB_PM_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct PopupMenuIFace *IPopupMenu;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_PM_PROTOS_H
  #include <clib/pm_protos.h>
 #endif /* CLIB_PM_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/pm.h>
  #else
   #include <ppcinline/pm.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/pm_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/pm_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_PM_H */
