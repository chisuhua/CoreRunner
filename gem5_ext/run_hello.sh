#!/bin/bash
# -*- coding: utf-8 -*-

OPTION="--debug-flags="
OPTION="${OPTION}AddrRanges"
OPTION="${OPTION},MemoryAccess"
OPTION="${OPTION},RubyPort"
OPTION="${OPTION},ShaderTLB"
OPTION="${OPTION},GPUCopyEngine"
RUN=./build/X86_VI_hammer/gem5.$GEM5_VARIANT


## NOTE: we have  4 option to run, simple_gpgpu.py is simplest
#		2. config/example/se.py is gem5 original se
#		3. design/gpgpu/config/se_fusion.py is gem5-gpu provoide se for gpgpu
#		4. design/cosim/config/se_fusion.py is for ppu cosim
#RUN=./build/X86/gem5.debug
#CONF="configs/example/simple_gpgpu.py"
#CONF="configs/example/se.py"
#CONF="design/gpgpu/configs/se_fusion.py"
CONF="design/cosim/configs/se_fusion.py"

CMD="-c ./design/gpgpu/gpgpu-sim/cuda_samples/0_Simple/hello/hello --ppu"


ENV="-e design/gpgpu/gpgpu-sim/gem5.env"
CMD="$CMD $ENV"

GDB="-w"
GDB=""

echo $RUN $OPTION $CONF $GDB $CMD
$RUN $OPTION $CONF $GDB $CMD
