# ====================================================================
# programs;
# --------------------------------------------------------------------

evse.o: evse.c channel.h config.h error.h flags.h getoptv.h memory.h number.h putoptv.h iso18115.h types.h
pev.o: pev.c channel.h config.h error.h files.h flags.h getoptv.h memory.h number.h putoptv.h iso18115.h types.h

# ====================================================================
# functions;
# --------------------------------------------------------------------

evse_cm_atten_char.o: evse_cm_atten_char.c channel.h error.h memory.h iso18115.h
evse_cm_atten_profile.o: evse_cm_atten_profile.c channel.h error.h memory.h iso18115.h types.h
evse_cm_mnbc_sound.o: evse_cm_mnbc_sound.c channel.h error.h memory.h iso18115.h
evse_cm_iso18115_match.o: evse_cm_iso18115_match.c channel.h error.h memory.h iso18115.h types.h
evse_cm_iso18115_param.o: evse_cm_iso18115_param.c channel.h error.h memory.h iso18115.h types.h
evse_cm_start_atten_char.o: evse_cm_start_atten_char.c channel.h error.h memory.h iso18115.h
pev_cm_atten_char.o: pev_cm_atten_char.c channel.h error.h memory.h iso18115.h
pev_cm_mnbc_sound.o: pev_cm_mnbc_sound.c channel.h error.h memory.h iso18115.h types.h
pev_cm_iso18115_match.o: pev_cm_iso18115_match.c channel.h error.h memory.h iso18115.h types.h
pev_cm_iso18115_param.o: pev_cm_iso18115_param.c channel.h error.h memory.h iso18115.h types.h
pev_cm_start_atten_char.o: pev_cm_start_atten_char.c channel.h error.h iso18115.h types.h
iso18115_session.o: iso18115_session.c memory.h iso18115.h
iso18115_structs.o: iso18115_structs.c iso18115.h

# ====================================================================
# headers;
# --------------------------------------------------------------------

iso18115.h: types.h endian.h ether.h mme.h

