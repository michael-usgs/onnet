/* Main program for onnet*/
/* this is the postscript version that uses the GMT */
/* pslib routines to do actual plotting */
/*
 * author-Andrew Michael- onnet
 * Added color - Andrew Michael Sept. 26, 1986
 * changed to postscript output March 10, 1997 
 */
#include       <gmt.h>

struct EPS *gmt_epsinfo();


#include        <stdio.h>
#include        <math.h>
#include        <sys/ioctl.h>
#include        <sgtty.h>


#define NAME "onnet"
#define WARN "warning"
#define FATAL "Fatal error"
#define LEN 200  /* length of input lines */
#define MAXCURVE 400  /* maximum number of points in a curve */
#define TORADS 57.29577951
#define PI 3.14159265
#define DEF_COLOR 8
#define DEF_FONT 8 /* courier */
#define FORM 0
char string[80],filename[80];
FILE *controltty;
FILE *error_file;
FILE *out_file;
struct sgttyb ttystat;


/* onnet variables */
struct { /* command data structure */
	int fat; /* fatness for this command */
	int color; /* color for this command */
	int font;  /* font for this command */
	int size; /* symbol size for this command */
	char upper; /* upper hemisphere symbol for this command */
	char lower; /* lower hemisphere symbol for this command */
	int possible; /* 0 if this command is impossible now */
	              /* 1 if it is possible */
	} circle,title,plane,pole,rake,line,small,mover,drawer,great,text;

struct { /* viewing parameter command data structure */
         /* for those parameters needing a int value */
	int possible; /* is this parameter possible at this time */
	int value; /* what is the value of this parameter */
	} fatt,fatness,viewtype,nettype,symbol,size,color,focal,fontt;
struct { /* viewing parameter command data structure */
         /* for those parameters needing a float value */
	int possible; /* is this parameter possible at this time */
	float value; /* what is the value of this parameter */
	} r;

int firstfile; /* 0 after first file processed, 1 before then */
int firstline; /* 0 after first line processed, 1 before then */
int lastfat;
int lastcolor;
double rnet2; /* corrected radius for projection */
char comline[LEN]; /* input line */
double x[MAXCURVE],y[MAXCURVE];  /* plotting arrays */
double xc,yc;  /* points in non-user coordinates, for use with where () */
int npts; /* number of points to plot */
char cap[LEN]; /* plot title */
char com[10],choice[10]; /* commands to be followed */
char c1,c2; /* characters for use in symbol command */
double ddir,dip,trend,plunge,rakeang; /* input values */
double trend2,plunge2;  /* for great circle segments */
double strike;
double n1,n2,n3;  /*normal vector */
double s1,s2,s3;  /*slip vector */
double h1,h2,h3;  /*a horizontal vector */
double head,rplu,rhed,rdis,nplu,beta,diff,nstr;
double dist;  /* for use with small circles */
double z,z2,z3; /* dummies */
int i,j,k;
int symsz;
char sym;
char syms[2];
double titlex,titley;
char texstr[200];  /* string to be plotted with text command */
char *progname;
/* end of onnet variables */

main(argc,argv)
int argc; 
char *argv[];
{
        FILE *temp;
        register char *cptr;
        int piped_in, piped_out,controlfd;

				progname= *argv;
				fprintf(stderr,"program name= %s\n",progname);
				/* set defaults */
				do_default();

				piped_in = ioctl((fileno(stdin)),TIOCGETP,&ttystat);
				out_file=stdout;
        error_file = stderr;

        /*
         * first process pipe input
         */
        if( piped_in )
                do_onnet(stdin);

        /*
         * next process input files on command line
         */

        for (argc--,argv++; argc; argc--,argv++)
        {
                cptr = *argv;
                if((temp=fopen(cptr,"r")) != NULL)
                {
                        do_onnet(temp);
                }
                else
                {
                        err(WARN,NAME,"cannot open %s",cptr);
                }
         }


	/* if no lines processed, quit */
	if(firstline)return;
	/* draw the title last */
	if(lastfat!=title.fat){
fprintf(stderr,"doing title\n");
		ps_setline(title.fat);
		lastfat=title.fat;
	}
	if(lastcolor!=title.color){
		setcol(title.color);
		lastcolor=title.color;
	}
	symsz=title.size;
/*	uText(titlex,titley,symsz,0,cap);*/
fprintf(stderr,"doing title 2\n");
	ps_text(titlex,titley,symsz,cap,0,7,FORM);
	fprintf(stderr,"FORM = %d\n",FORM);
	ps_plotend(1);
}

