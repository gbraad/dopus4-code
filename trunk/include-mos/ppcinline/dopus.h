/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_DOPUS_H
#define _PPCINLINE_DOPUS_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef DOPUS_BASE_NAME
#define DOPUS_BASE_NAME DOpusBase
#endif /* !DOPUS_BASE_NAME */

#define ActivateStrGad(gadget, window) \
	LP2NR(0x174, ActivateStrGad, struct Gadget *, gadget, a0, struct Window *, window, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define AddGadgetBorders(key, gadget, count, shine, shadow) \
	LP5NR(0x24c, AddGadgetBorders, struct DOpusRemember **, key, a0, struct Gadget *, gadget, a1, int, count, d0, int, shine, d1, int, shadow, d2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define AddGadgets(window, gadget, text, count, fg, bg, add) \
	LP7(0x16e, int, AddGadgets, struct Window *, window, a0, struct Gadget *, gadget, a1, char **, text, a2, int, count, d0, int, fg, d1, int, bg, d2, int, add, d3, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define AddListView(listview, count) \
	LP2(0xf0, int, AddListView, struct DOpusListView *, listview, a0, int, count, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define AddRequesterObject(reqbase, taglist) \
	LP2(0x21c, APTR, AddRequesterObject, struct RequesterBase *, reqbase, a0, struct TagItem *, taglist, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#ifndef NO_PPCINLINE_STDARG
#define AddRequesterObjectTags(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; AddRequesterObject((a0), (struct TagItem *)_tags);})
#endif /* !NO_PPCINLINE_STDARG */

#define Assign(name, dir) \
	LP2(0xc0, int, Assign, char *, name, a0, char *, dir, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define AssignGadget(cstuff, banknum, gadnum, name, func) \
	LP5(0x1b6, int, AssignGadget, struct ConfigStuff *, cstuff, a0, int, banknum, d0, int, gadnum, d1, char *, name, a1, char *, func, a2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define AssignMenu(cstuff, itemnum, name, func) \
	LP4(0x1bc, int, AssignMenu, struct ConfigStuff *, cstuff, a0, int, itemnum, d0, char *, name, a1, char *, func, a2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Atoh(str, len) \
	LP2(0x18c, int, Atoh, char *, str, a0, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define BaseName(path) \
	LP1(0xc6, char *, BaseName, char *, path, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define BtoCStr(bstr, cstr, len) \
	LP3NR(0xba, BtoCStr, BSTR, bstr, a0, char *, cstr, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CheckConfig(cstuff) \
	LP1(0x204, int, CheckConfig, struct ConfigStuff *, cstuff, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CheckExist(name, size) \
	LP2(0x13e, int, CheckExist, char *, name, a0, int *, size, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CheckHexGad(gadget, window, min, max) \
	LP4(0x186, int, CheckHexGad, struct Gadget *, gadget, a0, struct Window *, window, a1, int, min, d0, int, max, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CheckNumGad(gadget, window, min, max) \
	LP4(0x180, int, CheckNumGad, struct Gadget *, gadget, a0, struct Window *, window, a1, int, min, d0, int, max, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CloseRequester(reqbase) \
	LP1NR(0x216, CloseRequester, struct RequesterBase *, reqbase, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CompareDate(date1, date2) \
	LP2(0x144, int, CompareDate, struct DateStamp *, date1, a0, struct DateStamp *, date2, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CompareLock(lock1, lock2) \
	LP2(0xcc, int, CompareLock, BPTR, lock1, a0, BPTR, lock2, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define CreateGadgetBorders(key, w, h, selborder, unselborder, dogear, shine, shadow) \
	LP8NR(0x252, CreateGadgetBorders, struct DOpusRemember **, key, a0, int, w, d0, int, h, d1, struct Border **, selborder, a1, struct Border **, unselborder, a2, int, dogear, d2, int, shine, d3, int, shadow, d4, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Decode_RLE(source, dest, size) \
	LP3NR(0x234, Decode_RLE, char *, source, a0, char *, dest, a1, int, size, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DefaultConfig(cstuff) \
	LP1(0x1aa, int, DefaultConfig, struct ConfigStuff *, cstuff, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DisableGadget(gad, rp, xoff, yoff) \
	LP4NR(0x1e0, DisableGadget, struct Gadget *, gad, a0, struct RastPort *, rp, a1, int, xoff, d0, int, yoff, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Do3DBox(rp, x, y, w, h, tp, bp) \
	LP7NR(0x24, Do3DBox, struct RastPort *, rp, a0, int, x, d0, int, y, d1, int, w, d2, int, h, d3, int, tp, d4, int, bp, d5, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Do3DCycleBox(rp, x, y, w, h, tp, bp) \
	LP7NR(0x30, Do3DCycleBox, struct RastPort *, rp, a0, int, x, d0, int, y, d1, int, w, d2, int, h, d3, int, tp, d4, int, bp, d5, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Do3DFrame(rastport, x, y, w, h, title, tp, bp) \
	LP8NR(0x1c8, Do3DFrame, struct RastPort *, rastport, a0, int, x, d0, int, y, d1, int, w, d2, int, h, d3, char *, title, a1, int, tp, d4, int, bp, d5, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Do3DStringBox(rp, x, y, w, h, tp, bp) \
	LP7NR(0x2a, Do3DStringBox, struct RastPort *, rp, a0, int, x, d0, int, y, d1, int, w, d2, int, h, d3, int, tp, d4, int, bp, d5, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DoArrow(rp, x, y, w, h, fg, bg, dir) \
	LP8NR(0x36, DoArrow, struct RastPort *, rp, a0, int, x, d0, int, y, d1, int, w, d2, int, h, d3, int, fg, d4, int, bg, d5, int, dir, d6, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DoCycleGadget(gadget, window, choices, selection) \
	LP4NR(0x1d4, DoCycleGadget, struct Gadget *, gadget, a0, struct Window *, window, a1, char **, choices, a2, int, selection, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DoGlassImage(rp, gadget, shine, shadow, type) \
	LP5NR(0x22e, DoGlassImage, struct RastPort *, rp, a0, struct Gadget *, gadget, a1, int, shine, d0, int, shadow, d1, int, type, d2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DoRMBGadget(rmbgadget, window) \
	LP2(0x168, int, DoRMBGadget, struct RMBGadget *, rmbgadget, a0, struct Window *, window, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DoSimpleRequest(window, simplereq) \
	LP2(0x198, int, DoSimpleRequest, struct Window *, window, a0, struct DOpusSimpleRequest *, simplereq, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DrawCheckMark(rp, x, y, clear) \
	LP4NR(0x108, DrawCheckMark, struct RastPort *, rp, a0, int, x, d0, int, y, d1, int, clear, d2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define DrawRadioButton(rp, x, y, w, h, hi, lo) \
	LP7NR(0x1f2, DrawRadioButton, struct RastPort *, rp, a0, int, x, d0, int, y, d1, int, w, d2, int, h, d3, int, hi, d4, int, lo, d5, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define EnableGadget(gad, rp, xoff, yoff) \
	LP4NR(0x1e6, EnableGadget, struct Gadget *, gad, a0, struct RastPort *, rp, a1, int, xoff, d0, int, yoff, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FSSetMenuStrip(window, menu) \
	LP2(0x25e, int, FSSetMenuStrip, struct Window *, window, a0, struct Menu *, menu, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FileRequest(freq) \
	LP1(0x1e, int, FileRequest, struct DOpusFileReq *, freq, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FindSystemFile(name, buffer, size, type) \
	LP4(0x1c2, int, FindSystemFile, char *, name, a0, char *, buffer, a1, int, size, d0, int, type, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FixSliderBody(window, gadget, count, lines, show) \
	LP5NR(0x10e, FixSliderBody, struct Window *, window, a0, struct Gadget *, gadget, a1, int, count, d0, int, lines, d1, int, show, d2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FixSliderPot(window, gadget, offset, count, lines, show) \
	LP6NR(0x114, FixSliderPot, struct Window *, window, a0, struct Gadget *, gadget, a1, int, offset, d0, int, count, d1, int, lines, d2, int, show, d3, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FreeConfig(cstuff) \
	LP1(0x1ce, int, FreeConfig, struct ConfigStuff *, cstuff, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define FreeStringFile(stringdata) \
	LP1(0x240, int, FreeStringFile, struct StringData *, stringdata, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define GetButtonImage(w, h, fg, bg, fpen, bpen, key) \
	LP7(0x1f8, struct Image *, GetButtonImage, int, w, d0, int, h, d1, int, fg, d2, int, bg, d3, int, fpen, d4, int, bpen, d5, struct DOpusRemember **, key, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define GetCheckImage(fg, bg, pen, key) \
	LP4(0x20a, struct Image *, GetCheckImage, int, fg, d0, int, bg, d1, int, pen, d2, struct DOpusRemember **, key, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define GetDevices(cstuff) \
	LP1(0x1b0, int, GetDevices, struct ConfigStuff *, cstuff, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define GetSliderPos(gadget, count, lines) \
	LP3(0x11a, int, GetSliderPos, struct Gadget *, gadget, a0, int, count, d0, int, lines, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define GetWBScreen(scrbuf) \
	LP1NR(0x132, GetWBScreen, struct Screen *, scrbuf, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define GhostGadget(gad, rp, xoff, yoff) \
	LP4NR(0x1ec, GhostGadget, struct Gadget *, gad, a0, struct RastPort *, rp, a1, int, xoff, d0, int, yoff, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define HiliteGad(gadget, rastport) \
	LP2NR(0x192, HiliteGad, struct Gadget *, gadget, a0, struct RastPort *, rastport, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LAllocRemember(key, size, type) \
	LP3(0x120, void *, LAllocRemember, struct DOpusRemember **, key, a0, ULONG, size, d0, ULONG, type, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LCreateExtIO(port, size) \
	LP2(0x42, struct IORequest *, LCreateExtIO, struct MsgPort *, port, a0, int, size, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LCreatePort(name, pri) \
	LP2(0x48, struct MsgPort *, LCreatePort, char *, name, a0, int, pri, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LDeleteExtIO(ioext) \
	LP1NR(0x4e, LDeleteExtIO, struct IORequest *, ioext, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LDeletePort(port) \
	LP1NR(0x54, LDeletePort, struct MsgPort *, port, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LFreeRemEntry(key, pointer) \
	LP2NR(0x246, LFreeRemEntry, struct DOpusRemember **, key, a0, char *, pointer, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LFreeRemember(key) \
	LP1NR(0x126, LFreeRemember, struct DOpusRemember **, key, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LMatchPattern(parsepat, str) \
	LP2(0xa8, int, LMatchPattern, char *, parsepat, a0, char *, str, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LMatchPatternI(parsepat, str) \
	LP2(0xb4, int, LMatchPatternI, char *, parsepat, a0, char *, str, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LParsePattern(pat, parsepat) \
	LP2NR(0xa2, LParsePattern, char *, pat, a0, char *, parsepat, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LParsePatternI(pat, parsepat) \
	LP2NR(0xae, LParsePatternI, char *, pat, a0, char *, parsepat, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrCat(s1, s2) \
	LP2NR(0x66, LStrCat, char *, s1, a0, char *, s2, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrCmp(s1, s2) \
	LP2(0x7e, int, LStrCmp, char *, s1, a0, char *, s2, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrCmpI(s1, s2) \
	LP2(0x8a, int, LStrCmpI, char *, s1, a0, char *, s2, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrCpy(to, from) \
	LP2NR(0x72, LStrCpy, char *, to, a0, char *, from, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrnCat(s1, s2, len) \
	LP3NR(0x6c, LStrnCat, char *, s1, a0, char *, s2, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrnCmp(s1, s2, len) \
	LP3(0x84, int, LStrnCmp, char *, s1, a0, char *, s2, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrnCmpI(s1, s2, len) \
	LP3(0x90, int, LStrnCmpI, char *, s1, a0, char *, s2, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LStrnCpy(to, from, len) \
	LP3NR(0x78, LStrnCpy, char *, to, a0, char *, from, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LToLower(ch) \
	LP1(0x60, unsigned char, LToLower, char, ch, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define LToUpper(ch) \
	LP1(0x5a, unsigned char, LToUpper, char, ch, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define ListViewIDCMP(listview, imsg) \
	LP2(0xf6, struct DOpusListView *, ListViewIDCMP, struct DOpusListView *, listview, a0, struct IntuiMessage *, imsg, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define ObjectText(reqbase, left, top, width, height, text, textpos) \
	LP7NR(0x228, ObjectText, struct RequesterBase *, reqbase, a0, short, left, d0, short, top, d1, short, width, d2, short, height, d3, char *, text, a1, unsigned short, textpos, d4, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define OpenRequester(reqbase) \
	LP1(0x210, struct Window *, OpenRequester, struct RequesterBase *, reqbase, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define PathName(lock, buffer, len) \
	LP3(0xd2, int, PathName, BPTR, lock, a0, char *, buffer, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Random(limit) \
	LP1(0x150, int, Random, int, limit, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define RawkeyToStr(code, qual, buf, kbuf, len) \
	LP5(0x162, int, RawkeyToStr, USHORT, code, d0, USHORT, qual, d1, char *, buf, a0, char *, kbuf, a1, int, len, d2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define ReadConfig(name, cstuff) \
	LP2(0x19e, int, ReadConfig, char *, name, a0, struct ConfigStuff *, cstuff, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define ReadStringFile(stringdata, filename) \
	LP2(0x23a, int, ReadStringFile, struct StringData *, stringdata, a0, char *, filename, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define RefreshListView(listview, count) \
	LP2(0xfc, int, RefreshListView, struct DOpusListView *, listview, a0, int, count, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define RefreshRequesterObject(reqbase, object) \
	LP2NR(0x222, RefreshRequesterObject, struct RequesterBase *, reqbase, a0, struct RequesterObject *, object, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define RefreshStrGad(gadget, window) \
	LP2NR(0x17a, RefreshStrGad, struct Gadget *, gadget, a0, struct Window *, window, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define RemoveListView(listview, count) \
	LP2(0x102, int, RemoveListView, struct DOpusListView *, listview, a0, int, count, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define SaveConfig(name, cstuff) \
	LP2(0x1a4, int, SaveConfig, char *, name, a0, struct ConfigStuff *, cstuff, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define SearchPathList(name, buffer, size) \
	LP3(0x138, int, SearchPathList, char *, name, a0, char *, buffer, a1, int, size, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define Seed(seed) \
	LP1NR(0x14a, Seed, int, seed, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define SelectGadget(window, gadget) \
	LP2NR(0x258, SelectGadget, struct Window *, window, a0, struct Gadget *, gadget, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define SendPacket(port, action, args, nargs) \
	LP4(0xd8, int, SendPacket, struct MsgPort *, port, a0, int, action, d0, ULONG *, args, a1, int, nargs, d1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define SetBusyPointer(window) \
	LP1NR(0x12c, SetBusyPointer, struct Window *, window, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define ShowSlider(window, gadget) \
	LP2NR(0x1fe, ShowSlider, struct Window *, window, a0, struct Gadget *, gadget, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define StampToStr(datetime) \
	LP1NR(0xe4, StampToStr, struct DOpusDateTime *, datetime, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define StrCombine(s1, s2, s3, len) \
	LP4(0x96, int, StrCombine, char *, s1, a0, char *, s2, a1, char *, s3, a2, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define StrConcat(s1, s2, len) \
	LP3(0x9c, int, StrConcat, char *, s1, a0, char *, s2, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define StrToLower(from, to) \
	LP2NR(0x15c, StrToLower, char *, from, a0, char *, to, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define StrToStamp(datetime) \
	LP1(0xea, int, StrToStamp, struct DOpusDateTime *, datetime, a0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define StrToUpper(from, to) \
	LP2NR(0x156, StrToUpper, char *, from, a0, char *, to, a1, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define TackOn(path, name, len) \
	LP3(0xde, int, TackOn, char *, path, a0, char *, name, a1, int, len, d0, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#define UScoreText(rp, text, x, y, upos) \
	LP5NR(0x1da, UScoreText, struct RastPort *, rp, a0, char *, text, a1, int, x, d0, int, y, d1, int, upos, d2, \
	, DOPUS_BASE_NAME, IF_CACHEFLUSHALL, NULL, 0, IF_CACHEFLUSHALL, NULL, 0)

#endif /* !_PPCINLINE_DOPUS_H */
