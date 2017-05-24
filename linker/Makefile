# Makefile template
PROJ_ROOT = ..

include $(PROJ_ROOT)/IncludePre.mk

LIBS_CYCLING_DEPEND = 1
SUB_DIRS = 

TARGET_NAME = linker
TARGET_SRCS = $(wildcard *.cpp)

DEPEND_LIBS = 

CFLAGS := $(CFLAGS) -I. -I./shell
LIBS += -lftomc_shell -lftgm -lrdkafka++ -lhl++ /home/xtra/Workspace/KompexSQLiteWrapper-Source_1.11.14/lib/debug/KompexSQLiteWrapper_Static_d.a -ljson -lnetsnmp -lpthread -ldl
include $(PROJ_ROOT)/IncludePost.mk
