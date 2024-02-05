SHARED = -fPIC --shared

DFLIB_SO = dflib.so

INSTALL_LIB=$(PREFIX)/lib

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all default clean cleanall install

all: $(DFLIB_SO)

$(DFLIB_SO): src/sharedata.c src/uip_sensor_alarm.c src/lgvariant.c src/dflib.c src/worker.c src/lworker.c
	$(CC) $(CFLAGS) $(MYCFLAGS) $(SHARED) $^ -o $@ -I$(LUA_INCLUDE) -I../3rd -I$(GLIB_INCLUDE_PATH) -I$(GLIB_INCLUDE_PATH2) -I$(GLIB_INCLUDE_PATH3) -L$(DFLIB_PATH) -DCOMPATIBILITY_ENABLED=$(COMPATIBILITY_ENABLED) -DSECURITY_ENHANCED_COMPATIBLE_BOARD_V1=$(SECURITY_ENHANCED_COMPATIBLE_BOARD_V1) -DSECURITY_ENHANCED_COMPATIBLE_BOARD_V3=$(SECURITY_ENHANCED_COMPATIBLE_BOARD_V3) -ldflib -lsecurec 

install:
	mkdir -p $(INSTALL_LIB)
	cp $(DFLIB_SO) $(INSTALL_LIB)
	${STRIP} $(INSTALL_LIB)/$(DFLIB_SO)

clean:
	rm -f $(DFLIB_SO)

cleanall: clean

