SRCS = viewstate.c
BINPATH = /usr/bin/
CONFPATH = /etc/
MANPATH = /usr/share/man/

all:
	gcc -o viewstate $(SRCS) $(LDFLAGS) $(CFLAGS)

install:
	cp viewstate $(BINPATH)
	cp viewstate.1 $(MANPATH)man1

uninstall:
	rm -f $(BINPATH)viewstate
	rm -f $(MANPATH)man1/viewstate.1

clean:
	rm -f viewstate
