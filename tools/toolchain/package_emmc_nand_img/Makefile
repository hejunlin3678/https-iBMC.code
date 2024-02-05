CC=gcc
CFLAGS=-g -Wall -fsigned-char -fno-common 
INCLUDES=

OBJ=converFile.o

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o converfile

$(OBJ):%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f *.o 

