#include <proto/exec.h>
#include <proto/muimaster.h>

struct Library *AsyncIOBase;
struct Library *xadMasterBase;
struct Library *MUIMasterBase;

void close_libs(void)
{
	CloseLibrary(AsyncIOBase);
	CloseLibrary(xadMasterBase);
	CloseLibrary(MUIMasterBase);
}

int open_libs(void)
{
	int rc = 1;

	AsyncIOBase = OpenLibrary("asyncio.library", 1);
	xadMasterBase = OpenLibrary("xadmaster.library", 1);
	MUIMasterBase = OpenLibrary("muimaster.library", 20);

	if (!AsyncIOBase || !xadMasterBase || !MUIMasterBase)
	{
		if (MUIMasterBase)
		{
			MUI_RequestA(NULL, NULL, 0, "Directory Opus", "Abort", "Need version 1 of xadmaster.library", NULL);
			close_libs();
		}
		rc = 0;
	}

	return rc;
}
