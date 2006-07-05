/*
 *  $VER: CheckConfig.c $Revision$ (01-Feb-2006)
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

/****** dopus/main/CheckConfig ******************************************
*
*   NAME
*      CheckConfig -- Description
*
*   SYNOPSIS
*      int CheckConfig(struct ConfigStuff * cstuff);
*
*   FUNCTION
*
*   INPUTS
*       cstuff - 
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

int _DOpus_CheckConfig(struct ConfigStuff *cstuff)
{
	struct Config *config;

	if(!(config = cstuff->config))
		return (0);

	if(config->sliderheight < 5)
		config->sliderheight = 5;
	else if(config->sliderheight > 49)
		config->sliderheight = 49;
	if(config->sliderwidth < 8)
		config->sliderwidth = 8;
	else if(config->sliderwidth > 108)
		config->sliderwidth = 108;

	return 1;
}

