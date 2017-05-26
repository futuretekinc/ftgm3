# Makefile template
PROJ_ROOT = ..

include $(PROJ_ROOT)/IncludePre.mk

LIBS_CYCLING_DEPEND = 1
SUB_DIRS = shell

TARGET_NAME = ftgm
TARGET_SRCS = $(wildcard *.cpp)

DEPEND_LIBS =

CFLAGS := $(CFLAGS) -I. -I./shell
LIBS += -lshellftgm -lftgm -lrdkafka++ -lhl++ ../../KompexSQLiteWrapper-Source_1.11.14/lib/debug/KompexSQLiteWrapper_Static_d.a -ljson -lnetsnmp -lpthread -ldl
include $(PROJ_ROOT)/IncludePost.mk
