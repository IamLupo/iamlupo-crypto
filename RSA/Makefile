PROG = main

# Compiler
C=gcc
CFLAGS= -Wall -O2 -Wno-unused-result

# Directories
COMPDIR = compiled
OBJDIR = temp

# Files
CFILES := $(wildcard *.c)

# Objects
COBJFILES := $(patsubst %.c, %.o, $(CFILES))

OBJFILES := $(COBJFILES)
OBJDIRFILES := $(patsubst %.o, $(OBJDIR)/%.o, $(OBJFILES))

%.o: %.c
	$(C) $(CFLAGS) -o $(OBJDIR)/$@ -c $<

all: $(PROG)

$(PROG):	$(OBJFILES)
	$(C) $(CFLAGS) -o $(COMPDIR)/$(PROG) $(OBJDIRFILES) -static -lgmp -lm

clean:
	rm -f $(OBJDIR)/*
	rm -f $(COMPDIR)/$(PROG)
