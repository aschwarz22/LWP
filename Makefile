CC 	= gcc

CFLAGS  = -Wall -g -I .

LD 	= gcc

LDFLAGS  = -Wall -g -L.

PUBFILES =  README  hungrymain.c  libPLN.a  libsnakes.a  lwp.h\
	    numbersmain.c  snakemain.c  snakes.h

TARGET =  .

PROGS	= snakes nums hungry

SNAKEOBJS  = snakemain.o 

HUNGRYOBJS = hungrymain.o 

NUMOBJS    = numbersmain.o

OBJS	= $(SNAKEOBJS) $(HUNGRYOBJS) $(NUMOBJS) 

SRCS	= snakemain.c numbersmain.c

HDRS	= 

EXTRACLEAN = core $(PROGS)

all: 	$(PROGS)

allclean: clean
	@rm -f $(EXTRACLEAN)

clean:	
	rm -f $(OBJS) *~ TAGS

snakes: snakemain.o libPLN.a libsnakes.a
	$(LD) -o rs snakemain.c -L. -lPLN -lsnakes -lncurses

hungry: hungrymain.o libPLN.a libsnakes.a
	$(LD) -o hs hungrymain.c -L. -lPLN -lsnakes -lncurses

nums: numbersmain.o libPLN.a 
	$(LD) -o nums numbersmain.c -L. -lPLN -lsnakes

hungrymain.o: lwp.h snakes.h

snakemain.o: lwp.h snakes.h

numbermain.o: lwp.h

#libPLN.a: ../Publish/lwp.c
#	gcc -c ../Publish/lwp.c
#	ar r libPLN.a lwp.o
#	rm lwp.o

pub:
	scp $(PUBFILES) $(TARGET)


