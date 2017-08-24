/* Main program for onnet*/

char *documentation[] = {
"    syntax: onnet [parameters] [inputfiles]",
"",
"    parameters:",
"            in=filename       filename is the input file",
"                              (Default: in=stdin)",
"",
"            out=filename      filename is the output file",
"                              (Default: out=stdout)",
"",
"            error=filename    filename is a file into which dummy will place",
"                              its error messages.",
"",
"Onnet is a program that plots a variety of objects on stereonets see",
"the full documentation, including the user's manual, for instructions.",
"",
"The output is in vplot format."
};
int	doclength = { sizeof documentation/sizeof documentation[0] };


/*
 * author-Andrew Michael- onnet
 * author-Glenn Kroeger- GeoToolkit driver
 * modifications since GeoToolkit days
 * Added color - Andrew Michael Sept. 26, 1986
 */
#include        <stdio.h>
#include        <sys/ioctl.h>
#include        <sgtty.h>
#include        <math.h>

#define NAME "onnet"
#define WARN "warning"
#define FATAL "Fatal error"
#define LEN 100  /* length of input lines */
#define MAX 400  /* maximum number of points in a curve */
#define TORADS 57.29577951
#define PI 3.14159265
#define DEF_COLOR 7
char string[80],filename[80];
FILE *controltty;
FILE *error_file;
FILE *out_file;
struct sgttyb ttystat;

/* onnet variables */
struct { /* command data structure */
	int fat; /* fatness for this command */
	int color; /* color for this command */
	int size; /* symbol size for this command */
	char upper; /* upper hemisphere symbol for this command */
	char lower; /* lower hemisphere symbol for this command */
	int possible; /* 0 if this command is impossible now */
	              /* 1 if it is possible */
	} circle,title,plane,pole,rake,line,small,mover,drawer,great;

struct { /* viewing parameter command data structure */
         /* for those parameters needing a int value */
	int possible; /* is this parameter possible at this time */
	int value; /* what is the value of this parameter */
	} fatt,fatness,viewtype,nettype,symbol,size,color;
struct { /* viewing parameter command data structure */
         /* for those parameters needing a float value */
	int possible; /* is this parameter possible at this time */
	float value; /* what is the value of this parameter */
	} r;

int firstfile; /* 0 after first file processed, 1 before then */
int firstline; /* 0 after first line processed, 1 before then */
int lastfat;
int lastcolor;
float rnet2; /* corrected radius for projection */
char comline[LEN]; /* input line */
float x[MAX],y[MAX];  /* plotting arrays */
int npts; /* number of points to plot */
char cap[LEN]; /* plot title */
char com[10],choice[10]; /* commands to be followed */
char c1,c2; /* characters for use in symbol command */
float ddir,dip,trend,plunge,rakeang; /* input values */
float trend2,plunge2;  /* for great circle segments */
float strike;
double n1,n2,n3;  /*normal vector */
double s1,s2,s3;  /*slip vector */
double h1,h2,h3;  /*a horizontal vector */
double head,rplu,rhed,rdis,nplu,beta,diff,nstr;
float dist;  /* for use with small circles */
double z,z2,z3; /* dummies */
int i,j,k;
int symsz;
char sym;
float titlex,titley;
/* end of onnet variables */

/* for getpar */
int xargc;
char **xargv;

