#include <proto/exec.h>
#include <exec/execbase.h>
#include <exec/system.h>


#ifndef __MORPHOS__
static ULONG oldidle, olddisp;

int getusage(void)
{
	LONG idlediff, dispdiff;

	idlediff = SysBase->IdleCount - oldidle;
	dispdiff = SysBase->DispCount - olddisp;

	oldidle = SysBase->IdleCount;
	olddisp = SysBase->DispCount;

	return ((idlediff + dispdiff) ? (dispdiff * 100 / (idlediff + dispdiff)) : 100);

}
#else
int getusage(void)
{
	UQUAD lastsecticks, lastseccputime;
	int cpu_p;

	NewGetSystemAttrsA(&lastsecticks, sizeof(lastsecticks), SYSTEMINFOTYPE_LASTSECTICKS, NULL);
	NewGetSystemAttrs(&lastseccputime,  sizeof(lastseccputime),  SYSTEMINFOTYPE_LASTSECCPUTIME, TAG_DONE);
	cpu_p = ((DOUBLE)lastseccputime / (DOUBLE)lastsecticks) * 100.0;
	return cpu_p;
}
#endif