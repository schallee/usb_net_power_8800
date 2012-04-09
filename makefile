CLFAGS =
CFLAGS+=-g -DDEBUG
CFLAGS+=-Wall -Werror
CFLAGS+=-pedantic -pedantic-errors
CFLAGS+=-std=c99
LDLIBS=-lusb-1.0
CSOURCES:=$(wildcard *.c)
OBJECTS:=$(CSOURCES:.c=.o)
TARGETS=pwrctl

.PHONY: all
all: $(TARGETS)

pwrctl: $(OBJECTS)
	$(CC) $(LDFLAGS)-o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(TARGETS) core *.o
