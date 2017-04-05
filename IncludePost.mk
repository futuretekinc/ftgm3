# IncludePost.mk

LIB_FULL_NAME = $(ROOT_LIB_DIR)/$(OBJS_DIR)/lib$(LIB_NAME).a
LIB_OBJS = $(LIB_SRCS:%.cpp=$(OBJS_DIR)/%.o)

#ALL_LIBS = -l$(LIB_NAME) $(DEPEND_LIBS) $(LIBS)
ALL_LIBS = $(DEPEND_LIBS) $(LIBS)

TARGET_OBJS = $(TARGET_SRCS:%.cpp=$(OBJS_DIR)/%.o)
TARGET_NAMES = $(TARGET_SRCS:%.cpp=$(OBJS_DIR)/%)

.SUFFIXES : .cpp .o

all : lib subdirs targets

subdirs : 
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir all; \
		if [ $$? != 0 ]; then exit 1; fi; \
		done

lib : $(LIB_FULL_NAME)

liball : $(LIB_FULL_NAME) 
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir liball; \
		if [ $$? != 0 ]; then exit 1; fi; \
		done

targets : $(TARGET_NAMES)

$(LIB_FULL_NAME) : $(LIB_OBJS)
	@`[ -d $(ROOT_LIB_DIR)/$(OBJS_DIR) ] || $(MKDIR) -p $(ROOT_LIB_DIR)/$(OBJS_DIR)`
	$(AR) rcv $@ $(LIB_OBJS)
	$(RANLIB) $@

$(OBJS_DIR)/%.o : %.cpp
	@echo "==================================================="
	@echo "= Compiling $@ "
	@echo "==================================================="
	@`[ -d $(OBJS_DIR) ] || $(MKDIR) $(OBJS_DIR)`
	$(CC) $(CFLAGS) $(DBG_FLAGS) $(INC_DIRS) -c $< -o $@

.SECONDEXPANSION:
$(TARGET_NAMES): $$@.o
	@echo "==================================================="
	@echo "= Linking $@ "
	@echo "==================================================="
ifeq ($(LIBS_CYCLING_DEPEND),1)
	$(CC) -o $@ $< $(LIB_DIRS) -Wl,-\( $(ALL_LIBS) -Wl,-\)
else
	$(CC) -o $@ $< $(LIB_DIRS) $(ALL_LIBS)
endif
	@`[ -d $(ROOT_BIN_DIR)/$(OBJS_DIR) ] || $(MKDIR) -p $(ROOT_BIN_DIR)/$(OBJS_DIR)`
	$(CP) $@ $(ROOT_BIN_DIR)/$(OBJS_DIR)

depend :
	@`[ -d $(OBJS_DIR) ] || $(MKDIR) $(OBJS_DIR)`
	@$(RM) -f $(DEPEND_FILE)
	@for FILE in $(LIB_SRCS:%.c=%) $(TARGET_SRCS:%.c=%); do \
		$(CC) -MM -MT $(OBJS_DIR)/$$FILE.o $$FILE.c $(CFLAGS) $(DBG_FLAGS) $(INC_DIRS) >> $(DEPEND_FILE); \
	done

dependall : depend
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir dependall; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

clean :
	$(RM) -fr $(OBJS_DIR) $(LIB_FULL_NAME)

cleanall : clean
	@for dir in $(SUB_DIRS); do \
		$(MAKE) -C $$dir cleanall; \
		if [ $$? != 0 ]; then exit 1; fi; \
	done

$(TARGET_NAMES) : $(LIB_FULL_NAME) \
	$(DEPEND_LIBS:-l%=$(ROOT_LIB_DIR)/$(OBJS_DIR)/lib%.a)

ifneq ($(MAKECMDGOALS), clean)
ifneq ($(MAKECMDGOALS), cleanall)
ifneq ($(MAKECMDGOALS), depend)
ifneq ($(MAKECMDGOALS), dependall)
ifneq ($(strip $(LIB_SRCS) $(TARGET_SRCS)),)
-include $(DEPEND_FILE)
endif
endif
endif
endif
endif
