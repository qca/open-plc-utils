#!/bin/bash
# file: recurse.sh

# ====================================================================
#
# --------------------------------------------------------------------

HOST=${HOSTNAME}
MODE=6775
OWNER=${USER}
GROUP=plc

# ====================================================================
#
# --------------------------------------------------------------------

if [ ${HOSTNAME} != ${HOST} ]; then
	echo "This host is not ${HOST}."
	exit 1
	fi
if [ -z ${1} ]; then
	echo "Specify full path to folder."
	exit 1
	fi
for file in ${1}/*; do
	chown ${OWNER}:${GROUP} ${file}
	if [ -d ${file} ]; then
		echo ${file}
		chmod ${MODE} ${file}
		${0} ${file}
		fi
done

