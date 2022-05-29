BEGIN_DEF = Linux X86 Makefile
SYSLIBS=-L/usr/ucblib -ltermcap -lucb -lm
BINDIR=../../../bin
CFLAGS = -c -O6 -DUNDERSCOREFORTRAN -DLINUX
END_DEF = ****************************

DEFINEHELPDIR = /usr/local/pub/tools/src/Utils/inter

SRCS = main.c options.c labels.c moddata.c menu.c help.c \
       messages.c linefit.c runfile.c system.c myfgets.c  \
       hiddenplot.c filter.c getcol.c intermenus.c Get.c

OBJS = main.o options.o labels.o moddata.o menu.o help.o \
       messages.o linefit.o runfile.o system.o myfgets.o  \
       hiddenplot.o filter.o getcol.o intermenus.o Get.o

all: inter

help.o: help.c
	-$(CC) $(CFLAGS) -DHELPDIR=\"$(DEFINEHELPDIR)\" -c help.c

.c.o:
	-$(CC) $(CFLAGS) $(INCDIR) $(SRCS) $(EXTRA)

inter: $(OBJS)
	-$(CC) -o inter $(OBJS) $(SYSLIBS)

clean::
	rm -f *.o inter

install::
	-cp -p inter $(BINDIR)
	-cp -p inter.help $(BINDIR)
	-chmod 775 $(BINDIR)/inter
	-chmod 664 $(BINDIR)/inter.help
