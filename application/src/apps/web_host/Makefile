include platform.mk

SHARED = -fPIC --shared
ifeq ($(CC),aarch64-target-linux-gnu-gcc)
	CFLAGS = -D_FORTIFY_SOURCE=2 -O3 -std=gnu99 -fstack-protector-strong -Wl,-z,relro,-z,now,-z,noexecstack -fPIC -fPIE -pie -Wall -Wstrict-prototypes -Wpointer-arith -Wformat=2 -Wformat-security -Wformat-nonliteral -Wno-format-y2k -Wcast-qual -Wcast-align -fexceptions -fgnu89-inline
else
	CFLAGS = -O3 -std=gnu99 -fstack-protector-strong -Wl,-z,relro,-z,now,-z,noexecstack -fPIC -fPIE -pie -Wall -Wstrict-prototypes -Wpointer-arith -Wformat=2 -Wformat-security -Wformat-nonliteral -Wno-format-y2k -Wcast-qual -Wcast-align -fexceptions -fgnu89-inline
endif

PWD_DIR := $(shell pwd)
BIN_DIR := $(PWD_DIR)/release/opt/pme/bin

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all default DFLIB_SO CJSON_SO PORTAL RELEASE

default: $(PLAT)

all: DFLIB_SO CJSON_SO PORTAL RELEASE

CJSON_SO:
	cd $(PWD_DIR)/cjson && make CFLAGS='$(CFLAGS)' MYCFLAGS='$(MYCFLAGS)' && make install

DFLIB_SO:
	cd $(PWD_DIR)/ldflib && make CFLAGS='$(CFLAGS)' MYCFLAGS='$(MYCFLAGS)' && make install

PORTAL:
	LD_PRELOAD= CARGO_NET_GIT_FETCH_WITH_CLI=true cargo build --release --target=$(RS_TARGET) --target-dir=$(PREFIX)
	$(STRIP) $(PREFIX)/$(RS_TARGET)/release/portal
	mkdir -p $(PREFIX)/bin
	cp $(PREFIX)/$(RS_TARGET)/release/portal $(PREFIX)/bin

RELEASE: DFLIB_SO CJSON_SO PORTAL
	LD_PRELOAD= sudo LD_PRELOAD=${LD_PRELOAD} -E rm -rf $(BIN_DIR)/portal
	mkdir -p $(BIN_DIR)/portal
	mkdir -p $(BIN_DIR)/portal/webrest
	mkdir -p $(BIN_DIR)/portal/redfish
	cp $(PREFIX)/lib/dflib.so $(BIN_DIR)/portal
	cp $(PREFIX)/lib/cjson.so $(BIN_DIR)/portal
	cp $(PREFIX)/bin/portal $(BIN_DIR)/portal
	cp -rf $(PWD_DIR)/lualib $(BIN_DIR)/portal
	rm -rf $(BIN_DIR)/portal/lualib/redfish_handles
	rm -rf $(BIN_DIR)/portal/lualib/redfish.lua
	cp -rf $(PWD_DIR)/ldflib/dflib $(BIN_DIR)/portal/lualib/
	cp -rf $(PWD_DIR)/webrest/src/business/* $(BIN_DIR)/portal/webrest/
	cp -rf $(PWD_DIR)/lualib/redfish_handles/* $(BIN_DIR)/portal/redfish/
	cp -rf $(PWD_DIR)/lualib/redfish.lua $(BIN_DIR)/portal/redfish/

test: testdflib
	cargo test

testdflib:
	cd $(BIN_DIR)/portal && $(OUTDIR)/bin/luajit $(PWD_DIR)/ldflib/test/test.lua

cleandflib:
	cd $(PWD_DIR)/ldflib && make clean
	
cleancjson:
	cd $(PWD_DIR)/cjson && make clean

clean: cleandflib cleancjson
	rm -rf out/bin
	rm -rf out/include
	rm -rf out/lib
	rm -rf out/release
	rm -rf out/share
	rm -rf release
	rm -rf out

cleanall: cleandflib cleancjson
	rm -rf out
	rm -rf release
