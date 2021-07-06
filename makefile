CC = gcc

HEADER = wav.h 
SRCS= FIR.c

OBJS = $(SRCS:.c=.o)

LDFLAGS = -lm

MAIN = main

.PHONY: depend clean

all:    $(MAIN)
		@echo  Message: executable file ...main... has been created

$(MAIN): $(OBJS) 
		$(CC) $(CFLAGS) $(HEADER) -o $(MAIN) $(OBJS) $(LDFLAGS) $(LIBS)

.c++.o:
		$(CC) $(CFLAGS) $(HEADER) -c++ $<  -o $@ 

clean:
		$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
		makedepend $(HEADER) $^
