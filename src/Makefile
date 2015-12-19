# File: Makefile

##
## Note that you may have to make modifications below according
## to your machine, available libraries, compilation options,
## and your "visual module" of choice.  See below...
##
## Note that "main-ibm.c", "main-emx.c", and "main-win.c", all
## visual module files for IBM-ish computers, have their own
## Makefiles, see "Makefile.xxx".  And the Macintosh does not
## use a Makefile, but a Think C 6.0 project file, see elsewhere.
##

##
## BINDIR is the directory where the angband binary will be installed
##

BINDIR = /User/games/bin
PROGRAM = angband

##
## For SunOS 5.X (Solaris 2.X), if you don't have the BSD/SunOS 4.x
## compatibility kit.  Also, be sure to define NEEDS_USLEEP in config.h
# EXTRAOBJS=random.o bzero.o

## Otherwise, just use this:
EXTRAOBJS=

SOURCES = main.c misc1.c misc2.c store1.c files.c io.c create.c desc.c\
	generate.c sets.c dungeon.c creature.c death.c eat.c help.c magic.c\
	potions.c prayer.c save.c staffs.c wands.c scrolls.c spells.c\
	wizard.c store2.c signals.c moria1.c moria2.c monsters.c\
	treasure.c variable.c recall.c unix.c undef.c\
	player.c tables.c describe_mon.c rods.c random.c util.c bzero.c

OBJECTS = main.o misc1.o misc2.o store1.o files.o io.o create.o desc.o\
	generate.o sets.o dungeon.o creature.o death.o eat.o help.o magic.o\
	potions.o prayer.o save.o staffs.o wands.o scrolls.o spells.o\
	wizard.o store2.o signals.o moria1.o moria2.o monsters.o\
	treasure.o variable.o recall.o unix.o undef.o\
	player.o tables.o describe_mon.o rods.o util.o $(EXTRAOBJS)

LIBFILES = hours news origcmds.hlp owizcmds.hlp roglcmds.hlp rwizcmds.hlp \
	scores version.hlp welcome.hlp

###
### Compiler flags
###
### '-O' means optimize.  '-g' is for debugging.
###
### If you are using gcc, you may combine both '-O' and '-g'.  Also:
### '-pipe' uses pipes rather than temporary files (which is faster).
### '-pg' enables profiling (for coders only).  '-W<something>' provides
### additional warnings.
###
### To use an "alternative" definition, simply "modify" (or "replace")
### the definition below with one that you like.  For example, you can
### change the compiler to "cc".
###
### See also "config.h" and "h-config.h".
###
### Some "examples" are given below, they can be used by simply
### remove one of the '#' signs from the list of compiler flag choices below:
###


#
# This is my compiler of choice, it seems to work most everywhere
#
CC = gcc


## For normal compilers (ie, 'cc' on anything but a NeXT).
## Once you are sure it works, you can and should use '-O' instead.
# CFLAGS = -Wall -O1

## For HPUX, the compiler flags for 'cc' should be:
#CFLAGS = -Ae -Dunix -g

##
## For gcc (or NeXT) users only.
##
## Normal gcc users:
## For SunOS 5.X (Solaris 2.X), add '-DSOLARIS'
CFLAGS = -g -O1 -pipe

## Full optimization (but only when you know it works right!):
#CFLAGS = -O4 -pipe -fomit-frame-pointer

## Developers:
#CFLAGS = -g -O1 -pipe -Wall -Wcast-align -Wpointer-arith -Wwrite-strings

## Anal developers:     [ :-) ]
#CFLAGS = -g -O1 -pipe -Wall -Wcast-align -Wpointer-arith -Wwrite-strings \
 -Wshadow -Wcast-align -Waggregate-return 

## Profiling:
#CFLAGS = -pg -g -O1 -pipe-Wall -Wcast-align -Wpointer-arith -Wwrite-strings

##
## Libraries to link from
##
## some people might be able to (or even might have to) remove '-lrpcsvc'

LIBS = -lcurses -ltermcap -lrpcsvc

##
## For Linux systems, this line should look like:
# LIBS = -lncurses -ltermcap -lbsd

