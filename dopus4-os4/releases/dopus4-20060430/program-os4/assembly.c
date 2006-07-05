#include <proto/exec.h>
#include "dopus.h"
#include "view.h"

int countlines(struct ViewData *vd)
{
	char *buf, *lws;	// Last whitespace character (for wordwrap)

	int size, linecount;
	short int charcount, tabsize;

	short int max_line_length;

	size = vd->view_file_size;
	lws = NULL;

	max_line_length = vd->view_max_line_length;
	tabsize = vd->view_tab_size;
	buf = vd->view_text_buffer;

	charcount = linecount = 0;

	do
	{
		charcount++;
		if(charcount < max_line_length)
		{
			if(*buf == '\n')
			{
				lws = NULL;
				linecount++;
				charcount = 0;

			}
			else if(*buf == '\t')
			{
				charcount += tabsize - (charcount % tabsize);
				lws = buf;
			}
		}
		else
		{
			if(lws)
			{
				size += (int)buf - (int)lws;
				buf = lws;
			}

			*buf = '\n';
			linecount++;

			charcount = 0;
			lws = NULL;
		}

		if(*buf == ' ')
			lws = buf;

		if(--size)
			buf++;

	}
	while (size);

	if(*buf != '\n')
		linecount++;

	return linecount;
}

int ansicountlines(struct ViewData *viewdata)
{
	int8 *a0_buf, *a1_lws;
	int d0_linecount, d1_size, d2_mll, d3_charcount;
	int d7_tabskip;
	int tabsize;
	BOOL last_was_esc;

	tabsize = viewdata->view_tab_size;
	d1_size = viewdata->view_file_size;
	d2_mll = viewdata->view_max_line_length;
	a0_buf = (BYTE *)viewdata->view_text_buffer;

	d0_linecount = 0L;
	d3_charcount = 0L;
	last_was_esc = FALSE;
	d7_tabskip = tabsize;
	a1_lws = NULL;

	for(; d1_size; a0_buf++, d1_size--)
	{
		if((*a0_buf >= ' ') || (*a0_buf < 0) || (*a0_buf == '\t') || (*a0_buf == '\n') || (*a0_buf == 27))
		{
			if((*a0_buf) == 27)
			{
				last_was_esc = TRUE;
				continue;
			}
			else
			{
				if(last_was_esc)
				{
					if((*a0_buf != '[') && (*a0_buf != ';') && ((*a0_buf < 0) || (*a0_buf > '\t')))
						last_was_esc = FALSE;
					continue;
				}
				else
				{
					if((*a0_buf) == '\t')
					{
						d3_charcount += d7_tabskip;
						d7_tabskip = tabsize;
						goto notnexttab;
					}
				}
			}
		}
		else
			*a0_buf = ' ';

		d3_charcount++;
		d7_tabskip--;
		if(d7_tabskip < 0)
			d7_tabskip = tabsize;

	      notnexttab:
		if(d3_charcount < d2_mll)
		{
			if((*a0_buf) == '\n')
			{
				a1_lws = NULL;
				d0_linecount++;
				d3_charcount = 0;
				last_was_esc = FALSE;
				d7_tabskip = tabsize;
			}
		}
		else
		{
			if(a1_lws)
			{
				d1_size += a0_buf - a1_lws;
				a0_buf = a1_lws;
			}
			(*a0_buf) = '\n';
			d0_linecount++;
			d3_charcount = 0;
			last_was_esc = FALSE;
			d7_tabskip = tabsize;
			a1_lws = NULL;
		}
		if((*a0_buf == ' ') || (*a0_buf == '\t'))
			a1_lws = a0_buf;
	}
	return (d0_linecount);
}

int smartcountlines(struct ViewData *vd)
{
	STRPTR buf, lws;
	ULONG size;
	ULONG linecount, hexcount;
	UWORD charcount;

	size = vd->view_file_size;
	buf = vd->view_text_buffer;

	lws = NULL;
	linecount = 0;
	charcount = 0;
	hexcount = 0;

	do
	{
		charcount++;
		if(vd->view_max_line_length > charcount)
		{
			BOOL is_hex;

			if((*buf & 0x7F) >= 0x20)
				is_hex = FALSE;
			else
				switch (*buf)
				{
				case 9:	// TAB
					charcount += vd->view_tab_size - (charcount % vd->view_tab_size);
					lws = buf;
					is_hex = FALSE;
					break;
				case 10:	// LF
					lws = NULL;
					linecount++;
					charcount = 0;
					is_hex = FALSE;
					break;
				case 12:	// FF
				case 13:	// CR
					is_hex = FALSE;
					break;
				default:
					is_hex = TRUE;
					break;
				}
			if(is_hex)
			{
				hexcount++;
				if(hexcount >= 6)
					return -1;	/* hex */
				else if(vd->view_file_size < 6)
					return -1;	/* hex */

				if(*buf == 0x1B)	/* ESC */
				{
					if(buf[1] == '[')
						return -2;	/* ansi */
				}
				else if(*buf == 0x9B)	/* CSI */
				{
					if((buf[1] >= 0x20) && (buf[1] <= 0x7F))
						return -2;	/* ansi */
				}
			}
			else
				hexcount = 0;
		}
		else
		{
			if(lws)
			{
				size += (int)buf - (int)lws;
				buf = lws;
			}

			*buf = '\n';

			lws = NULL;
			linecount++;
			charcount = 0;
		}

		if(*buf == ' ')
			lws = buf;

		if(--size)
			buf++;

	}
	while (size);

	if(*buf != '\n')
		linecount++;

	return linecount;
}
