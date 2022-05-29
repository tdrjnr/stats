#include "menudef.h"

int dummyfn();
char nothinghelp[] = "Do nothing, return to inter menu";

int logx(), logy(), logxy();
int limx(), limy(), setxmin(), setymin(), setxmax(), setymax();
int grid_none(), grid_frame(), grid_arrow(), grid_full(), grid_box();
int move_up(), move_right(), aspect();
int xtickfull(), xticksemi(), xticknone();
int ytickfull(), yticksemi(), yticknone();
int xtickmark(), ytickmark();
int nonumberx(), nonumbery();
int clip_on(), clip_off();
int autoset(), transpose(), errorbars(), height(), width(), noclear();
int movie_mode();
int ignore(), xcolumn(), adjustdata(), addcurves(), subcurves(), logdata();
int dividecurves(), multiplycurves();
int sindata(), cosdata(), sqrtdata(), squaredata(), invertdata(), absdata();
int setmod(), moddata(), cubedata();
int linefit(), adjust_scale(), adjust_shift(), adjust_shift_increment();
int c_logstart(), c_logstop();
int labelxsize(), labelysize(), titlesize(), pointsize(), numbersize();
int labelxpos(), labelypos(), titlepos();
int pointlabel(), pointoff(), helpchars();
int labelx(), labely(), title(), arblabel(), curvelabel();
int histogram(), histbins(), hist_normal(), histweight();
int sethistmin(), sethistmax(), sethistlog(), sethistauto(), sethistbaseline();
int line_solid(), line_dotted(), line_dashed(), line_dotdashed();
int line_longdashed(), line_none(), line_style();
int setxrangemin(), setxrangemax(), setyrangemin(), setyrangemax();
int unsetxrange(), unsetyrange();
int namefile(), plot(), reset(), done(), help(), include_file(), retplot();
int include_next(), forgetdata();
int appendfile(), appendnextfile(), appendallfiles(), dupcolumn();
int hiddenplot();
int nextfile(), run_file(), ifyes();
int set_filter(), clear_filter(), show_filter(), set_tbirdfilter();
int set_inputspacing(), set_inputskip();
int changedir(), pwd(), cdback(), lpq(), ls(), c_system();
int rereadline();

