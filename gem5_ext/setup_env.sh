DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# is is used to build benchmark
#
if [ x"$PPFLOW_ROOT" == "x" ];
then
	PPFLOW_ROOT=$DIR/../../mixlang
fi


export GEM5_EXT_ROOT=$DIR
export GEM5_VARIANT=debug
#export GEM5_VARIANT=opt
#export GEM5_ARCH=X86_VI_hammer
export GEM5_ARCH=CoreRunner_GPU
#export CUDAHOME=/usr/local/cuda/
#export CUDAHOME=$DIR/cuda3.2

export GEM5_CPU_CORES="$(cat /proc/cpuinfo | grep "processor" | wc -l)"
echo "gem5 root $GEM5_EXT_ROOT, cpu cores $GEM5_CPU_CORES"

NVCC_PATH=`which nvcc`
if [ x$NVCC_PATH == "x" ]; then
	echo "Can't find nvcc "
	return 1
fi

export CUDAHOME=`echo $NVCC_PATH | sed -re 's#/bin/nvcc##'`
echo "find cuda path $CUDAHOME"

CUDA_VERSION=`nvcc --version |grep release | sed 's#.*release ##' | cut -d',' -f1 |sed 's#\.##'`


#export PATH="$GEM5_EXT_ROOT/bin:$CUDAHOME/bin:$PATH"
export PATH="CUDAHOME/bin:$PATH"
export CUDA_INSTALL_PATH=$CUDAHOME
export CUDA_PATH=$CUDAHOME
export CPU_ENGINE=cpu
export CMDIO_PATH=/mnt/d/source/github/sim/mixlang/mlvm/src/cmdio
export LD_LIBRARY_PATH=/mnt/d/source/github/sim/mixlang/build/lib:$LD_LIBRARY_PATH

export CUDA_VERSION_NUMBER=${CUDA_VERSION}00
#export CUDART_VERSION=${CUDA_VERSION}00

export PTX_SIM_USE_PTX_FILE=1.ptx
export PTX_SIM_KERNELFILE=_1.ptx
#export PTX_SIM_MODE_FUNC=1
export PTX_SIM_DEBUG=3
export PYTORCH_BIN=
export CUOBJDUMP_SIM_FILE=jj
export PTX_JIT_PATH=
export GPGPUSIM_ROOT=$GEM5_EXT_ROOT/gpgpu/gpgpu-sim/
export LD_LIBRARY_PATH=$GEM5_EXT_ROOT/src/libcuda:$LD_LIBRARY_PATH

# then update-alternative --config gcc/g++ to 4.7
# cd benchmarks/rodinia/vectorAdd or
# cd benchmarks/rodinia/backprop
# make gem5-fusion

#setup cosim
export CO_DESIGN=co_design/simple_ppu
export CO_BUILD_DIR=build
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$GEM5_EXT_ROOT/cosim

export PYTHONPATH=$GEM5_EXT_ROOT/src/python:$GEM5_EXT_ROOT/src/opu/3rdparty/pymtl3:$GEM5_EXT_ROOT/src/opu/oputest/python:$PYTHONPATH
export ISASIM_DUMP=0:0:0:0
