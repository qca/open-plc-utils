#!/bin/sh
# file: scripts/plctop.sh

# A script to display the network topology via plcstat. The view is refreshed
# every second like top.

WATCH=/usr/bin/watch

if [ ! -x $WATCH ]; then
  echo "Cannot find $WATCH\n"
fi

$WATCH -n 1 "plcstat -t $@"