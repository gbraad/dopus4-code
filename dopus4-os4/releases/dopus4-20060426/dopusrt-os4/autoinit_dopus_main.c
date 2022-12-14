/*
**	$Id$
**	Generated by IDLTool 51.6
**	Do not edit
**
**	AutoInit stub for dopus
**
**	(C) Copyright 2003-2005 Amiga, Inc.
**	    All Rights Reserved
*/

#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>

#include <interfaces/dopus.h>
#include <proto/exec.h>
#include <assert.h>

__attribute__((weak)) struct DOpusIFace * IDOpus = NULL;
/****************************************************************************/

extern struct Library * DOpusBase;

void __init_dopus_main(void) __attribute__((constructor));
void __exit_dopus_main(void) __attribute__((destructor));

/****************************************************************************/

void __init_dopus_main(void)
{
    struct Library * LibBase;
    if (DOpusBase == NULL) /* Library base is NULL, we need to open it */
    {
        LibBase = IExec->OpenLibrary("dopus.library", 0L);
        assert(LibBase != NULL);
    }
    else
    {
        LibBase = DOpusBase;
    }

    IDOpus = (struct DOpusIFace *)IExec->GetInterface((struct Library *)LibBase, "main", 1, NULL);
    assert(IDOpus != NULL);
}

/****************************************************************************/

void __exit_dopus_main(void)
{
    if (IDOpus) IExec->DropInterface ((struct Interface *)IDOpus);
}

/****************************************************************************/

