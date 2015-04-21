#ifndef DOPUS_DEBUG_H
#define DOPUS_DEBUG_H


#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

#define bug(fmt, args...)  {IExec->DebugPrintF("[%s:%ld %s] ", __FILE__, __LINE__, __FUNCTION__); IExec->DebugPrintF(fmt, ##args);}

#endif /* DOPUS_DEBUG_H */
