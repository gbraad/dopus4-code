/****************************************************************

   This file was created automatically by `FlexCat 2.6.7'
   from "../catalogs/DOpus4_Config.cd".

   Do NOT edit by hand!

****************************************************************/

/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

extern char **cfg_string;

enum
{
    STR_REALLY_DISCARD_CONFIG = 0,
    STR_EXIT_WITHOUT_SAVING = 1,
    STR_CONFIGURATION_MAIN_SCREEN = 2,
    STR_OPERATION_SCREEN = 3,
    STR_MENU_SCREEN = 4,
    STR_DRIVE_SCREEN = 5,
    STR_FILETYPE_SCREEN = 6,
    STR_SCREEN_SCREEN = 7,
    STR_SYSTEM_SCREEN = 8,
    STR_HOTKEYS_SCREEN = 9,
    STR_MAIN_MENU = 10,
    STR_LEFT_MOUSE_BUTTON = 11,
    STR_RIGHT_MOUSE_BUTTON = 12,
    STR_OKAY = 13,
    STR_CANCEL = 14,
    STR_AVAILABLE_DISPLAY_ITEMS = 15,
    STR_SELECTED_DISPLAY_ITEMS = 16,
    STR_DISPLAYED_LENGTHS = 17,
    STR_CHECK_INDICATES_SORT = 18,
    STR_ENTRY_SEPARATION = 19,
    STR_UNICONIFY_HOTKEY = 20,
    STR_PROJECTS_DEFAULT_TOOL = 21,
    STR_SELECT_DESIRED_FILE = 22,
    STR_SELECT_DESIRED_DIRECTORY = 23,
    STR_SELECT_DESIRED_FONT = 24,
    STR_HOTKEY_USCORE = 25,
    STR_NEW_GADGET_BANKS_APPENDED = 26,
    STR_DEFAULT_GADGET_BANKS_APPENDED = 27,
    STR_NO_GADGET_BANKS_CONFIGURED = 28,
    STR_GADGET_DELETED = 29,
    STR_SELECT_GADGET_TO_COPY = 30,
    STR_SELECT_GADGET_TO_SWAP = 31,
    STR_SELECT_A_GADGET_TO_COPY = 32,
    STR_SELECT_FIRST_GADGET_TO_SWAP = 33,
    STR_GADGET_EDIT_SCREEN = 34,
    STR_ENTER_BANK_TO_COPY = 35,
    STR_BANK_ALREADY_EXISTS = 36,
    STR_GADGET_BANK_COPIED = 37,
    STR_ENTER_BANK_TO_SWAP = 38,
    STR_BANK_DOES_NOT_EXIST = 39,
    STR_BANKS_SWAPPED = 40,
    STR_ONLY_BANK_CONFIGURED = 41,
    STR_REALLY_DELETE_BANK = 42,
    STR_BANK_DELETED = 43,
    STR_SELECT_GADGET_TO_DELETE = 44,
    STR_GADGET_BANK_NUMBER = 45,
    STR_GADGET_SCREEN_NO_BANKS = 46,
    STR_NO_MEMORY_FOR_NEW_BANK = 47,
    STR_NEW_BANK_CREATED = 48,
    STR_NEW_MENUS_LOADED = 49,
    STR_DEFAULT_MENUS_LOADED = 50,
    STR_ENTER_MENU_TITLE = 51,
    STR_MENU_ITEM_DELETED = 52,
    STR_MENU_ITEM_INSERTED = 53,
    STR_REALLY_DELETE_THIS_MENU = 54,
    STR_MENU_DELETED = 55,
    STR_MENU_SORTED = 56,
    STR_SELECT_MENU_TO_COPY = 57,
    STR_SELECT_MENU_TO_SWAP = 58,
    STR_SELECT_ITEM_TO_COPY = 59,
    STR_SELECT_ITEM_TO_SWAP = 60,
    STR_MENU_ITEM_EDIT_SCREEN = 61,
    STR_SELECT_ITEM_IN_MENU_TO_COPY = 62,
    STR_SELECT_ITEM_IN_MENU_TO_SWAP = 63,
    STR_SELECT_ITEM_IN_MENU_TO_DELETE = 64,
    STR_SELECT_ITEM_IN_MENU_TO_SORT = 65,
    STR_SELECT_ITEM_TO_INSERT_BEFORE = 66,
    STR_SELECT_MENU_ITEM_TO_COPY = 67,
    STR_SELECT_MENU_ITEM_TO_SWAP = 68,
    STR_SELECT_MENU_ITEM_TO_DELETE = 69,
    STR_EDIT = 70,
    STR_SHORTCUT_KEY = 71,
    STR_HOTKEY = 72,
    STR_ARGUMENT_CONTROL_SEQUENCES = 73,
    STR_AVAILABLE_COMMANDS = 74,
    STR_FOREGROUND = 75,
    STR_BACKGROUND = 76,
    STR_DROP_A_TOOL_HERE = 77,
    STR_NEW_DRIVE_BANKS_LOADED = 78,
    STR_DEFAULT_DRIVE_BANKS_RESET = 79,
    STR_DRIVE_GADGET_DELETED = 80,
    STR_SELECT_DRIVE_TO_COPY_TO = 81,
    STR_SELECT_DRIVE_TO_SWAP_WITH_FIRST = 82,
    STR_SELECT_DRIVE_BANK_TO_COPY_TO = 83,
    STR_SELECT_DRIVE_BANK_TO_SWAP_WITH_FIRST = 84,
    STR_REALLY_DELETE_DRIVE_BANK = 85,
    STR_PATH = 86,
    STR_SYSTEM_DEVICES_LISTED = 87,
    STR_SELECT_DRIVE_BANK_TO_COPY = 88,
    STR_SELECT_DRIVE_BANK_TO_SWAP = 89,
    STR_SELECT_DRIVE_BANK_TO_DELETE = 90,
    STR_SELECT_DRIVE_BANK_TO_SORT = 91,
    STR_SELECT_DRIVE_GADGET_TO_COPY = 92,
    STR_SELECT_DRIVE_GADGET_TO_SWAP = 93,
    STR_SELECT_DRIVE_GADGET_TO_DELETE = 94,
    STR_NEW_FILETYPES_MERGED = 95,
    STR_DEFAULT_FILETYPES_MERGED = 96,
    STR_REALLY_DELETE_THIS_CLASS = 97,
    STR_FILE_CLASS_DELETED = 98,
    STR_SELECT_CLASS_FILE_TO_LOAD = 99,
    STR_ENTER_NAME_TO_SAVE_CLASSES = 100,
    STR_SELECT_CLASSES_TO_IMPORT = 101,
    STR_ERASE_ALL_CLASSES = 102,
    STR_EXIT_WITHOUT_SAVING_CLASSES = 103,
    STR_REDEFINE_EXISTING_CLASS_ACTION = 104,
    STR_DEFAULT_ACTION_CAN_NOT_BE_MOVED = 105,
    STR_CLASS_ACTIONS_SWAPPED = 106,
    STR_SELECT_ACTION_TO_SWAP = 107,
    STR_SELECT_ACTION_TO_SWAP_WITH_FIRST = 108,
    STR_SELECT_ACTION_TO_DELETE = 109,
    STR_REALLY_DELETE_THIS_ACTION = 110,
    STR_ACTION_DELETED = 111,
    STR_FILETYPE_CLASS = 112,
    STR_EDIT_CLASS = 113,
    STR_FILE_CLASS_EDIT_SCREEN = 114,
    STR_OPEN_FAILED = 115,
    STR_SAVE_FAILED = 116,
    STR_LIST_OF_FILETYPE_COMMANDS = 117,
    STR_POSITION = 118,
    STR_OFFSET = 119,
    STR_FILE_NOT_FOUND = 120,
    STR_EXAMPLE = 121,
    STR_DISPLAY_MODE_DESCRIPTION = 122,
    STR_DISPLAY_ITEM = 123,
    STR_FONT = 124,
    STR_FONT_SIZE = 125,
    STR_MODIFY_SIZE = 126,
    STR_USE_MMB = 127,
    STR_MINIMUM_SIZE = 128,
    STR_MAXIMUM_SIZE = 129,
    STR_DEFAULT_SIZE = 130,
    STR_MAXIMUM_COLORS = 131,
    STR_LOADING_FONT = 132,
    STR_FONT_COULD_NOT_BE_LOADED = 133,
    STR_ENTER_CONFIGURATION_FILENAME = 134,
    STR_SELECT_CONFIGURATION_TO_LOAD = 135,
    STR_FILE_NOT_VALID_CONFIGURATION = 136,
    STR_SELECT_MENUS_TO_IMPORT = 137,
    STR_SELECT_FILETYPES_TO_IMPORT = 138,
    STR_SELECT_HOTKEYS_TO_IMPORT = 139,
    STR_SELECT_FUNCTION_TO_PASTE = 140,
    STR_NEW_HOTKEYS_MERGED = 141,
    STR_DEFAULT_HOTKEYS_MERGED = 142,
    STR_SELECT_HOTKEY_TO_DELETE = 143,
    STR_SELECT_HOTKEY_TO_DUPLICATE = 144,
    STR_REALLY_DELETE_HOTKEY = 145,
    STR_HOTKEY_DELETED = 146,
    STR_HOTKEY_EDIT_SCREEN = 147,
    STR_NEW_BANK = 148,
    STR_INSERT_BANK = 149,
    STR_NEXT_BANK = 150,
    STR_LAST_BANK = 151,
    STR_CLEAR = 152,
    STR_CLEAR_LAST = 153,
    STR_MENU_DEFAULT = 154,
    STR_MENU_OPEN = 155,
    STR_MENU_SAVE = 156,
    STR_MENU_SAVE_AS = 157,
    STR_MENU_CUT = 158,
    STR_MENU_COPY = 159,
    STR_MENU_PASTE = 160,
    STR_MENU_ERASE = 161,
    STR_MENU_NEW = 162,
    STR_MENU_EDIT = 163,
    STR_MENU_DELETE = 164,
    STR_GADGETROWS_NONE = 165,
    STR_GADGETROWS_ONE = 166,
    STR_GADGETROWS_TWO = 167,
    STR_GADGETROWS_THREE = 168,
    STR_GADGETROWS_SIX = 169,
    STR_MENUNAME_CONFIGURE = 170,
    STR_MENUNAME_GADGETROWS = 171,
    STR_MENUNAME_CLASSES = 172,
    STR_FILECLASS_DEFINE = 173,
    STR_FILECLASS_EDIT = 174,
    STR_FILECLASS_DELETE = 175,
    STR_FONTPLACE_CLOCK = 176,
    STR_FONTPLACE_GADGETS = 177,
    STR_FONTPLACE_MENUS = 178,
    STR_FONTPLACE_DISKNAMES = 179,
    STR_FONTPLACE_WINDOW = 180,
    STR_FONTPLACE_GENERAL = 181,
    STR_FONTPLACE_ICONIFY = 182,
    STR_FONTPLACE_PATHS = 183,
    STR_FONTPLACE_REQUESTERS = 184,
    STR_FONTPLACE_STATUS = 185,
    STR_FONTPLACE_TEXTVIEWER = 186,
    STR_PALETTE_PRESETS = 187,
    STR_ICONLIST_TITLE = 188,
    STR_FORMAT_NAME = 189,
    STR_FORMAT_SIZE = 190,
    STR_FORMAT_BITS = 191,
    STR_FORMAT_DATE = 192,
    STR_FORMAT_COMMENT = 193,
    STR_FORMAT_TYPE = 194,
    STR_ICONS_DRAWER = 195,
    STR_ICONS_TOOL = 196,
    STR_ICONS_PROJECT = 197,
    STR_MAINMENU_GADGETS = 198,
    STR_MAINMENU_DRIVES = 199,
    STR_MAINMENU_FILETYPES = 200,
    STR_MAINMENU_HOTKEYS = 201,
    STR_MAINMENU_MENUS = 202,
    STR_MAINMENU_OPERATION = 203,
    STR_MAINMENU_SCREEN = 204,
    STR_MAINMENU_SYSTEM = 205,
    STR_ALL = 206,
    STR_OPERATION_DATEFORMAT = 207,
    STR_OPERATION_DELETE = 208,
    STR_OPERATION_ERRORCHECK = 209,
    STR_OPERATION_GENERAL = 210,
    STR_OPERATION_ICONS = 211,
    STR_OPERATION_LISTFORMAT = 212,
    STR_OPERATION_UPDATE = 213,
    STR_SYSTEM_AMIGADOS = 214,
    STR_SYSTEM_CLOCKS = 215,
    STR_SYSTEM_DIRECTORIES = 216,
    STR_SYSTEM_ICONS = 217,
    STR_SYSTEM_MODULES = 218,
    STR_SYSTEM_SHOWPATTERN = 219,
    STR_SYSTEM_STARTUP = 220,
    STR_SYSTEM_VIEWPLAY = 221,
    STR_GADGET_COPYBANK = 222,
    STR_GADGET_SWAPBANK = 223,
    STR_GADGET_DELETEBANK = 224,
    STR_GADGET_COPYGADGET = 225,
    STR_GADGET_SWAPGADGET = 226,
    STR_GADGET_DELETEGADGET = 227,
    STR_FLAG_AUTOICONIFY = 228,
    STR_FLAG_CDDESTINATION = 229,
    STR_FLAG_CDSOURCE = 230,
    STR_FLAG_DOPUSTOFRONT = 231,
    STR_FLAG_DOALLFILES = 232,
    STR_FLAG_INCLUDEDOPUS = 233,
    STR_FLAG_INCLUDESHELL = 234,
    STR_FLAG_NOQUOTE = 235,
    STR_FLAG_OUTPUTFILE = 236,
    STR_FLAG_OUTPUTWINDOW = 237,
    STR_FLAG_RECURSIVE = 238,
    STR_FLAG_RELOAD = 239,
    STR_FLAG_RESCANDEST = 240,
    STR_FLAG_RESCANSOURCE = 241,
    STR_FLAG_RUNASYNC = 242,
    STR_FLAG_WORKBENCHTOFRONT = 243,
    STR_EDIT_NAME = 244,
    STR_EDIT_SAMPLE = 245,
    STR_EDIT_NEWENTRY = 246,
    STR_EDIT_DUPLICATE = 247,
    STR_EDIT_SWAP = 248,
    STR_EDIT_STACKSIZE = 249,
    STR_EDIT_PRIORITY = 250,
    STR_EDIT_CLOSEDELAY = 251,
    STR_EDIT_ACTION = 252,
    STR_EDITCLASS_FILECLASS = 253,
    STR_EDITCLASS_CLASSID = 254,
    STR_EDITCLASS_FILEVIEWER = 255,
    STR_MENU_COPYMENU = 256,
    STR_MENU_SWAPMENU = 257,
    STR_MENU_DELETEMENU = 258,
    STR_MENU_SORTMENU = 259,
    STR_MENU_INSERTITEM = 260,
    STR_MENU_COPYITEM = 261,
    STR_MENU_SWAPITEM = 262,
    STR_MENU_DELETEITEM = 263,
    STR_DRIVES_GETDRIVES = 264,
    STR_DRIVES_SORTBANK = 265,
    STR_DRIVES_COPYDRIVE = 266,
    STR_DRIVES_SWAPDRIVE = 267,
    STR_DRIVES_DELETEDRIVE = 268,
    STR_HOTKEYS_NEWHOTKEY = 269,
    STR_SCREEN_COLORS = 270,
    STR_SCREEN_FONTS = 271,
    STR_SCREEN_PALETTE = 272,
    STR_SCREEN_SCREENMODE = 273,
    STR_PALETTE_RED = 274,
    STR_PALETTE_GREEN = 275,
    STR_PALETTE_BLUE = 276,
    STR_COLOR_STATUS = 277,
    STR_COLOR_SELDISK = 278,
    STR_COLOR_UNSELDISK = 279,
    STR_COLOR_SELDIR = 280,
    STR_COLOR_UNSELDIR = 281,
    STR_COLOR_SELFILE = 282,
    STR_COLOR_UNSELFILE = 283,
    STR_COLOR_SLIDERS = 284,
    STR_COLOR_ARROWS = 285,
    STR_COLOR_TINY = 286,
    STR_COLOR_CLOCK = 287,
    STR_COLOR_REQUESTER = 288,
    STR_COLOR_BOXES = 289,
    STR_COLOR_PATHNAME = 290,
    STR_COLOR_SELPATHNAME = 291,
    STR_SCREENMODE_WIDTH = 292,
    STR_SCREENMODE_HEIGHT = 293,
    STR_SCREENMODE_COLORS = 294,
    STR_SCREENMODE_DEFAULT = 295,
    STR_SCREENMODE_HALFHEIGHT = 296,
    STR_LISTFORMAT_TITLE = 297,
    STR_LISTFORMAT_RESET = 298,
    STR_LISTFORMAT_NAME = 299,
    STR_LISTFORMAT_COMMENT = 300,
    STR_LISTFORMAT_REVERSE = 301,
    STR_ARROWS_INSIDE = 302,
    STR_ARROWS_OUTSIDE = 303,
    STR_ARROWS_EACH = 304,
    STR_ARROWTYPE_UPDOWN = 305,
    STR_ARROWTYPE_LEFTRIGHT = 306,
    STR_ARROWTYPE_PREVNEXT = 307,
    STR_NEW = 308,
    STR_MISC_FLAGS = 309,
    STR_OP_COPY_WHENCOPYING = 310,
    STR_OP_COPY_CHECKDEST = 311,
    STR_OP_COPY_SETARCHIVE = 312,
    STR_OP_COPY_ALSOCOPY = 313,
    STR_OP_COPY_DATESTAMP = 314,
    STR_OP_COPY_BITS = 315,
    STR_OP_COPY_COMMENT = 316,
    STR_OP_COPY_IFFILEEXISTS = 317,
    STR_OP_COPY_ALWAYSREPLACE = 318,
    STR_OP_COPY_NEVERREPLACE = 319,
    STR_OP_COPY_ONLYOLDER = 320,
    STR_OP_COPY_ASKBEFORE = 321,
    STR_OP_DATE_DATEFORMAT = 322,
    STR_OP_DATE_DDMMMYY = 323,
    STR_OP_DATE_YYMMDD = 324,
    STR_OP_DATE_MMDDYY = 325,
    STR_OP_DATE_DDMMYY = 326,
    STR_OP_DATE_NAMESUB = 327,
    STR_OP_DATE_12HOUR = 328,
    STR_OP_DEL_ASKBEFORE = 329,
    STR_OP_DEL_COMMENCING = 330,
    STR_OP_DEL_DELETEFILES = 331,
    STR_OP_DEL_DELETEDIRS = 332,
    STR_OP_DEL_IGNOREPROT = 333,
    STR_OP_ERROR_ENABLE = 334,
    STR_OP_ERROR_DOSREQ = 335,
    STR_OP_ERROR_OPUSREQ = 336,
    STR_OP_GENERAL_DRAG = 337,
    STR_OP_GENERAL_DISPLAYINFO = 338,
    STR_OP_GENERAL_DOUBLECLICK = 339,
    STR_OP_GENERAL_SLIDERACTIVE = 340,
    STR_OP_ICON_CREATEWITHDIR = 341,
    STR_OP_ICON_DOUNTOICONS = 342,
    STR_OP_ICON_SELECTAUTO = 343,
    STR_OP_UPDATE_WHENPROCESSING = 344,
    STR_OP_UPDATE_PROGRESSIND = 345,
    STR_OP_UPDATE_LEFTJUSTIFY = 346,
    STR_OP_UPDATE_SCROLLTOFOLLOW = 347,
    STR_OP_UPDATE_UPDATEFREE = 348,
    STR_OP_UPDATE_STARTNOTIFY = 349,
    STR_OP_UPDATE_REDRAWMORE = 350,
    STR_OP_UPDATE_QUIETGETDIR = 351,
    STR_SYS_AMIGADOS_TITLE = 352,
    STR_SYS_AMIGADOS_SHELL = 353,
    STR_SYS_AMIGADOS_CONSOLE = 354,
    STR_SYS_AMIGADOS_STARTUP = 355,
    STR_SYS_AMIGADOS_PRIORITY = 356,
    STR_SYS_CLOCK_TITLE = 357,
    STR_SYS_CLOCK_MEMORY = 358,
    STR_SYS_CLOCK_CPUMONITOR = 359,
    STR_SYS_CLOCK_DATE = 360,
    STR_SYS_CLOCK_TIME = 361,
    STR_SYS_CLOCK_WHENICONIFIED = 362,
    STR_SYS_CLOCK_WINDOW = 363,
    STR_SYS_CLOCK_NOWINDOW = 364,
    STR_SYS_CLOCK_APPICON = 365,
    STR_SYS_CLOCK_SHOWFREEAS = 366,
    STR_SYS_CLOCK_BYTES = 367,
    STR_SYS_CLOCK_KILOBYTES = 368,
    STR_SYS_CLOCK_TEXTFORMAT = 369,
    STR_SYS_CLOCK_CHIPANDFAST = 370,
    STR_SYS_CLOCK_CANDF = 371,
    STR_SYS_DIR_CACHES = 372,
    STR_SYS_DIR_NUMBUFFERS = 373,
    STR_SYS_DIR_ALWAYSEMPTY = 374,
    STR_SYS_DIR_REREADINCOMPLETE = 375,
    STR_SYS_DIR_SEARCHBUFFERS = 376,
    STR_SYS_DIR_SEARCHPARENT = 377,
    STR_SYS_DIR_DIRREAD = 378,
    STR_SYS_DIR_AUTODISKCHANGE = 379,
    STR_SYS_DIR_AUTODISKLOAD = 380,
    STR_SYS_DIR_EXPANDPATHS = 381,
    STR_SYS_DIR_USEEXALL = 382,
    STR_FILEVIEW_HEX = 383,
    STR_SYS_DIR_BLOCKSFREE = 384,
    STR_SYS_DIR_PERCENTAGE = 385,
    STR_PALETTE_RESET = 386,
    STR_PALETTE_DOPUS_DEFAULT = 387,
    STR_PALETTE_WB_DEFAULT = 388,
    STR_PALETTE_WB_CURRENT = 389,
    STR_OKAY_TO_QUIT = 390,
    STR_SCREEN_GENERAL = 391,
    STR_FTYPE_ACTION = 392,
    STR_FTYPE_COMMAND = 393,
    STR_FILEVIEW_DEC = 394,
    STR_EDIT_MENU = 395,
    STR_SYS_SHOWPATTERN_TITLE = 396,
    STR_SYS_SHOWPATTERN_HIDDENBIT = 397,
    STR_SYS_SHOWPATTERN_SHOW = 398,
    STR_SYS_SHOWPATTERN_HIDE = 399,
    STR_SYS_STARTUP_TITLE = 400,
    STR_SYS_STARTUP_LEFT = 401,
    STR_SYS_STARTUP_RIGHT = 402,
    STR_SYS_STARTUP_AREXX = 403,
    STR_SYS_STARTUP_STARTUP = 404,
    STR_SYS_STARTUP_UNICONIFY = 405,
    STR_SYS_VIEWPLAY_TITLE = 406,
    STR_SYS_VIEWPLAY_BLACK = 407,
    STR_SYS_VIEWPLAY_PAUSED = 408,
    STR_SYS_VIEWPLAY_8BITCOL = 409,
    STR_SYS_VIEWPLAY_SHOWDELAY = 410,
    STR_SYS_VIEWPLAY_FADEDELAY = 411,
    STR_SYS_VIEWPLAY_SOUNDPLAYER = 412,
    STR_SYS_VIEWPLAY_FILTER = 413,
    STR_SYS_VIEWPLAY_LOOP = 414,
    STR_SYS_VIEWPLAY_TEXTVIEW = 415,
    STR_SYS_VIEWPLAY_BORDERS = 416,
    STR_SYS_VIEWPLAY_TABSIZE = 417,
    STR_SCREEN_GENERAL_TINYGADS = 419,
    STR_SCREEN_GENERAL_GADGETSLIDERS = 420,
    STR_SCREEN_GENERAL_INDICATERMB = 421,
    STR_SCREEN_GENERAL_NEWLOOKSLIDERS = 422,
    STR_EDITLIST_TITLE1 = 423,
    STR_EDITLIST_TITLE2 = 424,
    STR_FILETYPEACTIONLIST_TITLE = 425,
    STR_HOTKEYSLIST_TITLE = 426,
    STR_SCREENMODELIST_TITLE = 427,
    STR_LEFT_WINDOW = 428,
    STR_RIGHT_WINDOW = 429,
    STR_SEP_MIX_FILES = 430,
    STR_SEP_DIRS_FIRST = 431,
    STR_SEP_FILES_FIRST = 432,
    STR_FUNCTIONLIST_ABOUT = 433,
    STR_FUNCTIONLIST_ADDICON = 434,
    STR_FUNCTIONLIST_ALARM = 435,
    STR_FUNCTIONLIST_ALL = 436,
    STR_FUNCTIONLIST_ANSIREAD = 437,
    STR_FUNCTIONLIST_AREXX = 438,
    STR_FUNCTIONLIST_ASSIGN = 439,
    STR_FUNCTIONLIST_BEEP = 440,
    STR_FUNCTIONLIST_BUFFERLIST = 441,
    STR_FUNCTIONLIST_BUTTONICONIFY = 442,
    STR_FUNCTIONLIST_CD = 443,
    STR_FUNCTIONLIST_CHECKFIT = 444,
    STR_FUNCTIONLIST_CLEARBUFFERS = 445,
    STR_FUNCTIONLIST_CLEARSIZES = 446,
    STR_FUNCTIONLIST_CLEARWIN = 447,
    STR_FUNCTIONLIST_CLONE = 448,
    STR_FUNCTIONLIST_COMMENT = 449,
    STR_FUNCTIONLIST_CONFIGURE = 450,
    STR_FUNCTIONLIST_CONTST = 451,
    STR_FUNCTIONLIST_COPY = 452,
    STR_FUNCTIONLIST_COPYAS = 453,
    STR_FUNCTIONLIST_COPYWINDOW = 454,
    STR_FUNCTIONLIST_DATESTAMP = 456,
    STR_FUNCTIONLIST_DEFAULTS = 457,
    STR_FUNCTIONLIST_DELETE = 458,
    STR_FUNCTIONLIST_DIRTREE = 459,
    STR_FUNCTIONLIST_DISKCOPY = 460,
    STR_FUNCTIONLIST_DISKCOPYBG = 461,
    STR_FUNCTIONLIST_DISKINFO = 462,
    STR_FUNCTIONLIST_DISPLAYDIR = 463,
    STR_FUNCTIONLIST_DOPUSTOBACK = 464,
    STR_FUNCTIONLIST_DOPUSTOFRONT = 465,
    STR_FUNCTIONLIST_ENCRYPT = 466,
    STR_FUNCTIONLIST_ENDFUNCTION = 467,
    STR_FUNCTIONLIST_ERRORHELP = 468,
    STR_FUNCTIONLIST_EXECUTE = 469,
    STR_FUNCTIONLIST_FORMAT = 470,
    STR_FUNCTIONLIST_FORMATBG = 471,
    STR_FUNCTIONLIST_GETDEVICES = 472,
    STR_FUNCTIONLIST_GETSIZES = 473,
    STR_FUNCTIONLIST_HELP = 474,
    STR_FUNCTIONLIST_HEXREAD = 475,
    STR_FUNCTIONLIST_HUNT = 476,
    STR_FUNCTIONLIST_ICONIFY = 477,
    STR_FUNCTIONLIST_ICONINFO = 478,
    STR_FUNCTIONLIST_INSTALL = 479,
    STR_FUNCTIONLIST_INSTALLBG = 480,
    STR_FUNCTIONLIST_LASTSAVED = 481,
    STR_FUNCTIONLIST_LOADCONFIG = 482,
    STR_FUNCTIONLIST_LOADSTRINGS = 483,
    STR_FUNCTIONLIST_LOOPPLAY = 484,
    STR_FUNCTIONLIST_LOOPPLAY8SVX = 485,
    STR_FUNCTIONLIST_MAKEDIR = 486,
    STR_FUNCTIONLIST_MODIFY = 487,
    STR_FUNCTIONLIST_MOVE = 488,
    STR_FUNCTIONLIST_MOVEAS = 489,
    STR_FUNCTIONLIST_NEWCLI = 490,
    STR_FUNCTIONLIST_NONE = 491,
    STR_FUNCTIONLIST_OTHERWINDOW = 492,
    STR_FUNCTIONLIST_PARENT = 493,
    STR_FUNCTIONLIST_PLAY = 494,
    STR_FUNCTIONLIST_PLAY8SVX = 495,
    STR_FUNCTIONLIST_PLAYST = 496,
    STR_FUNCTIONLIST_PRINT = 497,
    STR_FUNCTIONLIST_PRINTDIR = 498,
    STR_FUNCTIONLIST_PROTECT = 499,
    STR_FUNCTIONLIST_QUIT = 500,
    STR_FUNCTIONLIST_READ = 501,
    STR_FUNCTIONLIST_REDRAW = 502,
    STR_FUNCTIONLIST_RELABEL = 503,
    STR_FUNCTIONLIST_REMEMBER = 504,
    STR_FUNCTIONLIST_RENAME = 505,
    STR_FUNCTIONLIST_RESCAN = 506,
    STR_FUNCTIONLIST_RESELECT = 507,
    STR_FUNCTIONLIST_RESTORE = 508,
    STR_FUNCTIONLIST_ROOT = 509,
    STR_FUNCTIONLIST_RUN = 510,
    STR_FUNCTIONLIST_SAVECONFIG = 511,
    STR_FUNCTIONLIST_SCANDIR = 512,
    STR_FUNCTIONLIST_SEARCH = 513,
    STR_FUNCTIONLIST_SELECT = 514,
    STR_FUNCTIONLIST_SHOW = 515,
    STR_FUNCTIONLIST_SMARTREAD = 516,
    STR_FUNCTIONLIST_STOPST = 517,
    STR_FUNCTIONLIST_SWAPWINDOW = 518,
    STR_FUNCTIONLIST_TOGGLE = 519,
    STR_FUNCTIONLIST_UNICONIFY = 520,
    STR_FUNCTIONLIST_USER_FTYPE1 = 521,
    STR_FUNCTIONLIST_USER_FTYPE2 = 522,
    STR_FUNCTIONLIST_USER_FTYPE3 = 523,
    STR_FUNCTIONLIST_USER_FTYPE4 = 524,
    STR_FUNCTIONLIST_VERIFY = 525,
    STR_FUNCTIONLIST_VERSION = 526,
    STR_COMMANDSEQ_DESTINATION = 527,
    STR_COMMANDSEQ_FIRSTFILE = 528,
    STR_COMMANDSEQ_ALLFILES = 529,
    STR_COMMANDSEQ_FIRSTFILEUNSEL = 530,
    STR_COMMANDSEQ_ALLFILESUNSEL = 531,
    STR_COMMANDSEQ_FIRSTFILENOPATH = 532,
    STR_COMMANDSEQ_ALLFILESNOPATH = 533,
    STR_COMMANDSEQ_FIRSTFILENOPATHUNSEL = 534,
    STR_COMMANDSEQ_ALLFILESNOPATHUNSEL = 535,
    STR_COMMANDSEQ_PORTNAME = 536,
    STR_COMMANDSEQ_DIRREQ = 537,
    STR_COMMANDSEQ_FILEREQ = 538,
    STR_COMMANDSEQ_MULTIFILEREQ = 539,
    STR_COMMANDSEQ_FONTREQ = 540,
    STR_COMMANDSEQ_ARGUMENTS = 541,
    STR_COMMANDSEQ_SOURCE = 542,
    STR_CLASSOPS_AND = 543,
    STR_CLASSOPS_MATCH = 544,
    STR_CLASSOPS_MATCHBITS = 545,
    STR_CLASSOPS_MATCHCOMMENT = 546,
    STR_CLASSOPS_MATCHDATE = 547,
    STR_CLASSOPS_MATCHNAME = 548,
    STR_CLASSOPS_MATCHSIZE = 549,
    STR_CLASSOPS_MOVE = 550,
    STR_CLASSOPS_MOVETO = 551,
    STR_CLASSOPS_OR = 552,
    STR_CLASSOPS_SEARCHFOR = 553,
    STR_BY = 554,
    STR_SELECT_HOTKEY_TO_SWAP = 555,
    STR_SELECT_SECOND_HOTKEY_TO_SWAP = 556,
    STR_MODULE_LIST_TITLE = 557,
    STR_MODULE_LIST_CHECK_STRING = 559,
    STR_SYS_CLOCK_KILOBYTES_FREE = 561,
    STR_SYS_STARTUP_CONFIG = 562,
    STR_MENU_DUPLICATE = 563,
    STR_FILECLASS_DUPLICATE = 564,
    STR_MENU_CLEARCLIPS = 565,
    STR_REALLY_CLEAR_CLIPS = 566,
    STR_SCREEN_GENERAL_DRAGREQUESTERS = 567,
    STR_FORMAT_OWNER = 568,
    STR_FORMAT_GROUP = 569,
    STR_PALETTE_TINT = 570,
    STR_PALETTE_PHARAOH = 571,
    STR_PALETTE_SUNSET = 572,
    STR_PALETTE_OCEAN = 573,
    STR_PALETTE_STEEL = 574,
    STR_PALETTE_CHOCOLATE = 575,
    STR_PALETTE_PEWTER = 576,
    STR_PALETTE_WINE = 577,
    STR_PALETTE_A2024 = 578,
    STR_FTYPE_CLICKMCLICK = 579,
    STR_FTYPE_DOUBLECLICK = 580,
    STR_FTYPE_MMBCLICK = 581,
    STR_FUNCTION_COMMAND = 582,
    STR_FUNCTION_AMIGADOS = 583,
    STR_FUNCTION_WORKBENCH = 584,
    STR_FUNCTION_BATCH = 585,
    STR_FUNCTION_AREXX = 586,
    STR_MODE_WORKBENCH_CLONE = 588,
    STR_MODE_WORKBENCH_USE = 589,
    STR_COLOURS_TINY_GADS = 590,
    STR_FORMAT_GROUPBITS = 591,
    STR_MENUNAME_NEATSTUFF = 592,
    STR_NEAT_PAINT_MODE = 593,
    STR_PAINT_SELECT_COLOURS = 594,
    STR_PAINT_STATE = 595,
    STR_SCREEN_GENERAL_TITLEBARSTATUS = 597,
    STR_OP_UPDATE_PROGRESSIND_COPY = 598,
    STR_SCREEN_MODE_USE = 599,
    STR_SCREEN_GENERAL_WINDOWBORDERS = 600,
    STR_SCREEN_MODE_CLONE = 601,
    STR_SCREEN_SLIDERS = 602,
    STR_OP_UPDATE_PROGRESS = 603,
    STR_FONTPLACE_SCREEN = 604,
    STR_FUNCTIONLIST_SETVAR = 605,
    STR_COMMANDSEQ_VARIABLE = 606,
    STR_SYS_VIEWPLAY_BESTMODEID = 607,
    STR_FUNCTIONLIST_NOTIFY = 608,
    STR_FUNCTIONLIST_PARENTLIST = 609,
    STR_SIZE_KMGMODE = 610,
    STR_SYS_VIEWPLAY_INWINDOW = 611,
    STR_SYS_TOP_Y = 612,
    STR_SYS_LEFT_X = 613,
    STR_SYS_WIDTH = 614,
    STR_SYS_HEIGHT = 615,
    STR_CLASSOPS_MATCHI = 616,
    STR_MENU_MAX_REACHED = 617,
    STR_OP_GENERAL_FMPARENTMODE = 618,
    STR_OP_COPY_COPYARCHIVE = 619,
    STR_OP_GENERAL_FORCEQUIT = 620,
    STR_USE = 621,
    STR_FUNCTIONLIST_MAKELINK = 622,
    STR_OP_GENERAL_MMBSELECTS = 623,

    STR_STRING_COUNT
};
