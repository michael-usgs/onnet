.\"  This is a skeleton Unix manual page entry in unformatted form.
.\"  You must replace lowercase text with the appropriate values for your
.\"  command.  If a section is not applicable (e.g., "OPTIONS"), you can
.\"  leave out everything from the .SH control symbol for that section 
.\"  up to (not including) the .SH control symbol for the next section.
.\"  The NAME, SYNOPSIS, and DESCRIPTION sections are required.
.\"  Use the 'man' macro package to print ('nroff -man manskel').
.\"  For more information on the macros, see the manual entry for man(7)
.\"  (type 'man 7 man').
.TH sttics  L "13 February 1986"          .\"  Date in form: 1 August 1985
.SH NAME
sttics \- stereonet tic mark plotting program
.PP
.SH SYNOPSIS
sttics [options]
.PP                         .\"  Separates paragraphs
.SH DESCRIPTION
.PP
ststics generates the onnet command file to plot tic marks on a
stereonet.  This is useful for overlaying tic marks on onnet plots.
For instance the command:
.br
sttics | onnet in=file | ipen 
.br
will overlay tic marks with the onnet plot in file and plot it
on the imagen.
.PP
.SH OPTIONS
.PP
.TP 
.B out=string 
string is an output file containing vplot format commands.
.PP
.TP
.B error=string
string is the error file.
.PP
.TP
.B azstep=float
gcstep is the angular step size in the azimuth of the tics in degrees.
The default is 10.
.PP
.TP
.B plgstep=float
plgstep is the angular step size in the plunge of the tics in degrees.
The default is 10.
.PP
.TP
.B radius=float
radius of the plot in centimeters.  The default is 4.
.PP
.TP
.B type=string
the string is the type of projection to be used, either wulff or
schmidt.  The default is schmidt.
.SH SEE ALSO
.PP
There is a users manual with examples for sttics.  Also see the
programs onnet and stnet.
.SH AUTHOR
Andrew Michael
.PP
.SH BUGS
None known.
