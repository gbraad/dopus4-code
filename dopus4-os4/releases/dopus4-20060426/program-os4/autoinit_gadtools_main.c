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

__attribute__((weak)) struct GadToolsIFace * IGadTools = NULL;
/****************************************************************************/

extern struct Library * GadToolsBase;

void __init_gadtools_main(void) __attribute__((constructor));
void __exit_gadtools_main(void) __attribute__((destructor));

/****************************************************************************/

void __init_gadtools_main(void)
{
    struct Library * LibBase;
    if (GadToolsBase == NULL) /* Library base is NULL, we need to open it */
    {
        LibBase = IExec->OpenLibrary("gadtools.library", 0L);
        assert(LibBase != NULL);
    }
    else
    {
        LibBase = GadToolsBase;
    }

    IGadTools = (struct GadToolsIFace *)IExec->GetInterface((struct Library *)LibBase, "main", 1, NULL);
    assert(IGadTools != NULL);
}

/****************************************************************************/

void __exit_gadtools_main(void)
{
    if (IGadTools) IExec->DropInterface ((struct Interface *)IGadTools);
}

/****************************************************************************/

