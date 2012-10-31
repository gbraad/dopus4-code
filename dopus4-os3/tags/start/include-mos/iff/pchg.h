#ifndef	PCHG_H
#define	PCHG_H


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef IFF_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

#define	ID_PCHG		MAKE_ID('P','C','H','G')



#define PCHG_COMP_NONE 0
#define PCHG_COMP_HUFFMANN 1



#define PCHGB_12BIT			(0)
#define PCHGB_32BIT			(1)
#define PCHGB_USE_ALPHA		(2)


#define PCHGF_12BIT			(1<<0)	
#define PCHGF_32BIT			(1<<1)	
#define PCHGF_USE_ALPHA		(1<<2)	
struct PCHGHeader {
   UWORD Compression;
   UWORD Flags;
   WORD  StartLine;
   UWORD LineCount;
   UWORD ChangedLines;
   UWORD MinReg;
   UWORD MaxReg;
   UWORD MaxChanges;
   ULONG TotalChanges;
} __attribute__((packed));
struct PCHGCompHeader {
   ULONG CompInfoSize;
   ULONG OriginalDataSize;
} __attribute__((packed));
struct SmallLineChanges {
   UBYTE ChangeCount16;
   UBYTE ChangeCount32;

} __attribute__((packed));
struct BigLineChanges {
   UWORD ChangeCount;

} __attribute__((packed));
struct BigPaletteChange {
   UWORD Register;
   UBYTE Alpha, Red, Blue, Green;
} __attribute__((packed));



#define PCHGERR_NOMEM			(-1)
#define PCHGERR_UNKNOWNCOMP	(-2)


#endif
