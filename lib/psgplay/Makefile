# SPDX-License-Identifier: GPL-2.0

LIBPSGPLAY_STATIC := lib/psgplay/libpsgplay.a
LIBPSGPLAY_SHARED := lib/psgplay/libpsgplay.so

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

LIBPSGPLAY_SRC :=							\
	lib/psgplay/psgplay.c						\
	lib/psgplay/sndh.c
