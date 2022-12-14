/*
**	$Id$
**	Generated by IDLTool 51.6
**	Do not edit
**
**	AutoInit stub for gadtools
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#include <exec/libraries.h>
#include <intuition/intuition.h>
#include <utility/tagitem.h>
#include <libraries/gadtools.h>

#include <interfaces/gadtools.h>
#include <proto/exec.h>
#include <assert.h>

/****************************************************************************/

__attribute__((weak)) struct Library * GadToolsBase = NULL;

void __init_gadtools_base(void) __attribute__((constructor));
void __exit_gadtools_base(void) __attribute__((destructor));

/****************************************************************************/

void __init_gadtools_base(void)
{
    if (GadToolsBase != NULL) return; /* Someone was quicker */
    GadToolsBase = IExec->OpenLibrary("gadtools.library", 0L);
    assert(GadToolsBase != NULL);
}

/****************************************************************************/

void __exit_gadtools_base(void)
{
    if (GadToolsBase) IExec->CloseLibrary((struct Library *)GadToolsBase);
}

/****************************************************************************/

