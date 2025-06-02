#!/bin/bash
# -*- coding: utf-8 -*-
SCRIPTDIR=$(cd $(dirname "${BASH_SOURCE[0]}") > /dev/null && pwd)
export CO_BUILD_DIR=build

# CO_DESIGN is sub-dirc under src
export CO_DESIGN=co_design/simple_ppu
#export CO_DESIGN=example_slave/axi4lite
#export CO_DESIGN=example_master/axi4lite
#export CO_DESIGN=example_master/axi
#export CO_DESIGN=example_master/axis
#export CO_DESIGN=example_master/simple_ppu
#export GEM5_ARCH=X86_MSI
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SCRIPTDIR

#source ~/p372_init.sh
