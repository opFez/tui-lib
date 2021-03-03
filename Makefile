CFLAGS=-Wall -Wextra -pedantic -std=c99
CC=gcc
AR=ar

default:
	@echo "to make static library: make static"
	@echo "to make shared library: make shared"

static: tui.a
shared: tui.so
	
tui.a: tui_st.o
	$(AR) rcs $@ $<

tui_st.o: tui.c tui.h
	$(CC) $(CFLAGS) -c -o $@ tui.c

tui.so: tui_sh.o
	$(CC) -shared -o $@ $<

tui_sh.o: tui.c tui.h
	$(CC) $(CFLAGS) -fPIC -c -o $@ tui.c

.PHONY: clean
clean:
	rm -f *.o *.so *.a
