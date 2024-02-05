#CC=ppc_8xx-gcc
CC=gcc
#CFLAGS=-Wall -g -DLINUX 
CFLAGS= -DLINUX -Wall -fsigned-char -fno-common 
#CFLAGS= -g -DLINUX 
INCLUDES=-I.
LIBS=

OBJS=gpp_header.o

PROG=gpp_header

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o $(PROG)

clean:

	rm -f *.o $(PROG)



