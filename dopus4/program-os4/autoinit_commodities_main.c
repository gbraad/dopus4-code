/*
**	$Id$
**	Generated by IDLTool 51.6
**	Do not edit
**
**	AutoInit stub for commodities
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#include <exec/types.h>
#include <exec/nodes.h>
#include <libraries/commodities.h>
#include <devices/inputevent.h>
#include <devices/keymap.h>

#include <interfaces/commodities.h>
#include <proto/exec.h>
#include <assert.h>

__attribute__((weak)) struct CommoditiesIFace * ICommodities = NULL;
/****************************************************************************/

extern struct Library * CxBase;

void __init_commodities_main(void) __attribute__((constructor));
void __exit_commodities_main(void) __attribute__((destructor));

/****************************************************************************/

void __init_commodities_main(void)
{
    struct Library * LibBase;
    if (CxBase == NULL) /* Library base is NULL, we need to open it */
    {
        LibBase = IExec->OpenLibrary("commodities.library", 0L);
        assert(LibBase != NULL);
    }
    else
    {
        LibBase = CxBase;
    }

    ICommodities = (struct CommoditiesIFace *)IExec->GetInterface((struct Library *)LibBase, "main", 1, NULL);
    assert(ICommodities != NULL);
}

/****************************************************************************/

void __exit_commodities_main(void)
{
    if (ICommodities) IExec->DropInterface ((struct Interface *)ICommodities);
}

/****************************************************************************/

