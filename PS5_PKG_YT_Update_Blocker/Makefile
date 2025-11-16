PS5_PAYLOAD_SDK ?= /path/to/orbis-sdk
include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk

TARGET := yt_blocker.elf
TITLE_ID := YTBLK0001
CONTENT_ID := IV0000-$(TITLE_ID)_00-YTUPDATEBLOCKER0

SRCS := main.c sqlite3.c
OBJS := $(SRCS:.c=.o)

CFLAGS := -Wall -O2 -g
LDFLAGS := -lkernel -lSceSysmodule -lSceSaveData -lc

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

pkg: $(TARGET)
	@mkdir -p pkg/sce_sys
	@cp $(TARGET) pkg/eboot.bin
	@$(ORBIS_SFO) pkg/sce_sys/param.sfo \
		--title "YT Update Blocker" \
		--title-id $(TITLE_ID) \
		--content-id $(CONTENT_ID) \
		--category gd \
		--version 01.00
	@[ -f icon0.png ] && cp icon0.png pkg/sce_sys/icon0.png || echo "No icon"
	@orbis-pkg pkg yt_blocker.pkg pkg $(TITLE_ID)
	@echo "PKG created: yt_blocker.pkg"

clean:
	rm -f $(TARGET) *.o yt_blocker.pkg
	rm -rf pkg