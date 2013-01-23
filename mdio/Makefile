#!/usr/bin/make -f
# file: int6k/Makefile

# ====================================================================
# environment definitions;
# --------------------------------------------------------------------

include ../make.def

# ====================================================================
# symbols;
# --------------------------------------------------------------------

CFLAGS+=-DINT6x00
LDFLAGS+=
TRASH=*.o *.obj *.exe t t.* *~ *.[0-9][0-9][0-9]
FILES=Makefile *.mak *.sh *.c *.h 
TOOLS=mdioblock mdioblock2 mdiodump mdiogen
PAGES=mdioblock.7 mdioblock2.7 mdiodump.7

# ====================================================================
# pseudo targets;
# --------------------------------------------------------------------

all compile: ${TOOLS}
compact: compile
	if [ ! -z ${STRIP} ]; then ${STRIP} ${TOOLS}; fi
	if [ -x /usr/local/bin/upx ]; then upx --best ${TOOLS}; fi
library:
	install -m ${DIR_PERM} -o ${OWNER} -g ${GROUP} -d ${BIN}
scripts:
manuals:
	install -m ${MAN_PERM} -o ${OWNER} -g ${GROUP} ${PAGES} ${MAN}
install: compile library
	install -m ${SUID_PERM} -o ${OWNER} -g ${GROUP} ${TOOLS} ${BIN}
uninstall:
	cd ${BIN}; rm -f int6k ${TOOLS}
	cd ${MAN}; rm -f int6k ${PAGES}
fresh: clean compile
clean:
	rm -f ${TRASH} ${TOOLS}
ignore:
	echo ${TOOLS} | sed "s/ /\n/g" > .gitignore

# ====================================================================
# programs;     
# --------------------------------------------------------------------

%: %.o
	${CC} -o ${@} ${^} ${LDFLAGS}
%.o: %.c
	${CC} -c ${<} ${CFLAGS}
%.h: %.h

# ====================================================================
# programs;     
# --------------------------------------------------------------------

mdioblock: mdioblock.o error.o getoptv.o putoptv.o todigit.o version.o
mdioblock2: mdioblock2.o error.o getoptv.o putoptv.o todigit.o version.o
mdiodump: mdiodump.o assist.o error.o codelist.o getoptv.o hexdecode.o hexstring.o lookup.o putoptv.o version.o
mdiogen: mdiogen.o error.o

# ====================================================================
# includes;
# --------------------------------------------------------------------

include ../mdio/mdio.mak
include ../tools/tools.mak
include ../key/key.mak
include ../ether/ether.mak
include ../nodes/nodes.mak
include ../ram/ram.mak
include ../pib/pib.mak
include ../nvm/nvm.mak
include ../mme/mme.mak
include ../plc/plc.mak


