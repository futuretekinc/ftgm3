# IncludePre.mk

TARGET="FTM-100"

ifeq ($(RELEASE), 1)
OBJS_DIR = Release
DBG_FLAGS = -O2 -DNDEBUG
else
OBJS_DIR = Debug
DBG_FLAGS = -g -O0 -DNDEBUG
endif


ifeq ($(TARGET), "FTM-100")
CROSS_COMPILE=arm-openwrt-linux-uclibcgnueabi-
INC_DIRS =  -I$(PROJ_ROOT)/Include \
			-I$(PROJ_ROOT)/lib \
			-I$(PROJ_ROOT)/.. \
			-I$(PROJ_ROOT)/../cortina/target/usr/local/include \
			-I$(PROJ_ROOT)/../cortina/KompexSQLiteWrapper-Source_1.11.14/inc\
			-I$(PROJ_ROOT)/../ftm/build/ftm-100s/_root/usr/include\
			-I$(PROJ_ROOT)/../ftm/build/ftm-100s/_root/usr/local/include

LIB_DIRS = -L$(ROOT_LIB_DIR)/$(OBJS_DIR)\
			-L$(PROJ_ROOT)/../cortina/target/usr/local/lib \
			-L$(PROJ_ROOT)/../ftm/build/ftm-100s/_root/usr/lib \
			-L$(PROJ_ROOT)/../ftm/build/ftm-100s/_root/usr/local/lib
LIBS += -lshellftgm -lftgm -lmosquitto -lmodbus -lhl++ ../../cortina/KompexSQLiteWrapper-Source_1.11.14/lib/debug/KompexSQLiteWrapper_Static_d.a -ljson -lnetsnmp -lpthread -ldl
else
ifeq ($(TARGET), "FTM-50")
CROSS_COMPILE=armv5-linux-
INC_DIRS =  -I$(PROJ_ROOT)/Include \
			-I$(PROJ_ROOT)/lib \
			-I$(PROJ_ROOT)/.. \
			-I$(PROJ_ROOT)/../spear/target/usr/local/include\
			-I$(PROJ_ROOT)/../spear/KompexSQLiteWrapper-Source_1.11.14/inc\
			-I$(PROJ_ROOT)/../ftm/build/ftm-50s/_root/usr/include\
			-I$(PROJ_ROOT)/../ftm/build/ftm-50s/_root/usr/local/include

LIB_DIRS = -L$(ROOT_LIB_DIR)/$(OBJS_DIR)\
			-L$(PROJ_ROOT)/../spear/target/usr/lib \
			-L$(PROJ_ROOT)/../spear/target/usr/local/lib \
			-L$(PROJ_ROOT)/../ftm/build/ftm-50s/_root/usr/lib \
			-L$(PROJ_ROOT)/../ftm/build/ftm-50s/_root/usr/local/lib
LIBS += -lshellftgm -lftgm -lmosquitto -lmodbus -lhl++ ../../spear/KompexSQLiteWrapper-Source_1.11.14/lib/debug/KompexSQLiteWrapper_Static_d.a -ljson -lnetsnmp -lpthread -ldl
else
INC_DIRS = -I$(PROJ_ROOT)/Include -I$(PROJ_ROOT)/lib -I$(PROJ_ROOT)/../kompex-sqlite-wrapper/include -I$(PROJ_ROOT)/..
LIB_DIRS = -L$(ROOT_LIB_DIR)/$(OBJS_DIR)
LIBS += -L ../../kompex-sqlite-wrapper/lib -lshellftgm -lftgm -lmosquitto -lmodbus -lhl++ -lkompex-sqlite-wrapper -ljson -lnetsnmp -lpthread -ldl
endif
endif

CC = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib
RM = rm
MKDIR = mkdir
MAKE = make
CP = cp

ROOT_SRC_DIR = $(PROJ_ROOT)/src
ROOT_OUT_DIR = $(PROJ_ROOT)/out
ROOT_LIB_DIR = $(ROOT_OUT_DIR)/lib
ROOT_BIN_DIR = $(ROOT_OUT_DIR)/bin

DEPEND_FILE = $(OBJS_DIR)/depend_file
