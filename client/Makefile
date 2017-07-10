# Makefile template
PROJ_ROOT = ..

include $(PROJ_ROOT)/IncludePre.mk

LIBS_CYCLING_DEPEND = 1
SUB_DIRS = lib

TARGET_NAME = ftgmc
TARGET_SRCS = $(wildcard *.cpp)

DEPEND_LIBS = 

CFLAGS := $(CFLAGS) -I. -I./shell
LIBS := $(LIBS) -lftgmc_shell 
include $(PROJ_ROOT)/IncludePost.mk
