#!/bin/make -f
# file: docbook/Makefile

# ====================================================================
# environment definitions;
# --------------------------------------------------------------------

include ../make.def

DOCBOOK=/usr/share/xml/docbook/stylesheet/docbook-xsl/fo  
DOCBOOK=/usr/share/xml/docbook/stylesheet/docbook-xsl/html
LIBRARY=${DOC}/open-plc
XMLFLAGS=--skip-validation

# ====================================================================
# symbols;
# --------------------------------------------------------------------

TRASH=*~ *.[0-9][0-9][0-9] t t.* *.7 *.7a *.1.html *.c.html *.cpp.html *.h.html *.hpp.html *.7.html *.7a.html *.sh.html *.xml.html *.xsd.html ch[0-9][0-9]*.html 
FILES=Makefile *.txt *.xml *.xsl
STYLES=param.xsl lists.xsl synop.xsl
PAGES=plc-utils.xml 
IMAGES=BootFromHost.png SimpleNetwork.png UpdateLocalHost.png UpgradeDevice.png
# BOOKS=index.html plc-utils.txt plc-utils.pdf
# BOOKS=index.html plc-utils.txt 
BOOKS=index.html 

# ====================================================================
# pseudo targets;
# --------------------------------------------------------------------

all compile: ${BOOKS} 
	rm -f *.[0-9][0-9][0-9]
manuals: 
#	${SHELL} ../docbook/manuals
scripts:
library:
	install -m ${DIR_PERM} -o ${OWNER} -g ${GROUP} -d ${LIBRARY}
install: compile library manuals
	if [ -d ${LIBRARY} ]; then install -m 0666 -o ${OWNER} -g ${GROUP} *.html *.css *.png ${LIBRARY}; fi
uninstall:
	rm -f ${LIBRARY}/*.c.html ${LIBRARY}/*.h.html ${LIBRARY}/*.7.html
setup:
	install ${STYLES} ${DOCBOOK}
clean:
	rm -f ${TRASH} ${BOOKS}
fresh: clean compile

# ====================================================================
# pseudo targets;
# --------------------------------------------------------------------

index.html: ${PAGES}
	xmlto ${XMLFLAGS} -o . html ${<} 
plc-utils.pdf: ${PAGES} ${IMAGES}
	docbook2pdf ${<}
plc-utils.txt: ${PAGES} 
	xmlto ${XMLFLAGS} txt ${<}
plc-utils.xml: overview.xml hardware.xml software.xml firmware.xml procedures.xml support.xml scripting.xml embedding.xml security.xml library.xml management.xml 
overview.xml: install.xml programs.xml folders.xml packages.xml
install.xml: install-linux.xml install-macosx.xml install-windows-xp.xml install-windows-7.xml
support.xml: support1.xml support2.xml support3.xml support4.xml support5.xml support6.xml support7.xml support8.xml support9.xml support10.xml qcaspi.xml
firmware.xml: firmware-6000-boot.xml firmware-6000-flash.xml firmware-6000-upload.xml firmware-6300-boot.xml firmware-6400-boot.xml firmware-7400-boot.xml  firmware-7420-boot.xml firmware-7420-flash.xml
install-windows-xp.xml: WinXP-AddRemoveProgram.png WinXP-PathVariable.png WinXP-VisualStudioNET.png WinXP-WindowsInstaller.png
install-windows-7.xml: Win7-ftp-market.png Win7-ftp-toolkit.png Win7-ftp-installer.png Win7-run-installer.png

