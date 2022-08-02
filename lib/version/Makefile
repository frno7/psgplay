# SPDX-License-Identifier: GPL-2.0

VERSION_SRC := lib/version/version.c

.PHONY: $(shell script/version --check $(VERSION_SRC))
$(VERSION_SRC):
	$(QUIET_GEN)script/version $@
