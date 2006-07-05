#include <proto/exec.h>
#include <proto/muimaster.h>

struct Library *PopupMenuBase;
struct Library *AsyncIOBase;
struct Library *xadMasterBase;

void close_libs(void)
{
	CloseLibrary(PopupMenuBase);
	CloseLibrary(AsyncIOBase);
	CloseLibrary(xadMasterBase);
}

int open_libs(void)
{
	int rc = 1;

	PopupMenuBase = OpenLibrary("popupmenu.library", 1);
	AsyncIOBase = OpenLibrary("asyncio.library", 1);
	xadMasterBase = OpenLibrary("xadmaster.library", 1);

	if (!PopupMenuBase || !AsyncIOBase || !xadMasterBase)
	{
		struct Library *MUIMasterBase = OpenLibrary("muimaster.library", 0);

		if (MUIMasterBase)
		{
			MUI_RequestA(NULL, NULL, 0, "Directory Opus", "Abort", "Need version 1 of popupmenu.library and version 1 of xadmaster.library", NULL);
			CloseLibrary(MUIMasterBase);
			close_libs();
		}
		rc = 0;
	}

	return rc;
}
