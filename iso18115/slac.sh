#!/bin/sh
# file: iso18115/iso18115.sh

# ====================================================================
# programs;
# --------------------------------------------------------------------

gcc -Wall -Wextra -Wno-unused-parameter -o evse evse.c
gcc -Wall -Wextra -Wno-unused-parameter -o pev pev.c

# ====================================================================
# functions; 
# --------------------------------------------------------------------

gcc -Wall -Wextra -Wno-unused-parameter -c pev_cm_iso18115_param.c
gcc -Wall -Wextra -Wno-unused-parameter -c pev_cm_start_atten_char.c
gcc -Wall -Wextra -Wno-unused-parameter -c pev_cm_atten_char.c
gcc -Wall -Wextra -Wno-unused-parameter -c pev_cm_mnbc_sound.c
gcc -Wall -Wextra -Wno-unused-parameter -c evse_cm_iso18115_param.c
gcc -Wall -Wextra -Wno-unused-parameter -c evse_cm_start_atten_char.c
gcc -Wall -Wextra -Wno-unused-parameter -c evse_cm_atten_char.c
gcc -Wall -Wextra -Wno-unused-parameter -c evse_cm_mnbc_sound.c
gcc -Wall -Wextra -Wno-unused-parameter -c iso18115_session.c
gcc -Wall -Wextra -Wno-unused-parameter -c iso18115_structs.c

# ====================================================================
# cleanse; 
# --------------------------------------------------------------------

rm -f *.o

