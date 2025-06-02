#!/bin/bash
# -*- coding: utf-8 -*-

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

TEST=$DIR/configs/learning_gem5/part3/ruby_test.py

OPTION=""
OPTION="$OPTION -v"
#OPTION="$OPTION -d TrafficGen"
#OPTION="$OPTION -d MemTest"
#OPTION="$OPTION -d MemoryAccess"
#OPTION="$OPTION -d CxxConfig"
#OPTION="$OPTION -d ProtocolTrace"
#OPTION="$OPTION -d RubyTest"
OPTION="$OPTION -d RubyGenerated"
OPTION="$OPTION -d RubyNetwork"
OPTION="$OPTION -d RubyQueue"
OPTION="$OPTION -d RubyMessageBuffer"

#OPTION="$OPTION -d MemCheker"
OPTION="$OPTION -e 500000000"

OPTION="$OPTION -d Event"

#OPTION="-v -d CxxConfig -d TrafficGen -d Event -e 10000"
#OPTION="-v -d TrafficGen -d Event -e 2000000"
#OPTION="-v -e 500000 -d TrafficGen -d Event"
#OPTION="-v -e 500000 -d Event"
#OPTION="-v -e 5000000 -d TrafficGen"


if [ ! -e "m5out/config.ini" ]; then
GEM5=$DIR/../../build/X86_MSI/gem5.debug
echo "Running $GEM5 $TEST to generate m5out/config.ini"
$GEM5 $TEST
fi

#CMD="./build/examples/slave_port/gem5.sc m5out/config.ini $OPTION "
#CMD="./build/output/example_master/axi4lite/gem5.sc "
CMD="./build/output/$CO_DESIGN/gem5.sc "

CMD="$CMD m5out/config.ini $OPTION "


echo "Running cosim"
echo $CMD
#$CMD
