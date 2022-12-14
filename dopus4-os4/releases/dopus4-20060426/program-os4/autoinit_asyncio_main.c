/*
**	$Id$
**	Generated by IDLTool 50.8
**	Do not edit
**
**	AutoInit stub for asyncio
**
**	(C) Copyright 2003 Amiga, Inc.
**	    All Rights Reserved
*/

#include <libraries/asyncio.h>

#include <interfaces/asyncio.h>
#include <proto/exec.h>
#include <assert.h>

__attribute__((weak)) struct AsyncIOIFace * IAsyncIO = NULL;
/****************************************************************************/

extern struct Library * AsyncIOBase;

void __init_asyncio_main(void) __attribute__((constructor));
void __exit_asyncio_main(void) __attribute__((destructor));

/****************************************************************************/

void __init_asyncio_main(void)
{
    struct Library * LibBase;
    if (AsyncIOBase == NULL) /* Library base is NULL, we need to open it */
    {
        LibBase = IExec->OpenLibrary("asyncio.library", 0L);
        assert(LibBase != NULL);
    }
    else
    {
        LibBase = AsyncIOBase;
    }

    IAsyncIO = (struct AsyncIOIFace *)IExec->GetInterface((struct Library *)LibBase, "main", 1, NULL);
    assert(IAsyncIO != NULL);
}

/****************************************************************************/

void __exit_asyncio_main(void)
{
    if (IAsyncIO) IExec->DropInterface ((struct Interface *)IAsyncIO);
}

/****************************************************************************/

