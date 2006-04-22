/* From dopus_stuff.c */

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
	"Arc Ext",
	"Edit",
	"",
	"DELETE",
	"",
	"",
	"",
	"Encrypt",
	"Print"
}, *revgads[35] =
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
	"*User1",
	"Ed {f}",
	"",
	"*DELETE",
	"",
	"",
	"",
	"*Encrypt",
	"*Print"
}, *revfuncs[35] =
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
	"Ed {RsEnter filename to create new file:Untitled}"
}, *defmenus1[9] =
{
	"Current dir~",
	"Help!",
	"Error help~",
	"Configure~",
	"About",
	"Version~",
	"Iconify",
	"Button Iconify",
	"Quit"
}, *deffuncs1[9] =
{
	"*CD",
	"*Help",
	"*ErrorHelp",
	"*Configure",
	"*About",
	"*Version",
	"*Iconify",
	"*ButtonIconify",
	"*Quit"
}, *defmenus2[9] =
{
	"Disk copy",
	"Format",
	"Install~",
	"Relabel",
	"Print dir",
	"Disk info~",
	"LHA add",
	"Arc add",
	"Zoo add"
}, *deffuncs2[9] =
{
	"*Diskcopy",
	"*Format",
	"*Install",
	"*Relabel",
	"*PrintDir",
	"*DiskInfo",
	"LHA -x a \"{d}{RsEnter LHA archive name}\" {O}",
	"Arc a \"{d}{RsEnter Arc archive name}\" {O}",
	"Zoo a \"{d}{RsEnter Zoo archive name}\" {O}"
}, *deftype_type[5] =
{
	"LHA archive",
	"ARC archive",
	"ZOO archive",
	"Icon",
	"Default"
}, *deftype_typeid[5] =
{
	"LHA",
	"ARC",
	"ZOO",
	"ICON",
	"DEFAULT"
}, *deftype_recog[6] =
{
	"\002*.(lzh|lha)",
	"\002*.arc",
	"\002*.zoo",
	"\001$E310\376\002*.info",
	"\002*"
}, *deftype_funcs[5][4] =
{
	{
		"LHA v {f}",
		"LHA -x -M x {f}",
		"LHA v {f}",
		"LHA -x -M x {f}"
	},
	{
		"Arc v {f}",
		"Arc x {f}",
		"Arc v {f}",
		"Arc x {f}"
	},
	{
		"Zoo v {f}",
		"Zoo x// {f}",
		"Zoo v {f}",
		"Zoo x// {f}"
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
		FTFUNC_DOUBLECLICK,
		FTFUNC_CLICKMCLICK,
		FTFUNC_READ,
		FTFUNC_AUTOFUNC1
	},
	{
		FTFUNC_DOUBLECLICK,
		FTFUNC_CLICKMCLICK,
		FTFUNC_READ,
		FTFUNC_AUTOFUNC1
	},
	{
		FTFUNC_DOUBLECLICK,
		FTFUNC_CLICKMCLICK,
		FTFUNC_READ,
		FTFUNC_AUTOFUNC1
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
		"Listing LHA archive...",
		"Extracting files from LHA archive...",
		"Listing LHA archive...",
		"Extracting files from LHA archive..."
	},
	{
		"Listing Arc archive...",
		"Extracting files from Arc archive...",
		"Listing Arc archive...",
		"Extracting files from Arc archive..."
	},
	{
		"Listing Zoo archive...",
		"Extracting files from Zoo archive...",
		"Listing Zoo archive...",
		"Extracting files from Zoo archive..."
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
		0,
		2,
		0,
		2
	},
	{
		-1,
		2,
		-1,
		2
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
	0xff,
	0x5f,
	0xff,
	0x33,
	0xff,
	0xff,
	0xff,
	0x17,
	0xff,
	0x10
}, defmenqual1[10] =
{
	0,
	0,
	0,
	0x80,
	0,
	0,
	0,
	0x80,
	0,
	0x80
}, defmenkeys2[9] =
{
	0x22,
	0x23,
	0x36,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff
}, defmenqual2[9] =
{
	0x80,
	0x80,
	0x80,
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
		FLAG_DOALL | FLAG_OUTFILE,
	 	FLAG_SCANDEST | FLAG_DOALL | FLAG_CDDEST | FLAG_OUTWIND,
		FLAG_DOALL | FLAG_OUTFILE,
		FLAG_SCANDEST | FLAG_DOALL | FLAG_CDDEST | FLAG_OUTWIND
	},
	{
		FLAG_DOALL | FLAG_OUTFILE,
		FLAG_SCANDEST | FLAG_DOALL | FLAG_CDDEST | FLAG_OUTWIND,
		FLAG_DOALL | FLAG_OUTFILE,
		FLAG_SCANDEST | FLAG_DOALL | FLAG_CDDEST | FLAG_OUTWIND
	},
	{
		FLAG_DOALL | FLAG_OUTWIND,
		FLAG_SCANDEST | FLAG_DOALL | FLAG_CDDEST | FLAG_OUTWIND,
		FLAG_DOALL | FLAG_OUTWIND,
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
	}
};

/* END From dopus_stuff.c */



