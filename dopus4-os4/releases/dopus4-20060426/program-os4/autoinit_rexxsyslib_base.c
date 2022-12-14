/*
**	$Id$
**	Generated by IDLTool 51.6
**	Do not edit
**
**	AutoInit stub for rexxsyslib
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#include <exec/types.h>
#include <rexx/rxslib.h>
#include <rexx/rexxio.h>

#include <interfaces/rexxsyslib.h>
#include <proto/exec.h>
#include <assert.h>

/****************************************************************************/

__attribute__((weak)) struct Library * RexxSysBase = NULL;

void __init_rexxsyslib_base(void) __attribute__((constructor));
void __exit_rexxsyslib_base(void) __attribute__((destructor));

/****************************************************************************/

void __init_rexxsyslib_base(void)
{
    if (RexxSysBase != NULL) return; /* Someone was quicker */
    RexxSysBase = IExec->OpenLibrary("rexxsyslib.library", 0L);
    assert(RexxSysBase != NULL);
}

/****************************************************************************/

void __exit_rexxsyslib_base(void)
{
    if (RexxSysBase) IExec->CloseLibrary((struct Library *)RexxSysBase);
}

/****************************************************************************/

