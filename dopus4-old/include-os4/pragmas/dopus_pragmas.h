#ifndef PRAGMAS_DOPUS_PRAGMAS_H
#define PRAGMAS_DOPUS_PRAGMAS_H

#ifndef CLIB_DOPUS_PROTOS_H
#include <clib/dopus_protos.h>
#endif

#pragma  libcall DOpusBase FileRequest          01E 801
#pragma  libcall DOpusBase Do3DBox              024 543210807
#pragma  libcall DOpusBase Do3DStringBox        02A 543210807
#pragma  libcall DOpusBase Do3DCycleBox         030 543210807
#pragma  libcall DOpusBase DoArrow              036 6543210808
#pragma  libcall DOpusBase LStrCat              066 9802
#pragma  libcall DOpusBase LStrnCat             06C 09803
#pragma  libcall DOpusBase LStrCpy              072 9802
#pragma  libcall DOpusBase LStrnCpy             078 09803
#pragma  libcall DOpusBase LStrCmp              07E 9802
#pragma  libcall DOpusBase LStrnCmp             084 09803
#pragma  libcall DOpusBase LStrCmpI             08A 9802
#pragma  libcall DOpusBase LStrnCmpI            090 09803
#pragma  libcall DOpusBase StrCombine           096 0A9804
#pragma  libcall DOpusBase StrConcat            09C 09803
#pragma  libcall DOpusBase LParsePattern        0A2 9802
#pragma  libcall DOpusBase LMatchPattern        0A8 9802
#pragma  libcall DOpusBase LParsePatternI       0AE 9802
#pragma  libcall DOpusBase LMatchPatternI       0B4 9802
#pragma  libcall DOpusBase BtoCStr              0BA 09803
#pragma  libcall DOpusBase Assign               0C0 9802
#pragma  libcall DOpusBase PathName             0D2 09803
#pragma  libcall DOpusBase SendPacket           0D8 190804
#pragma  libcall DOpusBase TackOn               0DE 09803
#pragma  libcall DOpusBase StampToStr           0E4 801
#pragma  libcall DOpusBase StrToStamp           0EA 801
#pragma  libcall DOpusBase AddListView          0F0 0802
#pragma  libcall DOpusBase ListViewIDCMP        0F6 9802
#pragma  libcall DOpusBase RefreshListView      0FC 0802
#pragma  libcall DOpusBase RemoveListView       102 0802
#pragma  libcall DOpusBase DrawCheckMark        108 210804
#pragma  libcall DOpusBase FixSliderBody        10E 2109805
#pragma  libcall DOpusBase FixSliderPot         114 32109806
#pragma  libcall DOpusBase GetSliderPos         11A 10803
#pragma  libcall DOpusBase LAllocRemember       120 10803
#pragma  libcall DOpusBase LFreeRemember        126 801
#pragma  libcall DOpusBase SetBusyPointer       12C 801
#pragma  libcall DOpusBase GetWBScreen          132 801
#pragma  libcall DOpusBase SearchPathList       138 09803
#pragma  libcall DOpusBase CheckExist           13E 9802
#pragma  libcall DOpusBase Seed                 14A 001
#pragma  libcall DOpusBase Random               150 001
#pragma  libcall DOpusBase StrToUpper           156 9802
#pragma  libcall DOpusBase StrToLower           15C 9802
#pragma  libcall DOpusBase RawkeyToStr          162 2981005
#pragma  libcall DOpusBase DoRMBGadget          168 9802
#pragma  libcall DOpusBase AddGadgets           16E 3210A9807
#pragma  libcall DOpusBase ActivateStrGad       174 9802
#pragma  libcall DOpusBase RefreshStrGad        17A 9802
#pragma  libcall DOpusBase CheckNumGad          180 109804
#pragma  libcall DOpusBase CheckHexGad          186 109804
#pragma  libcall DOpusBase Atoh                 18C 0802
#pragma  libcall DOpusBase HiliteGad            192 9802
#pragma  libcall DOpusBase DoSimpleRequest      198 9802
#pragma  libcall DOpusBase ReadConfig           19E 9802
#pragma  libcall DOpusBase SaveConfig           1A4 9802
#pragma  libcall DOpusBase DefaultConfig        1AA 801
#pragma  libcall DOpusBase GetDevices           1B0 801
#pragma  libcall DOpusBase AssignGadget         1B6 A910805
#pragma  libcall DOpusBase AssignMenu           1BC A90804
#pragma  libcall DOpusBase FindSystemFile       1C2 109804
#pragma  libcall DOpusBase Do3DFrame            1C8 5493210808
#pragma  libcall DOpusBase FreeConfig           1CE 801
#pragma  libcall DOpusBase DoCycleGadget        1D4 0A9804
#pragma  libcall DOpusBase UScoreText           1DA 2109805
#pragma  libcall DOpusBase DisableGadget        1E0 109804
#pragma  libcall DOpusBase EnableGadget         1E6 109804
#pragma  libcall DOpusBase GhostGadget          1EC 109804
#pragma  libcall DOpusBase DrawRadioButton      1F2 543210807
#pragma  libcall DOpusBase GetButtonImage       1F8 854321007
#pragma  libcall DOpusBase ShowSlider           1FE 9802
#pragma  libcall DOpusBase CheckConfig          204 801
#pragma  libcall DOpusBase GetCheckImage        20A 821004
#pragma  libcall DOpusBase OpenRequester        210 801
#pragma  libcall DOpusBase CloseRequester       216 801
#pragma  libcall DOpusBase AddRequesterObject   21C 9802
#pragma  libcall DOpusBase RefreshRequesterObject 222 9802
#pragma  libcall DOpusBase ObjectText           228 493210807
#pragma  libcall DOpusBase DoGlassImage         22E 2109805
#pragma  libcall DOpusBase Decode_RLE           234 09803
#pragma  libcall DOpusBase ReadStringFile       23A 9802
#pragma  libcall DOpusBase FreeStringFile       240 801
#pragma  libcall DOpusBase LFreeRemEntry        246 9802
#pragma  libcall DOpusBase AddGadgetBorders     24C 2109805
#pragma  libcall DOpusBase CreateGadgetBorders  252 432A910808
#pragma  libcall DOpusBase SelectGadget         258 9802
#pragma  libcall DOpusBase FSSetMenuStrip       25E 9802

#endif	/*  PRAGMAS_DOPUS_PRAGMA_H  */
