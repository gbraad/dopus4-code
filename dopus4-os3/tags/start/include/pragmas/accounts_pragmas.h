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

#ifndef ACCOUNTS_PRAGMAS_H
#define ACCOUNTS_PRAMGAS_H

#pragma libcall AccountsBase AllocUserInfo 1e 0
#pragma libcall AccountsBase AllocGroupInfo 24 0
/**/
#pragma libcall AccountsBase FreeUserInfo 2a 801
#pragma libcall AccountsBase FreeGroupInfo 30 801
/**/
#pragma libcall AccountsBase VerifyUser 36 A9803
#pragma libcall AccountsBase MemberOf 3c 9802
/**/
#pragma libcall AccountsBase NameToUser 42 9802
#pragma libcall AccountsBase NameToGroup 48 9802
#pragma libcall AccountsBase IDToUser 4e 8002
#pragma libcall AccountsBase IDToGroup 54 8002
#pragma libcall AccountsBase NextUser 5a 801
#pragma libcall AccountsBase NextGroup 60 801
#pragma libcall AccountsBase NextMember 66 9802
/**/
#pragma libcall AccountsBase ECrypt 6c A9803
#pragma libcall AccountsBase VerifyUserCrypt 72 A9803

#endif /* ACCOUNTS_PRAGMAS_H */