do_default()
{
	/* set all the defaults */
	plane.fat=1;
	plane.color=DEF_COLOR;
	plane.font=DEF_FONT;
	plane.size=0; /* not used */
	plane.upper=' '; /* not used */
	plane.lower=' '; /* not used */
	plane.possible=0; /* not till the circle is drawn */
	great.fat=1;
	great.color=DEF_COLOR;
	great.font=DEF_FONT;
	great.size=0; /* not used */
	great.upper=' '; /* not used */
	great.lower=' '; /* not used */
	great.possible=0; /* not till the circle is drawn */
	pole.fat=1;
	pole.color=DEF_COLOR;
	pole.font=DEF_FONT;
	pole.size=3*2.18;
	pole.upper=' '; /* not used */
	pole.lower='o'; 
	pole.possible=0; /* not till the circle is drawn */
	rake.fat=1;
	rake.color=DEF_COLOR;
	rake.font=DEF_FONT;
	rake.size=3*2.18;
	rake.upper='*'; /* not used */
	rake.lower='+'; 
	rake.possible=0; /* not till the circle is drawn */
	                 /* and then only after pole or plane */
	line.fat=1;
	line.color=DEF_COLOR;
	line.font=DEF_FONT;
	line.size=3*2.18;
	line.upper='s'; /* not used */
	line.lower='x'; 
	line.possible=0; /* not till the circle is drawn */
	small.fat=1;
	small.color=DEF_COLOR;
	small.font=DEF_FONT;
	small.size=0; /* not used */
	small.upper=' '; /* not used */
	small.lower=' '; /* not used */
	small.possible=0; /* not till the circle is drawn */
	mover.fat=1; /* not used */
	mover.color=DEF_COLOR;
	mover.font=DEF_FONT;
	mover.size=0; /* not used */
	mover.upper=' '; /* not used */
	mover.lower=' '; /* not used */
	mover.possible=0; /* not till the circle is drawn */
	drawer.fat=1; 
	drawer.color=DEF_COLOR;
	drawer.font=DEF_FONT;
	drawer.size=0; /* not used */
	drawer.upper=' '; /* not used */
	drawer.lower=' '; /* not used */
	drawer.possible=0; /* not till the circle is drawn */
	                 /* and then only after move or draw */
	circle.fat=1;
	circle.color=DEF_COLOR;
	circle.font=DEF_FONT;
	circle.size=0; /* not used */
	circle.upper=' '; /* not used */
	circle.lower=' '; /* not used */
	circle.possible=1; /* the first thing drawn */
	title.fat=1;
	title.color=DEF_COLOR;
	title.font=DEF_FONT;
	title.size=10*2.18;
	title.upper=' '; /* not used */
	title.lower=' '; /* not used */
	title.possible=1; /* the first thing drawn */
	text.fat=1;
	text.color=DEF_COLOR;
	text.font=DEF_FONT;
	text.size=3*2.18;
	text.upper=' '; /* not used */
	text.lower=' '; /* not used */
	text.possible=1; /* can be done now */
	fatt.possible=1;
	fatt.value=0;
	fatness.possible=1;
	fatness.value=0; /* not used */
	viewtype.possible=1;
	viewtype.value=0; /* start with reflect viewing */
	focal.possible=1;
	focal.value=0; /* start with non-focal mechanism type values */
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
	fontt.possible=1;
	fontt.value=DEF_FONT; /* not used */
	firstfile=1;
	firstline=1;
	lastfat=1;
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
	text.possible=1; /* usually possible */
	/* title.possible is not reset because it is only affected by itself */
	fatt.possible=1; /* always possible */
	fatness.possible=1; /* always possible */
	viewtype.possible=1; /* always possible */
	focal.possible=1; /* always possible */
	nettype.possible=1; /* always possible */
	r.possible=0; /* only at beginning */
	symbol.possible=1; /* always possible */
	size.possible=1; /* always possible */
	color.possible=1; /* always possible */
	fontt.possible=1; /* always possible */
}

