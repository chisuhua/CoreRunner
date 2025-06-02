#!/bin/bash
# -*- coding: utf-8 -*-


# please source this script before compile the cuda_sample

export CUDA_INSTALL_PATH=/usr/local/cuda
export GPGPUSIM_ROOT="$( cd "$( dirname "$BASH_SOURCE" )" && pwd )"

export CUDA_VERSION_NUMBER=10010
export PTX_SIM_USE_PTX_FILE=1.ptx
export PTX_SIM_KERNELFILE=_1.ptx
export CUOBJDUMP_SIM_FILE=jj
export GPGPUSIM_CONFIG=libcuda

#export LD_LIBRARY_PATH=`echo $LD_LIBRARY_PATH | sed -re 's#'$GPGPUSIM_ROOT'\/lib\/[0-9]+\/(debug|release):##'`

#export LD_LIBRARY_PATH=$CUDA_INSTALL_PATH/lib64:$CUDA_INSTALL_PATH/lib:$CUDNN_PATH/lib64:$LD_LIBRARY_PATH

if [ -z "$LD_LIBRARY_PATH" ]; then
export LD_LIBRARY_PATH=$GPGPUSIM_ROOT/$GPGPUSIM_CONFIG
else
export LD_LIBRARY_PATH=$GPGPUSIM_ROOT/$GPGPUSIM_CONFIG:$LD_LIBRARY_PATH
fi

if [ -z "$PTXAS_CUDA_INSTALL_PATH" ]; then
    export PTXAS_CUDA_INSTALL_PATH=$CUDA_INSTALL_PATH;
fi

