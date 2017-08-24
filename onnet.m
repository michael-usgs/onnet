.\"  This is a skeleton Unix manual page entry in unformatted form.
.\"  You must replace lowercase text with the appropriate values for your
.\"  command.  If a section is not applicable (e.g., "OPTIONS"), you can
.\"  leave out everything from the .SH control symbol for that section 
.\"  up to (not including) the .SH control symbol for the next section.
.\"  The NAME, SYNOPSIS, and DESCRIPTION sections are required.
.\"  Use the 'man' macro package to print ('nroff -man manskel').
.\"  For more information on the macros, see the manual entry for man(7)
.\"  (type 'man 7 man').
.TH onnet  L "13 February 1986"          .\"  Date in form: 1 August 1985
.SH NAME
onnet \- general utility stereonet plotting program
.PP
.SH SYNOPSIS
onnet [options] [in_file2 in_file3]
.PP                         .\"  Separates paragraphs
onnet also accepts piped input and output, the output is in vplot.
.PP
.SH DESCRIPTION
.PP
onnet is a program that allows the plotting of planes, lines, poles,
slip vectors, small circles, great circle segments, and arbitrary
shapes on either Schmidt (equal-area) or Wulff (equal-angle)
projection stereonets.  The input is in fairly plain english, see
the onnet users manual for more details.
.PP
.SH OPTIONS
.PP
.TP 
.B in=string 
string is an input file containing onnet commands.
.PP
.TP 
.B out=string 
string is an output file containing vplot format commands.
.PP
.TP
.B error=string
string is the error file.
.PP
.SH SEE ALSO
.PP
Onnet commands are described in the onnet users manual.  You should
also see the programs stnet and sttics.
.PP
.SH AUTHOR
Andrew Michael
.PP
.SH BUGS
Small circles do not always reach the edge of the stereonet when going
from one hemisphere to the other.  Bizarre (very large out of range angles)
can cause problems.  Great circle segments that go from the upper
hemisphere to the lower hemisphere do not work in reflect viewing mode.