main(argc,argv)
int argc; 
char *argv[];
{
		static char *name="name";
        FILE *srcfile;
        FILE *temp;
        register char *cptr;
        char c ;
        int piped_in, piped_out,controlfd;
        int Aflag,bflag;


				/* set defaults */
				do_default();
        /* 
         * if no arguments, and not in a pipeline, self document
         * if output is redirected, don't wait for input between 
         * pages of documentation
         */
        piped_in = ioctl((fileno(stdin)),TIOCGETP,&ttystat);
        piped_out = ioctl((fileno(stdout)),TIOCGETP,&ttystat);
        if (argc == 1 && !piped_in)
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
		for (xargc--,xargv++; xargc; xargc--,xargv++)
		{
			cptr = *xargv; 
			if( *cptr=='-' )
			{
			while( *(++cptr))
			{
				switch( *cptr )
				{
				default:
					break;
				}
			}
			}
		}

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
        else if( !piped_out)
        {
				/*
				 * if output directed toward screen kill program
				 */
                err(FATAL,NAME,"output not directed to file or filter!");
				exit(0);
        }

        /*
         * first process pipe input
         */
        if( piped_in )
                do_onnet(stdin);

        /*
         * next process in= inputfiles
         */
        if(getpar_("in","s",string))
        {
                sscanf(string,"%s",filename);
                if((temp=fopen(filename,"r")) != NULL)
                        do_onnet(temp);
                else
                        err(WARN,NAME,"cannot open %s",filename);
        }

        /*
         * finally process input line for non-getpar arguments and assume
         * they are also input files
         */
        for (xargc--,xargv++; xargc; xargc--,xargv++)
        {
                cptr = *xargv; 
                if( *cptr=='-' ) continue; /* this is a flag */
                while (*cptr)
                {
                        if (*cptr == '=')  break; /* this is for getpar */
                        cptr++;
                }       
                if (*cptr)  continue;
                cptr = *xargv;
                if((temp=fopen(cptr,"r")) != NULL)
                {
                        do_onnet(temp);
                }
                else
                {
                        err(WARN,NAME,"cannot open %s",cptr);
                }
         }

	/* draw the title last */
	if(lastfat!=title.fat){
		setfat(title.fat);
		lastfat=title.fat;
	}
	if(lastcolor!=title.color){
		setcol(title.color);
		lastcolor=title.color;
	}
	symsz=title.size;
	uText(titlex,titley,symsz,0,cap);
}

do_default()
{
	/* set all the defaults */
	plane.fat=0;
	plane.color=DEF_COLOR;
	plane.size=0; /* not used */
	plane.upper=' '; /* not used */
	plane.lower=' '; /* not used */
	plane.possible=0; /* not till the circle is drawn */
	great.fat=0;
	great.color=DEF_COLOR;
	great.size=0; /* not used */
	great.upper=' '; /* not used */
	great.lower=' '; /* not used */
	great.possible=0; /* not till the circle is drawn */
	pole.fat=0;
	pole.color=DEF_COLOR;
	pole.size=3;
	pole.upper=' '; /* not used */
	pole.lower='o'; 
	pole.possible=0; /* not till the circle is drawn */
	rake.fat=0;
	rake.color=DEF_COLOR;
	rake.size=3;
	rake.upper='*'; /* not used */
	rake.lower='+'; 
	rake.possible=0; /* not till the circle is drawn */
	                 /* and then only after pole or plane */
	line.fat=0;
	line.color=DEF_COLOR;
	line.size=3;
	line.upper='s'; /* not used */
	line.lower='x'; 
	line.possible=0; /* not till the circle is drawn */
	small.fat=0;
	small.color=DEF_COLOR;
	small.size=0; /* not used */
	small.upper=' '; /* not used */
	small.lower=' '; /* not used */
	small.possible=0; /* not till the circle is drawn */
	mover.fat=0; /* not used */
	mover.color=DEF_COLOR;
	mover.size=0; /* not used */
	mover.upper=' '; /* not used */
	mover.lower=' '; /* not used */
	mover.possible=0; /* not till the circle is drawn */
	drawer.fat=0; 
	drawer.color=DEF_COLOR;
	drawer.size=0; /* not used */
	drawer.upper=' '; /* not used */
	drawer.lower=' '; /* not used */
	drawer.possible=0; /* not till the circle is drawn */
	                 /* and then only after move or draw */
	circle.fat=0;
	circle.color=DEF_COLOR;
	circle.size=0; /* not used */
	circle.upper=' '; /* not used */
	circle.lower=' '; /* not used */
	circle.possible=1; /* the first thing drawn */
	title.fat=0;
	title.color=DEF_COLOR;
	title.size=10;
	title.upper=' '; /* not used */
	title.lower=' '; /* not used */
	title.possible=1; /* the first thing drawn */
	fatt.possible=1;
	fatt.value=0;
	fatness.possible=1;
	fatness.value=0; /* not used */
	viewtype.possible=1;
	viewtype.value=0; /* start with reflect viewing */
	nettype.possible=1;
	nettype.value=0; /* start with Schmidt net */
	r.possible=1;
	r.value=4.;
	symbol.possible=1;
	symbol.value=0; /* not used */
	size.possible=1;
	size.value=0; /* not used */
	color.possible=1;
	color.value=0; /*not used */
	firstfile=1;
	firstline=1;
	lastfat=0;
	lastcolor=DEF_COLOR;
}

