#!/usr/bin/make -f
# file: mme/Makefile

# ====================================================================
# environment definitions;
# --------------------------------------------------------------------

include ../make.def

# ====================================================================
# symbols;
# --------------------------------------------------------------------

CFLAGS+=
LDFLAGS+=
TRASH=*.o *.obj *.exe t t.* *~ *.[0-9][0-9][0-9] *.out
FILES=Makefile *.mak *.sh *.txt *.c *.h *.xml *.css
TOOLS=mme hpav
PAGES=hpav.1 mme.1

# ====================================================================
# pseudo targets;
# --------------------------------------------------------------------

all compile: ${TOOLS}
compact: compile
	if [ ! -z ${STRIP} -a ! -z ${TOOLS} ]; then ${STRIP} ${TOOLS}; fi
	if [ -x /usr/local/bin/upx -a ! -z ${TOOLS} ]; then upx --best ${TOOLS}; fi
library:
	install -m ${DIR_PERM} -o ${OWNER} -g ${GROUP} -d ${BIN}
scripts:
manuals:
	test -d ${MAN} || install -m ${DIR_PERM} -o ${OWNER} -g ${GROUP} -d ${MAN}
	install -m ${MAN_PERM} -o ${OWNER} -g ${GROUP} ${PAGES} ${MAN}
install: $(TOOLS) library
	install -m ${BIN_PERM} -o ${OWNER} -g ${GROUP} ${TOOLS} ${BIN}
uninstall:
	cd ${BIN}; rm -f ${TOOLS}
check:
	${SHELL} mme.sh
fresh: clean compile
clean:
	rm -f ${TOOLS} ${TRASH}
ignore:
	echo ${TOOLS} | tr ' ' '\n' > .gitignore

# ====================================================================
# rules;
# --------------------------------------------------------------------

%: %.o
	${CC} -o ${@} ${^} ${LDFLAGS}
%.o: %.c
	${CC} -c ${<} ${CFLAGS}
%.h: %.h

# ====================================================================
# programs; 
# --------------------------------------------------------------------

ifneq ($(TARGET), mingw32)
hpav: hpav.o getoptv.o putoptv.o version.o hexdump.o hexdecode.o hexstring.o error.o channel.o openchannel.o closechannel.o sendpacket.o readpacket.o MMEPeek.o MMEName.o MMEMode.o uintspec.o todigit.o gethwaddr.o
else
hpav: hpav.o getoptv.o putoptv.o version.o hexdump.o hexdecode.o hexstring.o error.o channel.o openchannel.o closechannel.o sendpacket.o readpacket.o MMEPeek.o MMEName.o MMEMode.o uintspec.o todigit.o gethwaddr.o getifname.o
endif

hpavd: hpavd.o getoptv.o putoptv.o version.o hexdump.o hexdecode.o hexstring.o error.o channel.o openchannel.o closechannel.o sendpacket.o readpacket.o MMEPeek.o MMEName.o MMEMode.o 
mme: mme.o getoptv.o putoptv.o version.o uintspec.o todigit.o output.o error.o MMEName.o MMEMode.o

# ====================================================================
# includes; 
# --------------------------------------------------------------------

include ../tools/tools.mak
include ../ether/ether.mak
include ../mme/mme.mak
include ../plc/plc.mak

