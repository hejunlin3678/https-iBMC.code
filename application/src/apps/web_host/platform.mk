PLATS = linux arm-linux
PLAT= none

CC ?= gcc
HOSTCC ?= gcc
RS_TARGET ?= x86_64-unknown-linux-gnu
STRIP ?= strip

PWD_DIR := $(shell pwd)
OUTDIR ?= $(PWD_DIR)/out

.PHONY : none $(PLATS)

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

arm-linux: CC = arm32-linux-gnueabi-gcc
arm-linux: RS_TARGET = armv7-unknown-linux-gnueabi
arm-linux: STRIP = arm32-linux-gnueabi-strip

aarch64-linux: CC = aarch64-target-linux-gnu-gcc
aarch64-linux: RS_TARGET = aarch64-unknown-linux-gnu
aarch64-linux: STRIP = aarch64-target-linux-gnu-strip

# Glib include path
#GLIB_INCLUDE_PATH ?= $(PWD_DIR)/3rd/glib-2.0

# 3rd/glib-2.0 默认是 x64 的 glib 头文件,必须禁止 overflow 检查才能在 arm 32 下编译通过
arm-linux: MYCFLAGS ?= -D_GLIB_TEST_OVERFLOW_FALLBACK

# libdflib.so path
linux: DFLIB_PATH ?= /opt/pme/lib

# luajit
arm-linux: CROSS = arm32-linux-gnueabi-
arm-linux: LUAJIT_CROSS = CC=gcc HOST_CC=\"gcc -m32\" CROSS=$(CROSS) TARGET_CFLAGS=\"-mfloat-abi=soft\"
aarch64-linux: CROSS = aarch64-target-linux-gnu-
aarch64-linux: LUAJIT_CROSS = CC=gcc CROSS=$(CROSS)

linux arm-linux aarch64-linux:
	$(MAKE) all CC="$(CC)" \
              STRIP="$(STRIP)" \
              RS_TARGET="$(RS_TARGET)" \
              LUAJIT_CROSS="$(LUAJIT_CROSS)" \
              PREFIX="$(OUTDIR)" \
              INSTALL_TOP="$(OUTDIR)" \
              GLIB_INCLUDE_PATH="$(GLIB_INCLUDE_PATH)" \
              GLIB_INCLUDE_PATH2="$(GLIB_INCLUDE_PATH2)" \
              GLIB_INCLUDE_PATH3="$(GLIB_INCLUDE_PATH3)" \
              LUA_INCLUDE="$(LUA_INCLUDE)" \
              LUA_JSONC_INCLUDE="$(LUA_JSONC_INCLUDE)" \
              DFLIB_PATH="$(DFLIB_PATH)" \
              MYCFLAGS="$(MYCFLAGS)" \
              COMPATIBILITY_ENABLED="$(COMPATIBILITY_ENABLED)" \
              SECURITY_ENHANCED_COMPATIBLE_BOARD_V1="$(SECURITY_ENHANCED_COMPATIBLE_BOARD_V1)" \
              SECURITY_ENHANCED_COMPATIBLE_BOARD_V3="$(SECURITY_ENHANCED_COMPATIBLE_BOARD_V3)" \
              PLAT=$@
