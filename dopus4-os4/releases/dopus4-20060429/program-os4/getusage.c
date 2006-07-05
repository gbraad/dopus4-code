#include <proto/exec.h>
#include <exec/execbase.h>


static ULONG oldidle, olddisp;

int getusage(void)
{
	LONG idlediff, dispdiff;

	idlediff = ((struct ExecBase *)(IExec->Data.LibBase))->IdleCount - oldidle;
	dispdiff = ((struct ExecBase *)(IExec->Data.LibBase))->DispCount - olddisp;

	oldidle = ((struct ExecBase *)(IExec->Data.LibBase))->IdleCount;
	olddisp = ((struct ExecBase *)(IExec->Data.LibBase))->DispCount;

	return ((idlediff + dispdiff) ? (dispdiff * 100 / (idlediff + dispdiff)) : 100);

}