reset_poss()
{
	/* reset possiblities to usual state */
	plane.possible=1; /* usually possible */
	great.possible=1; /* usually possible */
	pole.possible=1;  /* usually possible */
	rake.possible=0; /* only after plane or pole */
	line.possible=1; /* usually possible */
	small.possible=1; /* usually possible */
	mover.possible=1; /* usually possible */
	drawer.possible=0; /* only after move or draw */
	circle.possible=0; /* only at beginning */
	/* title.possible is not reset because it is only affected by itself */
	fatt.possible=1; /* always possible */
	fatness.possible=1; /* always possible */
	viewtype.possible=1; /* always possible */
	nettype.possible=1; /* always possible */
	r.possible=0; /* only at beginning */
	symbol.possible=1; /* always possible */
	size.possible=1; /* always possible */
	color.possible=1; /* always possible */
}

do_onnet(fp)
FILE *fp;
/* do onnet for this file */
{

	if(firstfile)erase();
	while(fgets(comline,LEN,fp)!=NULL){
		/* see which command it is */
		sscanf(comline,"%s",com);
		if(strcmp(com,"r")==0)do_radius();
		else if(strcmp(com,"radius")==0)do_radius();
		else if(strcmp(com,"fat")==0)do_fat();
		else if(strcmp(com,"fatness")==0)do_fatness();
		else if(strcmp(com,"f")==0)do_fatness();
		else if(strcmp(com,"reflect")==0)do_reflect();
		else if(strcmp(com,"through")==0)do_through();
		else if(strcmp(com,"schmidt")==0)do_schmidt();
		else if(strcmp(com,"wulff")==0)do_wulff();
		else if(strcmp(com,"size")==0)do_size();
		else if(strcmp(com,"sz")==0)do_size();
		else if(strcmp(com,"symbol")==0)do_symbol();
		else if(strcmp(com,"sy")==0)do_symbol();
		else if(strcmp(com,"color")==0)do_color();
		else if(strcmp(com,"c")==0)do_color();
		else if(strcmp(com,"plane")==0)do_plane();
		else if(strcmp(com,"p")==0)do_plane();
		else if(strcmp(com,"pole")==0)do_pole();
		else if(strcmp(com,"po")==0)do_pole();
		else if(strcmp(com,"rake")==0)do_rake();
		else if(strcmp(com,"rk")==0)do_rake();
		else if(strcmp(com,"line")==0)do_line();
		else if(strcmp(com,"l")==0)do_line();
		else if(strcmp(com,"small")==0)do_small();
		else if(strcmp(com,"s")==0)do_small();
		else if(strcmp(com,"move")==0)do_move();
		else if(strcmp(com,"m")==0)do_move();
		else if(strcmp(com,"draw")==0)do_draw();
		else if(strcmp(com,"d")==0)do_draw();
		else if(strcmp(com,"title")==0)do_title();
		else if(strcmp(com,"great")==0)do_great();
		else if(strcmp(com,"g")==0)do_great();
		else if(comline[0]=='#')continue; /* comment line */
		else {
			/* not understandable first line is a title */
			if(firstline){
				j=strlen(comline);
				for(i=0;i<j;++i)cap[i]=comline[i];
				title.possible=0;
			}
			else err(WARN,NAME,"Doesn't understand: %s",comline);
		}
	firstline=0; /* done with at least one line */
	}
	firstfile=0; /* done with at least one file */
}