do_onnet(fp)
FILE *fp;
/* do onnet for this file */
{

	while(fgets(comline,LEN,fp)!=NULL){
/*		if(firstfile && firstline)erase();*/
		/* see which command it is */
		sscanf(comline,"%s",com);
		if(strcmp(com,"r")==0)do_radius();
		else if(strcmp(com,"radius")==0)do_radius();
		else if(strcmp(com,"fat")==0)do_fat();
		else if(strcmp(com,"fatness")==0)do_fatness();
		else if(strcmp(com,"f")==0)do_fatness();
		else if(strcmp(com,"width")==0)do_width();
		else if(strcmp(com,"w")==0)do_width();
		else if(strcmp(com,"reflect")==0)do_reflect();
		else if(strcmp(com,"through")==0)do_through();
		else if(strcmp(com,"schmidt")==0)do_schmidt();
		else if(strcmp(com,"wulff")==0)do_wulff();
		else if(strcmp(com,"focal")==0)do_focal();
		else if(strcmp(com,"notfocal")==0)do_notfocal();
		else if(strcmp(com,"size")==0)do_size();
		else if(strcmp(com,"sz")==0)do_size();
		else if(strcmp(com,"pointsize")==0)do_pointsize();
		else if(strcmp(com,"ps")==0)do_pointsize();
		else if(strcmp(com,"symbol")==0)do_symbol();
		else if(strcmp(com,"sy")==0)do_symbol();
		else if(strcmp(com,"color")==0)do_color();
		else if(strcmp(com,"c")==0)do_color();
		else if(strcmp(com,"font")==0)do_font();
		else if(strcmp(com,"fo")==0)do_font();
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
		else if(strcmp(com,"text")==0)do_text();
		else if(strcmp(com,"t")==0)do_text();
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

	int rgb[3];
	rgb[0]=255;
	rgb[1]=255;
	rgb[2]=255;
	
fprintf(stderr,"doing circle\n");
	if(circle.possible){
		if(lastfat!=circle.fat){
			ps_setline(circle.fat);
			lastfat=circle.fat;
		}
		if(lastcolor!=circle.color){
			setcol(circle.color);
			lastcolor=circle.color;
		}
	r.value;



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
/*	titlex=x[150];*/
	titlex=0;
	titley=y[200]+0.2*r.value;
	z=1.+ r.value;

	/* call ps_plotinit to set up plot */
	/* first parameter is output file, second says not in overlay mode,
		third says use portrait mode
    forth and fifth are origin, six and seventh are scale factors,
		eigth is number of copies
		ninth is dpi for printer, tenth is unit and 0 means cm
		11th is pagewidth, 12th is rgb of paper color, last is an eps structure */
    
fprintf(stderr,"Enter plotinit\n");
	ps_plotinit("", 0, 1, 1+r.value, 1+r.value, 1., 1., 1, 300, 0, 8.5,rgb, gmt_epsinfo(progname));
fprintf(stderr,"Exit plotinit\n");
	/* call ps_setfont to set the default font */
	ps_setfont(DEF_FONT);

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
fprintf(stderr,"doing radius\n");
	if(r.possible)sscanf(comline,"%s %f",com,&r.value);
	else err(WARN,NAME,"%s not possible now.",comline);
}


do_title()
/* reset the title */
{
fprintf(stderr,"doing title\n");
	if(title.possible){
		/* strip out title */
		j=strlen(comline);
		k=0;
		while(comline[k]!=' ')++k;
		for(i=k+1;i<j;++i)cap[i-k-1]=comline[i];
		cap[j]='\0';
	}
	else err(WARN,NAME,"%s not possible now.",comline);
	/* only allow one title */
	title.possible=0;
}

do_fat()
/* reset all fats */
/* largely here for backwards compatibility */
{
fprintf(stderr,"doing fat\n");
	if(fatt.possible){
		sscanf(comline,"%s %d",com,&i);
		i*= 9.09; /* convert to ps units (300/inch) */
		if(i==0)i=1; /* keep things from dissappearing */
		plane.fat=i;
		pole.fat=i;
		rake.fat=i;
		line.fat=i;
		small.fat=i;
		mover.fat=i; /* for consistency */
		drawer.fat=i;
		circle.fat=i;
		title.fat=i;
		great.fat=i;
		text.fat=i;
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}
	
do_reflect()
/* set to reflect viewing */
{
fprintf(stderr,"doing reflect\n");
	if(viewtype.possible)viewtype.value=0;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_through()
/* set to through viewing */
{
fprintf(stderr,"doing through\n");
	if(viewtype.possible)viewtype.value=1;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_schmidt()
/* set to schmidt net */
{
fprintf(stderr,"doing schmidt\n");
	if(nettype.possible)nettype.value=0;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_wulff()
/* set to wulff net */
{
fprintf(stderr,"doing wulff\n");
	if(nettype.possible)nettype.value=1;
	else err(WARN,NAME,"%s not possible now.",comline);
}
	
do_focal()
/* set to focal type values */
{
fprintf(stderr,"doing focal\n");
	if(focal.possible)focal.value=1;
	else err(WARN,NAME,"%s not possible now.",comline);
}
	
do_notfocal()
/* set to not focal type values */
{
fprintf(stderr,"doing notfocal\n");
	if(focal.possible)focal.value=0;
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_pointsize()
/* reset point size for some command */
{
fprintf(stderr,"doing size\n");
	if(size.possible){
		sscanf(comline,"%s %s %lf",com,choice,&z);
		if(strcmp(choice,"pole")==0)pole.size=z;
		else if(strcmp(choice,"line")==0)line.size=z;
		else if(strcmp(choice,"rake")==0)rake.size=z;
		else if(strcmp(choice,"title")==0)title.size=z;
		else if(strcmp(choice,"text")==0)text.size=z;
		/* can't change size for other commands, so... */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_size()
/* reset size for some command */
/* this does text sizes in old units (33/inch) and exists for
backwards compatibility */
{
fprintf(stderr,"doing size\n");
	if(size.possible){
		sscanf(comline,"%s %s %lf",com,choice,&z);
		z*= 2.18; /* conversion to points for use with postscript */
		if(strcmp(choice,"pole")==0)pole.size=z;
		else if(strcmp(choice,"line")==0)line.size=z;
		else if(strcmp(choice,"rake")==0)rake.size=z;
		else if(strcmp(choice,"title")==0)title.size=z;
		else if(strcmp(choice,"text")==0)text.size=z;
		/* can't change size for other commands, so... */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}
		
do_symbol()
/* change symbols as appropriate */
{
fprintf(stderr,"doing symbol\n");
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

do_width()
/* change fatness */
/* uses postscript units */
{
fprintf(stderr,"doing width\n");
	if(fatness.possible){
		sscanf(comline,"%s %s %d",com,choice,&i);
		if(i==0)i=1; /* keep things from disappearing */
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
		else if(strcmp(choice,"text")==0)text.fat=i;
		else if(strcmp(choice,"all")==0){
			plane.fat=i;
			pole.fat=i;
			rake.fat=i;
			line.fat=i;
			small.fat=i;
			mover.fat=i;
			drawer.fat=i;
			title.fat=i;
			circle.fat=i;
			great.fat=i;
			text.fat=i;
		}
		/* can't change fat for other commands */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_fatness()
/* change fatness */
/* uses vplot (33/inch units for backwards compatibility) */
{
fprintf(stderr,"doing fatness\n");
	if(fatness.possible){
		sscanf(comline,"%s %s %d",com,choice,&i);
		i*= 9.09; /* convert to postscript units (300/inch) from vplot (33/inch) */
		if(i==0)i=1; /* keep things from disappearing */
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
		else if(strcmp(choice,"text")==0)text.fat=i;
		/* can't change fat for other commands */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_font()
/* change font */
/* uses gmt font numbers */
{
fprintf(stderr,"doing font\n");
	if(fontt.possible){
		sscanf(comline,"%s %s %d",com,choice,&i);
		if(strcmp(choice,"pole")==0)pole.font=i;
		else if(strcmp(choice,"rake")==0)rake.font=i;
		else if(strcmp(choice,"title")==0)title.font=i;
		else if(strcmp(choice,"text")==0)text.font=i;
		/* can't change font for other commands */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_color()
/* change color */
{
fprintf(stderr,"doing color\n");
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
		else if(strcmp(choice,"text")==0)text.color=i;
		/* can't change color for other commands */
		else err(WARN,NAME,"%s doesn't make sense.",comline);
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_plane()
/* draw a plane */
{
fprintf(stderr,"doing plane\n");
	if(circle.possible)do_circle();
	if(plane.possible){
		if(lastfat!=plane.fat){
	 		ps_setline(plane.fat);
			lastfat=plane.fat;
		}
		if(lastcolor!=plane.color){
			setcol(plane.color);
			lastcolor=plane.color;
		}
		sscanf(comline,"%s %lf %lf",com,&ddir,&dip);
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
fprintf(stderr,"doing great\n");
	if(circle.possible)do_circle();
	if(great.possible){
		if(lastfat!=great.fat){
			ps_setline(great.fat);
			lastfat=great.fat;
		}
		if(lastcolor!=great.color){
			setcol(great.color);
			lastcolor=great.color;
		}
		sscanf(comline,"%s %lf %lf %lf %lf",com,&trend,&plunge,&trend2,&plunge2);
		if(focal.value){
			plunge=90-plunge;
			plunge2=90-plunge2;
		}
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
fprintf(stderr,"doing pole\n");
	if(circle.possible)do_circle();
	if(pole.possible){
		if(lastfat!=pole.fat){
			ps_setline(pole.fat);
			lastfat=pole.fat;
		}
		if(lastcolor!=pole.color){
			setcol(pole.color);
			lastcolor=pole.color;
		}
		sscanf(comline,"%s %lf %lf",com,&ddir,&dip);
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
		syms[0]=pole.lower;
		syms[1]='\0';
		symsz=pole.size;
/*		center(x[0],y[0],sym,symsz);*/
fprintf(stderr,"HERE");
		ps_text(x[0],y[0],symsz,syms,0,6,FORM);
	fprintf(stderr,"FORM = %d\n",FORM);
fprintf(stderr,"THERE");
		fflush(out_file);
		reset_poss();
		rake.possible=1; /*rake possible after pole */
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_rake()
/* draw a rake */
{
fprintf(stderr,"doing rake\n");
	if(circle.possible)do_circle();
	if(rake.possible){
		if(lastfat!=rake.fat){
			ps_setline(rake.fat);
			lastfat=rake.fat;
		}
		if(lastcolor!=rake.color){
			setcol(rake.color);
			lastcolor=rake.color;
		}
		sscanf(comline,"%s %lf",com,&rakeang);
		z=ddir/TORADS;
		z2=dip/TORADS;
		z3=rakeang/TORADS;
		/* slickenside vector calculation */
		s1= -cos(z3)*cos(z)-sin(z3)*sin(z)*cos(z2);
		s2= cos(z3)*sin(z)-sin(z3)*cos(z)*cos(z2);
		s3= sin(z3)*sin(z2);
		syms[0]=rake.lower;
		syms[1]='\0';
		if(s3>0){
			if(viewtype.value)s3= -s3;
				else {
				s1= -s1;
				s2= -s2;
				s3= -s3;
				}
				syms[0]=rake.upper;
				syms[1]='\0';
			}
		dirplg(s1,s2,s3,&z2,&z);
		z= z/TORADS;
		z2= z2/TORADS;
/*		z=rnet2*sin(((PI/2.)-z)/2.);*/
		comrad();
		x[0]=z*sin(z2);
		y[0]=z*cos(z2);
		symsz=rake.size;
/*		center(x[0],y[0],sym,symsz);*/
		ps_text(x[0],y[0],symsz,syms,0,6,FORM);
	fprintf(stderr,"FORM = %d\n",FORM);
		reset_poss();
		rake.possible=1; /* no reason not to draw multiple rakes on a plane */
		}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_line()
/* draw a line */
{
fprintf(stderr,"doing line\n");
	if(circle.possible)do_circle();
	if(line.possible){
		if(lastfat!=line.fat){
			ps_setline(line.fat);
			lastfat=line.fat;
		}
		if(lastcolor!=line.color){
			setcol(line.color);
			lastcolor=line.color;
		}
	/* draw a line */
	sscanf(comline,"%s %lf %lf",com,&trend,&plunge);
	if(focal.value)plunge=90.-plunge;
	if(plunge>=0){
		z= plunge/TORADS;
		z2= trend/TORADS;
		sym=line.lower;
		syms[0]=line.lower;
		syms[1]='\0';
	}
	else {
		z= -plunge/TORADS;
		if(!viewtype.value)z2= (180.+trend)/TORADS;
		else z2= trend/TORADS;
		sym=line.upper;
		syms[0]=line.upper;
		syms[1]='\0';
	}
/*	z=rnet2*sin(((PI/2.)-z)/2.);*/
	comrad();
	x[0]=z*sin(z2);
	y[0]=z*cos(z2);
	symsz=line.size;
	center(x[0],y[0],sym,symsz);
/*		ps_text(x[0],y[0],symsz,syms,0,6,FORM);*/
	fprintf(stderr,"FORM = %d\n",FORM);
	fflush(out_file);
	reset_poss();
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_text()
/* post some text at point trend,plunge */
/* A capital letter symbol will be centered on the point given */
/* command format: t[ext] trend plunge :string(can include spaces) */
{
fprintf(stderr,"doing text\n");
	if(circle.possible)do_circle();
	if(text.possible){
		if(lastfat!=text.fat){
			ps_setline(text.fat);
			lastfat=text.fat;
		}
		if(lastcolor!=text.color){
			setcol(text.color);
			lastcolor=text.color;
		}
	/* draw a text */
	sscanf(comline,"%s %lf %lf",com,&trend,&plunge);
	if(focal.value)plunge=90.-plunge;
	for(i=0;i<LEN;++i)if(comline[i]==':')break;
	++i;
	for(j=i;j<LEN;++j)if(comline[j]=='\n')comline[j]=' ';
	if(plunge>=0){
		z= plunge/TORADS;
		z2= trend/TORADS;
	}
	else {
		z= -plunge/TORADS;
		if(!viewtype.value)z2= (180.+trend)/TORADS;
		else z2= trend/TORADS;
	}
/*	z=rnet2*sin(((PI/2.)-z)/2.);*/
	comrad();
	x[0]=z*sin(z2);
	y[0]=z*cos(z2);
	symsz=text.size;
	z=symsz;
	xc-= z/100.;
	yc-= 3.*z/200.;
/*	Text(xc,yc,symsz,0,&comline[i]);*/
	ps_text(x[0],y[0],symsz,&comline[i],0,5,FORM);
	fprintf(stderr,"FORM = %d\n",FORM);
	fflush(out_file);
	reset_poss();
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_small()
/* draw a small circle */
{
fprintf(stderr,"doing small\n");
	if(circle.possible)do_circle();
	if(small.possible){
		if(lastfat!=small.fat){
			ps_setline(small.fat);
			lastfat=small.fat;
		}
		if(lastcolor!=small.color){
			setcol(small.color);
			lastcolor=small.color;
		}
		sscanf(comline,"%s %lf %lf %lf",com,&trend,&plunge,&dist);
		if(focal.value)plunge=90.-plunge;
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
fprintf(stderr,"doing move\n");
	if(circle.possible)do_circle();
	if(mover.possible){
		sscanf(comline,"%s %lf %lf",com,&trend,&plunge);
		if(focal.value)plunge=90-plunge;
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
		ps_plot(x[0],y[0],3);
		fflush(out_file);
		reset_poss();
		drawer.possible=1;
	}
	else err(WARN,NAME,"%s not possible now.",comline);
}

do_draw()
/* draw to new point */
{
fprintf(stderr,"doing draw\n");
	if(circle.possible)do_circle();
	if(drawer.possible){
		if(lastfat!=drawer.fat){
			ps_setline(drawer.fat);
			lastfat=drawer.fat;
		}
		if(lastcolor!=drawer.color){
			setcol(drawer.color);
			lastcolor=drawer.color;
		}
		sscanf(comline,"%s %lf %lf",com,&trend,&plunge);
		if(focal.value)plunge=90-plunge;
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
		ps_plot(x[0],y[0],2);
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
fprintf(stderr,"doing error\n");
	fprintf(error_file,"%s ",p1);
	fprintf(error_file,"%s: ",p2);
	fprintf(error_file,p3,p4);
	fprintf(error_file,"\n");
}

drline(nn,xx,yy)
int nn;
double xx[],yy[];
{
fprintf(stderr,"doing line\n");
	ps_line(xx,yy,nn,3,0,1);
	return;
}

comrad()
{
/* get the radius needed to plot the point */
	if(nettype.value)z=r.value*tan((PI/4.)-z/2.);
	else z=rnet2*sin(((PI/2.)-z)/2.);
}

setcol(i)
int i;
{
fprintf(stderr,"doing setcol\n");
/* set an rgb color based on a number 0 to 9 */

	if(i==1)ps_setpaint(0,0,255);
	else if(i==2)ps_setpaint(255,0,0);
	else if(i==3)ps_setpaint(160,32,240);
	else if(i==4)ps_setpaint(0,255,0);
	else if(i==5)ps_setpaint(135,206,255);
	else if(i==6)ps_setpaint(255,255,0);
	else if(i==7)ps_setpaint(255,255,255);
	else ps_setpaint(0,0,0);
	return;
}
