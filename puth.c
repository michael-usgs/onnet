#include <stdio.h>

puth(w, iop)
register FILE *iop;
{
        register char *p;
        register i;

        p = (char *)&w;
        for (i=sizeof(short); --i>=0;)
                putc(*p++, iop);
        return(ferror(iop));
}
