# Copyright (c) 2009 by Tor M. Aamodt, Ali Bakhoda and the
# University of British Columbia
# Vancouver, BC  V6T 1Z4
# All Rights Reserved.
#
# THIS IS A LEGAL DOCUMENT BY DOWNLOADING GPGPU-SIM, YOU ARE AGREEING TO THESE
# TERMS AND CONDITIONS.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNERS OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# NOTE: The files libcuda/cuda_runtime_api.c and src/cuda-sim/cuda-math.h
# are derived from the CUDA Toolset available from http://www.nvidia.com/cuda
# (property of NVIDIA).  The files benchmarks/BlackScholes/ and
# benchmarks/template/ are derived from the CUDA SDK available from
# http://www.nvidia.com/cuda (also property of NVIDIA).  The files from
# src/intersim/ are derived from Booksim (a simulator provided with the
# textbook "Principles and Practices of Interconnection Networks" available
# from http://cva.stanford.edu/books/ppin/). As such, those files are bound by
# the corresponding legal terms and conditions set forth separately (original
# copyright notices are left in files from these sources and where we have
# modified a file our copyright notice appears before the original copyright
# notice).
#
# Using this version of GPGPU-Sim requires a complete installation of CUDA
# which is distributed seperately by NVIDIA under separate terms and
# conditions.  To use this version of GPGPU-Sim with OpenCL requires a
# recent version of NVIDIA's drivers which support OpenCL.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the University of British Columbia nor the names of
# its contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# 4. This version of GPGPU-SIM is distributed freely for non-commercial use only.
#
# 5. No nonprofit user may place any restrictions on the use of this software,
# including as modified by the user, by any other authorized user.
#
# 6. GPGPU-SIM was developed primarily by Tor M. Aamodt, Wilson W. L. Fung,
# Ali Bakhoda, George L. Yuan, at the University of British Columbia,
# Vancouver, BC V6T 1Z4


#include ../version_detection.mk

ifeq ($(OPENGL_SUPPORT),1)
	GL = -DOPENGL_SUPPORT
endif

ifeq ($(GNUC_CPP0X), 1)
    CXXFLAGS = -std=c++0x
endif

CUDA_INSTALL_PATH ?= /usr/local/cuda/
CPP = g++ $(SNOW)
CC = gcc $(SNOW)
CREATELIBRARY    =  1
DEBUG ?= 1
ifeq ($(DEBUG),1)
	CXXFLAGS +=  -Wall -Wno-unused-function -Wno-sign-compare -g -fPIC $(GL)
	CCFLAGS += -Wall -Wno-unused-function -Wno-sign-compare -ggdb -fPIC
else
	CXXFLAGS += -O3 -g -Wall -Wno-unused-function -Wno-sign-compare -fPIC $(GL)
	CCFLAGS += -Wall -Wno-unused-function -Wno-sign-compare -fPIC
endif


# TODO build to so file in next step

PROG     =cuda
M5OP = m5op_x86
INCLUDES = -I./ -I../src -I$(OUTPUT_DIR) -I$(CUDA_INSTALL_PATH)/include -I../../../../include -I../../../../build/X86_VI_hammer -I../opu/isasim/isa -I../opu/isasim/model -I../opu/umd -I../opu/coasm -I../opu/opusim/src

CXX_SRCS =  cuda_runtime_api.cc stream_manager.cc gpgpusim_entrypoint.cc abstract_hardware_model.cc \
	    gpu-sim.cc \
	    addrdec.cc \
	    gpu-misc.cc \
		debug.cc \
	    option.cc \
	    shader.cc

	#   mem_latency_stat.cc \
	#	gpu-cache.cc \
	#    ../src/gpgpu-sim/dram.cc \
#            ../src/cuda-sim/cuda_device_printf.cc \
#            ../src/cuda-sim/cuda_device_runtime.cc \

#C_SRCS =  ../src/cuda-sim/ptxinfo.tab.c \
#	  ../src/cuda-sim/ptx.tab.c \
#          ../src/cuda-sim/lex.ptx_.c \
#          ../src/cuda-sim/lex.ptxinfo_.c

