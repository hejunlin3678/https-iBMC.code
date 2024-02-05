#CC=ppc_8xx-gcc
CC=gcc
#CFLAGS=-Wall -g -DLINUX 
CFLAGS= -DLINUX
#CFLAGS= -g -DLINUX 
INCLUDES=-I./include
LIBPATH= -L./lib
LIBS= -lm -lpthread -lsecurec

OBJS=make_emmc_flash_bin.o

PROG=make_emmc_flash_bin

%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(LIBPATH) $(LIBS) $(OBJS) -o $(PROG)

clean:

	rm -f *.o $(PROG)



