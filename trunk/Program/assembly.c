#include <proto/exec.h>
#include "dopus.h"
#include "view.h"


#ifndef __PPC__
void stuffChar(register char c __asm("d0"), register char ** buf __asm("a3"))
{
    *(*buf) ++ = c;
}

void lsprintf(char *str, char *fmt, ...)
{
    RawDoFmt(fmt,&fmt+1,stuffChar,&str);
}
#endif

int countlines(struct ViewData *vd)
{
    register char *buf;
    register char *lws; // Last whitespace character (for wordwrap)

    register int size, linecount;
    register short int charcount, tabsize;

    register short int max_line_length;

    size = vd->view_file_size;
    lws = NULL;

    max_line_length = vd->view_max_line_length;
    tabsize = vd->view_tab_size;
    buf = vd->view_text_buffer;

    charcount = linecount = 0;

    D(bug("tabsize: %ld\n",tabsize));
    do
    {
        charcount ++;
        if (charcount < max_line_length)
        {
            // skip1

            // Not max length yet
            if (*buf == '\n')
            {
                // EOL
                lws = NULL;
                linecount ++;
                charcount = 0;

            }
            else if (*buf == '\t')
            {
                // skip2

                // TAB

                charcount += tabsize - (charcount % tabsize);
                lws = buf;
            }
        }
        else
        {
            if (lws)
            {
                // We restart from last whitespace

                size += (int)buf - (int)lws;
                buf = lws;
            }

            // notlastspace
            *buf = '\n';
            linecount ++;

            charcount = 0;
            lws = NULL;
        }

        // skip3
        if (*buf == ' ') lws = buf;

        // notspace

        if (--size) buf ++;

    }
    while (size);

    // endcountlines
    if (*buf != '\n') linecount ++;

    // lastlf
D(bug("countlines() = %ld\n",linecount));
    return linecount;
}

int ansicountlines(struct ViewData *viewdata)
{
  register char *a0_buf, *a1_lws;
  int d0_linecount, d1_size, d2_mll, d3_charcount;
  int /*d5_wrap,*/ d7_tabskip;
  int tabsize;
  BOOL last_was_esc;

  tabsize = viewdata->view_tab_size;
  d1_size = viewdata->view_file_size;
  d2_mll = viewdata->view_max_line_length;
  a0_buf = viewdata->view_text_buffer;

  d0_linecount = 0L;
  d3_charcount = 0L;
//  d5_wrap = d2_mll - 10L;
  last_was_esc = FALSE;
  d7_tabskip = tabsize;
  a1_lws = NULL;

  //nloop:
  for(; d1_size; a0_buf++, d1_size--)
   {
    if ((*a0_buf >= ' ') || (*a0_buf < 0) || (*a0_buf == '\t' ) || (*a0_buf == '\n' ) || (*a0_buf == 27 ))
     {
      //  notillegal:
      if( (*a0_buf) == 27 )
       {
        last_was_esc = TRUE;
        continue;
       }
      else
       {
        //  notescape:
        if( last_was_esc )
         {
          if( (*a0_buf != '[' ) && (*a0_buf != ';' ) && ((*a0_buf < 0) || (*a0_buf > '\t')))
            //  endansi:
            last_was_esc = FALSE;
          continue;
         }
        else
         {
          //  notansi:
          if( (*a0_buf) == '\t' )
           {
            d3_charcount += d7_tabskip;
            d7_tabskip = tabsize;
            goto notnexttab;
           }
         }
       }
     }
    else *a0_buf = ' ';

    //nottab:
    d3_charcount ++;
    d7_tabskip --;
    if( d7_tabskip < 0 ) d7_tabskip = tabsize;

notnexttab:
    if( d3_charcount < d2_mll )
     {
      //nskip1:
      if( (*a0_buf) == '\n' )
       {
        a1_lws = NULL;
        d0_linecount ++;
        d3_charcount = 0;
        last_was_esc = FALSE;
        d7_tabskip = tabsize;
       }
     }
    else
     {
      if( a1_lws )
       {
        d1_size += a0_buf - a1_lws;
        a0_buf = a1_lws;
       }
      //nnolastspace:
      (*a0_buf) = '\n';
      d0_linecount ++;
      d3_charcount = 0;
      last_was_esc = FALSE;
      d7_tabskip = tabsize;
      a1_lws = NULL;
     }
    //nskip2:
    if( (*a0_buf == ' ' ) || (*a0_buf == '\t')) a1_lws = a0_buf;

    //nnotspace:
    //  addq.l #1,a0
    //  subq.l #1,d1
    //  cmpi.l #0,d1
    //  bgt nloop
   }
  return(d0_linecount);
}

