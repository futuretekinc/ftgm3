# Makefile template
PROJ_ROOT = ..

include $(PROJ_ROOT)/IncludePre.mk

LIBS_CYCLING_DEPEND = 1
SUB_DIRS = shell

TARGET_NAME = ftgm
TARGET_SRCS = $(wildcard *.cpp)

DEPEND_LIBS =

CFLAGS := $(CFLAGS) -I. -I./shell
ifeq ($(TARGET), "FTM-50")
LIBS += -lshellftgm -lftgm -lmosquitto -lrdkafka++ -lrdkafka -lhl++ ../../spear/KompexSQLiteWrapper-Source_1.11.14/lib/debug/KompexSQLiteWrapper_Static_d.a -ljson -lnetsnmp -lpthread -ldl
else 
ifeq ($(TARGET), "FTM-100")
LIBS += -lshellftgm -lftgm -lmosquitto -lrdkafka++ -lrdkafka -lhl++ ../../cortina/KompexSQLiteWrapper-Source_1.11.14/lib/debug/KompexSQLiteWrapper_Static_d.a -ljson -lnetsnmp -lpthread -ldl
else
LIBS += -lshellftgm -lftgm -lmosquitto -lrdkafka++ -lrdkafka -lhl++ ../../KompexSQLiteWrapper-Source_1.11.14/lib/debug/KompexSQLiteWrapper_Static_d.a -ljson -lnetsnmp -lpthread -ldl
endif
endif

include $(PROJ_ROOT)/IncludePost.mk
