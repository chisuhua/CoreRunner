

GNUC_CPP0X := $(shell gcc --version | perl -ne 'if (/gcc\s+\(.*\)\s+([0-9.]+)/){ if($$1 >= 4.3) {$$n=1} else {$$n=0;} } END { print $$n; }')
ifeq ($(GNUC_CPP0X), 1)
    CXXFLAGS = -std=c++0x
endif


CPP = g++
CC = gcc
AR = ar
ifeq ($(ARCH),ARM32)
	CPP = arm-linux-gnueabihf-g++-4.6
	CC = arm-linux-gnueabihf-gcc-4.6
	AR = arm-linux-gnueabihf-ar
endif

DEBUG ?= 1
RELEASE ?= 0
ifeq ($(RELEASE),1)
ifeq ($(DEBUG),1)
$(error Cannot build both debug and release)
endif
	CXXFLAGS += -DNDEBUG
endif

ifeq ($(DEBUG),1)
	CXXFLAGS += -Wall -g -fPIC $(GL) -D__DEBUG__
else
	CXXFLAGS += -O3 -g -Wall -fPIC $(GL)
endif

MEM_DEBUG ?= 0
ifeq ($(MEM_DEBUG),1)
	CXXFLAGS += -D__MEM_DEBUG__
endif

NO_TOUCH_PAGES ?= 0
ifeq ($(NO_TOUCH_PAGES),1)
	CXXFLAGS += -DNO_TOUCH_PAGES
endif

# NOTE: You may need to change this path if you are not using the
# canonical gem5-gpu directory structure. This directory includes
# m5op.h, m5ops.h, and the m5op_<arch>.S assembly files
#M5OP_PATH = ../../gem5/util/m5
# we assume the depot dir is gem5
M5OP_PATH = ../../../../util/m5
INCLUDES = -I./ -I$(CMDIO_PATH) -I../libgem5cuda  -I$(M5OP_PATH) -I../../../../include

PROG = gem5ppucmdio
M5OP = m5op_x86

CXX_SRCS = gem5_ppucmdio_api.cc

.PHONY: clean

OBJS = $(CXX_SRCS:%.cc=%.o)


#--- Make rules ---
lib$(PROG).a: $(OBJS) $(M5OP).o
	$(AR) rcs lib$(PROG).a $(OBJS) $(M5OP).o
	#$(AR) rcs lib$(M5OP).a $(M5OP).o

%.o: %.cc
	$(CPP) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(M5OP).o: $(M5OP_PATH)/$(M5OP).S
	$(CPP) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o
	rm -f lib$(PROG).a lib$(M5OP).a
