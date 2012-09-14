#
# Regular cron jobs for the plc-utils package
#
0 4	* * *	root	[ -x /usr/bin/plc-utils_maintenance ] && /usr/bin/plc-utils_maintenance
