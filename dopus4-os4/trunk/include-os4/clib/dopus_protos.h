#ifndef  CLIB_DOPUS_PROTOS_H
#define  CLIB_DOPUS_PROTOS_H

#ifndef  DOPUS_H
#include <dopus/dopusbase.h>
#endif

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

#ifndef DOPUS_REQUESTERS_H
#include <dopus/requesters.h>
#endif

#ifndef DOPUS_CONFIG_H
#include <dopus/config.h>
#endif

#ifndef DOPUS_STRINGDATA_H
#include <dopus/stringdata.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void ActivateStrGad(struct Gadget *,struct Window *);
int AddGadgets(struct Window *,struct Gadget *,char **,int,int,int,int);
int AddListView(struct DOpusListView *,int);
int Assign(char *,char *);
int Atoh(char *,int);
//void BtoCStr(BSTR,char *,int);
int CheckExist(char *,int *);
int CheckHexGad(struct Gadget *,struct Window *,int,int);
int CheckNumGad(struct Gadget *,struct Window *,int,int);
void Decode_RLE(char *source,char *dest,int size);
void DisableGadget(struct Gadget *,struct RastPort *,int,int);
void Do3DBox(struct RastPort *,int,int,int,int,int,int);
void Do3DCycleBox(struct RastPort *,int,int,int,int,int,int);
void Do3DFrame(struct RastPort *,int,int,int,int,char *,int,int);
void Do3DStringBox(struct RastPort *,int,int,int,int,int,int);
void DoArrow(struct RastPort *,int,int,int,int,int,int,int);
void DoCycleGadget(struct Gadget *,struct Window *,char **,int);
void DoGlassImage(struct RastPort *,struct Gadget *,int,int,int);
int DoRMBGadget(struct RMBGadget *,struct Window *);
int DoSimpleRequest(struct Window *,struct DOpusSimpleRequest *);
void DrawCheckMark(struct RastPort *,int,int,int);
void EnableGadget(struct Gadget *,struct RastPort *,int,int);
int FileRequest(struct DOpusFileReq *);
void FixSliderBody(struct Window *,struct Gadget *,int,int,int);
void FixSliderPot(struct Window *,struct Gadget *,int,int,int,int);
int GetSliderPos(struct Gadget *,int,int);
void GetWBScreen(struct Screen *);
void GhostGadget(struct Gadget *,struct RastPort *,int,int);
void HiliteGad(struct Gadget *,struct RastPort *);
void *LAllocRemember(struct DOpusRemember **,ULONG,ULONG);
void LFreeRemEntry(struct DOpusRemember **,char *);
void LFreeRemember(struct DOpusRemember **);
struct DOpusListView *ListViewIDCMP(struct DOpusListView *,struct IntuiMessage *);
void LStrCat(char *,char *);
int LStrCmp(char *,char *);
int LStrCmpI(char *,char *);
void LStrCpy(char *,char *);
void LStrnCat(char *,char *,int);
int LStrnCmp(char *,char *,int);
int LStrnCmpI(char *,char *,int);
void LStrnCpy(char *,char *,int);
int PathName(BPTR,char *,int);
int RawkeyToStr(USHORT,USHORT,char *,char *,int);
int RefreshListView(struct DOpusListView *,int);
void RefreshStrGad(struct Gadget *,struct Window *);
int RemoveListView(struct DOpusListView *,int);
int SearchPathList(char *,char *,int);
//int SendPacket(struct MsgPort *,int,ULONG *,int);
void SetBusyPointer(struct Window *);
void ShowSlider(struct Window *,struct Gadget *);
void StampToStr(struct DateTime *);
int StrCombine(char *,char *,char *,int);
int StrConcat(char *,char *,int);
void StrToLower(char *,char *);
int StrToStamp(struct DateTime *);
void StrToUpper(char *,char *);
int TackOn(char *,char *,int);
void UScoreText(struct RastPort *,char *,int,int,int);
struct Image *GetButtonImage(int,int,int,int,int,int,struct DOpusRemember **);
void DrawRadioButton(struct RastPort *,int,int,int,int,int,int);
struct Image *GetCheckImage(UBYTE,UBYTE,int,struct DOpusRemember **);
struct Window *OpenRequester(struct RequesterBase *);
void CloseRequester(struct RequesterBase *);
APTR AddRequesterObject(struct RequesterBase *,struct TagItem *);
void RefreshRequesterObject(struct RequesterBase *,struct RequesterObject *);
void ObjectText(struct RequesterBase *,short,short,short,short,char *,unsigned short);
void CreateGadgetBorders(struct DOpusRemember **,int,int,struct Border **,struct Border **,int,int,int);
void AddGadgetBorders(struct DOpusRemember **,struct Gadget *,int,int,int);
void SelectGadget(struct Window *,struct Gadget *);
int FSSetMenuStrip(struct Window *,struct Menu *);

/* DOpus-private functions */

int ReadConfig(char *,struct ConfigStuff *);
int SaveConfig(char *,struct ConfigStuff *);
int DefaultConfig(struct ConfigStuff *);
int GetDevices(struct ConfigStuff *);
int AssignGadget(struct ConfigStuff *,int,int,char *,char *);
int AssignMenu(struct ConfigStuff *,int,char *,char *);
int FreeConfig(struct ConfigStuff *);
int CheckConfig(struct ConfigStuff *);
int FindSystemFile(char *,char *,int,int);
int ReadStringFile(struct StringData *,char *);
int FreeStringFile(struct StringData *);

#ifdef __cplusplus
}
#endif

#endif   /* CLIB_DOPUS_PROTOS_H */