int smartcountlines(struct ViewData *vd)
{
    register unsigned int size, sizebak;

    register char *buf, *lws = NULL;

    register unsigned int linecount, hexcount;
    register unsigned short int charcount, max_line_length/*, mll_p*/;
    register BOOL last_was_escape;

    size = vd->view_file_size;
    sizebak = size;
    buf = vd->view_text_buffer;
    max_line_length = vd->view_max_line_length;

    linecount = 0;
    charcount = 0;
    hexcount = 0;
//    mll_p = max_line_length - 10;
    last_was_escape = FALSE;

    D(bug("tabsize: %ld\n",vd->view_tab_size));
    do {
        charcount ++;
        if (max_line_length > charcount) {
            BOOL is_hex = TRUE;
            /* askip1 */

            if (*buf < -33) {
                is_hex = FALSE;
            } else if (*buf <= -1) {
                is_hex = TRUE;
            } else if (*buf < '\t') {
                is_hex = TRUE;
            } else if (*buf < 14) {
                is_hex = FALSE;
            } else if (*buf < ' ') {
                is_hex = TRUE;
            } else if (((unsigned char)*buf) <= 127) {
                is_hex = FALSE;
            } else if (((unsigned char)*buf) > 160) {
                is_hex = FALSE;
            }

            if (is_hex)
            {
                /* aishex */

                if (sizebak < 6) return -1;
                hexcount++;
                if (hexcount >= 6) return -1;
                if (*buf == 27) last_was_escape = TRUE;
            }
            else {
                /* aokay */
                hexcount = 0;

                if (last_was_escape)
                {
                    if (*buf == '[') return -2; /* isansi */
                    last_was_escape = FALSE;
                }
            }

            /* aokay1 */
            if (*buf == '\n')
            {
                lws = NULL;
                linecount++;
                charcount = 0;
            }
            else if (*buf == '\t')
            {
                // TAB

                charcount += vd->view_tab_size - (charcount % vd->view_tab_size);
                lws = buf;
            }


        } else  {

            if (lws) {
                size += (int)buf - (int)lws;
                buf = lws;
            }

            /* nolastspace1 */
            *buf = '\n';

            lws = NULL;
            linecount ++;
            charcount = 0;

        }

        /* askip2 */
        if (*buf == ' ')
        {
            lws = buf;
        }

        /* notspace1 */

        if (--size) buf++;

    } while (size);

    if (*buf != '\n') linecount++;

D(bug("smartcountlines() = %ld\n",linecount));
    return linecount;
}

/*
void removetabs(struct ViewData *vd)
{
    int size;
    char *buf;

    size = vd->view_file_size;
    buf = vd->view_text_buffer;


    while (size --) {

        if (9 == *buf) {
            *buf = ' ';
        }
        buf ++;
    }

}
*/



    //;-- referenced from Program/main7.c

                            //XDEF _filteroff
/*
int filteroff(void)             //_filteroff:   ;int
{
  char *filter_register = (char *)0xBFE001L;

  if( (*filter_register) & 2 )            //  btst.b #1,$bfe001
    return(0);              //  bne alreadyon

  (*filter_register) |= 2;            //  bset.b #1,$bfe001
  return(1);                    //  moveq.l #1,d0
                            //  rts
                        //alreadyon:
                            //  moveq.l #0,d0
                        //  rts
}

void filteron(void)                //  XDEF _filteron
{                       //_filteron:    ;void

	  char *filter_register = (char *)0xBFE001L;

  (*filter_register) &= (~2);           //  bclr.b #1,$bfe001

  return;                   //  rts
}
*/