do_circle()
/* draw the circle when needed */
{
	if(circle.possible){
		if(lastfat!=circle.fat){
			setfat(circle.fat);
			lastfat=circle.fat;
		}
		if(lastcolor!=circle.color){
			setcol(circle.color);
			lastcolor=circle.color;
		}
	r.value/= 2.54;
	/* draw the circle */
	for(i=0;i<199;++i){
		z= i*2.*PI/(199);
		x[i]=r.value*sin(z);
		y[i]=r.value*cos(z);
		}
	x[199]=x[0]; /* close the circle */
	y[199]=y[0];
	x[200]=x[199];
	y[200]=y[199]+0.2*r.value;
	z=r.value/12.;
	x[201]=x[200]+z;
	y[201]=y[200]-z;
	x[202]=x[201]-z;
	y[202]=y[201]+z;
	x[203]=x[202]-z;
	y[203]=y[202]-z;
	titlex=x[150];
	titley=y[200]+0.2*r.value;
	z=1.+ r.value;
	setfp(out_file);
	set0(z,z);
	setscl(1.,1.);
	setu0(0.,0.);
	uwindow(-z,-z,z,z+1.+(z/10.));
/*	setfilp(out_file);*/
/*	set0(z,z);*/
/*	setscl(1.,1.);*/
/*	setu0(0.,0.);*/
/*	uwndo(-z,-z,z,z+1.+(z/10.));*/
	drline(204,x,y);
	fflush(out_file);
	/* adjust r.value for projection equation */
	rnet2= r.value/sin(PI/4.);
	reset_poss();
	}
}

