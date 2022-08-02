# SPDX-License-Identifier: GPL-2.0

LIBPSGPLAY_SRC :=							\
	lib/psgplay/psgplay.c						\
	lib/psgplay/sndh.c

LIBPSGPLAY_HIDDEN_SRC :=						\
	$(ATARI_SRC)							\
	$(INTERNAL_SRC)							\
	$(M68K_SRC)
LIBPSGPLAY_HIDDEN_OBJ := $(patsubst %.c, %.o, $(LIBPSGPLAY_HIDDEN_SRC))

$(LIBPSGPLAY_HIDDEN_OBJ): %.o : %.c
	$(QUIET_CC)$(HOST_CC) $(MOST_CFLAGS) -fvisibility=hidden $(HOST_CFLAGS) -c -o $@ $<

LIBPSGPLAY_PUBLIC_SRC := $(LIBPSGPLAY_SRC) $(ICE_SRC) $(VERSION_SRC)
LIBPSGPLAY_PUBLIC_OBJ := $(patsubst %.c, %.o, $(LIBPSGPLAY_PUBLIC_SRC))

$(LIBPSGPLAY_PUBLIC_OBJ): %.o : %.c
	$(QUIET_CC)$(HOST_CC) $(MOST_CFLAGS) $(HOST_CFLAGS) -c -o $@ $<

LIBPSGPLAY_OBJ := $(LIBPSGPLAY_HIDDEN_OBJ) $(LIBPSGPLAY_PUBLIC_OBJ)

LIBPSGPLAY_STATIC := lib/psgplay/libpsgplay.a
LIBPSGPLAY_SHARED := lib/psgplay/libpsgplay.so

$(LIBPSGPLAY_STATIC): $(LIBPSGPLAY_OBJ)
	$(QUIET_AR)$(HOST_AR) rcs $@ $^

$(LIBPSGPLAY_SHARED): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(HOST_CC) $(SOFLAGS) $(HOST_CFLAGS) -o $@ $^

LIBPSGPLAY_JAVASCRIPT := lib/psgplay/libpsgplay.js
LIBPSGPLAY_WEBASSEMBLY := lib/psgplay/libpsgplay.wasm

LIBPSGPLAY_WEB_FUNCTIONS =						\
	_psgplay_init							\
	_psgplay_read_stereo						\
	_psgplay_free							\
	_ice_identify							\
	_ice_decrunched_size						\
	_ice_decrunch							\
	_sndh_tag_default_subtune					\
	_sndh_tag_subtune_time						\
	_malloc								\
	_free

WEBFUNCS = ["$(shell echo $(LIBPSGPLAY_WEB_FUNCTIONS) | sed 's/ /","/g')"]

WEBFLAGS = -s ASYNCIFY -s EXPORT_NAME="'libpsgplay'"			\
	-s EXPORTED_FUNCTIONS='$(WEBFUNCS)'				\
	-s EXPORTED_RUNTIME_METHODS=ccall,cwrap

$(LIBPSGPLAY_JAVASCRIPT) $(LIBPSGPLAY_WEBASSEMBLY): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(HOST_CC) $(WEBFLAGS) $(HOST_CFLAGS) -o $@ $^