##
## For NCR System 3000, you'll probably need this:
# LIBS=-lcurses -ltermcap -lmalloc -L/usr/ucblib -lucb

##
## For SunOS 5.X (Solaris 2.X), this line should look like:
# LIBS = -ltermlib -ltermcap -lrpcsvc -lnsl -lucb

## For SunOS 5.X (Solaris 2.X), this line should look like:
## ....unless you don't have the BSD/SunOS 4.x compatibility kit:
# LIBS = -ltermlib -ltermcap -lrpcsvc -lnsl


##
## If your system lacks the random() call, define EXTRAOBJS to include
## random.o.  Otherwise, you can leave it out.

#EXTRAOBJS=random.o



default: all

all: $(PROGRAM)

.c.o:
	$(CC) $(CFLAGS) -c $*.c

#
# Build the "Angband" program.
#

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(PROGRAM) $(LIBS)

install:	$(PROGRAM)
	cp $(PROGRAM) $(BINDIR)/$(PROGRAM)
	chown games.bin $(BINDIR)/$(PROGRAM)
	chmod 4511 $(BINDIR)/$(PROGRAM)

clean:;
	rm -f *.o ./core *~ .*~ temp1 temp2 gmon.*

realclean: clean
	rm -f $(PROGRAM)

spotless: clean
	rm -f $(PROGRAM)
	rm -f $(BINDIR)/$(PROGRAM)

#
# Generate dependancies automatically (not required)
#

depend:
	makedepend $(INCLUDES) -p$(OBJDIR)/ $(SRCS)

####
# verify that the monsters.c and describe_mon.c files correspond

check:
	@/bin/rm -f temp1 temp2
	@egrep '^{' monsters.c | awk -F\" '{print $$2}' > temp1
	@egrep '^{' describe_mon.c | awk -F\" '{print $$2}' > temp2
	@echo Monster name problems present:
	@diff -c1 temp1 temp2

query:
	@echo "Monsters.c:"
	egrep '^{"' monsters.c | wc -l
	@echo "Describe_Mon.c:"
	egrep '^{"' describe_mon.c | wc -l

create.o: constant.h types.h externs.h config.h
creature.o: constant.h types.h externs.h config.h monster.h
death.o: constant.h types.h externs.h config.h
desc.o: constant.h types.h externs.h config.h
dungeon.o: constant.h types.h externs.h config.h
eat.o: constant.h types.h externs.h config.h
files.o: constant.h types.h externs.h config.h
generate.o: constant.h types.h externs.h config.h
help.o: constant.h types.h externs.h config.h monster.h
io.o: constant.h types.h externs.h config.h
magic.o: constant.h types.h externs.h config.h
main.o: constant.h types.h externs.h config.h
misc1.o: constant.h types.h externs.h config.h monster.h
misc2.o: constant.h types.h externs.h config.h monster.h
monsters.o: constant.h types.h config.h monster.h
moria1.o: constant.h types.h externs.h config.h monster.h
moria2.o: constant.h types.h externs.h config.h monster.h
player.o: constant.h types.h config.h
potions.o: constant.h types.h externs.h config.h
prayer.o: constant.h types.h externs.h config.h monster.h
recall.o: constant.h config.h types.h externs.h monster.h
rnd.o: constant.h types.h
save.o: constant.h types.h externs.h config.h
scrolls.o: constant.h types.h externs.h config.h monster.h
sets.o: constant.h config.h types.h
signals.o: constant.h types.h externs.h config.h
spells.o: constant.h types.h externs.h config.h monster.h
staffs.o: constant.h types.h externs.h config.h monster.h
store1.o: constant.h types.h externs.h config.h
store2.o: constant.h types.h externs.h config.h
tables.o: constant.h types.h config.h
treasure.o: constant.h types.h config.h
unix.o: constant.h config.h types.h externs.h
variable.o: constant.h types.h config.h
rods.o: constant.h types.h externs.h config.h monster.h
wands.o: constant.h types.h externs.h config.h monster.h
wizard.o: constant.h types.h externs.h config.h
describe_mon.o: monster.h externs.h types.h

# DO NOT DELETE THIS LINE -- make depend depends on it.

