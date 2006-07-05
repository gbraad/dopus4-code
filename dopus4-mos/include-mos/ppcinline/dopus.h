/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_DOPUS_H
#define _PPCINLINE_DOPUS_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef DOPUS_BASE_NAME
#define DOPUS_BASE_NAME DOpusBase
#endif /* !DOPUS_BASE_NAME */

#define TackOn(__p0, __p1, __p2) \
	LP3(222, int , TackOn, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define AddRequesterObject(__p0, __p1) \
	LP2(540, APTR , AddRequesterObject, \
		struct RequesterBase *, __p0, a0, \
		struct TagItem *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LFreeRemEntry(__p0, __p1) \
	LP2NR(582, LFreeRemEntry, \
		struct DOpusRemember **, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define RemoveListView(__p0, __p1) \
	LP2(258, int , RemoveListView, \
		struct DOpusListView *, __p0, a0, \
		int , __p1, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define StrToLower(__p0, __p1) \
	LP2NR(348, StrToLower, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SaveConfig(__p0, __p1) \
	LP2(420, int , SaveConfig, \
		char *, __p0, a0, \
		struct ConfigStuff *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Decode_RLE(__p0, __p1, __p2) \
	LP3NR(564, Decode_RLE, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DoSimpleRequest(__p0, __p1) \
	LP2(408, int , DoSimpleRequest, \
		struct Window *, __p0, a0, \
		struct DOpusSimpleRequest *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FileRequest(__p0) \
	LP1(30, int , FileRequest, \
		struct DOpusFileReq *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrCmpI(__p0, __p1) \
	LP2(138, int , LStrCmpI, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CheckHexGad(__p0, __p1, __p2, __p3) \
	LP4(390, int , CheckHexGad, \
		struct Gadget *, __p0, a0, \
		struct Window *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define AssignGadget(__p0, __p1, __p2, __p3, __p4) \
	LP5(438, int , AssignGadget, \
		struct ConfigStuff *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		char *, __p3, a1, \
		char *, __p4, a2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CreateGadgetBorders(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7) \
	LP8NR(594, CreateGadgetBorders, \
		struct DOpusRemember **, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		struct Border **, __p3, a1, \
		struct Border **, __p4, a2, \
		int , __p5, d2, \
		int , __p6, d3, \
		int , __p7, d4, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define StrConcat(__p0, __p1, __p2) \
	LP3(156, int , StrConcat, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrCpy(__p0, __p1) \
	LP2NR(114, LStrCpy, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Do3DBox(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	LP7NR(36, Do3DBox, \
		struct RastPort *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		int , __p3, d2, \
		int , __p4, d3, \
		int , __p5, d4, \
		int , __p6, d5, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define EnableGadget(__p0, __p1, __p2, __p3) \
	LP4NR(486, EnableGadget, \
		struct Gadget *, __p0, a0, \
		struct RastPort *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CheckConfig(__p0) \
	LP1(516, int , CheckConfig, \
		struct ConfigStuff *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Do3DFrame(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7) \
	LP8NR(456, Do3DFrame, \
		struct RastPort *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		int , __p3, d2, \
		int , __p4, d3, \
		char *, __p5, a1, \
		int , __p6, d4, \
		int , __p7, d5, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CloseRequester(__p0) \
	LP1NR(534, CloseRequester, \
		struct RequesterBase *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GetWBScreen(__p0) \
	LP1NR(306, GetWBScreen, \
		struct Screen *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrnCmpI(__p0, __p1, __p2) \
	LP3(144, int , LStrnCmpI, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FixSliderPot(__p0, __p1, __p2, __p3, __p4, __p5) \
	LP6NR(276, FixSliderPot, \
		struct Window *, __p0, a0, \
		struct Gadget *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		int , __p4, d2, \
		int , __p5, d3, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrCat(__p0, __p1) \
	LP2NR(102, LStrCat, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GetCheckImage(__p0, __p1, __p2, __p3) \
	LP4(522, struct Image *, GetCheckImage, \
		UBYTE , __p0, d0, \
		UBYTE , __p1, d1, \
		int , __p2, d2, \
		struct DOpusRemember **, __p3, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GetDevices(__p0) \
	LP1(432, int , GetDevices, \
		struct ConfigStuff *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ShowSlider(__p0, __p1) \
	LP2NR(510, ShowSlider, \
		struct Window *, __p0, a0, \
		struct Gadget *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ListViewIDCMP(__p0, __p1) \
	LP2(246, struct DOpusListView *, ListViewIDCMP, \
		struct DOpusListView *, __p0, a0, \
		struct IntuiMessage *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrCmp(__p0, __p1) \
	LP2(126, int , LStrCmp, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FindSystemFile(__p0, __p1, __p2, __p3) \
	LP4(450, int , FindSystemFile, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define AddGadgetBorders(__p0, __p1, __p2, __p3, __p4) \
	LP5NR(588, AddGadgetBorders, \
		struct DOpusRemember **, __p0, a0, \
		struct Gadget *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		int , __p4, d2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define AddGadgets(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	LP7(366, int , AddGadgets, \
		struct Window *, __p0, a0, \
		struct Gadget *, __p1, a1, \
		char **, __p2, a2, \
		int , __p3, d0, \
		int , __p4, d1, \
		int , __p5, d2, \
		int , __p6, d3, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define StrToUpper(__p0, __p1) \
	LP2NR(342, StrToUpper, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define HiliteGad(__p0, __p1) \
	LP2NR(402, HiliteGad, \
		struct Gadget *, __p0, a0, \
		struct RastPort *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FSSetMenuStrip(__p0, __p1) \
	LP2(606, int , FSSetMenuStrip, \
		struct Window *, __p0, a0, \
		struct Menu *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DoGlassImage(__p0, __p1, __p2, __p3, __p4) \
	LP5NR(558, DoGlassImage, \
		struct RastPort *, __p0, a0, \
		struct Gadget *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		int , __p4, d2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ReadConfig(__p0, __p1) \
	LP2(414, int , ReadConfig, \
		char *, __p0, a0, \
		struct ConfigStuff *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ReadStringFile(__p0, __p1) \
	LP2(570, int , ReadStringFile, \
		struct StringData *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DrawRadioButton(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	LP7NR(498, DrawRadioButton, \
		struct RastPort *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		int , __p3, d2, \
		int , __p4, d3, \
		int , __p5, d4, \
		int , __p6, d5, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CheckNumGad(__p0, __p1, __p2, __p3) \
	LP4(384, int , CheckNumGad, \
		struct Gadget *, __p0, a0, \
		struct Window *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define AssignMenu(__p0, __p1, __p2, __p3) \
	LP4(444, int , AssignMenu, \
		struct ConfigStuff *, __p0, a0, \
		int , __p1, d0, \
		char *, __p2, a1, \
		char *, __p3, a2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DoArrow(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7) \
	LP8NR(54, DoArrow, \
		struct RastPort *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		int , __p3, d2, \
		int , __p4, d3, \
		int , __p5, d4, \
		int , __p6, d5, \
		int , __p7, d6, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define AddListView(__p0, __p1) \
	LP2(240, int , AddListView, \
		struct DOpusListView *, __p0, a0, \
		int , __p1, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DisableGadget(__p0, __p1, __p2, __p3) \
	LP4NR(480, DisableGadget, \
		struct Gadget *, __p0, a0, \
		struct RastPort *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Assign(__p0, __p1) \
	LP2(192, int , Assign, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define RefreshListView(__p0, __p1) \
	LP2(252, int , RefreshListView, \
		struct DOpusListView *, __p0, a0, \
		int , __p1, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define RefreshRequesterObject(__p0, __p1) \
	LP2NR(546, RefreshRequesterObject, \
		struct RequesterBase *, __p0, a0, \
		struct RequesterObject *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Do3DStringBox(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	LP7NR(42, Do3DStringBox, \
		struct RastPort *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		int , __p3, d2, \
		int , __p4, d3, \
		int , __p5, d4, \
		int , __p6, d5, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Atoh(__p0, __p1) \
	LP2(396, int , Atoh, \
		char *, __p0, a0, \
		int , __p1, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SetBusyPointer(__p0) \
	LP1NR(300, SetBusyPointer, \
		struct Window *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define OpenRequester(__p0) \
	LP1(528, struct Window *, OpenRequester, \
		struct RequesterBase *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define RefreshStrGad(__p0, __p1) \
	LP2NR(378, RefreshStrGad, \
		struct Gadget *, __p0, a0, \
		struct Window *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ActivateStrGad(__p0, __p1) \
	LP2NR(372, ActivateStrGad, \
		struct Gadget *, __p0, a0, \
		struct Window *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DrawCheckMark(__p0, __p1, __p2, __p3) \
	LP4NR(264, DrawCheckMark, \
		struct RastPort *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		int , __p3, d2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FixSliderBody(__p0, __p1, __p2, __p3, __p4) \
	LP5NR(270, FixSliderBody, \
		struct Window *, __p0, a0, \
		struct Gadget *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		int , __p4, d2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DefaultConfig(__p0) \
	LP1(426, int , DefaultConfig, \
		struct ConfigStuff *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrnCpy(__p0, __p1, __p2) \
	LP3NR(120, LStrnCpy, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DoRMBGadget(__p0, __p1) \
	LP2(360, int , DoRMBGadget, \
		struct RMBGadget *, __p0, a0, \
		struct Window *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define StampToStr(__p0) \
	LP1NR(228, StampToStr, \
		struct DateTime *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define StrToStamp(__p0) \
	LP1(234, int , StrToStamp, \
		struct DateTime *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Do3DCycleBox(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	LP7NR(48, Do3DCycleBox, \
		struct RastPort *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		int , __p3, d2, \
		int , __p4, d3, \
		int , __p5, d4, \
		int , __p6, d5, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LAllocRemember(__p0, __p1, __p2) \
	LP3(288, void *, LAllocRemember, \
		struct DOpusRemember **, __p0, a0, \
		ULONG , __p1, d0, \
		ULONG , __p2, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CheckExist(__p0, __p1) \
	LP2(318, int , CheckExist, \
		char *, __p0, a0, \
		int *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrnCat(__p0, __p1, __p2) \
	LP3NR(108, LStrnCat, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SelectGadget(__p0, __p1) \
	LP2NR(600, SelectGadget, \
		struct Window *, __p0, a0, \
		struct Gadget *, __p1, a1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define PathName(__p0, __p1, __p2) \
	LP3(210, int , PathName, \
		BPTR , __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define RawkeyToStr(__p0, __p1, __p2, __p3, __p4) \
	LP5(354, int , RawkeyToStr, \
		USHORT , __p0, d0, \
		USHORT , __p1, d1, \
		char *, __p2, a0, \
		char *, __p3, a1, \
		int , __p4, d2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define DoCycleGadget(__p0, __p1, __p2, __p3) \
	LP4NR(468, DoCycleGadget, \
		struct Gadget *, __p0, a0, \
		struct Window *, __p1, a1, \
		char **, __p2, a2, \
		int , __p3, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GhostGadget(__p0, __p1, __p2, __p3) \
	LP4NR(492, GhostGadget, \
		struct Gadget *, __p0, a0, \
		struct RastPort *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LStrnCmp(__p0, __p1, __p2) \
	LP3(132, int , LStrnCmp, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GetSliderPos(__p0, __p1, __p2) \
	LP3(282, int , GetSliderPos, \
		struct Gadget *, __p0, a0, \
		int , __p1, d0, \
		int , __p2, d1, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define UScoreText(__p0, __p1, __p2, __p3, __p4) \
	LP5NR(474, UScoreText, \
		struct RastPort *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		int , __p3, d1, \
		int , __p4, d2, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define StrCombine(__p0, __p1, __p2, __p3) \
	LP4(150, int , StrCombine, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		char *, __p2, a2, \
		int , __p3, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FreeConfig(__p0) \
	LP1(462, int , FreeConfig, \
		struct ConfigStuff *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define FreeStringFile(__p0) \
	LP1(576, int , FreeStringFile, \
		struct StringData *, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ObjectText(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	LP7NR(552, ObjectText, \
		struct RequesterBase *, __p0, a0, \
		short , __p1, d0, \
		short , __p2, d1, \
		short , __p3, d2, \
		short , __p4, d3, \
		char *, __p5, a1, \
		unsigned , __p6, d4, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define LFreeRemember(__p0) \
	LP1NR(294, LFreeRemember, \
		struct DOpusRemember **, __p0, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GetButtonImage(__p0, __p1, __p2, __p3, __p4, __p5, __p6) \
	LP7(504, struct Image *, GetButtonImage, \
		int , __p0, d0, \
		int , __p1, d1, \
		int , __p2, d2, \
		int , __p3, d3, \
		int , __p4, d4, \
		int , __p5, d5, \
		struct DOpusRemember **, __p6, a0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SearchPathList(__p0, __p1, __p2) \
	LP3(312, int , SearchPathList, \
		char *, __p0, a0, \
		char *, __p1, a1, \
		int , __p2, d0, \
		, DOPUS_BASE_NAME, 0, 0, 0, 0, 0, 0)

#endif /* !_PPCINLINE_DOPUS_H */
