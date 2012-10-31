#ifndef  CLIB_ACCOUNTS_PROTOS_H
#define  CLIB_ACCOUNTS_PROTOS_H

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


struct UserInfo
{
	UBYTE	ui_UserName[32];
	UWORD	ui_UserID;
	UWORD	ui_PrimaryGroupID;
	ULONG   ui_Flags;
};

struct GroupInfo
{
	UBYTE	gi_GroupName[32];
	UWORD	gi_GroupID;
};

/* allocation functions */

struct UserInfo *AllocUserInfo(void);
struct GroupInfo *AllocGroupInfo(void);
void FreeUserInfo(struct UserInfo *user);
void FreeGroupInfo(struct GroupInfo *group);

/* xxxToyyy functions */

ULONG NameToUser(STRPTR uname, struct UserInfo *user);
ULONG NameToGroup(STRPTR gname, struct GroupInfo *group);
ULONG IDToUser(UWORD uid, struct UserInfo *user);
ULONG IDToGroup(UWORD gid, struct GroupInfo *group);

/* Nextxxx functions */

ULONG NextUser(struct UserInfo *user);
ULONG NextGroup(struct GroupInfo *group);
ULONG NextMember(struct GroupInfo *group, struct UserInfo *user);

/* memberof */

ULONG MemberOf(struct GroupInfo *group, struct UserInfo *user);

/* encryption & verification */

STRPTR ECrypt(STRPTR buff, STRPTR passwd, STRPTR uname);
ULONG VerifyUser(STRPTR uname, STRPTR passwd, struct UserInfo *user);
ULONG VerifyUserCrypt(STRPTR uname, STRPTR passwd, struct UserInfo *user);

#ifdef __cplusplus
}
#endif

#endif	 /* C_ACCOUNTS_PROTOS_H */
