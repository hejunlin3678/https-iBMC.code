CC=gcc -w
CFLAG= -Wall -fsigned-char -fno-common 
PROG=filesizecheck
SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c,%.o,$(SRCS))

.c.o: 
	$(CC) $(CFLAG)  -c $<

all:$(PROG)
	

$(PROG):$(OBJS)
	$(CC) $(CFLAG) -o $@ $(OBJS)

clean:
	rm $(PROG) *.o
	