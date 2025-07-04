# SPDX-License-Identifier: GPL-2.0

LIBPSGPLAY_CFLAGS =							\
	$(BASIC_HOST_CFLAGS) -fPIC					\
	$(CF2149_CFLAGS)

LIBPSGPLAY_SRC :=							\
	lib/psgplay/psgplay.c						\
	lib/psgplay/sndh.c

LIBPSGPLAY_PUBLIC_SRC :=						\
	$(ICE_SRC)							\
	$(VERSION_SRC)							\
	$(LIBPSGPLAY_SRC)

LIBPSGPLAY_HIDDEN_SRC :=						\
	$(INTERNAL_SRC)							\
	$(ATARI_SRC)							\
	$(M68K_SRC)

LIBPSGPLAY_object = $(addprefix lib/psgplay/,$(notdir $(1:%.c=%.o)))

LIBPSGPLAY_PUBLIC_OBJ = $(call LIBPSGPLAY_object,$(LIBPSGPLAY_PUBLIC_SRC))
LIBPSGPLAY_HIDDEN_OBJ = $(call LIBPSGPLAY_object,$(LIBPSGPLAY_HIDDEN_SRC))

LIBPSGPLAY_SRC := $(LIBPSGPLAY_PUBLIC_SRC) $(LIBPSGPLAY_HIDDEN_SRC)
LIBPSGPLAY_OBJ := $(LIBPSGPLAY_PUBLIC_OBJ) $(LIBPSGPLAY_HIDDEN_OBJ)

ALL_OBJ += $(LIBPSGPLAY_OBJ) lib/psgplay/m68kda.o lib/psgplay/m68kds.o

$(LIBPSGPLAY_HIDDEN_OBJ):						\
	LIBPSGPLAY_CFLAGS += -fvisibility=hidden -I lib/toslibc/include

define LIBPSGPLAY_target
$(call LIBPSGPLAY_object,$(1)): $(1)
	$$(QUIET_CC)$$(HOST_CC) $$(LIBPSGPLAY_CFLAGS) $$(HOST_CFLAGS) -c -o $$@ $$<
endef

$(foreach f,$(LIBPSGPLAY_SRC),$(eval $(call LIBPSGPLAY_target,$(f))))

LIBPSGPLAY_STATIC := lib/psgplay/libpsgplay.a
LIBPSGPLAY_SHARED := lib/psgplay/libpsgplay.$(SHLIB_EXT)

$(LIBPSGPLAY_STATIC): $(LIBPSGPLAY_OBJ)
	$(QUIET_AR)$(HOST_AR) $(HOST_ARFLAGS) $@ $^

ifeq (Darwin,$(BUILD_SYSTEM))
LIBPSGPLAY_SOFLAGS = -dynamiclib -install_name @rpath/libpsgplay.$(PSGPLAY_VERSION_MAJOR).$(SHLIB_EXT)
else
LIBPSGPLAY_SOFLAGS = -shared -Wl,-soname,libpsgplay.$(SHLIB_EXT).$(PSGPLAY_VERSION_MAJOR) $(HOST_CFLAGS)
endif

$(LIBPSGPLAY_SHARED): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(HOST_CC) $(LIBPSGPLAY_SOFLAGS) -o $@ $^

LIBPSGPLAY_PC := lib/psgplay/libpsgplay.pc

$(LIBPSGPLAY_PC):
	$(QUIET_GEN)script/pkg $@

LIBPSGPLAY_JAVASCRIPT := lib/psgplay/libpsgplay.js
LIBPSGPLAY_WEBASSEMBLY := lib/psgplay/libpsgplay.wasm