do_radius()
/* reset the redius */
{
	if(r.possible)sscanf(comline,"%s %f",com,&r.value);
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_title()
/* reset the title */
{
	if(title.possible){
		/* strip out title */
		j=strlen(comline);
		k=0;
		while(comline[k]!=' ')++k;
		for(i=k+1;i<j;++i)cap[i-k-1]=comline[i];
	}
	else err(WARN,NAME,"%s not possible now.",comline);
	/* only allow one title */
	title.possible=0;
}

do_fat()
/* reset all fats */
/* largely here for backwards compatibility */
{
	if(fatt.possible){
		sscanf(comline,"%s %d",com,&i);
		plane.fat=i;
		pole.fat=i;
		rake.fat=i;
		line.fat=i;
		small.fat=i;
		mover.fat=i; /* just for consistency */
		drawer.fat=i;
		circle.fat=i;
		title.fat=i;
		great.fat=i;
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}
	
do_reflect()
/* set to reflect viewing */
{
	if(viewtype.possible)viewtype.value=0;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_through()
/* set to through viewing */
{
	if(viewtype.possible)viewtype.value=1;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_schmidt()
/* set to schmidt net */
{
	if(nettype.possible)nettype.value=0;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_wulff()
/* set to schmidt net */
{
	if(nettype.possible)nettype.value=1;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_size()
/* reset size for some command */
{
	if(size.possible){
		sscanf(comline,"%s %s %lf",com,choice,&z);
		if(strcmp(choice,"pole")==0)pole.size=z;
		else if(strcmp(choice,"line")==0)line.size=z;
		else if(strcmp(choice,"rake")==0)rake.size=z;
		else if(strcmp(choice,"title")==0)title.size=z;
		/* can't change size for other commands, so... */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}
		
do_symbol()
/* change symbols as appropriate */
{
	if(symbol.possible){
		sscanf(comline,"%s %s %c %c",com,choice,&c1,&c2);
		if(strcmp(choice,"pole")==0)pole.lower=c1;
		else if(strcmp(choice,"line")==0){
			line.lower=c1;
			line.upper=c2;
			}
		else if(strcmp(choice,"rake")==0){
			rake.lower=c1;
			rake.upper=c2;
			}
		/* can't change symbols for other commands */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_fatness()
/* change fatness */
{
	if(fatness.possible){
		sscanf(comline,"%s %s %d",com,choice,&i);
		if(strcmp(choice,"plane")==0)plane.fat=i;
		else if(strcmp(choice,"pole")==0)pole.fat=i;
		else if(strcmp(choice,"rake")==0)rake.fat=i;
		else if(strcmp(choice,"line")==0)line.fat=i;
		else if(strcmp(choice,"small")==0)small.fat=i;
		else if(strcmp(choice,"move")==0)mover.fat=i;
		else if(strcmp(choice,"draw")==0)drawer.fat=i;
		else if(strcmp(choice,"title")==0)title.fat=i;
		else if(strcmp(choice,"circle")==0)circle.fat=i;
		else if(strcmp(choice,"great")==0)great.fat=i;
		/* can't change fat for other commands */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_color()
/* change color */
{
	if(color.possible){
		sscanf(comline,"%s %s %d",com,choice,&i);
		if(strcmp(choice,"plane")==0)plane.color=i;
		else if(strcmp(choice,"pole")==0)pole.color=i;
		else if(strcmp(choice,"rake")==0)rake.color=i;
		else if(strcmp(choice,"line")==0)line.color=i;
		else if(strcmp(choice,"small")==0)small.color=i;
		else if(strcmp(choice,"move")==0)mover.color=i;
		else if(strcmp(choice,"draw")==0)drawer.color=i;
		else if(strcmp(choice,"title")==0)title.color=i;
		else if(strcmp(choice,"circle")==0)circle.color=i;
		else if(strcmp(choice,"great")==0)great.color=i;
		/* can't change color for other commands */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_plane()
/* draw a plane */
{
	if(circle.possible)do_circle();
	if(plane.possible){
		if(lastfat!=plane.fat){
	 		setfat(plane.fat);
			lastfat=plane.fat;
		}
		if(lastcolor!=plane.color){
			setcol(plane.color);
			lastcolor=plane.color;
		}
		sscanf(comline,"%s %f %f",com,&ddir,&dip);
		/* n is the dip vector here */
		z=ddir/TORADS;
		z2=dip/TORADS;
		n1= sin(z)*cos(z2);
		n2= cos(z)*cos(z2);
		n3= -sin(z2);
		/* h is the strike vector */
		h1= -cos(z);
		h2= sin(z);
		h3=0;
		for(i=0;i<180;++i){
			z=i;
			z/= TORADS;
			z2=cos(z);
			z3=sin(z);
			s1=z2*h1 + z3*n1;
			s2=z2*h2 + z3*n2;
			s3=        z3*n3;
			dirplg(s1,s2,s3,&z2,&z);
			z/= TORADS;
			z2/= TORADS;
			comrad();
			x[i]=z*sin(z2);
			y[i]=z*cos(z2);
		}
		drline(180,x,y);
		fflush(out_file);
		reset_poss();
		rake.possible=1; /*rake possible after plane */
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_great()
/* draw a great circle segment */
{
	if(circle.possible)do_circle();
	if(great.possible){
		if(lastfat!=great.fat){
			setfat(great.fat);
			lastfat=great.fat;
		}
		if(lastcolor!=great.color){
			setcol(great.color);
			lastcolor=great.color;
		}
		sscanf(comline,"%s %f %f %f %f",com,&trend,&plunge,&trend2,&plunge2);
		/* n is the vector to the first point */
		z=trend/TORADS;
		z2=plunge/TORADS;
		n1= sin(z)*cos(z2);
		n2= cos(z)*cos(z2);
		n3= -sin(z2);
		/* h is the vector to the second point */
		z=trend2/TORADS;
		z2=plunge2/TORADS;
		h1= sin(z)*cos(z2);
		h2= cos(z)*cos(z2);
		h3= -sin(z2);
		/* compute the angle between them to decide how many points to plot */
		z=h1*n1+h2*n2+h3*n3;
		j=acos(z)*TORADS*2;
		for(i=0;i<=j;++i){
			z=i;
			z2=j;
			z=(z/z2)*90./TORADS;
			z2=cos(z);
			z3=sin(z);
			s1=z2*h1 + z3*n1;
			s2=z2*h2 + z3*n2;
			s3=z2*h3 + z3*n3;
			dirplg(s1,s2,s3,&z2,&z);
			if(s3>0){ /* we are on the upper hemisphere */
				if(z>0){
					z= -z;
					z2+= 180;
				}
				z= -z/TORADS;
				if(!viewtype.value)z2= (180.+z2)/TORADS;
				else z2= z2/TORADS;
			}
			else {
				z/= TORADS;
				z2/= TORADS;
			}
			comrad();
			x[i]=z*sin(z2);
			y[i]=z*cos(z2);
		}
		drline(j+1,x,y);
		fflush(out_file);
		reset_poss();
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_pole()
/* draw a pole */
{
	if(circle.possible)do_circle();
	if(pole.possible){
		if(lastfat!=pole.fat){
			setfat(pole.fat);
			lastfat=pole.fat;
		}
		if(lastcolor!=pole.color){
			setcol(pole.color);
			lastcolor=pole.color;
		}
		sscanf(comline,"%s %f %f",com,&ddir,&dip);
		z=ddir/TORADS;
		z2=dip/TORADS;
		n1=sin(z)*sin(z2);  /* normal vector to fault plane */
		n2=cos(z)*sin(z2);
		n3=cos(z2);
		dirplg(n1,n2,n3,&z2,&z);
		z= z/TORADS;
		z2= z2/TORADS;
/*		z=rnet2*sin(((PI/2.)-z)/2.);*/
		comrad();
		x[0]=z*sin(z2);
		y[0]=z*cos(z2);
		sym=pole.lower;
		symsz=pole.size;
		center(x[0],y[0],sym,symsz);
		fflush(out_file);
		reset_poss();
		rake.possible=1; /*rake possible after pole */
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_rake()
/* draw a rake */
{
	if(circle.possible)do_circle();
	if(rake.possible){
		if(lastfat!=rake.fat){
			setfat(rake.fat);
			lastfat=rake.fat;
		}
		if(lastcolor!=rake.color){
			setcol(rake.color);
			lastcolor=rake.color;
		}
		sscanf(comline,"%s %f",com,&rakeang);
		z=ddir/TORADS;
		z2=dip/TORADS;
		z3=rakeang/TORADS;
		/* slickenside vector calculation */
		s1= -cos(z3)*cos(z)-sin(z3)*sin(z)*cos(z2);
		s2= cos(z3)*sin(z)-sin(z3)*cos(z)*cos(z2);
		s3= sin(z3)*sin(z2);
		sym=rake.lower;
		if(s3>0){
			if(viewtype.value)s3= -s3;
				else {
				s1= -s1;
				s2= -s2;
				s3= -s3;
				}
				sym=rake.upper;
			}
		dirplg(s1,s2,s3,&z2,&z);
		z= z/TORADS;
		z2= z2/TORADS;
/*		z=rnet2*sin(((PI/2.)-z)/2.);*/
		comrad();
		x[0]=z*sin(z2);
		y[0]=z*cos(z2);
		symsz=rake.size;
		center(x[0],y[0],sym,symsz);
		reset_poss();
		rake.possible=1; /* no reason not to draw multiple rakes on a plane */
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_line()
/* draw a line */
{
	if(circle.possible)do_circle();
	if(line.possible){
		if(lastfat!=line.fat){
			setfat(line.fat);
			lastfat=line.fat;
		}
		if(lastcolor!=line.color){
			setcol(line.color);
			lastcolor=line.color;
		}
	/* draw a line */
	sscanf(comline,"%s %f %f",com,&trend,&plunge);
	if(plunge>=0){
		z= plunge/TORADS;
		z2= trend/TORADS;
		sym=line.lower;
	}
	else {
		z= -plunge/TORADS;
		if(!viewtype.value)z2= (180.+trend)/TORADS;
		else z2= trend/TORADS;
		sym=line.upper;
	}
/*	z=rnet2*sin(((PI/2.)-z)/2.);*/
	comrad();
	x[0]=z*sin(z2);
	y[0]=z*cos(z2);
	symsz=line.size;
	center(x[0],y[0],sym,symsz);
	fflush(out_file);
	reset_poss();
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_small()
/* draw a small circle */
{
	if(circle.possible)do_circle();
	if(small.possible){
		if(lastfat!=small.fat){
			setfat(small.fat);
			lastfat=small.fat;
		}
		if(lastcolor!=small.color){
			setcol(small.color);
			lastcolor=small.color;
		}
		sscanf(comline,"%s %f %f %f",com,&trend,&plunge,&dist);
		if (plunge == 90.) plunge = 89.999;
		i=0; /* use as plot vector counter */
		k=0; /* use as flag for if negative plunges */
		for (j =0;j<=180;++j){
			head = j * 2.0;
			if (head > 360.) head = head - 360.;
 			rplu = plunge/TORADS;
 			rhed = head/TORADS;
 			rdis = dist/TORADS;
 			nplu = asin(sin(rplu)*cos(rdis)
				+ cos(rplu)*sin(rdis)*cos(rhed));
 			beta = asin(sin(rdis)*sin(rhed)/cos(nplu));
 			beta = beta*TORADS;
 			diff = cos(rdis) - (sin(rplu)*sin(nplu));
 			if (diff > 0.  || fabs(diff) < .000001) nstr = trend + beta;
 			else nstr = trend + 180. - beta;
 			nplu = nplu;
			if(nplu<0){
				if(!k){
					if(i)drline(i,x,y);
					k=1;
					i=0;
				}
				nplu= -nplu;
				if(!viewtype.value)nstr+= 180.;
			}
			else if(k){
				if(i)drline(i,x,y);
				k=0;
				i=0;
			}
			z= nplu;
			z2= nstr/TORADS;
/*			z=rnet2*sin(((PI/2.)-z)/2.);*/
			comrad();
			x[i]=z*sin(z2);
			y[i]=z*cos(z2);
			i++;
		}
		if(i)drline(i,x,y);
		fflush(out_file);
		reset_poss();
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_move()
/* move pen */
{
	if(circle.possible)do_circle();
	if(mover.possible){
		sscanf(comline,"%s %f %f",com,&trend,&plunge);
		if(plunge>=0){
			z= plunge/TORADS;
			z2= trend/TORADS;
		}
		else {
			z= -plunge/TORADS;
			if(!viewtype.value)z2= (180.+trend)/TORADS;
			else z2= trend/TORADS;
		}
/*		z=rnet2*sin(((PI/2.)-z)/2.);*/
		comrad();
		x[0]=z*sin(z2);
		y[0]=z*cos(z2);
		umove(x[0],y[0]);
		fflush(out_file);
		reset_poss();
		drawer.possible=1;
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_draw()
/* draw to new point */
{
	if(circle.possible)do_circle();
	if(drawer.possible){
		if(lastfat!=drawer.fat){
			setfat(drawer.fat);
			lastfat=drawer.fat;
		}
		if(lastcolor!=drawer.color){
			setcol(drawer.color);
			lastcolor=drawer.color;
		}
		sscanf(comline,"%s %f %f",com,&trend,&plunge);
		if(plunge>=0){
			z= plunge/TORADS;
			z2= trend/TORADS;
		}
		else {
			z= -plunge/TORADS;
			if(!viewtype.value)z2= (180.+trend)/TORADS;
			else z2= trend/TORADS;
		}
/*		z=rnet2*sin(((PI/2.)-z)/2.);*/
		comrad();
		x[0]=z*sin(z2);
		y[0]=z*cos(z2);
		udraw(x[0],y[0]);
		fflush(out_file);
		reset_poss();
		drawer.possible=1;
	}
	else err(WARN,NAME,"%s not possible now.",comline);
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

drline(nn,xx,yy)
int nn;
float xx[],yy[];
{
	int ii;
	umove(xx[0],yy[0]);
	for(ii=1;ii<nn;++ii)udraw(xx[ii],yy[ii]);
	return;
}

comrad()
{
/* get the radius needed to plot the point */
	if(nettype.value)z=r.value*tan((PI/4.)-z/2.);
	else z=rnet2*sin(((PI/2.)-z)/2.);
}
