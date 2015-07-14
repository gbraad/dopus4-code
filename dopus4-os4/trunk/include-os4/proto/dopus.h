#ifndef PROTO_DOPUS_H
#define PROTO_DOPUS_H

/*
**	$Id$
**	Includes Release 50.1
**
**	Prototype/inline/pragma header file combo
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#ifndef LIBRARIES_DOPUS_H
#include <libraries/dopus.h>
#endif
#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef DOPUS_REQUESTERS_H
#include <dopus/requesters.h>
#endif
#ifndef DOPUS_CONFIG_H
#include <dopus/config.h>
#endif
#ifndef DOPUS_STRINGDATA_H
#include <dopus/stringdata.h>
#endif
#ifndef DOPUS_DEBUG_H
#include <dopus/debug.h>
#endif

/****************************************************************************/

#ifndef __NOLIBBASE__
 #ifndef __USE_BASETYPE__
  extern struct Library * DOpusBase;
 #else
  extern struct Library * DOpusBase;
 #endif /* __USE_BASETYPE__ */
#endif /* __NOLIBBASE__ */

/****************************************************************************/

#ifdef __amigaos4__
 #include <interfaces/dopus.h>
 #ifdef __USE_INLINE__
  #include <inline4/dopus.h>
 #endif /* __USE_INLINE__ */
 #ifndef CLIB_DOPUS_PROTOS_H
  #define CLIB_DOPUS_PROTOS_H 1
 #endif /* CLIB_DOPUS_PROTOS_H */
 #ifndef __NOGLOBALIFACE__
  extern struct DOpusIFace *IDOpus;
 #endif /* __NOGLOBALIFACE__ */
#else /* __amigaos4__ */
 #ifndef CLIB_DOPUS_PROTOS_H
  #include <clib/dopus_protos.h>
 #endif /* CLIB_DOPUS_PROTOS_H */
 #if defined(__GNUC__)
  #ifndef __PPC__
   #include <inline/dopus.h>
  #else
   #include <ppcinline/dopus.h>
  #endif /* __PPC__ */
 #elif defined(__VBCC__)
  #ifndef __PPC__
   #include <inline/dopus_protos.h>
  #endif /* __PPC__ */
 #else
  #include <pragmas/dopus_pragmas.h>
 #endif /* __GNUC__ */
#endif /* __amigaos4__ */

/****************************************************************************/

#endif /* PROTO_DOPUS_H */
