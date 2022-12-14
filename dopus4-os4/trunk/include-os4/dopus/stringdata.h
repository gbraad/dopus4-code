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

All users of Directory Opus 4 distributed under the GPL may be entitled
to upgrade to the latest versions of Directory Opus at a reduced price.
Please see http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#ifndef DOPUS_STRINGDATA_H
#define DOPUS_STRINGDATA_H

#ifdef __GNUC__
	#ifdef __PPC__
		#pragma pack(2)
	#endif
#elif defined(__VBCC__)
	#pragma amiga-align
#endif

/* Structure used by programs internally to define the default strings */

struct DefaultString
{
	USHORT string_id;	/* Unique ID number for this string */
	CONST_STRPTR string;	/* Pointer to the string itself */
};


/* Data structure passed to the string routines */

struct StringData
{
	const struct DefaultString *default_table;	/* Pointer to array of default strings */
	int string_count;	/* Number of strings in the table */
	char **string_table;	/* This will point to the table of string pointers to actually use */
	char *string_buffer;	/* Buffer of loaded string file */
	ULONG string_size;	/* Size of buffer */
	int min_version;	/* Minimum version to recognise */
	struct Library *LocaleBase;	// JRZ
	struct Catalog *catalog;	// JRZ
};

#ifdef __GNUC__
   #ifdef __PPC__
    #pragma pack()
   #endif
#elif defined(__VBCC__)
   #pragma default-align
#endif

#endif /* DOPUS_STRINGDATA_H */

