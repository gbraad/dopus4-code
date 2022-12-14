#ifndef DOPUS_INTERFACE_DEF_H
#define DOPUS_INTERFACE_DEF_H

/*
** This file was machine generated by idltool 51.6.
** Do not edit
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

struct DOpusIFace
{
	struct InterfaceData Data;

	ULONG APICALL (*Obtain)(struct DOpusIFace *Self);
	ULONG APICALL (*Release)(struct DOpusIFace *Self);
	void APICALL (*Expunge)(struct DOpusIFace *Self);
	struct Interface * APICALL (*Clone)(struct DOpusIFace *Self);
	int APICALL (*FileRequest)(struct DOpusIFace *Self, struct DOpusFileReq * freq);
	void APICALL (*Do3DBox)(struct DOpusIFace *Self, struct RastPort * rp, int x, int y, int w, int h, int tp, int bp);
	void APICALL (*Do3DStringBox)(struct DOpusIFace *Self, struct RastPort * rp, int x, int y, int w, int h, int tp, int bp);
	void APICALL (*Do3DCycleBox)(struct DOpusIFace *Self, struct RastPort * rp, int x, int y, int w, int h, int tp, int bp);
	void APICALL (*DoArrow)(struct DOpusIFace *Self, struct RastPort * rp, int x, int y, int w, int h, int fg, int bg, int dir);
	void APICALL (*Reserved1)(struct DOpusIFace *Self);
	void APICALL (*LStrCat)(struct DOpusIFace *Self, char * s1, char * s2);
	void APICALL (*LStrnCat)(struct DOpusIFace *Self, char * s1, char * s2, int len);
	void APICALL (*LStrCpy)(struct DOpusIFace *Self, char * to, char * from);
	void APICALL (*LStrnCpy)(struct DOpusIFace *Self, char * to, char * from, int len);
	int APICALL (*LStrCmp)(struct DOpusIFace *Self, char * s1, char * s2);
	int APICALL (*LStrnCmp)(struct DOpusIFace *Self, char * s1, char * s2, int len);
	int APICALL (*LStrCmpI)(struct DOpusIFace *Self, char * s1, char * s2);
	int APICALL (*LStrnCmpI)(struct DOpusIFace *Self, char * s1, char * s2, int len);
	int APICALL (*StrCombine)(struct DOpusIFace *Self, char * s1, char * s2, char * s3, int len);
	int APICALL (*StrConcat)(struct DOpusIFace *Self, char * s1, char * s2, int len);
	void APICALL (*LParsePattern)(struct DOpusIFace *Self, char * pat, char * parsepat);
	int APICALL (*LMatchPattern)(struct DOpusIFace *Self, char * parsepat, char * str);
	void APICALL (*LParsePatternI)(struct DOpusIFace *Self, char * pat, char * parsepat);
	int APICALL (*LMatchPatternI)(struct DOpusIFace *Self, char * parsepat, char * str);
	void APICALL (*BtoCStr)(struct DOpusIFace *Self, BSTR bstr, char * cstr, int len);
	int APICALL (*Assign)(struct DOpusIFace *Self, char * name, char * dir);
	int APICALL (*PathName)(struct DOpusIFace *Self, BPTR lock, char * buffer, int len);
	int APICALL (*SendPacket)(struct DOpusIFace *Self, struct MsgPort * port, int action, ULONG * args, int nargs);
	int APICALL (*TackOn)(struct DOpusIFace *Self, char * path, char * name, int len);
	void APICALL (*StampToStr)(struct DOpusIFace *Self, struct DateTime * datetime);
	int APICALL (*StrToStamp)(struct DOpusIFace *Self, struct DateTime * datetime);
	int APICALL (*AddListView)(struct DOpusIFace *Self, struct DOpusListView * listview, int count);
	struct DOpusListView * APICALL (*ListViewIDCMP)(struct DOpusIFace *Self, struct DOpusListView * listview, struct IntuiMessage * imsg);
	int APICALL (*RefreshListView)(struct DOpusIFace *Self, struct DOpusListView * listview, int count);
	int APICALL (*RemoveListView)(struct DOpusIFace *Self, struct DOpusListView * listview, int count);
	void APICALL (*DrawCheckMark)(struct DOpusIFace *Self, struct RastPort * rp, int x, int y, int clear);
	void APICALL (*FixSliderBody)(struct DOpusIFace *Self, struct Window * window, struct Gadget * gadget, int count, int lines, int show);
	void APICALL (*FixSliderPot)(struct DOpusIFace *Self, struct Window * window, struct Gadget * gadget, int offset, int count, int lines, int show);
	int APICALL (*GetSliderPos)(struct DOpusIFace *Self, struct Gadget * gadget, int count, int lines);
	void * APICALL (*LAllocRemember)(struct DOpusIFace *Self, struct DOpusRemember ** key, ULONG size, ULONG type);
	void APICALL (*LFreeRemember)(struct DOpusIFace *Self, struct DOpusRemember ** key);
	void APICALL (*SetBusyPointer)(struct DOpusIFace *Self, struct Window * window);
	void APICALL (*GetWBScreen)(struct DOpusIFace *Self, struct Screen * scrbuf);
	int APICALL (*SearchPathList)(struct DOpusIFace *Self, char * name, char * buffer, int size);
	int APICALL (*CheckExist)(struct DOpusIFace *Self, char * name, int * size);
	void APICALL (*Seed)(struct DOpusIFace *Self, int seed);
	int APICALL (*Random)(struct DOpusIFace *Self, int limit);
	void APICALL (*StrToUpper)(struct DOpusIFace *Self, char * from, char * to);
	void APICALL (*StrToLower)(struct DOpusIFace *Self, char * from, char * to);
	int APICALL (*RawkeyToStr)(struct DOpusIFace *Self, USHORT code, USHORT qual, char * buf, char * kbuf, int len);
	int APICALL (*DoRMBGadget)(struct DOpusIFace *Self, struct RMBGadget * rmbgadget, struct Window * window);
	int APICALL (*AddGadgets)(struct DOpusIFace *Self, struct Window * window, struct Gadget * gadget, char ** text, int count, int fg, int bg, int add);
	void APICALL (*ActivateStrGad)(struct DOpusIFace *Self, struct Gadget * gadget, struct Window * window);
	void APICALL (*RefreshStrGad)(struct DOpusIFace *Self, struct Gadget * gadget, struct Window * window);
	int APICALL (*CheckNumGad)(struct DOpusIFace *Self, struct Gadget * gadget, struct Window * window, int min, int max);
	int APICALL (*CheckHexGad)(struct DOpusIFace *Self, struct Gadget * gadget, struct Window * window, int min, int max);
	int APICALL (*Atoh)(struct DOpusIFace *Self, char * str, int len);
	void APICALL (*HiliteGad)(struct DOpusIFace *Self, struct Gadget * gadget, struct RastPort * a1arg);
	int APICALL (*DoSimpleRequest)(struct DOpusIFace *Self, struct Window * window, struct DOpusSimpleRequest * simplereq);
	int APICALL (*ReadConfig)(struct DOpusIFace *Self, char * name, struct ConfigStuff * cstuff);
	int APICALL (*SaveConfig)(struct DOpusIFace *Self, char * name, struct ConfigStuff * cstuff);
	int APICALL (*DefaultConfig)(struct DOpusIFace *Self, struct ConfigStuff * cstuff);
	int APICALL (*GetDevices)(struct DOpusIFace *Self, struct ConfigStuff * cstuff);
	int APICALL (*AssignGadget)(struct DOpusIFace *Self, struct ConfigStuff * cstuff, int banknum, int gadnum, char * name, char * func);
	int APICALL (*AssignMenu)(struct DOpusIFace *Self, struct ConfigStuff * cstuff, int itemnum, char * name, char * func);
	int APICALL (*FindSystemFile)(struct DOpusIFace *Self, char * name, char * buffer, int size, int type);
	void APICALL (*Do3DFrame)(struct DOpusIFace *Self, struct RastPort * a0arg, int x, int y, int w, int h, char * title, int tp, int bp);
	int APICALL (*FreeConfig)(struct DOpusIFace *Self, struct ConfigStuff * cstuff);
	void APICALL (*DoCycleGadget)(struct DOpusIFace *Self, struct Gadget * gadget, struct Window * window, char ** choices, int selection);
	void APICALL (*UScoreText)(struct DOpusIFace *Self, struct RastPort * rp, char * text, int x, int y, int upos);
	void APICALL (*DisableGadget)(struct DOpusIFace *Self, struct Gadget * gad, struct RastPort * rp, int xoff, int yoff);
	void APICALL (*EnableGadget)(struct DOpusIFace *Self, struct Gadget * gad, struct RastPort * rp, int xoff, int yoff);
	void APICALL (*GhostGadget)(struct DOpusIFace *Self, struct Gadget * gad, struct RastPort * rp, int xoff, int yoff);
	void APICALL (*DrawRadioButton)(struct DOpusIFace *Self, struct RastPort * rp, int x, int y, int w, int h, int hi, int lo);
	struct Image * APICALL (*GetButtonImage)(struct DOpusIFace *Self, int w, int h, int fg, int bg, int fpen, int bpen, struct DOpusRemember ** key);
	void APICALL (*ShowSlider)(struct DOpusIFace *Self, struct Window * window, struct Gadget * gadget);
	int APICALL (*CheckConfig)(struct DOpusIFace *Self, struct ConfigStuff * cstuff);
	struct Image * APICALL (*GetCheckImage)(struct DOpusIFace *Self, UBYTE fg, UBYTE bg, int pen, struct DOpusRemember ** key);
	struct Window * APICALL (*OpenRequester)(struct DOpusIFace *Self, struct RequesterBase * reqbase);
	void APICALL (*CloseRequester)(struct DOpusIFace *Self, struct RequesterBase * reqbase);
	APTR APICALL (*AddRequesterObject)(struct DOpusIFace *Self, struct RequesterBase * reqbase, struct TagItem * taglist);
	void APICALL (*RefreshRequesterObject)(struct DOpusIFace *Self, struct RequesterBase * reqbase, struct RequesterObject * object);
	void APICALL (*ObjectText)(struct DOpusIFace *Self, struct RequesterBase * reqbase, short left, short top, short width, short height, char * text, unsigned short textpos);
	void APICALL (*DoGlassImage)(struct DOpusIFace *Self, struct RastPort * rp, struct Gadget * gadget, int shine, int shadow, int type);
	void APICALL (*Decode_RLE)(struct DOpusIFace *Self, char * source, char * dest, int size);
	int APICALL (*ReadStringFile)(struct DOpusIFace *Self, struct StringData * stringdata, char * filename);
	int APICALL (*FreeStringFile)(struct DOpusIFace *Self, struct StringData * stringdata);
	void APICALL (*LFreeRemEntry)(struct DOpusIFace *Self, struct DOpusRemember ** key, char * pointer);
	void APICALL (*AddGadgetBorders)(struct DOpusIFace *Self, struct DOpusRemember ** key, struct Gadget * gadget, int count, int shine, int shadow);
	void APICALL (*CreateGadgetBorders)(struct DOpusIFace *Self, struct DOpusRemember ** key, int w, int h, struct Border ** selborder, struct Border ** unselborder, int dogear, int shine, int shadow);
	void APICALL (*SelectGadget)(struct DOpusIFace *Self, struct Window * window, struct Gadget * gadget);
	int APICALL (*FSSetMenuStrip)(struct DOpusIFace *Self, struct Window * window, struct Menu * menu);
};

#endif /* DOPUS_INTERFACE_DEF_H */
