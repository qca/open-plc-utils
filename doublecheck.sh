#!/bin/sh

# ====================================================================
#   Copyright (c) 2010-2012 Qualcomm Atheros Incorporated.
# --------------------------------------------------------------------

PROJECTS=VisualStudioNET
FOLDERS="ether key mdio mme nodes nvm pib plc ram serial tools"
CATALOG1=solution.txt
CATALOG2=programs.txt
CATALOG=catalog.txt

# ====================================================================
#   compile all programs stand-alone to verify include statements;
# --------------------------------------------------------------------

for folder in ${FOLDERS}; do
	echo ${folder}
	cd ${folder}
	sh ${folder}.sh
	cd ..
done

# ====================================================================
#
# --------------------------------------------------------------------

if [ ! -d /usr/local/bin/cmassoc ]; then
	echo "Motley Tools are not installed!"
	exit 1
	fi

# ====================================================================
#
# --------------------------------------------------------------------

echo "--- 2003 ---"
if [ -f ${PROJECTS}/qca-utils/qca-utils-2003.vdproj ]; then
	grope ${PROJECTS}/qca-utils/qca-utils-2003.vdproj -e "[A-Za-z0-9]+[.]exe" > ${CATALOG1}
	ff -f ${PROJECTS}/Programs/2003/*.exe > ${CATALOG2} 
	srt ${CATALOG1} ${CATALOG2}
	diff -ru ${CATALOG1} ${CATALOG2}
	for file in ${PROJECTS}/*-utils; do
		if [ -d ${file} ]; then
			echo ${file}
	 		PROJECT=${file}/$(basename ${file})-2010.vdproj
	 		CATALOG=${file}/$(basename ${file}).catalog
			grope ${PROJECT} -e "[[:alnum:]]+[.]exe" > ${CATALOG}
			srt ${CATALOG}
			fi
		done
	rm ${CATALOG1} ${CATALOG2}
	fi

# ====================================================================
#
# --------------------------------------------------------------------

echo "--- 2010 ---"
if [ -f ${PROJECTS}/qca-utils/qca-utils-2010.vdproj ]; then
	diff -ru ${PROJECTS}/Programs/2003 ${PROJECTS}/Programs/2010 | grep "Only in"
	fi

