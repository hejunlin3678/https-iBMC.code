CC=arm-wrs-linux-gnueabi-armv5tel-glibc_small-gcc
#CC=gcc
CFLAGS= -g -DLINUX -DBD_LITTLE_ENDIAN  --sysroot=/WRLProject/iMana200_sysroot
INCLUDES=-I . -I ../mscm/include -I ../mscm/ipmi -I ../msam/include
#maoali add -lmscm_debug.so
LIBS=-lpthread -lm -L../msam/libmsam -L../mscm/libmscm -lipmc_drv_intf -lmscm_ipc -lmscm_eventparse -lmscm_vos -lmscm_cfgparse -lmscm_debug -lmscm_tar

OBJSGET=ipmcgetmain.o \
     ipmcgetcommand.o      \
     ipmcgethelp.o 
     
OBJSSET=ipmcsetmain.o \
     ipmcsetcommand.o      \
     ipmcsethelp.o  

OBJSCMD=ipmicommand.o      

OBJBAKPOINT=setbackpointmain.o
     
COMOBJS1=ipmc_sdr.o\
     ipmc_fru.o     \
     ipmc_sel.o       \
     ipmc_sensor.o    \
     ipmc_public.o \
     ipmi_pack_sel.o 
     
all:$(OBJSGET) $(OBJSSET) $(OBJSCMD) $(COMOBJS1)
	$(CC) $(LIBS) $(CFLAGS) $(OBJSGET) $(COMOBJS1) -o ipmcget \
	& $(CC) $(LIBS) $(CFLAGS) $(OBJSSET) $(COMOBJS1) -o ipmcset \
	& $(CC) $(LIBS) $(CFLAGS) $(OBJSCMD) -o ipmicmd
	& $(CC) $(LIBS) $(CFLAGS) $(OBJBAKPOINT) -o setbackpoint
	
GET:$(OBJSGET) $(COMOBJS1)
	$(CC) $(LIBS) $(OBJSGET) $(COMOBJS1) -o ipmcget
SET:$(OBJSSET) $(COMOBJS1)
	$(CC) $(LIBS) $(OBJSSET) $(COMOBJS1) -o ipmcset
CMD:$(OBJSCMD)
	$(CC) $(LIBS) $(OBJSCMD)) -o ipmicmd
BAKPOINT:
	$(CC) $(LIBS) $(OBJBAKPOINT)) -o setbackpoint

$(OBJSGET):%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES)  -c $<
$(OBJSSET):%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES)  -c $<	
$(OBJSCMD):%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES)  -c $<
$(COMOBJS1):%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES)  -c $<
$(COMOBJS2):%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES)  -c $<
$(OBJBAKPOINT):%.o:%.c
	$(CC) $(CFLAGS) $(INCLUDES)  -c $<	
clean:
	rm -f *.o 
