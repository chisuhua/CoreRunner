#!/bin/bash
# -*- coding: utf-8 -*-


#TEST=configs/learning_gem5/part3/simple_ruby.py
TEST=configs/learning_gem5/part3/ruby_test.py
TEST=configs/example/ruby_random_test.py
TEST=configs/example/ruby_mem_test.py

OPTION="--num-cpus=16 --num-dirs=16 --network=simple --topology=Mesh_XY --mesh-rows=4"
OPTION="$OPTON --debug-flags=AddrRanges,ProtocolTrace,RubyTest,RubyQueue,RubyMessageBuffer,RubySequencer"
#OPTION="--debug-flags=ProtocolTrace,RubyTest,RubyQueue,RubyMessageBuffer,RubySequencer"
#TEST=configs/learning_gem5/part3/simple_ruby.py

#RUN_GEM5=./build/X86_MSI/gem5.debug
RUN_GEM5=./build/X86_VI_hammer/gem5.$GEM5_VARIANT
#RUN_GEM5=./build/NULL_MOESI_hammer/gem5.debug

echo $RUN_GEM5 $OPTION $TEST
$RUN_GEM5 $OPTION $TEST
