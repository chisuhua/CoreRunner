# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

ifeq (, $(shell which verilator))
 $(error "No Verilator in $(PATH), consider doing apt-get install verilator")
endif

# Change VERILATOR_INC_DIR if Verilator is installed on a different location
ifeq (, $(VERILATOR_INC_DIR))
  ifeq (, $(wildcard /usr/local/share/verilator/include/*))
    ifeq (, $(wildcard /usr/share/verilator/include/*))
      $(error "Verilator include directory is not set properly")
    else
      VERILATOR_INC_DIR := /usr/share/verilator/include
    endif
  else
      VERILATOR_INC_DIR := /usr/local/share/verilator/include
  endif
endif

TOP = TestAccel
BUILD_NAME = $(CO_BUILD_DIR)
USE_TRACE = 1
LIBNAME = libhw

vta_dir = $(abspath ../../../../)
tvm_dir = $(abspath ../../../../../../)
build_dir = $(abspath ../../../)/$(BUILD_NAME)/$(TOP)
verilator_build_dir = $(build_dir)/verilator
chisel_build_dir = $(build_dir)/chisel

verilator_opt = --cc
verilator_opt += +define+RANDOMIZE_GARBAGE_ASSIGN
verilator_opt += +define+RANDOMIZE_REG_INIT
verilator_opt += +define+RANDOMIZE_MEM_INIT
verilator_opt += --x-assign unique
verilator_opt += --output-split 20000
verilator_opt += --output-split-cfuncs 20000
verilator_opt += --top-module ${TOP}
verilator_opt += -Mdir ${verilator_build_dir}
verilator_opt += -I$(chisel_build_dir)

cxx_flags = -O2 -Wall -fPIC -shared
cxx_flags += -fvisibility=hidden -std=c++11
cxx_flags += -DVL_TSIM_NAME=V$(TOP)
cxx_flags += -DVL_PRINTF=printf
cxx_flags += -DVL_USER_FINISH
cxx_flags += -DVM_COVERAGE=0
cxx_flags += -DVM_SC=0
cxx_flags += -Wno-sign-compare
cxx_flags += -include V$(TOP).h
cxx_flags += -I$(verilator_build_dir)
cxx_flags += -I$(VERILATOR_INC_DIR)
cxx_flags += -I$(VERILATOR_INC_DIR)/vltstd
cxx_flags += -I$(vta_dir)/include
cxx_flags += -I$(tvm_dir)/include
cxx_flags += -I$(tvm_dir)/3rdparty/dlpack/include

cxx_files = $(VERILATOR_INC_DIR)/verilated.cpp
cxx_files += $(VERILATOR_INC_DIR)/verilated_dpi.cpp
cxx_files += $(wildcard $(verilator_build_dir)/*.cpp)
cxx_files += $(vta_dir)/hardware/dpi/tsim_device.cc

ifneq ($(USE_TRACE), 0)
  verilator_opt += --trace
  cxx_flags += -DVM_TRACE=1
  cxx_flags += -DTSIM_TRACE_FILE=$(verilator_build_dir)/$(TOP).vcd
  cxx_files += $(VERILATOR_INC_DIR)/verilated_vcd_c.cpp
else
  cxx_flags += -DVM_TRACE=0
endif

# The following is to be consistent with cmake
ifeq ($(shell uname), Darwin)
  lib_path = $(build_dir)/$(LIBNAME).dylib
else
  lib_path = $(build_dir)/$(LIBNAME).so
endif

default: lib

lib: $(lib_path)
$(lib_path): $(verilator_build_dir)/V$(TOP).cpp
	g++ $(cxx_flags) $(cxx_files) -o $@

verilator: $(verilator_build_dir)/V$(TOP).cpp
$(verilator_build_dir)/V$(TOP).cpp: $(chisel_build_dir)/$(TOP).v
	verilator $(verilator_opt) $<

verilog: $(chisel_build_dir)/$(TOP).v
	echo "Running make verilog to generate " $<

$(chisel_build_dir)/$(TOP).v:
	sbt 'test:runMain test.Elaborate --target-dir $(chisel_build_dir) --top-name $(TOP)'

install_vta_package:
	cd $(vta_dir)/hardware/chisel && sbt publishLocal

clean:
	-rm -rf $(build_dir) target project/target project/project
