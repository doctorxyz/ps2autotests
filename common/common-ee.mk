# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: Makefile.eeglobal_cpp_sample 1525 2009-01-23 00:24:13Z radad $

# Define pefixes for the various toolchain binaries.
include $(COMMON_DIR)/common-defines.mk

# Include directories
EE_INCS := -I$(PS2DEV)/ee/ee/include -I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I. -I$(COMMON_DIR) $(EE_INCS)

# C compiler flags
EE_CFLAGS := -D_EE -O2 -G0 -Wall $(EE_CFLAGS)

# C++ compiler flags
EE_CXXFLAGS := -D_EE -O2 -G0 -Wall $(EE_CXXFLAGS)

# Linker flags
EE_LDFLAGS := -L$(PS2SDK)/ee/lib $(EE_LDFLAGS)

# Assembler flags
EE_ASFLAGS := -G0 $(EE_ASFLAGS)

# Link with following libraries.  This is a special case, and instead of
# allowing the user to override the library order, we always make sure
# libkernel is the last library to be linked.
EE_LIBS += -lstdc++ -lc -lkernel

EXTRA_OBJS += $(COMMON_DIR)/common-ee.o

# These macros can be used to simplify certain build rules.
EE_C_COMPILE = $(EE_CC) $(EE_CFLAGS) $(EE_INCS)
EE_CXX_COMPILE = $(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS)

%.o : %.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

%.o : %.cpp
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

%.o : %.S
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

%.o : %.s
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

%.vo : %.vsm
	$(DVP_AS) $(DVP_ASFLAGS) $< -o $@

%.elf : %.o $(EXTRA_OBJS) $(COMMON_DIR)/crt0-ee.o
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(EE_CXXFLAGS) \
		-o $@ $(COMMON_DIR)/crt0-ee.o $< $(EXTRA_OBJS) $(EE_LDFLAGS) $(EE_LIBS)
	$(EE_STRIP) --strip-all $@

all: $(TARGETS:=.elf)

clean:
	-$(RM) -f $(TARGETS:=.elf) $(TARGETS:=.o)

rebuild: clean all
