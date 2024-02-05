#CC=ppc_8xx-gcc
CC=gcc
#CFLAGS=-Wall -g -DLINUX 
CFLAGS= -DLINUX -Wall -fsigned-char -fno-common 
#CFLAGS= -g -DLINUX 
INCLUDES=-I.
LIBS=

OBJS=hpmimage.o
     
PROG=hpmimage

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o $(PROG)

clean:

	rm -f *.o $(PROG)

