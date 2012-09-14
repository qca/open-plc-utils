#!/bin/make -f
# file: pib/Makefile

# ====================================================================
# folders; 
# --------------------------------------------------------------------

BIN=/usr/local/bin
MAN=/usr/share/man/man7

# ====================================================================
# symbols;
# --------------------------------------------------------------------

CC=${CROSS}gcc
CFLAGS+=
LFLAGS+=
TRASH=*.o *.obj int5500-utils.exe t t.* *~ *.[0-9][0-9][0-9] *.test CHANGELOG.txt
FILES=*.c *.h *.txt *.mak Makefile Makefile.*
TOOLS=int5500-utils.exe

# ====================================================================
# targets;
# --------------------------------------------------------------------

.PHONY: compile library scripts manuals install uninstall clean fresh
compile: ${TOOLS}
scripts:
library:
	install -m 0755 -o root -g bin -d ${BIN}
install: compile library
	install -m 0555 -o root -g bin -s ${TOOLS} ${BIN}
uninstall:
	cd ${BIN}; rm -f ${TOOLS}
clean:
	rm -f ${TRASH} ${TOOLS}
test:
	@ ./test.sh
fresh: clean compile

int5500-utils.exe: int5500-utils.nsi CHANGELOG.txt

CHANGELOG.txt:
	cp ../CHANGELOG ./CHANGELOG.txt && crlf -w CHANGELOG.txt

%.exe: %.nsi
	makensis -X"SetCompressor /SOLID lzma" ${<}

# ====================================================================
# includes;
# --------------------------------------------------------------------

include ../make.def
