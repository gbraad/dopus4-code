/*
**	$Id$
**	Generated by IDLTool 51.8
**	Do not edit
**
**	AutoInit stub for xfdmaster
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#include <libraries/xfdmaster.h>

#include <interfaces/xfdmaster.h>
#include <proto/exec.h>
#include <assert.h>

/****************************************************************************/

__attribute__((weak)) struct Library * xfdMasterBase = NULL;

void __init_xfdmaster_base(void) __attribute__((constructor));
void __exit_xfdmaster_base(void) __attribute__((destructor));

/****************************************************************************/

void __init_xfdmaster_base(void)
{
    if (xfdMasterBase != NULL) return; /* Someone was quicker */
    xfdMasterBase = IExec->OpenLibrary("xfdmaster.library", 0L);
    assert(xfdMasterBase != NULL);
}

/****************************************************************************/

void __exit_xfdmaster_base(void)
{
    if (xfdMasterBase) IExec->CloseLibrary((struct Library *)xfdMasterBase);
}

/****************************************************************************/

