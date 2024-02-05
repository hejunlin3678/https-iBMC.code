SHARED = -fPIC --shared

CJSON_SO = cjson.so

INSTALL_LIB=$(PREFIX)/lib

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all default clean cleanall install

all: $(CJSON_SO)

$(CJSON_SO): lcjson.c
	$(CC) $(CFLAGS) $(MYCFLAGS) $(SHARED) $^ -o $@ -L$(LIBRARY_PATH) -I$(LUA_INCLUDE) -I$(LUA_JSONC_INCLUDE) -I../3rd -I$(GLIB_INCLUDE_PATH) -I$(GLIB_INCLUDE_PATH2) -I$(GLIB_INCLUDE_PATH3) -L$(DFLIB_PATH) -lsecurec -ljson

install:
	mkdir -p $(INSTALL_LIB)
	cp $(CJSON_SO) $(INSTALL_LIB)
	${STRIP} $(INSTALL_LIB)/$(CJSON_SO)

clean:
	rm -f $(CJSON_SO)

cleanall: clean

