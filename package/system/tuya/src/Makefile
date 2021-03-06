ROOT_DIR        = $(abspath .)
OUT_DIR         = $(ROOT_DIR)/output/$(APP_BIN_NAME)
OUT_OBJS_DIR    = $(OUT_DIR)/objs
	
COMPILE_PREX   	= mipsel-openwrt-linux-musl-

AR   = $(COMPILE_PREX)ar
CC   = $(COMPILE_PREX)gcc
NM   = $(COMPILE_PREX)nm
CPP  = $(COMPILE_PREX)g++

CFLAGS    += $(SDK_INC) $(USER_INC) -g
LDFLAGS   += -L$(SDK_DIR)/lib -ltuya_gw_ext_sdk -pthread -lm -ldl

APP_BIN_NAME = demo_dev

#
# Tuya SDK
#
SDK_DIR    = $(ROOT_DIR)/sdk
SDK_INC    = $(addprefix -I, $(shell find $(SDK_DIR)/include -type d))

# 
# User APP
#
USER_PATH  = $(ROOT_DIR)/demos/$(APP_BIN_NAME)
# USER_INC   = $(addprefix -I, $(shell find $(USER_DIR)/include -type d))
USER_FILES = $(foreach dir, $(shell find $(USER_PATH) -type d), $(wildcard $(dir)/*.c))
USER_OBJS  = $(addsuffix .o, $(basename $(USER_FILES)))

-include $(USER_PATH)/custom.conf

CFLAGS    += $(USER_CFLAGS)
LDFLAGS   += $(USER_LDFLAGS)

PROC = user_iot

build_app: $(USER_OBJS)
	$(CC) $(subst $(ROOT_DIR),$(OUT_OBJS_DIR),$(USER_OBJS)) -o $(ROOT_DIR)/user_iot $(LDFLAGS)
	@echo "compile done"
	@echo "ROOT_DIR = $(ROOT_DIR)"
	@echo "OUT_DIR = $(OUT_DIR)"
	@echo "OUT_OBJS_DIR = $(OUT_OBJS_DIR)"

%.o : %.c
	@mkdir -p $(dir $(subst $(ROOT_DIR),$(OUT_OBJS_DIR), $@));
	$(CC) $(CFLAGS) -o  $(subst $(ROOT_DIR),$(OUT_OBJS_DIR), $@) -c $<

%.o: %.cpp
	@mkdir -p $(dir $(subst $(ROOT_DIR),$(OUT_OBJS_DIR), $@));
	$(CC) $(CFLAGS) -o  $(subst $(ROOT_DIR),$(OUT_OBJS_DIR), $@) -c $<

%.o: %.s
	@mkdir -p $(dir $(subst $(ROOT_DIR),$(OUT_OBJS_DIR), $@));
	$(CC) $(CFLAGS) -o  $(subst $(ROOT_DIR),$(OUT_OBJS_DIR), $@) -c $<

clean:
	rm -fr $(ROOT_DIR)/output
