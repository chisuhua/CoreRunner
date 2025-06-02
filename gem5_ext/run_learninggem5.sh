#!/bin/bash
# -*- coding: utf-8 -*-

OPTION="--debug-flags="
OPTION="${OPTION}AddrRanges"
OPTION="${OPTION},MemoryAccess"
OPTION="${OPTION},RubyPort"
OPTION="${OPTION},ShaderTLB"
OPTION="${OPTION},GPUCopyEngine"
RUN=./build/X86_MSI/gem5.debug


#CONF="configs/learning_gem5/part1/simple.py"
#CONF="configs/learning_gem5/part1/two_level.py"
#CONF="configs/learning_gem5/part2/hello_goodbye.py"
#CONF="configs/learning_gem5/part2/simple_memobj.py"
#CONF="configs/learning_gem5/part2/simple_cache.py"
#CONF="configs/learning_gem5/part3/simple_ruby.py"
CONF="configs/learning_gem5/part3/ruby_test.py"

CMD=""


ENV=""
CMD="$CMD $ENV"

GDB="-w"
GDB=""

echo $RUN $OPTION $CONF $GDB $CMD
$RUN $OPTION $CONF $GDB $CMD
