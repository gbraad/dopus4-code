/*
 *  $VER: AddRequesterObject.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 *  (C) Copyright 2006 Hyperion Entertainment
 *      All rights reserved
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <string.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/AddRequesterObject ******************************************
*
*   NAME
*      AddRequesterObject -- Description
*
*   SYNOPSIS
*      APTR AddRequesterObject(struct RequesterBase * reqbase, 
*          struct TagItem * taglist);
*
*   FUNCTION
*
*   INPUTS
*       reqbase - 
*       taglist - 
*
*   RESULT
*       The result ...
*
*   EXAMPLE
*
*   NOTES
*
*   BUGS
*
*   SEE ALSO
*
*****************************************************************************
*
*/

struct PrivateData
{
	struct Image *check_off_image;
	struct Image *check_on_image;
	struct Image *button_off_image;
	struct Image *button_on_image;
};

APTR _DOpus_AddRequesterObject(struct DOpusIFace *Self, struct RequesterBase *reqbase, struct TagItem *taglist)
{
	struct RequesterObject *object = NULL, *tempobject;
	struct PrivateData *private;
	ULONG data;
	int tag, size, type, xoffset, yoffset;
	struct StringInfo *stringinfo = NULL;
	struct PropInfo *propinfo;
	struct Image *propimage;
	struct Gadget *gadget = NULL;
	char **string;

	if(!taglist || !reqbase || !reqbase->rb_window)
		return (NULL);

	private = (struct PrivateData *)reqbase->rb_privatedata;

	if(reqbase->rb_flags & RBF_BORDERS)
	{
		xoffset = reqbase->rb_window->BorderLeft;
		yoffset = reqbase->rb_window->BorderTop;
	}
	else
	{
		xoffset = 0;
		yoffset = 0;
	}

	for(tag = 0; tag > -1; tag++)
	{
		data = taglist[tag].ti_Data;
		switch (taglist[tag].ti_Tag)
		{
		case TAG_END:
			tag = -2;
			break;

		case TAG_SKIP:
			break;

		case TAG_MORE:
			taglist = (struct TagItem *)data;
			tag = -1;
			break;

			/* Object type */

		case RO_Type:
			if(object)
				break;
			if(!(object = Self->LAllocRemember(&reqbase->rb_memory, sizeof(struct RequesterObject), MEMF_CLEAR)))
				return (NULL);
			switch (object->ro_type = data)
			{
			case OBJECT_TEXT:
				size = sizeof(Object_Text);
				break;
			case OBJECT_BORDER:
				size = sizeof(Object_Border);
				break;
			case OBJECT_GADGET:
				size = sizeof(Object_Gadget);
				break;
			case OBJECT_LISTVIEW:
				size = sizeof(Object_ListView);
				break;
			}
			if(!
			   (object->ro_data = Self->LAllocRemember(&reqbase->rb_memory, size, MEMF_CLEAR)))
				return (NULL);

			switch (object->ro_type)
			{
			case OBJECT_TEXT:
			case OBJECT_BORDER:
				((Object_Text *) object->ro_data)->ot_left = reqbase->rb_leftoffset + xoffset;
				((Object_Text *) object->ro_data)->ot_top = reqbase->rb_topoffset + yoffset;
				break;
			case OBJECT_GADGET:
				((Object_Gadget *) object->ro_data)->og_gadget.LeftEdge = reqbase->rb_leftoffset + xoffset;
				((Object_Gadget *) object->ro_data)->og_gadget.TopEdge = reqbase->rb_topoffset + yoffset;
				break;
			case OBJECT_LISTVIEW:
				((Object_ListView *) object->ro_data)->ol_listview.x = reqbase->rb_leftoffset + xoffset;
				((Object_ListView *) object->ro_data)->ol_listview.y = reqbase->rb_topoffset + yoffset;
				break;
			}

			if((tempobject = reqbase->rb_objects))
				while(tempobject->ro_next)
					tempobject = tempobject->ro_next;
			if(tempobject)
				tempobject->ro_next = object;
			else
				reqbase->rb_objects = object;
			break;

			/* Gadget type (object->ro_Type==OBJECT_GADGET) */

		case RO_GadgetType:
			if(!object || object->ro_type != OBJECT_GADGET)
				break;
			gadget = &((Object_Gadget *) object->ro_data)->og_gadget;
			((Object_Gadget *) object->ro_data)->og_type = data;
			switch (data)
			{
			case GADGET_CYCLE:
				gadget->MutualExclude = GAD_CYCLE;

			case GADGET_TINYGLASS:
			case GADGET_BOOLEAN:
				type = GTYP_BOOLGADGET;
				gadget->Flags = GFLG_GADGHCOMP;
				gadget->Activation = GACT_RELVERIFY;
				break;

			case GADGET_INTEGER:
				gadget->Activation |= GACT_LONGINT;
			case GADGET_STRING:
				type = GTYP_STRGADGET;
				if(!(stringinfo = (struct StringInfo *)Self->LAllocRemember(&reqbase->rb_memory, sizeof(struct StringInfo), MEMF_CLEAR)))
					return (NULL);
				gadget->SpecialInfo = (APTR) stringinfo;
				gadget->Flags = GFLG_GADGHCOMP|GFLG_TABCYCLE;
				if(reqbase->rb_extend)
				{
					gadget->Flags |= GFLG_STRINGEXTEND;
					stringinfo->Extension = reqbase->rb_extend;
				}
				gadget->Activation |= GACT_RELVERIFY | GACT_TOGGLESELECT;
				break;

			case GADGET_PROP:
				type = GTYP_PROPGADGET;
				if(!(propinfo = (struct PropInfo *)Self->LAllocRemember(&reqbase->rb_memory, sizeof(struct PropInfo), MEMF_CLEAR)))
					return (NULL);
				gadget->SpecialInfo = (APTR) propinfo;
				if(!(propimage = (struct Image *)Self->LAllocRemember(&reqbase->rb_memory, sizeof(struct Image), MEMF_CLEAR)))
					return (NULL);
				gadget->GadgetRender = propimage;
				break;

			case GADGET_CHECK:
				type = GTYP_BOOLGADGET;
				gadget->Width = 26;
				gadget->Height = 11;
				gadget->Flags = GFLG_GADGIMAGE|GFLG_GADGHIMAGE;
				gadget->Activation = GACT_RELVERIFY|GACT_TOGGLESELECT;
				if(private)
				{
					if(!private->check_off_image)
						private->check_off_image = Self->GetCheckImage(reqbase->rb_shadow, reqbase->rb_bg, 0, &reqbase->rb_memory);
					if(!private->check_on_image)
						private->check_on_image = Self->GetCheckImage(reqbase->rb_shadow, reqbase->rb_bg, 1, &reqbase->rb_memory);
					gadget->GadgetRender = (APTR)private->check_off_image;
					gadget->SelectRender = (APTR)private->check_on_image;
				}
				gadget->MutualExclude = GAD_CHECK;
				break;

			case GADGET_RADIO:
				type = GTYP_BOOLGADGET;
				gadget->Width = 15;
				gadget->Height = 7;
				gadget->Flags = GFLG_GADGIMAGE|GFLG_GADGHIMAGE;
				gadget->Activation = GACT_IMMEDIATE|GACT_TOGGLESELECT;
				if(private)
				{
					if(!private->button_off_image)
						private->button_off_image = Self->GetButtonImage(15, 9, reqbase->rb_shine, reqbase->rb_shadow, reqbase->rb_bg, reqbase->rb_bg, &reqbase->rb_memory);
					if(!private->button_on_image)
						private->button_on_image = Self->GetButtonImage(15, 9, reqbase->rb_shine, reqbase->rb_shadow, reqbase->rb_fg, reqbase->rb_bg, &reqbase->rb_memory);
					gadget->GadgetRender = (APTR)private->button_off_image;
					gadget->SelectRender = (APTR)private->button_on_image;
				}
				gadget->MutualExclude = GAD_RADIO;
				break;
			}
			gadget->GadgetType = type;
			break;

			/* Gadget ID (object->ro_type==OBJECT_GADGET) */

		case RO_GadgetID:
			if(!object || object->ro_type != OBJECT_GADGET)
				break;
			((Object_Gadget *) object->ro_data)->og_gadget.GadgetID = data;
			break;

			/* Border type (object->ro_type==OBJECT_BORDER) */

		case RO_BorderType:
			if(!object || object->ro_type != OBJECT_BORDER)
				break;
			((Object_Border *) object->ro_data)->ob_type = data;
			break;

			/* ListView ID (object->ro_type==OBJECT_LISTVIEW) */

		case RO_ListViewID:
			if(!object || object->ro_type != OBJECT_LISTVIEW)
				break;
			((Object_ListView *) object->ro_data)->ol_listview.listid = data;
			break;

			/* Left edge positioning */

		case RO_Left:
			data *= reqbase->rb_font->tf_XSize;
		case RO_LeftFine:
			if(!object)
				break;
			switch (object->ro_type)
			{
			case OBJECT_TEXT:
			case OBJECT_BORDER:
				((Object_Text *) object->ro_data)->ot_left += data;
				break;

			case OBJECT_GADGET:
				((Object_Gadget *) object->ro_data)->og_gadget.LeftEdge += data;
				break;

			case OBJECT_LISTVIEW:
				((Object_ListView *) object->ro_data)->ol_listview.x += data;
				break;
			}
			break;

			/* Top edge positioning */

		case RO_Top:
			data *= reqbase->rb_font->tf_YSize;
		case RO_TopFine:
			if(!object)
				break;
			switch (object->ro_type)
			{
			case OBJECT_TEXT:
			case OBJECT_BORDER:
				((Object_Text *) object->ro_data)->ot_top += data;
				break;

			case OBJECT_GADGET:
				((Object_Gadget *) object->ro_data)->og_gadget.TopEdge += data;
				break;

			case OBJECT_LISTVIEW:
				((Object_ListView *) object->ro_data)->ol_listview.y += data;
				break;
			}
			break;

			/* Width adjustment */

		case RO_Width:
			data *= reqbase->rb_font->tf_XSize;
		case RO_WidthFine:
			if(!object)
				break;
			switch (object->ro_type)
			{
			case OBJECT_BORDER:
				((Object_Border *) object->ro_data)->ob_width += data;
				break;

			case OBJECT_GADGET:
				((Object_Gadget *) object->ro_data)->og_gadget.Width += data;
				break;

			case OBJECT_LISTVIEW:
				((Object_ListView *) object->ro_data)->ol_listview.w += data;
				break;
			}
			break;

			/* Height adjustment */

		case RO_Height:
			data *= reqbase->rb_font->tf_YSize;
		case RO_HeightFine:
			if(!object)
				break;
			switch (object->ro_type)
			{
			case OBJECT_BORDER:
				((Object_Border *) object->ro_data)->ob_height += data;
				break;

			case OBJECT_GADGET:
				((Object_Gadget *) object->ro_data)->og_gadget.Height += data;
				break;

			case OBJECT_LISTVIEW:
				((Object_ListView *) object->ro_data)->ol_listview.h += data;
				break;
			}
			break;

			/* Object text */

		case RO_TextNum:
			if(!(reqbase->rb_flags & RBF_STRINGS) || !reqbase->rb_string_table)
				break;
			data = (ULONG) reqbase->rb_string_table[data];

		case RO_Text:
			if(!object || !data)
				break;
			string = NULL;
			switch (object->ro_type)
			{
			case OBJECT_TEXT:
				string = &((Object_Text *) object->ro_data)->ot_text;
				break;

			case OBJECT_GADGET:
				string = &((Object_Gadget *) object->ro_data)->og_text;
				break;

			case OBJECT_BORDER:
				string = &((Object_Border *) object->ro_data)->ob_text;
				break;

			case OBJECT_LISTVIEW:
				string = &((Object_ListView *) object->ro_data)->ol_listview.title;
				break;
			}
			if(string && (*string = Self->LAllocRemember(&reqbase->rb_memory, strlen((char *)data) + 1, 0)))
				Self->LStrCpy(*string, (char *)data);
			break;

			/* Text positioning */

		case RO_TextPos:
			if(!object)
				break;
			switch (object->ro_type)
			{
			case OBJECT_GADGET:
				((Object_Gadget *) object->ro_data)->og_textpos = data;
				break;

			case OBJECT_BORDER:
				((Object_Border *) object->ro_data)->ob_textpos = data;
				break;

			case OBJECT_LISTVIEW:
				if(data == TEXTPOS_ABOVE)
					((Object_ListView *) object->ro_data)->ol_listview.flags |=
		    DLVF_TTOP;
				break;
			}
			break;

			/* String gadget settings (gadgettype==GTYP_STRGADGET) */

		case RO_StringBuf:
			if(!stringinfo)
				break;
			stringinfo->Buffer = (UBYTE *) data;
			break;

		case RO_StringLen:
			if(!stringinfo)
				break;
			stringinfo->MaxChars = data;
			break;

		case RO_StringUndo:
			if(!stringinfo)
				break;
			stringinfo->UndoBuffer = (UBYTE *) data;
			break;

			/* Boolean gadget setting */

		case RO_BoolOn:
			if(data && object && object->ro_type == OBJECT_GADGET)
				((Object_Gadget *) object->ro_data)->og_gadget.Flags |= GFLG_SELECTED;
			break;

			/* Various other tags */

		case RO_ChkCenter:
			if(data && object && (object->ro_type == OBJECT_GADGET) && gadget && ((gadget->MutualExclude == GAD_CHECK) || (gadget->MutualExclude == GAD_RADIO)) && (reqbase->rb_font->tf_YSize > gadget->Height))
				gadget->TopEdge += (reqbase->rb_font->tf_YSize - gadget->Height) / 2;
			break;

		case RO_HighRecess:
			if(data && object)
			{
				if((object->ro_type == OBJECT_GADGET) && gadget && (gadget->GadgetType == GTYP_BOOLGADGET))
				{
					Self->AddGadgetBorders(&reqbase->rb_memory, gadget, 1, reqbase->rb_shine, reqbase->rb_shadow);
				}
				else if(object->ro_type == OBJECT_LISTVIEW)
					((Object_ListView *) object->ro_data)->ol_listview.flags |= DLVF_HIREC;
			}
			break;
		}
	}

	if(stringinfo && !stringinfo->Buffer && (stringinfo->MaxChars > 0))
	{
		if(!(stringinfo->Buffer = Self->LAllocRemember(&reqbase->rb_memory, stringinfo->MaxChars + 1, MEMF_CLEAR)))
			return (NULL);
	}

	return ((object) ? object->ro_data : (APTR) NULL);
}
