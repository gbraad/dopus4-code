struct DefaultGadFlag
{
	char code;
	char qual;
	char fpen;
	char bpen;
};

const static ULONG defaultpalette[48] =
{
	0xafffffff, 0xafffffff, 0xafffffff,
	0x00000000, 0x00000000, 0x00000000,
	0xffffffff, 0xffffffff, 0xffffffff,
	0x0fffffff, 0x5fffffff, 0xbfffffff,
	0xefffffff, 0xafffffff, 0x4fffffff,
	0x7fffffff, 0x00000000, 0x7fffffff,
	0xffffffff, 0xffffffff, 0x00000000,
	0xcfffffff, 0x2fffffff, 0x00000000,
	0xffffffff, 0x8fffffff, 0x00000000,
	0xffffffff, 0x00000000, 0xffffffff,
	0x9fffffff, 0x6fffffff, 0x3fffffff,
	0x00000000, 0xffffffff, 0x9fffffff,
	0x4fffffff, 0xffffffff, 0x3fffffff,
	0x00000000, 0x00000000, 0x00000000,
	0xffffffff, 0xffffffff, 0xffffffff,
	0x2fffffff, 0x5fffffff, 0x9fffffff
};

const static char *defgads[42] =
{
	"All",
	"Copy",
	"Makedir",
	"Hunt",
	"Run",
	"Comment",
	"Read",

	"None",
	"Move",
	"Assign",
	"Search",
	"",
	"Datestamp",
	"Hex Read",

	"Parent",
	"Rename",
	"Check Fit",
	"",
	"",
	"Protect",
	"Show",
	
	"Root",
	"",
	"GetSizes",
	"",
	"",
	"Icon Info",
	"Play",
	
	"",
	"",
	"",
	"",
	"",
	"Extract",
	"Edit",
	
	"",
	"DELETE",
	"",
	"",
	"",
	"",
	"Print"
}, *revgads[42] =
{
	"Toggle",
	"Copy As",
	"",
	"",
	"",
	"",
	"ANSI Read",
	
	"",
	"Move As",
	"",
	"",
	"",
	"",
	"",
	
	"",
	"Clone",
	"",
	"",
	"",
	"",
	"",
	
	"",
	"",
	"ClearSizes",
	"",
	"",
	"Add Icon",
	"Loop Play",
	
	"",
	"",
	"",
	"",
	"",
	"",
	"New File"
	
	"",
	"",
	"",
	"",
	"",
	"",
	"",
}, *deffuncs[42] =
{
	"*All",
	"*Copy",
	"*Makedir",
	"*Hunt",
	"*Run",
	"*Comment",
	"*Read",

	"*None",
	"*Move",
	"*Assign",
	"*Search",
	"",
	"*Datestamp",
	"*HexRead",

	"*Parent",
	"*Rename",
	"*CheckFit",
	"",
	"",
	"*Protect",
	"*Show",

	"*Root",
	"",
	"*GetSizes",
	"",
	"",
	"*IconInfo",
	"*Play",

	"",
	"",
	"",
	"",
	"",
	"*Extract",
	"NotePad {f} PUBSCREEN {Qs}",
	"",
	"*DELETE",
	"",
	"",
	"",
	"",
	"*Print"
}, *revfuncs[42] =
{
	"*Toggle",
	"*CopyAs",
	"",
	"",
	"",
	"",
	"*ANSIRead",

	"",
	"*MoveAs",
	"",
	"",
	"",
	"",
	"",
	"",
	"*Clone",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"*ClearSizes",
	"",
	"",
	"*AddIcon",
	"*LoopPlay",

	"",
	"",
	"",
	"",
	"",
	"",
	"NotePad {RsEnter filename to create new file:Untitled} PUBSCREEN {Qs}"

	"",
	"",
	"",
	"",
	"",
	"",
	"",
}, *defmenus1[7] =
{
	"Help!",
	"Error help~",
	"Configure~",
	"About",
	"Iconify",
	"Button Iconify",
	"Quit"
}, *deffuncs1[7] =
{
	"*Help",
	"*ErrorHelp",
	"*Configure",
	"*About",
	"*Iconify",
	"*ButtonIconify",
	"*Quit"
}, *defmenus2[9] =
{
	"Relabel",
	"Disk info~",
	"Create LhA archive",
	"Create ZIP archive",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
}, *deffuncs2[9] =
{
	"*Relabel",
	"*DiskInfo",
	"LHA -er a \"{d}{RsArchive name:.lha}\" {O}",
	"ZIP -rN \"{d}{RsArchive name:.zip}\" {O}",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
}, *deftype_type[5] =
{
	"Archive",
	"PDF document",
	"PS document",
	"Icon",
	"Default"
}, *deftype_typeid[5] =
{
	"ARC",
	"PDF",
	"PS",
	"ICON",
	"DEFAULT"
}, *deftype_recog[6] =
{
	"\002#?.(lzh|lha|lzx|zip|tar|tgz|tar.gz|tar.bz2)",
	"\001%PDF-1.?\376\002#?.pdf",
	"\001%!PS\376\002#?.(ps|eps|pdf)",
	"\001$E310\376\002#?.info",
	"\002*"
}, *deftype_funcs[5][4] =
{
	{
		"xadUnFile FROM {f} DESTDIR {d} SFS NOABS",
		"xadUnFile {f} INFO VERBOSE",
		"xadUnFile {f} INFO VERBOSE",
		"xadUnFile FROM {f} DESTDIR {d} SFS NOABS"
	},
	{
		"SYS:Utilities/AmiPDF/AmiPDF {f} PUBSCREEN {Qs}",
		NULL,
		NULL,
		NULL
	},
	{
		"SYS:Utilities/AmiGS/AmiGS {f} PUBSCREEN {Qs}",
		NULL,
		NULL,
		NULL
	},
	{
		"*IconInfo",
		NULL,
		NULL,
		NULL
	},
	{
		"*SmartRead",
		"*Copy",
		NULL,
		NULL
	}
}, deftype_funcpos[5][4] =
{
	{
		FTFUNC_CLICKMCLICK,
		FTFUNC_READ,
		FTFUNC_SHOW,
		FTFUNC_AUTOFUNC1
	},
	{
		FTFUNC_DOUBLECLICK,
	},
	{
		FTFUNC_DOUBLECLICK,
	},
	{
		FTFUNC_DOUBLECLICK
	},
	{
		FTFUNC_DOUBLECLICK,
		FTFUNC_CLICKMCLICK
	}
}, *deftype_action[5][4] =
{
	{
		"Extracting files from archive...",
		"Listing archive...",
		"Listing archive...",
		"Extracting files from archive..."
	},
	{
		"Displaying PDF document..."
	},
	{
		"Displaying PS document..."
	},
	{
		"Examining icon..."
	},
	{
		"Reading file...",
		"Copying file..."
	}
}, deftype_delay[5][4] =
{
	{
		0,
		2,
		0,
		2
	},
	{
		0
	},
	{
		0
	},
	{
		2,
		2,
		2,
		2
	},
	{
		2,
		2,
		2,
		2
	}
};

