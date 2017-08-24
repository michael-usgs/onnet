#include <stdio.h>
#define FORM 0

center(x,y,symbol,symbsz)
double x,y;
char symbol;
int symbsz;
{
	float ch,chd2,cw,cwd2;
	float xc,yc;
	char string[80];

	double units;
	double deltvert;
	double delthoriz;

	string[0]=symbol;
	string[1]='\0';


	units=symbsz; /* get a symbol size dependent unit */
	units/=72; /* convert points to inches */
	units*=2.54; /* convert inches to cm */

	if(strchr("ABCDEFGHIJKLMNOPQRSTUVWXYX1234567890!@#$%&|<>?-=",symbol)!=NULL){
		deltvert=units/3.3; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	if(strchr("(){}[]",symbol)!=NULL){
		deltvert=units/4.1; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr("aceimnoprsuvwxz;:",symbol)!=NULL){
		deltvert=units/4.5; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr("bdfhklt",symbol)!=NULL){
		deltvert=units/3.3; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr("gjpqy",symbol)!=NULL){
		deltvert=units/8.0; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr(".",symbol)!=NULL){
		deltvert=units/20.0; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr(",",symbol)!=NULL){
		deltvert=units/40.0; /* adjust height for capitals */
		delthoriz=units/4.3; /* adjust width for capitals */
	}
	else if (strchr("+",symbol)!=NULL){
		deltvert=units/3.6; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr("*",symbol)!=NULL){
		deltvert=units/2.4; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr("^",symbol)!=NULL){
		deltvert=units/2.1; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else if (strchr("_",symbol)!=NULL){
		deltvert= -1.*units/4.0; /* adjust height for capitals */
		delthoriz=units/3.3; /* adjust width for capitals */
	}
	else {
		fprintf(stderr,"warning onnet does not know how to center this symbol: %c\n",symbol);
	}


fprintf(stderr,"deltvert= %g\n",deltvert);

	ps_text(x-delthoriz,y-deltvert,symbsz,string,0,5,FORM);
	fprintf(stderr,"FORM = %d\n",FORM);

	return;
}
