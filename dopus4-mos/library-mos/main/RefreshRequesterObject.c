/*
 *  $VER: RefreshRequesterObject.c $Revision$ (01-Feb-2006)
 *
 *  This file is part of dopus.
 *
 * $Id$
 *
 * $Log$
 *
 *
 */


#include <exec/exec.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <libraries/dopus.h>
#include <intuition/intuition.h>
#include <dopus/requesters.h>
#include <dopus/config.h>
#include <dopus/stringdata.h>
#include <proto/dopus.h>
#include <stdarg.h>

/****** dopus/main/RefreshRequesterObject ******************************************
*
*   NAME
*      RefreshRequesterObject -- Description
*
*   SYNOPSIS
*      void RefreshRequesterObject(struct RequesterBase * reqbase, 
*          struct RequesterObject * object);
*
*   FUNCTION
*
*   INPUTS
*       reqbase - 
*       object - 
*
*   RESULT
*       This function does not return a result
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

void _DOpus_RefreshRequesterObject(struct RequesterBase *reqbase, struct RequesterObject *object)
{
	USHORT count;
	struct RastPort *rp;

	if(!reqbase || !reqbase->rb_window)
		return;
	if(object)
		count = 1;
	else if((object = reqbase->rb_objects))
		count = 65535;
	else
		return;

	rp = reqbase->rb_window->RPort;

	while (count-- && object)
	{
		switch (object->ro_type)
		{
		case OBJECT_TEXT:
			ObjectText(reqbase, ((Object_Text *) object->ro_data)->ot_left, ((Object_Text *) object->ro_data)->ot_top, -1, -1, ((Object_Text *) object->ro_data)->ot_text, TEXTPOS_ABSOLUTE);
			break;

		case OBJECT_BORDER:
			switch(((Object_Border *) object->ro_data)->ob_type)
			{
			case BORDER_NORMAL:
				Do3DBox(rp, ((Object_Border *) object->ro_data)->ob_left, ((Object_Border *) object->ro_data)->ob_top, ((Object_Border *) object->ro_data)->ob_width, ((Object_Border *) object->ro_data)->ob_height, reqbase->rb_shine, reqbase->rb_shadow);
				break;

			case BORDER_RECESSED:
				Do3DBox(rp, ((Object_Border *) object->ro_data)->ob_left, ((Object_Border *) object->ro_data)->ob_top, ((Object_Border *) object->ro_data)->ob_width, ((Object_Border *) object->ro_data)->ob_height, reqbase->rb_shadow, reqbase->rb_shine);
				break;

			case BORDER_GROUP:
				Do3DFrame(rp, ((Object_Border *) object->ro_data)->ob_left, ((Object_Border *) object->ro_data)->ob_top, ((Object_Border *) object->ro_data)->ob_width, ((Object_Border *) object->ro_data)->ob_height, ((Object_Border *) object->ro_data)->ob_text, reqbase->rb_shine, reqbase->rb_shadow);
				break;

			case BORDER_STRING:
				Do3DStringBox(rp, ((Object_Border *) object->ro_data)->ob_left, ((Object_Border *) object->ro_data)->ob_top, ((Object_Border *) object->ro_data)->ob_width, ((Object_Border *) object->ro_data)->ob_height, reqbase->rb_shine, reqbase->rb_shadow);
				break;
			}
			if((((Object_Border *) object->ro_data)->ob_type != BORDER_GROUP) && ((Object_Border *) object->ro_data)->ob_text)
			{
				ObjectText(reqbase, ((Object_Border *) object->ro_data)->ob_left, ((Object_Border *) object->ro_data)->ob_top, ((Object_Border *) object->ro_data)->ob_width, ((Object_Border *) object->ro_data)->ob_height, ((Object_Border *) object->ro_data)->ob_text, ((Object_Border *) object->ro_data)->ob_textpos);
			}
			break;

		case OBJECT_GADGET:
			if(((Object_Gadget *) object->ro_data)->og_text)
			{
				ObjectText(reqbase, ((Object_Gadget *) object->ro_data)->og_gadget.LeftEdge, ((Object_Gadget *) object->ro_data)->og_gadget.TopEdge, ((Object_Gadget *) object->ro_data)->og_gadget.Width, ((Object_Gadget *) object->ro_data)->og_gadget.Height, ((Object_Gadget *) object->ro_data)->og_text, ((Object_Gadget *) object->ro_data)->og_textpos);
			}
			if(((Object_Gadget *) object->ro_data)->og_type == GADGET_TINYGLASS)
			{
				DoGlassImage(reqbase->rb_window->RPort, &((Object_Gadget *) object->ro_data)->og_gadget, reqbase->rb_shine, reqbase->rb_shadow, 0);
			}
			break;
		}
		object = object->ro_next;
	}
}