const unsigned char defmenkeys1[10] =
{
	0x5f,
	0xff,
	0x33,
	0xff,
	0x17,
	0xff,
	0x10,
	0xff,
	0xff,
	0xff
}, defmenqual1[10] =
{
	0,
	0,
	0x80,
	0,
	0x80,
	0,
	0x80,
	0,
	0,
	0
}, defmenkeys2[9] =
{
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff
}, defmenqual2[9] =
{
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

const static struct DefaultGadFlag default_gadflags[] =
{
	{ 0x20, IEQUALIFIER_CONTROL, 6, 3 },
	{ 0x33, IEQUALIFIER_CONTROL, 6, 5 },
	{ 0x37, IEQUALIFIER_CONTROL, 5, 4 },
	{ 0x25, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 4, 1 },
	{ 0x16, IEQUALIFIER_CONTROL, 1, 4 },
	{ 0x33, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 5, 0 },
	{ 0x13, IEQUALIFIER_CONTROL, 6, 7 },

	{ 0x36, IEQUALIFIER_CONTROL, 6, 3 },
	{ 0x37, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 6, 5 },
	{ 0xff, 0, 5, 4 },
	{ 0x21, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 4, 1 },
	{ 0xff, 0, 1, 0 },
	{ 0x22, IEQUALIFIER_CONTROL, 5, 0 },
	{ 0x25, IEQUALIFIER_CONTROL, 6, 7 },

	{ 0x19, IEQUALIFIER_CONTROL, 6, 3 },
	{ 0x13, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 6, 5 },
	{ 0x23, IEQUALIFIER_CONTROL, 5, 4 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0x14, IEQUALIFIER_CONTROL, 5, 0 },
	{ 0x21, IEQUALIFIER_CONTROL, 6, 7 },

	{ 0x18, IEQUALIFIER_CONTROL, 6, 3 },
	{ 0xff, 0, 1, 0 },
	{ 0x24, IEQUALIFIER_CONTROL, 5, 4 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0x17, IEQUALIFIER_CONTROL, 5, 0 },
	{ 0x15, IEQUALIFIER_CONTROL, 6, 7 },

	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0x12, IEQUALIFIER_CONTROL, 5, 0 },
	{ 0x12, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 6, 7 },

	{ 0xff, 0, 1, 0 },
	{ 0x22, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 2, 7 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 1, 0 },
	{ 0xff, 0, 5, 0 },
	{ 0x19, IEQUALIFIER_CONTROL | IEQUALIFIER_LSHIFT, 6, 7 }
};

const static short deftype_which[5][4] =
{
	{
	 	FLAG_SCANDEST | FLAG_DOALL | FLAG_CDDEST | FLAG_OUTWIND,
		FLAG_DOALL | FLAG_OUTFILE,
		FLAG_DOALL | FLAG_OUTFILE,
		FLAG_SCANDEST | FLAG_DOALL | FLAG_CDDEST | FLAG_OUTWIND
	},
	{
		0,
		0,
		0,
		0
	},
	{
		0,
		0,
		0,
		0
	},
	{
		0,
		0,
		0,
		0
	},
	{
		0,
		0,
		0,
		0
	}
};


