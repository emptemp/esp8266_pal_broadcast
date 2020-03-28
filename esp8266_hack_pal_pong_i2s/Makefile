FW_FILE_1:=image.elf-0x00000.bin
FW_FILE_2:=image.elf-0x40000.bin
TARGET_OUT:=image.elf
all : $(TARGET_OUT) $(FW_FILE_1) $(FW_FILE_2)

SRCS:=driver/uart.c \
      user_main.c  \
      user/nosdki2s.c  \
      user/startup.S \
      user/broadcast.c  \
      user/tsgp.c


#	    /user/tsgpal.c
#      /user/nosdki2s.c
      
# use new esp-open-sdk for gcc, esptool
# use _OLD_ esp-open-sdk as actual sdk, some stuff is not working with the new sdk
GCC_FOLDER:=../SDK/esp-open-sdk/xtensa-lx106-elf
ESPTOOL_PY:=../SDK/esp-open-sdk/esptool/esptool.py
SDK:=../SDK/OLD_esp_sdk/esp-open-sdk/esp_iot_sdk_v1.5.2
PORT:=/dev/ttyUSB0

XTLIB:=$(SDK)/lib
XTGCCLIB:=$(GCC_FOLDER)/lib/gcc/xtensa-lx106-elf/4.8.5/libgcc.a
FOLDERPREFIX:=$(GCC_FOLDER)/bin
CC:=$(FOLDERPREFIX)/$(PREFIX)gcc
PREFIX:=$(FOLDERPREFIX)/xtensa-lx106-elf-

#CFLAGS:=-mlongcalls -I$(SDK)/include -Imyclib -Iinclude -Iuser -Os -I$(SDK)/include/ -Icommon -DICACHE_FLASH
CFLAGS:=-mlongcalls -I$(SDK)/include -Imyclib -Iinclude -Iuser -Os -I$(SDK)/include/ -Icommon -DICACHE_FLASH -DUSE_I2S

LDFLAGS_CORE:=\
	-nostdlib \
	-L$(XTLIB) \
	-L$(XTGCCLIB) \
	-g \
	$(SDK)/lib/libmain.a \
	$(SDK)/lib/libpp.a \
	$(SDK)/lib/libnet80211.a \
	$(SDK)/lib/libwpa.a \
	$(SDK)/lib/liblwip.a \
	$(SDK)/lib/libssl.a \
	$(SDK)/lib/libupgrade.a \
	$(SDK)/lib/libnet80211.a \
	$(SDK)/lib/liblwip.a \
	$(SDK)/lib/libphy.a \
	$(SDK)/lib/libcrypto.a \
	$(XTGCCLIB) \
	-T $(SDK)/ld/eagle.app.v6.ld

LINKFLAGS:= \
	$(LDFLAGS_CORE) \
	-B$(XTLIB)


$(TARGET_OUT) : $(SRCS)
	echo $$PATH
	$(PREFIX)gcc $(CFLAGS) $^  $(LINKFLAGS) -o $@
	nm -S -n $(TARGET_OUT) > image.map
	$(PREFIX)objdump -S $@ > image.lst

$(FW_FILE_1): $(TARGET_OUT)
	@echo "FW $@"
	PATH=$(FOLDERPREFIX):$$PATH;$(ESPTOOL_PY) elf2image $(TARGET_OUT)

$(FW_FILE_2): $(TARGET_OUT)
	@echo "FW $@"
	PATH=$(FOLDERPREFIX):$$PATH;$(ESPTOOL_PY) elf2image $(TARGET_OUT)

burn : $(FW_FILE_1) $(FW_FILE_2)
	($(ESPTOOL_PY) --baud 115200 --port $(PORT) write_flash 0x00000 image.elf-0x00000.bin 0x40000 image.elf-0x40000.bin)||(true)

clean :
	rm -rf *.o user/*.o driver/*.o $(TARGET_OUT) $(FW_FILE_1) $(FW_FILE_2)

CFLAGS:=-mlongcalls -I$(SDK)/include -Imyclib -Iinclude -Iuser -Os -I$(SDK)/include/ -Icommon -DICACHE_FLASH