LIBPSGPLAY_WEB_FUNCTIONS =						\
	_psgplay_init							\
	_psgplay_read_stereo						\
	_psgplay_read_digital						\
	_psgplay_digital_to_stereo_callback				\
	_psgplay_digital_to_stereo_empiric				\
	_psgplay_digital_to_stereo_linear				\
	_psgplay_digital_to_stereo_balance				\
	_psgplay_digital_to_stereo_volume				\
	_psgplay_stereo_downsample_callback				\
	_psgplay_stop							\
	_psgplay_stop_at_time						\
	_psgplay_stop_digital_at_sample					\
	_psgplay_free							\
	_ice_identify							\
	_ice_crunched_size						\
	_ice_decrunched_size						\
	_ice_decrunch							\
	_sndh_identify							\
	_sndh_tag_subtune_count						\
	_sndh_tag_default_subtune					\
	_sndh_tag_subtune_time						\
	_sndh_tag_timer							\
	_sndh_tag_title							\
	_sndh_init_address						\
	_sndh_exit_address						\
	_sndh_play_address						\
	_malloc								\
	_free

LIBPSGPLAY_WEB_EXPORTS =						\
	["$(shell echo $(LIBPSGPLAY_WEB_FUNCTIONS) | sed 's/ /","/g')"]

LIBPSGPLAY_WEB_FLAGS = -s ASYNCIFY -s EXPORT_NAME="'libpsgplay'"	\
	-s EXPORTED_FUNCTIONS='$(LIBPSGPLAY_WEB_EXPORTS)'		\
	-s EXPORTED_RUNTIME_METHODS=ccall,cwrap \
	-s STACK_SIZE=5MB

$(LIBPSGPLAY_JAVASCRIPT) $(LIBPSGPLAY_WEBASSEMBLY): $(LIBPSGPLAY_OBJ)
	$(QUIET_CC)$(HOST_CC) $(LIBPSGPLAY_WEB_FLAGS) $(HOST_CFLAGS) -o $@ $^

.PHONY: web
web: $(LIBPSGPLAY_JAVASCRIPT)

LIBPSGPLAY_HEADERS =							\
	include/ice/ice.h						\
	include/psgplay/digital.h					\
	include/psgplay/psgplay.h					\
	include/psgplay/sndh.h						\
	include/psgplay/stereo.h					\
	include/psgplay/version.h

.PHONY: install-lib
install-lib: install-include install-lib-static install-lib-shared install-pkg-config

.PHONY: install-include
install-include:
	$(INSTALL) -d -m 755 $(DESTDIR)$(includedir)/psgplay
	$(INSTALL) -m 644 $(LIBPSGPLAY_HEADERS) $(DESTDIR)$(includedir)/psgplay

.PHONY: install-lib-static
install-lib-static: $(LIBPSGPLAY_STATIC)
	$(INSTALL) -d -m 755 $(DESTDIR)$(libdir)
	$(INSTALL) $(LIBPSGPLAY_STATIC) $(DESTDIR)$(libdir)

.PHONY: install-lib-shared
install-lib-shared: $(LIBPSGPLAY_SHARED)
	$(INSTALL) -d -m 755 $(DESTDIR)$(libdir)
	$(INSTALL) $(LIBPSGPLAY_SHARED) $(DESTDIR)$(libdir)/libpsgplay.$(SHLIB_EXT).$(PSGPLAY_VERSION_MINOR)
	ln -s libpsgplay.$(SHLIB_EXT).$(PSGPLAY_VERSION_MINOR) $(DESTDIR)$(libdir)/libpsgplay.$(SHLIB_EXT).$(PSGPLAY_VERSION_MAJOR)
	ln -s libpsgplay.$(SHLIB_EXT).$(PSGPLAY_VERSION_MAJOR) $(DESTDIR)$(libdir)/libpsgplay.$(SHLIB_EXT)

.PHONY: install-pkg-config
install-pkg-config: $(LIBPSGPLAY_PC)
	$(INSTALL) -d -m 755 $(DESTDIR)$(pkgconfigdir)
	$(INSTALL) -m 644 $(LIBPSGPLAY_PC) $(DESTDIR)$(pkgconfigdir)

OTHER_CLEAN += $(LIBPSGPLAY_STATIC) $(LIBPSGPLAY_SHARED)		\
	$(LIBPSGPLAY_JAVASCRIPT) $(LIBPSGPLAY_WEBASSEMBLY)		\
	$(LIBPSGPLAY_PC)
