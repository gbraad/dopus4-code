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

#include "DirectoryOpus_rev.h"
#include "dopus.h"

#define DOPUS_VERSION  "4"
#define DOPUS_REV      "17"
#define DOPUS_BETAREV  "pre19"

#define DOPUS_REVISION DOPUS_REV

#define STR(x)  STR2(x)
#define STR2(x) #x

#if defined(__GNUC__)
  #if defined(__GNUC_PATCHLEVEL__)
    const char * const compiler = "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__);
  #else
    const char * const compiler = "GCC " STR(__GNUC__) "." STR(__GNUC_MINOR__) ".x";
  #endif
#endif

static const char * const version __attribute__ ((used)) = NEW_VERSTAG " OS4";
STRPTR comp_date = __DATE__, comp_time = __TIME__;

void about()
{
	char buf[1024] = { 0, };

	sprintf(buf, globstring[STR_ABOUT], "Directory Opus " VERSION_STR "." REVISION_STR "." SUBREVISION_STR, "Jacek Rzeuski\nCopyright 1993-2000 Jonathan Potter");
	simplerequest(buf, globstring[STR_CONTINUE], NULL);
}

void give_version_info()
{
	char buf[1024] = { 0, };
	uint32 ver = DOpusBase->lib_Version, rev = DOpusBase->lib_Revision;

//	sprintf(buf,globstring[STR_VERSION_CONTENTS],globstring[STR_VERSION_HEADER], 

	sprintf(buf, "DirectoryOpus: %d.%d.%d\n" \
		     "dopus.library: %ld.%ld\n\n" \
		     "Compiled with: %s\n" \
		     "Compilation date: %s\n" \
		     "Compilation time: %s\n", VERSION, REVISION, SUBREVISION, ver, rev, compiler, comp_date, comp_time);
	simplerequest(buf, globstring[STR_CONTINUE], NULL);
}
