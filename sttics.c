/* sttics - makes an input file of tic marks for onnet */

char *documentation[] = {
"    syntax: sttics [parameters]",
"",
"    parameters:",
"            azstep=x          x is the stepsize for azimuths",
"                              (default=10 degrees)",
"",
"            plgstep=y         y is the stepsize for plunges",
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
"A typical usage is: sttics | onnet | ipen",
"this would put a stereonet full of tics on the imagen.",
"For more information see the manual and the user's manual."
};
int	doclength = { sizeof documentation/sizeof documentation[0] };

/*
 * author- Andrew Michael
 * author of gtk dummy - Glenn Kroeger
 */
#include        <stdio.h>
#include        <sys/ioctl.h>
#include        <sgtty.h>

#define NAME  "sttics"
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
float azstep,plgstep,az,plg;
float rad;
char typestr[20];

main(argc,argv)
int argc; 
char *argv[];
{
		static char *name="onnet";
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
				azstep=10;
				getpar_("azstep","f",&azstep);
				plgstep=10;
				getpar_("plgstep","f",&plgstep);
				rad=4.;
				getpar_("radius","f",&rad);
				if(rad!=4)fprintf(out_file,"r %g\n",rad);
				sprintf(typestr,"schmidt");
				getpar_("type","s",typestr);
				/* ignore anything but wulff nets */
				if(strcmp(typestr,"wulff")==0)fprintf(out_file,"wulff\n");


				for(az=0;az<360;az+=azstep){
					for(plg=0;plg<90;plg+=plgstep){
						if(plg==0){
							fprintf(out_file,"m %g 2\n",az);
							fprintf(out_file,"d %g 0\n",az);
						}
						else {
						fprintf(out_file,"m %g %g\n",az-1,plg);
						fprintf(out_file,"d %g %g\n",az,plg);
						fprintf(out_file,"d %g %g\n",az+1,plg);
						fprintf(out_file,"m %g %g\n",az,plg-1);
						fprintf(out_file,"d %g %g\n",az,plg+1);
						}
					}
				}
				fprintf(out_file,"m 0 89\n");
				fprintf(out_file,"d 180 89\n");
				fprintf(out_file,"m 90 89\n");
				fprintf(out_file,"d 270 89\n");
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
