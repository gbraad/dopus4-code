#ifndef INLINE4_DOPUS_H
#define INLINE4_DOPUS_H

/*
** This file was auto generated by idltool 51.6.
**
** It provides compatibility to OS3 style library
** calls by substituting functions.
**
** Do not edit manually.
*/ 

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_EXEC_H
#include <exec/exec.h>
#endif
#ifndef EXEC_INTERFACES_H
#include <exec/interfaces.h>
#endif

#ifndef LIBRARIES_DOPUS_H
#include <libraries/dopus.h>
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

/* Inline macros for Interface "main" */
#define FileRequest(freq) IDOpus->FileRequest(freq) 
#define Do3DBox(rp, x, y, w, h, tp, bp) IDOpus->Do3DBox(rp, x, y, w, h, tp, bp) 
#define Do3DStringBox(rp, x, y, w, h, tp, bp) IDOpus->Do3DStringBox(rp, x, y, w, h, tp, bp) 
#define Do3DCycleBox(rp, x, y, w, h, tp, bp) IDOpus->Do3DCycleBox(rp, x, y, w, h, tp, bp) 
#define DoArrow(rp, x, y, w, h, fg, bg, dir) IDOpus->DoArrow(rp, x, y, w, h, fg, bg, dir) 
#define LStrCat(s1, s2) IDOpus->LStrCat(s1, s2) 
#define LStrnCat(s1, s2, len) IDOpus->LStrnCat(s1, s2, len) 
#define LStrCpy(to, from) IDOpus->LStrCpy(to, from) 
#define LStrnCpy(to, from, len) IDOpus->LStrnCpy(to, from, len) 
#define LStrCmp(s1, s2) IDOpus->LStrCmp(s1, s2) 
#define LStrnCmp(s1, s2, len) IDOpus->LStrnCmp(s1, s2, len) 
#define LStrCmpI(s1, s2) IDOpus->LStrCmpI(s1, s2) 
#define LStrnCmpI(s1, s2, len) IDOpus->LStrnCmpI(s1, s2, len) 
#define StrCombine(s1, s2, s3, len) IDOpus->StrCombine(s1, s2, s3, len) 
#define StrConcat(s1, s2, len) IDOpus->StrConcat(s1, s2, len) 
//#define BtoCStr(bstr, cstr, len) IDOpus->BtoCStr(bstr, cstr, len) 
#define Assign(name, dir) IDOpus->Assign(name, dir) 
#define PathName(lock, buffer, len) IDOpus->PathName(lock, buffer, len) 
//#define SendPacket(port, action, args, nargs) IDOpus->SendPacket(port, action, args, nargs) 
#define TackOn(path, name, len) IDOpus->TackOn(path, name, len) 
#define StampToStr(datetime) IDOpus->StampToStr(datetime) 
#define StrToStamp(datetime) IDOpus->StrToStamp(datetime) 
#define AddListView(listview, count) IDOpus->AddListView(listview, count) 
#define ListViewIDCMP(listview, imsg) IDOpus->ListViewIDCMP(listview, imsg) 
#define RefreshListView(listview, count) IDOpus->RefreshListView(listview, count) 
#define RemoveListView(listview, count) IDOpus->RemoveListView(listview, count) 
#define DrawCheckMark(rp, x, y, clear) IDOpus->DrawCheckMark(rp, x, y, clear) 
#define FixSliderBody(window, gadget, count, lines, show) IDOpus->FixSliderBody(window, gadget, count, lines, show) 
#define FixSliderPot(window, gadget, offset, count, lines, show) IDOpus->FixSliderPot(window, gadget, offset, count, lines, show) 
#define GetSliderPos(gadget, count, lines) IDOpus->GetSliderPos(gadget, count, lines) 
#define LAllocRemember(key, size, type) IDOpus->LAllocRemember(key, size, type) 
#define LFreeRemember(key) IDOpus->LFreeRemember(key) 
#define SetBusyPointer(window) IDOpus->SetBusyPointer(window) 
#define GetWBScreen(scrbuf) IDOpus->GetWBScreen(scrbuf) 
#define SearchPathList(name, buffer, size) IDOpus->SearchPathList(name, buffer, size) 
#define CheckExist(name, size) IDOpus->CheckExist(name, size) 
#define StrToUpper(from, to) IDOpus->StrToUpper(from, to) 
#define StrToLower(from, to) IDOpus->StrToLower(from, to) 
#define RawkeyToStr(code, qual, buf, kbuf, len) IDOpus->RawkeyToStr(code, qual, buf, kbuf, len) 
#define DoRMBGadget(rmbgadget, window) IDOpus->DoRMBGadget(rmbgadget, window) 
#define AddGadgets(window, gadget, text, count, fg, bg, add) IDOpus->AddGadgets(window, gadget, text, count, fg, bg, add) 
#define ActivateStrGad(gadget, window) IDOpus->ActivateStrGad(gadget, window) 
#define RefreshStrGad(gadget, window) IDOpus->RefreshStrGad(gadget, window) 
#define CheckNumGad(gadget, window, min, max) IDOpus->CheckNumGad(gadget, window, min, max) 
#define CheckHexGad(gadget, window, min, max) IDOpus->CheckHexGad(gadget, window, min, max) 
#define Atoh(str, len) IDOpus->Atoh(str, len) 
#define HiliteGad(gadget, a1arg) IDOpus->HiliteGad(gadget, a1arg) 
#define DoSimpleRequest(window, simplereq) IDOpus->DoSimpleRequest(window, simplereq) 
#define ReadConfig(name, cstuff) IDOpus->ReadConfig(name, cstuff) 
#define SaveConfig(name, cstuff) IDOpus->SaveConfig(name, cstuff) 
#define DefaultConfig(cstuff) IDOpus->DefaultConfig(cstuff) 
#define GetDevices(cstuff) IDOpus->GetDevices(cstuff) 
#define AssignGadget(cstuff, banknum, gadnum, name, func) IDOpus->AssignGadget(cstuff, banknum, gadnum, name, func) 
#define AssignMenu(cstuff, itemnum, name, func) IDOpus->AssignMenu(cstuff, itemnum, name, func) 
#define FindSystemFile(name, buffer, size, type) IDOpus->FindSystemFile(name, buffer, size, type) 
#define Do3DFrame(a0arg, x, y, w, h, title, tp, bp) IDOpus->Do3DFrame(a0arg, x, y, w, h, title, tp, bp) 
#define FreeConfig(cstuff) IDOpus->FreeConfig(cstuff) 
#define DoCycleGadget(gadget, window, choices, selection) IDOpus->DoCycleGadget(gadget, window, choices, selection) 
#define UScoreText(rp, text, x, y, upos) IDOpus->UScoreText(rp, text, x, y, upos) 
#define DisableGadget(gad, rp, xoff, yoff) IDOpus->DisableGadget(gad, rp, xoff, yoff) 
#define EnableGadget(gad, rp, xoff, yoff) IDOpus->EnableGadget(gad, rp, xoff, yoff) 
#define GhostGadget(gad, rp, xoff, yoff) IDOpus->GhostGadget(gad, rp, xoff, yoff) 
#define DrawRadioButton(rp, x, y, w, h, hi, lo) IDOpus->DrawRadioButton(rp, x, y, w, h, hi, lo) 
#define GetButtonImage(w, h, fg, bg, fpen, bpen, key) IDOpus->GetButtonImage(w, h, fg, bg, fpen, bpen, key) 
#define ShowSlider(window, gadget) IDOpus->ShowSlider(window, gadget) 
#define CheckConfig(cstuff) IDOpus->CheckConfig(cstuff) 
#define GetCheckImage(fg, bg, pen, key) IDOpus->GetCheckImage(fg, bg, pen, key) 
#define OpenRequester(reqbase) IDOpus->OpenRequester(reqbase) 
#define CloseRequester(reqbase) IDOpus->CloseRequester(reqbase) 
#define AddRequesterObject(reqbase, taglist) IDOpus->AddRequesterObject(reqbase, taglist) 
#define RefreshRequesterObject(reqbase, object) IDOpus->RefreshRequesterObject(reqbase, object) 
#define ObjectText(reqbase, left, top, width, height, text, textpos) IDOpus->ObjectText(reqbase, left, top, width, height, text, textpos) 
#define DoGlassImage(rp, gadget, shine, shadow, type) IDOpus->DoGlassImage(rp, gadget, shine, shadow, type) 
#define Decode_RLE(source, dest, size) IDOpus->Decode_RLE(source, dest, size) 
#define ReadStringFile(stringdata, filename) IDOpus->ReadStringFile(stringdata, filename) 
#define FreeStringFile(stringdata) IDOpus->FreeStringFile(stringdata) 
#define LFreeRemEntry(key, pointer) IDOpus->LFreeRemEntry(key, pointer) 
#define AddGadgetBorders(key, gadget, count, shine, shadow) IDOpus->AddGadgetBorders(key, gadget, count, shine, shadow) 
#define CreateGadgetBorders(key, w, h, selborder, unselborder, dogear, shine, shadow) IDOpus->CreateGadgetBorders(key, w, h, selborder, unselborder, dogear, shine, shadow) 
#define SelectGadget(window, gadget) IDOpus->SelectGadget(window, gadget) 
#define FSSetMenuStrip(window, menu) IDOpus->FSSetMenuStrip(window, menu) 

#endif /* INLINE4_DOPUS_H */
