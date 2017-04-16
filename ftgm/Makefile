# Makefile template
PROJ_ROOT = ..

include $(PROJ_ROOT)/IncludePre.mk

LIBS_CYCLING_DEPEND = 1
SUB_DIRS = shell

TARGET_NAME = ftgm
TARGET_SRCS = $(wildcard *.cpp)

DEPEND_LIBS = 

CFLAGS := $(CFLAGS) -I. -I./shell
LIBS += -lshellftgm -lftgm -ljson -lnetsnmp -lpthread -ldl

include $(PROJ_ROOT)/IncludePost.mk