LEX_SRCS = cuobjdump.l
YACC_SRCS = cuobjdump.y
CXXFLAGS += -DCUDART_VERSION=$(CUDA_VERSION_NUMBER) -DLIBCUDA
CCFLAGS += -DCUDART_VERSION=$(CUDA_VERSION_NUMBER) -DLIBCUDA
CUDA_SIM_OBJS = $(shell ls cuda-sim/*.o cuda-sim/decuda_pred_table/*.o)
LEX			= flex
LEXFLAGS	= -B
YACC		= bison
YFLAGS		= -t -d -v --report=all

.PHONY: clean cuda-sim gem5cuda gem5m5op

#OUTPUT_DIR=$(SIM_OBJ_FILES_DIR)/libcuda
OUTPUT_DIR=.

OBJS = $(CXX_SRCS:%.cc=$(OUTPUT_DIR)/%.o)
#OBJS += $(C_SRCS:%.c=$(OUTPUT_DIR)/%.o)
OBJS += $(LEX_SRCS:%.l=$(OUTPUT_DIR)/%_lexer.o)
OBJS += $(YACC_SRCS:%.y=$(OUTPUT_DIR)/%_parser.o)


#--- Make rules ---
#lib$(PROG).a: $(OBJS) $(M5OP).o
#	echo $(OBJS)
#	ar rcs $(OUTPUT_DIR)/lib$(PROG).a $(OBJS)
#	ar rcs $(OUTPUT_DIR)/lib$(M5OP).a $(M5OP).o
all: libcudart.so
#all: libcudart.so libcudart_static.a

#libcudart.so: cuda-sim gem5cuda gem5umd gem5m5op $(OBJS)
libcudart.so: cuda-sim $(OBJS)
	#g++ -shared -Wl,-soname,libcudart.so -Wl,--version-script=./linux-so-version.txt $(OBJS) $(CUDA_SIM_OBJS) gem5/m5op/m5op_x86.o gem5/m5op/gem5_syscall_util.o gem5/cuda/gem5cuda_runtime_api.o gem5/opuumd/gem5umd_runtime_api.o -lm -lz -lGL -ldl -pthread -L$(DESIGN_ROOT)/cosim -lumd -lisasim -o $(OUTPUT_DIR)/libcudart.so
	g++ -shared -Wl,-soname,libcudart.so -Wl,--version-script=./linux-so-version.txt $(OBJS) $(CUDA_SIM_OBJS) gem5/m5op/m5op_x86.o gem5/m5op/gem5_syscall_util.o gem5/cuda/gem5cuda_runtime_api.o gem5/opuumd/gem5umd_runtime_api.o -lm -lz -lGL -ldl -pthread -L$(DESIGN_ROOT)/cosim -o $(OUTPUT_DIR)/libcudart.so

#libcudart_static.a: $(OBJS) gem5cuda gem5umd
#	$(AR) rcs libcudart_static.a $(OBJS) ./gem5/cuda/gem5cuda_runtime_api.o ./gem5/opuumd/gem5umd_runtime_api.o ./gem5/m5op/m5op_x86.o ./gem5/m5op/gem5_syscall_util.o

$(OUTPUT_DIR)/%.o: %.cc
	$(CPP) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OUTPUT_DIR)/%.o: %.c
	$(CPP) $(CCFLAGS) $(INCLUDES)  -c $< -o $@

#$(OUTPUT_DIR)/$(M5OP).o: $(M5OP_PATH)/$(M5OP).S
#	$(CC) $(CCFLAGS) $(INCLUDES)  -c $< -o $@


$(OUTPUT_DIR)/cuobjdump_parser.c: cuobjdump.y
	$(YACC) $(YFLAGS) -p cuobjdump_  -o$@ $< --file-prefix=$(OUTPUT_DIR)/cuobjdump

$(OUTPUT_DIR)/cuobjdump_lexer.c: cuobjdump.l $(OUTPUT_DIR)/cuobjdump_parser.h
	$(LEX) $(LEXFLAGS) -P cuobjdump_ -o$@ $<

# The next rule means just get parser.c and you will get parser.h with it
# in other words, get parser.c and do nothing to get parser.h
$(OUTPUT_DIR)/%_parser.h: $(OUTPUT_DIR)/%_parser.c
	:
#$(OUTPUT_DIR)/%_parser.h: $(OUTPUT_DIR)/%_parser.cc
#	:
$(OUTPUT_DIR)/Makefile.makedepend: depend

depend: cuda-sim
	$(MAKE) -C ./cuda-sim
	touch $(OUTPUT_DIR)/Makefile.makedepend
	makedepend -f$(OUTPUT_DIR)/Makefile.makedepend -p$(OUTPUT_DIR)/ $(CXX_SRCS) 2> /dev/null

cuda-sim:
	$(MAKE) -C ./cuda-sim/ depend
	$(MAKE) -C ./cuda-sim/

#gem5m5op:
#	$(MAKE) -C ./gem5/m5op/
#
#gem5cuda: gem5m5op
#	$(MAKE) -C ./gem5/cuda/
#
#gem5umd: gem5m5op
#	$(MAKE) -C ./gem5/opuumd/

clean:
	make -C cuda-sim clean
	make -C gem5/m5op clean
	make -C gem5/cuda clean
	make -C gem5/opuumd clean
	rm -f $(PROG)
	rm -f *.o
	rm -f lib$(PROG).a
	rm -f *_parser.*
	rm -f *_lexer.*
	rm -f Makefile.makedepend Makefile.makedepend.bak

include $(OUTPUT_DIR)/Makefile.makedepend

