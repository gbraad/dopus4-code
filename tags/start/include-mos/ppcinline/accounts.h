/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_ACCOUNTS_H
#define _PPCINLINE_ACCOUNTS_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef ACCOUNTS_BASE_NAME
#define ACCOUNTS_BASE_NAME AccountsBase
#endif /* !ACCOUNTS_BASE_NAME */

#define AllocGroupInfo() \
	LP0(0x24, struct GroupInfo *, AllocGroupInfo, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define AllocUserInfo() \
	LP0(0x1e, struct UserInfo *, AllocUserInfo, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define ECrypt(buff, passwd, uname) \
	LP3(0x6c, STRPTR, ECrypt, STRPTR, buff, a0, STRPTR, passwd, a1, STRPTR, uname, a2, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FreeGroupInfo(group) \
	LP1NR(0x30, FreeGroupInfo, struct GroupInfo *, group, a0, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FreeUserInfo(user) \
	LP1NR(0x2a, FreeUserInfo, struct UserInfo *, user, a0, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define IDToGroup(gid, group) \
	LP2(0x54, ULONG, IDToGroup, UWORD, gid, d0, struct GroupInfo *, group, a0, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define IDToUser(uid, user) \
	LP2(0x4e, ULONG, IDToUser, UWORD, uid, d0, struct UserInfo *, user, a0, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define MemberOf(group, user) \
	LP2(0x3c, ULONG, MemberOf, struct GroupInfo *, group, a0, struct UserInfo *, user, a1, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define NameToGroup(gname, group) \
	LP2(0x48, ULONG, NameToGroup, STRPTR, gname, a0, struct GroupInfo *, group, a1, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define NameToUser(uname, user) \
	LP2(0x42, ULONG, NameToUser, STRPTR, uname, a0, struct UserInfo *, user, a1, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define NextGroup(group) \
	LP1(0x60, ULONG, NextGroup, struct GroupInfo *, group, a0, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define NextMember(group, user) \
	LP2(0x66, ULONG, NextMember, struct GroupInfo *, group, a0, struct UserInfo *, user, a1, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define NextUser(user) \
	LP1(0x5a, ULONG, NextUser, struct UserInfo *, user, a0, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VerifyUser(uname, passwd, user) \
	LP3(0x36, ULONG, VerifyUser, STRPTR, uname, a0, STRPTR, passwd, a1, struct UserInfo *, user, a2, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define VerifyUserCrypt(uname, passwd, user) \
	LP3(0x72, ULONG, VerifyUserCrypt, STRPTR, uname, a0, STRPTR, passwd, a1, struct UserInfo *, user, a2, \
	, ACCOUNTS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#endif /* !_PPCINLINE_ACCOUNTS_H */
