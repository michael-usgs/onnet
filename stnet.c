/* stnet - makes an input file of stereonet commands for onnet */

char *documentation[] = {
"    syntax: stnet [parameters]",
"",
"    parameters:",
"            gcstep=x          x is the stepsize for great circles",
"                              (default=10 degrees)",
"",
"            scstep=y          y is the stepsize for small circles",
"                              (default=10 degrees)",
"",
"            type=string       string is the type of stereonet to use",
"                              wulff or schmidt (default is schmidt)",
"",
"            radius=r          r is the radius of the stereonet in cm",
"                              (default=4).",
"",
"            out=filename      filename is the output file",
"                              (Default: out=stdout)",
"",
"            error=filename    filename is a file into which dummy will place",
"                              its error messages.",
"A typical usage would be: stnet | onnet | ipen",
"this would put a stereonet on the imagen, for more info see",
"the manual and user's manual.",
};
int	doclength = { sizeof documentation/sizeof documentation[0] };

/*
 * author - Andrew Michael
 * author of gtk dummy -Glenn Kroeger
 */
#include        <stdio.h>
#include        <sys/ioctl.h>
#include        <sgtty.h>

#define NAME  "stnet"
#define WARN "warning"
#define FATAL "fatal error"
char string[80],filename[80];
FILE *controltty;
FILE *error_file;
FILE *out_file;
struct sgttyb ttystat;

/* for getpar */
int xargc;
char **xargv;
float gcstep,scstep,dip,size;
float rad;
char typestr[20];

main(argc,argv)
int argc; 
char *argv[];
{
		static char *name="stnet";
        FILE *srcfile;
        FILE *temp;
        register char *cptr;
        char c ;
        int piped_in, piped_out,controlfd;
        int Aflag,bflag;
				int i;


        /* 
         * if no arguments, and not in a pipeline, self document
         * if output is redirected, don't wait for input between 
         * pages of documentation
				 * if piped out do the job
         */
        piped_in = ioctl((fileno(stdin)),TIOCGETP,&ttystat);
        piped_out = ioctl((fileno(stdout)),TIOCGETP,&ttystat);
        if (argc == 1 && !piped_in && !piped_out)
        {
					for( i=0; i<doclength; i++)
						printf("%s\n",documentation[i]);
          exit(0);
        }

        controlfd = open("/dev/tty",0);
        controltty = fdopen(controlfd,"r");

        /*
         * process option flags
         */
        xargc = argc;
        xargv = argv;

        /*
         * process getpar parameters
         */
        xargc = argc;
        xargv = argv;
        error_file = stderr;
        if(getpar_("error","s",string))
        {
                sscanf(string,"%s",filename);
                if((temp=fopen(filename,"w")) == NULL)
                        err(WARN,NAME,"cannot open error file %s",filename);
                else
                        error_file = temp;
        }

				out_file=stdout;
        if(getpar_("out","s",string))
        {
                sscanf(string,"%s",filename);
                if((temp=fopen(filename,"w")) == NULL)
                        err(WARN,NAME,"cannot open output file %s",filename);
                else
                        out_file = temp;
        }
				gcstep=10;
				getpar_("gcstep","f",&gcstep);
				scstep=10;
				getpar_("scstep","f",&scstep);
				rad=4.;
				getpar_("radius","f",&rad);
				if(rad!=4)fprintf(out_file,"r %g\n",rad);
				sprintf(typestr,"schmidt");
				getpar_("type","s",typestr);
				/* ignore anything but wulff nets */
				if(strcmp(typestr,"wulff")==0)fprintf(out_file,"wulff\n");

				for(dip=gcstep;dip<90;dip+=gcstep){
					fprintf(out_file,"p 90 %g\n",dip);
					fprintf(out_file,"p -90 %g\n",dip);
				}
				for(size=scstep;size<90;size+=scstep){
					fprintf(out_file,"s 0 0 %g\n",size);
				}
				fprintf(out_file,"m 0 0\n");
				fprintf(out_file,"d 180 0\n");
				fprintf(out_file,"m 90 0\n");
				fprintf(out_file,"d 270 0\n");
}

err(p1,p2,p3,p4)
/* error printing routine */
char *p1,*p2,*p3,*p4;
{
	fprintf(error_file,"%s ",p1);
	fprintf(error_file,"%s: ",p2);
	fprintf(error_file,p3,p4);
	fprintf(error_file,"\n");
}
