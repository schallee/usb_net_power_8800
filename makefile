CLFAGS =
CFLAGS+=-g -DDEBUG
CFLAGS+=-Wall -Werror
CFLAGS+=-pedantic -pedantic-errors
CFLAGS+=-std=c99
LDLIBS=-lusb-1.0
CSOURCES:=$(wildcard *.c)
OBJECTS:=$(CSOURCES:.c=.o)
TARGETS=power_ctl

.PHONY: all
all: $(TARGETS)

power_ctl: $(OBJECTS)
	$(CC) $(LDFLAGS)-o $@ $^ $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(TARGETS) core *.o
