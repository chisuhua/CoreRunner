SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)
BUILD_DIR=$SCRIPT_DIR/build

BUILD_OPT=(dbg rel dbgrel clean)
OPT=(n)
TGT=(all riscv gem5 quarkts)

[ -z "${BUILD_TYPE}" ] && BUILD_TYPE=Debug

CMD="build"

function Usage {
    echo "Usage: run_build.sh with one below argument"
    echo "       the argument ${BUILD_OPT[@]}: build option, dbgrel is the default"
    echo "       the argument ${TGT[@]}: the target will be built, all is default"
    echo "       the argument n: will print the run command but not executed"
    exit
}

BUILD_ARG=

for arg in "$@"; do
  if [ ${arg:0:1} == "-" ]; then
    Usage
  elif echo "${BUILD_OPT[@]}" | grep -wq "$arg"; then
    if [ $arg == clean ]; then
      CMD=clean
    else
      if [ $arg == dbg ]; then
        BUILD_TYPE=Debug
      elif [ $arg == rel ]; then
        BUILD_TYPE=Release
      elif [ $arg == rel ]; then
        BUILD_TYPE=RelWithDebInfo
      fi
    fi
  elif echo "${TGT[@]}" | grep -wq "$arg"; then
    if [ $arg != all ]; then
      CMD="${CMD}_$arg"
    fi
  elif echo "${OPT[@]}" | grep -wq "$arg"; then
    BUILD_ARG+="$arg "
  else
    Usage
  fi
done

echo "will running $CMD $BUILD_ARG"

##### riscv
RISCV_SRC=riscv-gnu-toolchain
RISCV_BUILD=${BUILD_DIR}/riscv

function build_riscv {
   cd $RISCV_SRC
   RUN_CMD="./configure --enable-qemu-system  --prefix=$RISCV_BUILD --with-arch=rv32imc --with-abi=ilp32d --enable-multilib"
   if [ "$1" = "n" ]; then
     echo $RUN_CMD;
   else
     $RUN_CMD;
   fi
   make_riscv
   make_qemu
   cd -
#make newlib
#make report-newlib SIM=gdb
}
function make_riscv {
   make
}
function make_qemu {
   make build-sim SIM=qemu
}
function clean_riscv {
   rm -rf ${RISCV_BUILD}
}

### gem5
function build_gem5 {
   cd gem5
   RUN="scons -j 1 --verbose --debug=explain build/RISCV/gem5.debug"
   if [ "$1" = "n" ]; then
     echo $RUN
   else
     $RUN
   fi
   cd -
}

function build_quarkts {
   cd QuarkTS
   RUN="make"
   if [ "$1" = "n" ]; then
     echo $RUN
   else
     $RUN
   fi
   cd -
}

function build {
   build_riscv $@
   build_gem5 $@
   build_quarkts $@
}

function clean {
  clean_riscv
}

$CMD $@
