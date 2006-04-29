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

__attribute__((weak)) struct RexxSysIFace * IRexxSys = NULL;
/****************************************************************************/

extern struct Library * RexxSysBase;

void __init_rexxsyslib_main(void) __attribute__((constructor));
void __exit_rexxsyslib_main(void) __attribute__((destructor));

/****************************************************************************/

void __init_rexxsyslib_main(void)
{
    struct Library * LibBase;
    if (RexxSysBase == NULL) /* Library base is NULL, we need to open it */
    {
        LibBase = IExec->OpenLibrary("rexxsyslib.library", 0L);
        assert(LibBase != NULL);
    }
    else
    {
        LibBase = RexxSysBase;
    }

    IRexxSys = (struct RexxSysIFace *)IExec->GetInterface((struct Library *)LibBase, "main", 1, NULL);
    assert(IRexxSys != NULL);
}

/****************************************************************************/

void __exit_rexxsyslib_main(void)
{
    if (IRexxSys) IExec->DropInterface ((struct Interface *)IRexxSys);
}

/****************************************************************************/

