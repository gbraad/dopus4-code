#include <proto/exec.h>
#include <exec/execbase.h>


ULONG oldidle = 0;
ULONG olddisp = 0;

int getusage(void)
{
    LONG idlediff, dispdiff;
    
    idlediff = SysBase->IdleCount - oldidle;
    dispdiff = SysBase->DispCount - olddisp;
    
    oldidle = SysBase->IdleCount;
    olddisp = SysBase->DispCount;

    return ((idlediff+dispdiff)?(dispdiff * 100 / (idlediff + dispdiff)):100);
    
}
