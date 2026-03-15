# SPDX-License-Identifier: GPL-2.0

PSGPLAY_VERSION := $(shell script/version)
PSGPLAY_VERSION_MAJOR := $(shell echo '$(PSGPLAY_VERSION)' | sed 's/\..*$$//')
PSGPLAY_VERSION_MINOR := $(shell echo '$(PSGPLAY_VERSION)' | sed 's/-.*$$//')

export PSGPLAY_VERSION_MINOR PSGPLAY_VERSION_MAJOR

VERSION_H = include/psgplay/version.h

.PHONY: $(shell script/version --check $(VERSION_H))
$(VERSION_H):
	$(QUIET_GEN)script/version $@

OTHER_CLEAN += $(VERSION_H)
