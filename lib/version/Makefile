# SPDX-License-Identifier: GPL-2.0

PSGPLAY_VERSION := $(shell script/version)
PSGPLAY_VERSION_MINOR := $(shell echo '$(PSGPLAY_VERSION)' | sed 's/-.*$$//')
PSGPLAY_VERSION_MAJOR := $(shell echo '$(PSGPLAY_VERSION)' | sed 's/\..*$$//')

export PSGPLAY_VERSION_MINOR PSGPLAY_VERSION_MAJOR

VERSION_SRC = lib/version/version.c

.PHONY: $(shell script/version --check $(VERSION_SRC))
$(VERSION_SRC):
	$(QUIET_GEN)script/version $@

OTHER_CLEAN += $(VERSION_SRC)
