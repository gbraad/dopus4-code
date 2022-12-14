/*
**	$Id$
**	Generated by IDLTool 51.6
**	Do not edit
**
**	AutoInit stub for pm
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#include <libraries/pm.h>

#include <interfaces/pm.h>
#include <proto/exec.h>
#include <assert.h>

__attribute__((weak)) struct PopupMenuIFace * IPopupMenu = NULL;
/****************************************************************************/

extern struct Library * PopupMenuBase;

void __init_pm_main(void) __attribute__((constructor));
void __exit_pm_main(void) __attribute__((destructor));

/****************************************************************************/

void __init_pm_main(void)
{
    struct Library * LibBase;
    if (PopupMenuBase == NULL) /* Library base is NULL, we need to open it */
    {
        LibBase = IExec->OpenLibrary("popupmenu.library", 0L);
        assert(LibBase != NULL);
    }
    else
    {
        LibBase = PopupMenuBase;
    }

    IPopupMenu = (struct PopupMenuIFace *)IExec->GetInterface((struct Library *)LibBase, "main", 1, NULL);
    assert(IPopupMenu != NULL);
}

/****************************************************************************/

void __exit_pm_main(void)
{
    if (IPopupMenu) IExec->DropInterface ((struct Interface *)IPopupMenu);
}

/****************************************************************************/

