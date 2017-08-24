#include    <stdio.h>
#define MIXED    union { char *s; int *i; float *f;}
#define MAXLINE    256
#define MAXNAMES 20
#define MAXLENGTH 30
static int INPAR;

getpar_(names,type,ptr)
char *names, *type;
MIXED ptr;
{
   extern int xargc; 
   extern char **xargv;
   int ac, found, numnames;
   char name_list[MAXNAMES][MAXLENGTH];
   char **av, line[MAXLINE], fname[64], *progname;
   register char *pl, t1, t2;
   FILE *file, *fopen();
   progname= *xargv;
   if( (ac= xargc)==0 || (av= xargv)== NULL) {
      fprintf(stderr,"*****\n");
      fprintf(stderr,"getpar: xargc or xargv not initialized\n");
      exit(-1);
   }
   found=0;
   numnames = break_names(names,name_list);
   while(--ac>0) {
      av++;
      if(getm_same(name_list,numnames,*av))
      {
         getp_getval(*av,ptr,*type);
         found++;
         continue;
      }
      if(getp_same("par",*av)) {
         INPAR = 1;
         getp_getval(*av,fname,'s');
         if((file=fopen(fname,"r"))==NULL) {
            fprintf(stderr,"*****\n");
            fprintf(stderr, "%s: getpar: cannot open parameter file %s\n", progname,fname);
            exit(-1);
         }
         while( fgets(line,MAXLINE,file) != NULL ) {
            pl= line;
            if(*pl=='#') continue; /* comment line */
            /* loop over entries on each line */
loop:    
            if(*pl=='\0'|| *pl=='\n') continue;
            while(*pl==' ' || *pl=='\t') pl++;
            if(*pl=='\0'|| *pl=='\n') continue;
            if(getm_same(name_list,numnames,pl)) {
               getp_getval(pl,ptr,*type);
               found++;
            }
            if(getp_same("par",pl)) {
               fprintf(stderr,"*****\n");
               fprintf(stderr,"%s: getpar: cannot handle the recurrsive par file in %s\n",progname,fname);
               exit(-1);
            }
            while(*pl!='=' || *pl=='\0') pl++;
            pl++;
            if(*pl=='"' || *pl=='\'') { 
               t1= t2= *pl++; 
            }
            else { 
               t1= ' '; 
               t2= '\t'; 
            }
            while(*pl!=t1 && *pl!=t2 && *pl!='\n' && *pl!='\0') pl++;
            if(*pl=='"' || *pl=='\'') pl++;
            goto loop;
         }
         fclose(file);
         INPAR = 0;
      }
   }
   return(found);
}

getp_same(s1,s2)
register char *s1, *s2;
{
   while(*s1) if(*s1++ != *s2++) return(0);
   if(*s2=='=' || *s2=='[' || *s2=='(') return(1);
   return(0);
}

getm_same(name_list,num, s1)
char (*name_list)[MAXNAMES], *s1;
int num;
{
   int ii;
   char *s2;
   s2 = s1;
   ii = 0;
   while(getp_same(name_list[ii], s2) == 0 && ++ii <= num) s2 = s1;
   if (ii == num+1) return(0);
   return(1);
}

getp_getval(str,ptr,type)
register char *str, type;
MIXED ptr;
{
   register char *sptr;
   register int index, endindex;
   char t1, t2;
   double atof();
   float flt;
   int integer, count;

   while(getp_neq(*str,"=[(")) str++;
   index=0;
   if(*str=='(' || *str=='[')
   {
      str++;
      index= atoi(str);
      while(getp_neq(*str,")]")) str++;
      if(*str=='\0') return;
      if(*str==')') index--;
      if(index<0)
      {
         fprintf(stderr,"*****\n");
         fprintf(stderr,"getpar: invalid array index %d\n",
         (*str==')'?index+1:index));
         exit(-1);
      }
      str++;
   }
   if(*str=='=') str++; 
   else return;
loop:
   endindex= index+1;
   if(type != 's')
   {
      sptr= str;
      while(getp_neq(*sptr,"*x\t ,")) sptr++;
      if(*sptr=='*' || *sptr=='x')
      {
         count= atoi(str);
         endindex= index+count;
         str= sptr+1;
      }
   }
   switch(type)
   {
   case 'd':
      integer= atoi(str);
      while(index<endindex) ptr.i[index++]= integer;
      break;
   case 'f':
      flt= atof(str);
      while(index<endindex) ptr.f[index++]= flt;
      break;
   case 's':
      sptr= ptr.s;
      if(*str=='"' || *str=='\'')
      { 
         t1= t2= *str++; 
      }
      else
      { 
         t1= ' '; 
         t2= '\t'; 
      }
      if (!INPAR)
         t1 = t2 = '\0';
      while(*str!=t1 && *str!=t2 && *str!='\0' && *str!='\n')
         *sptr++ = *str++;
      *sptr= '\0';
      break;
   default:
      fprintf(stderr,"*****\n");
      fprintf(stderr,"getpar: unknown conversion type %c\n",
      type);
      exit(-1);
   }
   while(getp_neq(*str,"\t\n ,")) str++;
   if(*str==',')
   {
      str++; 
      goto loop;
   }
}

getp_neq(c,s)
register char c, *s;
{
   do if(*s == c) return(0); 
   while(*s++);
   return(1);
}

break_names(names,name_list)
char *names,(*name_list)[MAXNAMES];
{
   char *namp;
   int kk,num;
   namp = names;
   num = 0;
   kk = 0;
   while (*namp != '\0' && num < MAXNAMES) {
      switch (*namp) {
      case ' ':
      case ',':
      case ';':
      case ':':
         if (kk != 0) {
            ++num;
            kk = 0;
         }
         break;
      default:
         if (kk < (MAXLENGTH - 1)) {
            name_list[num][kk++] = *namp;
            name_list[num][kk] = '\0';
         }
         break;
      }
      ++namp;
   }
   return(num);
}