MENU sysmenu = {
   "system",
  {{"!", c_system, NULLMENU,
      "execute rest of line as a system command (sh, not csh)"},
   {"back", cdback, NULLMENU, "go back to old directory"},
   {"cd", changedir, NULLMENU, "change directory"},
   {"lpq", lpq, NULLMENU, "check printer queue"},
   {"ls", ls, NULLMENU, "list directory"},
   {"pwd", pwd, NULLMENU, "print working directory"},
   {"help", help, NULLMENU, "help <command> gives help on a command"},
   {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU logmenu = {
   "log axes",
   {{"x", logx, NULLMENU, "make x axis logarithmic"},
    {"y", logy, NULLMENU, "make y axis logarithmic"},
    {"xy", logxy, NULLMENU, "make x and y axes logarithmic"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

char limxhelp[] =
   "set xmin and xmax to <arg><arg>.  If arguments are not given,\n\
    and a curve has been plotted, the defaults will be its x min and max.";
char limyhelp[] =
   "set ymin and ymax to <arg><arg>.  If arguments are not given,\n\
    and a curve has been plotted, the defaults will be its y min and max.";

MENU limitmenu = {
    "limit",
   {{"x", limx, NULLMENU, limxhelp},
    {"y", limy, NULLMENU, limyhelp},
    {"xmin", setxmin, NULLMENU, "set x min to <arg>"},
    {"ymin", setymin, NULLMENU, "set y min to <arg>"},
    {"xmax", setxmax, NULLMENU, "set x max to <arg>"},
    {"ymax", setymax, NULLMENU, "set y max to <arg>"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU filtermenu = {
   "filter",
  {{"set", set_filter, NULLMENU, "filter input files through <arg>"},
   {"tbird", set_tbirdfilter, NULLMENU,
      "filter input files through \"tbird -p <arg>\""},
   {"clear", clear_filter, NULLMENU, "don't filter input files"},
   {"show", show_filter, NULLMENU, "show current filter"},
   {"help", help, NULLMENU, "help <command> gives help on a command"},
   {"nothing", dummyfn, NULLMENU, nothinghelp},
   ENDMENU}
};

MENU gridmenu = {
    "grid style",
   {{"arrow", grid_arrow, NULLMENU, "draw arrows at x=0, y=0"},
    {"box", grid_box, NULLMENU,
      "draw a bow without tick marks or numbers (newaxis only)"},
    {"frame", grid_frame, NULLMENU, "draw a box with tick marks and numbers"},
    {"full", grid_full, NULLMENU, "draw a full grid"},
    {"none", grid_none, NULLMENU, "don't draw any axes"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU movemenu = {
    "move",
   {{"up", move_up, NULLMENU, "move the graph up the page by <arg>"},
    {"right", move_right, NULLMENU, "move the graph to the right by <arg>"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU xtickmenu = {
   "x tick style",
  {{"full", xtickfull, NULLMENU, "major and minor x tick marks (newaxis only)"},
   {"none", xticknone, NULLMENU, "no x tick marks (newaxis only)"},
   {"semi", xticksemi, NULLMENU, "major x tick marks only (newaxis only)"},
   {"help", help, NULLMENU, "help <command> gives help on a command"},
   {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU ytickmenu = {
   "y tick style",
  {{"full", ytickfull, NULLMENU, "major and minor y tick marks (newaxis only)"},
   {"none", yticknone, NULLMENU, "no y tick marks (newaxis only)"},
   {"semi", yticksemi, NULLMENU, "major y tick marks only (newaxis only)"},
   {"help", help, NULLMENU, "help <command> gives help on a command"},
   {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU tickstylemenu = {
   "tick style",
   {{"x", commands, &xtickmenu, "change x tick style (newaxis only)"},
    {"y", commands, &ytickmenu, "change y tick style (newaxis only)"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

char xtickhelp[] = "set x tickmark spacing to <arg>. Do this before 'lim x'";
char ytickhelp[] = "set y tickmark spacing to <arg>. Do this before 'lim y'";

MENU tickmenu = {
    "tick spacing",
   {{"x", xtickmark, NULLMENU, xtickhelp},
    {"y", ytickmark, NULLMENU, ytickhelp},
    {"style", commands, &tickstylemenu, "change tick style (newaxis only)"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU numbermenu = {
    "no numbers",
   {{"x", nonumberx, NULLMENU, "no numbers on x axis (newaxis only)"},
    {"y", nonumbery, NULLMENU, "no numbers on y axis (newaxis only)"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU clipmenu = {
    "clip",
   {{"on", clip_on, NULLMENU, "turn clipping on"},
    {"off", clip_off, NULLMENU, "turn clipping off"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU xrangemenu = {
    "x range",
   {{"minimum", setxrangemin, NULLMENU, "only plot x > <arg>"},
    {"maximum", setxrangemax, NULLMENU, "only plot x < <arg>"},
    {"off", unsetxrange, NULLMENU, "turn off limits on x range"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU yrangemenu = {
    "y range",
   {{"minimum", setyrangemin, NULLMENU, "only plot y > <arg>"},
    {"maximum", setyrangemax, NULLMENU, "only plot y < <arg>"},
    {"off", unsetyrange, NULLMENU, "turn off limits on y range"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU rangemenu = {
    "range",
   {{"x", commands, &xrangemenu, "set range of x values to plot"},
    {"y", commands, &yrangemenu, "set range of y values to plot"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

char adjusthelp[] = "shift and scale column <arg>.\n\
    Use 'opt shift' and 'opt scale' to set the shift and scale factors.";

MENU optionmenu = {
    "options",
   {{"abs", absdata, NULLMENU, "take absolute value of data in column <arg>"},
    {"addcurves", addcurves, NULLMENU, "add column <arg1> to column <arg2>"},
    {"adjustdata", adjustdata, NULLMENU, adjusthelp},
    {"aspectratio", aspect, NULLMENU, "set ratio of plotted x and y ranges"},
    {"automatic", autoset, NULLMENU,
      "Use automatic abscissas. x starts at <arg1> and increments by <arg2>\n\
       Omitted arguments are not changed.\n\
       \"option xcolumn\" can be used to cancel automatic abscissas."},
    {"clip", commands, &clipmenu, "turn clipping on or off (newaxis only)"},
    {"cosine", cosdata, NULLMENU, "take cosine of data in column <arg>"},
    {"divide", dividecurves, NULLMENU, "divide column <arg2> by <arg1>"},
    {"duplicate", dupcolumn, NULLMENU, "duplicate a column of data"}, 
    {"errorbars", errorbars, NULLMENU, "data does/doesn't have errorbars"},
    {"fit_line", linefit, NULLMENU, "fit a line to column <arg>"},
    {"grid", commands, &gridmenu, "choose a grid style"},
    {"height", height, NULLMENU, "set height of plot (normal = 1)"},
    {"ignore", ignore, NULLMENU, "ignore/don't ignore axis commands in data"},
    {"invert", invertdata, NULLMENU, "take inverse of data in column <arg>"},
    {"logaxes", commands, &logmenu, "make x or y axes logarithmic"},
    {"logdata", logdata, NULLMENU, "take log10 of column <arg>"},
    {"modulus", setmod, NULLMENU, "set modulus to use with moddata"},
    {"moddata", moddata, NULLMENU, "take mod of column <arg>"},
    {"moviemode", movie_mode, NULLMENU, "clear screen between curves"}, 
    {"multiply", multiplycurves, NULLMENU, "multiply column <arg2> by <arg1>"},
    {"noclear", noclear, NULLMENU, "don't clear the page before plotting"},
    {"nonumber", commands, &numbermenu,
          "turn off numbers on axes (newaxis only)"},
    {"range", commands, &rangemenu, "set range of values to plot"},
    {"scale", adjust_scale, NULLMENU, "set adjustdata scale factor to <arg>"},
    {"shift", adjust_shift, NULLMENU, "set adjustdata shift to <arg>"},
    {"shift_increment", adjust_shift_increment, NULLMENU,
           "increment adjustdata shift by <arg> each time it is used"},
    {"sine", sindata, NULLMENU, "take sine of data in column <arg>"},
    {"square", squaredata, NULLMENU, "take square of data in column <arg>"},
    {"cube", cubedata, NULLMENU, "take cube of data in column <arg>"},
    {"sqrt", sqrtdata, NULLMENU, "take square root of data in column <arg>"},
    {"skip_lines", set_inputskip, NULLMENU, "skip <arg> initial data lines"},
    {"space_lines", set_inputspacing, NULLMENU,"use every <arg>th data line"},
    {"subtract", subcurves, NULLMENU, "subtract col <arg1> from col <arg2>"},
    {"tick", commands, &tickmenu, "set x or y tickmark spacing and style"},
    {"transpose", transpose, NULLMENU, "turn the plot on its side"},
    {"width", width, NULLMENU, "set width of plot (normal = 1)"},
    {"xcolumn", xcolumn, NULLMENU, "get x data from column <arg>"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU savemenu = {
    "save",
   {{"file", c_logstart, NULLMENU, "save commands in file <arg>"},
    {"off", c_logstop, NULLMENU, "stop saving commands"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU labelsizemenu = {
    "label size",
   {{"x", labelxsize, NULLMENU, "set size of x axis label to <arg>"},
    {"y", labelysize, NULLMENU, "set size of y axis label to <arg>"},
    {"title", titlesize, NULLMENU, "set size of title to <arg>"},
    {"points", pointsize, NULLMENU, "set size of point labels to <arg>"},
    {"numbers", numbersize, NULLMENU, "set size of numbers on axes to <arg>"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU labelposmenu = {
    "label position",
   {{"xaxis", labelxpos, NULLMENU, "set position of x axis label"},
    {"yaxis", labelypos, NULLMENU, "set position of y axis label"},
    {"title", titlepos, NULLMENU, "set position of title"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU pointmenu = {
    "points",
   {{"on", pointlabel, NULLMENU, "label points with character <arg>"},
    {"off", pointoff, NULLMENU, "stop labelling points"},
    {"list", helpchars, NULLMENU, "list special axis characters"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

char arbhelp[] = "label arbitrary point <arg><arg> with character <arg>";
char curvehelp[] = "label a point in curve <arg>.\n\
    The second argument tells which point in the curve\n\
    ('$' means the last pt), and the third argument is the label.";
char poshelp[] = "set position of xy labels and title";

MENU labelmenu = {
   "label",
   {{"arbitrary_point", arblabel, NULLMENU, arbhelp},
    {"curve", curvelabel, NULLMENU, curvehelp},
    {"datapoints", commands, &pointmenu, "label data points"},
    {"position", commands, &labelposmenu, poshelp},
    {"size", commands, &labelsizemenu, "set size of labels"},
    {"title", title, NULLMENU, "set title to remainder of input line"},
    {"xaxis", labelx, NULLMENU, "set x axis label to remainder of input line"},
    {"yaxis", labely, NULLMENU, "set y axis label to remainder of input line"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

char histnormhelp[] = "normalize histogram area to <arg>.\n\
    normalization = 0 means y will be number of counts per bin.";

MENU histmenu = {
    "histogram",
   {{"bins", histbins, NULLMENU, "set number of histogram bins to <arg>"},
    {"log", sethistlog, NULLMENU, "toggle log y for histograms"},
    {"auto", sethistauto, NULLMENU, "toggle automatic limits for histograms"},
    {"max", sethistmax, NULLMENU, "set maximum value for histogram"},
    {"min", sethistmin, NULLMENU, "set minimum value for histogram"},
    {"normalize", hist_normal, NULLMENU, histnormhelp},
    {"plot", histogram, NULLMENU, "plot a histogram from column <arg>"},
    {"weight", histweight, NULLMENU, "set column for histogram weights"},
    {"baseline", sethistbaseline, NULLMENU, "toggle drawing of histogram baseline"},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

char stylehelp[] = "set line style to <arg>\n\
        0   no lines\n\
        1   solid lines\n\
        2   dotted lines\n\
        3   dot-dashed lines\n\
        4   short dashed lines\n\
        5   long dashed lines";

MENU linemenu = {
    "line styles",
   {{"solid", line_solid, NULLMENU, "lines will be solid"},
    {"dotted", line_dotted, NULLMENU, "lines will be dotted"},
    {"dashed", line_dashed, NULLMENU, "lines will be dashed"},
    {"longdashed", line_longdashed, NULLMENU, "lines will be long-dashed"},
    {"dotdash", line_dotdashed, NULLMENU, "lines will be dot-dashed"},
    {"none", line_none, NULLMENU, "plot points only, no lines"},
    {"style", line_style, NULLMENU, stylehelp},
    {"help", help, NULLMENU, "help <command> gives help on a command"},
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU appendmenu = {
    "append", 
   {{"file", appendfile, NULLMENU, "append file <arg> to data set"}, 
    {"next", appendnextfile, NULLMENU, "append next startup file to data set"}, 
    {"all", appendallfiles, NULLMENU, "append all startup files to data set"}, 
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

MENU includemenu = {
    "include", 
   {{"file", include_file, NULLMENU, "include an axis file verbatim"}, 
    {"next", include_next, NULLMENU, "include next startup file verbatim"}, 
    {"nothing", dummyfn, NULLMENU, nothinghelp},
    ENDMENU}
};

char helphelp[] = "'help <command>' gives help on a command.\n\
    'help' by itself gives generally helpful remarks on inter.";

MENU rootmenu = {
    "inter",
   {{"append", commands, &appendmenu, "append file to data set"}, 
    {"done", done, NULLMENU, "quit inter and draw the plot"},
    {"file", namefile, NULLMENU, "read file <arg>"},
    {"filter", commands, &filtermenu, "Set and clear filters for input files.\n\
     A filter can be any external program that reads from standard\n\
     input and writes to standard output. Data files will be preprocessed\n\
     by the filter before being read by inter."},
    {"forget", forgetdata, NULLMENU, "discard all data"}, 
    {"help", help, NULLMENU, helphelp},
    {"hiddenplot", hiddenplot, NULLMENU,
        "make a hidden line plot of columns <arg>"},
    {"histogram", commands, &histmenu, "make a histogram"},
    {"if", ifyes, NULLMENU,
      "ask question <arg> and run next line if reply is 'y'"},
    {"include", commands, &includemenu, "include axis files in the plot"},
    {"label", commands, &labelmenu, "set labels, sizes, and positions"},
    {"limit", commands, &limitmenu, "set limits on x and y"},
    {"line", commands, &linemenu, "choose a line style"},
    {"logaxes", commands, &logmenu, "make x or y axes logarithmic"},
    {"move", commands, &movemenu, "move the plot around on the page"},
    {"next_file", nextfile, NULLMENU,
      "read the next data file if more than one was given when starting up"},
    {"option", commands, &optionmenu, "set a variety of options"},
    {"plot", plot, NULLMENU,
      "plot column <arg> of current data file. <arg> = * plots all columns"},
    {"quit", done, NULLMENU, "quit inter and draw the plot"},
    {"repeat", rereadline, NULLMENU, "repeat previous command line"},
    {"reset", reset, NULLMENU, "reset all inter flags, clear data"},
    {"returnplot", retplot, NULLMENU, "make a return plot of columns <arg>"},
    {"run_file", run_file, NULLMENU, "read commands from a file"},
    {"save", commands, &savemenu, "turn command saving on and off"},
    {"system", commands, &sysmenu, "cd, back, other system stuff"},
    {"nothing", dummyfn, NULLMENU, ""},
    ENDMENU}
};
