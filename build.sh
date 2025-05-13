SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)
BUILD_DIR=$SCRIPT_DIR/build

THREADS=$(nproc)

BUILD_OPT=(dbg rel dbgrel)
RUN=(build clean run gdb)
OPT=(n)
TGT=(all riscv gem5 glib riscv_make riscv_qemu mytest)

[ -z "${BUILD_TYPE}" ] && BUILD_TYPE=Debug

CMD="build"

function Usage {
    echo "Usage: run_build.sh with one below argument"
    echo "       the argument ${RUN[@]}: build is the default"
    echo "       the argument ${BUILD_OPT[@]}: build option, dbgrel is the default"
    echo "       the argument ${TGT[@]}: the target will be built, all is default"
    echo "       the argument n: will print the run command but not executed"
    exit
}

BUILD_ARG=
NORUN=n

function is_in_list() {
  local target="$1"
  shift
  local list=("$@")

  for item in "${list[@]}"; do
    if [[ "$item" == "$target" ]]; then
      return 0
    fi
  done

  return 1
}

if [ -z "$1" ]; then
  echo "using default action: build"
else
  if is_in_list "$1" "${RUN[@]}"; then
    CMD=$1
    echo "will running $CMD $BUILD_ARG"
    shift
  fi
fi

for arg in "$@"; do
  if [ ${arg:0:1} == "-" ]; then
    Usage
  elif [ "$arg" == "n" ]; then
    NORUN=y
  elif echo "${BUILD_OPT[@]}" | grep -wq "$arg"; then
    if [ $arg == dbg ]; then
      BUILD_TYPE=Debug
    elif [ $arg == rel ]; then
      BUILD_TYPE=Release
    elif [ $arg == reldbg ]; then
      BUILD_TYPE=RelWithDebInfo
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

function run_cmd {
   echo $1;
   if [ "$NORUN" != "y" ]; then
     eval $1 || { echo "Failed to run $1"; exit 1;}
   fi
}

##### riscv
RISCV_SRC=riscv-gnu-toolchain
RISCV_BUILD=${BUILD_DIR}/riscv
#RISCV_ARCH=--with-arch=rv32imc
RISCV_ARCH=--with-arch=rv32gc
RISCV_ABI=--with-abi=ilp32

function build_riscv {
   run_cmd "cd $SCRIPT_DIR/$RISCV_SRC"
   run_cmd "./configure --enable-qemu-system  --prefix=$RISCV_BUILD $RISCV_ARCH $RISCV_ABI --with-multilib-generator=\"rv32gc-ilp32--\""
}
function build_riscv_make {
   run_cmd "cd $SCRIPT_DIR/$RISCV_SRC"
   run_cmd "make -j${THREADS}"
}
function build_riscv_qemu {
   run_cmd "cd $SCRIPT_DIR/$RISCV_SRC"
   run_cmd "make -j${THREADS} build-sim SIM=qemu"
}
function clean_riscv {
   rm -rf ${RISCV_BUILD}
   rm -rf ${SCRIPT_DIR}/${RISCV_SRC}/build-*
   rm -rf ${SCRIPT_DIR}/${RISCV_SRC}/stamps
}

### gem5
GEM5_SRC=gem5
GEM5_BUILD=${SCRIPT_DIR}/${GEM5_SRC}/build
GEM5_TARGET=RISCV/gem5.debug
function build_gem5 {
   run_cmd "cd $SCRIPT_DIR/$GEM5_SRC"
   run_cmd "scons -j${THREADS} --verbose ${GEM5_BUILD}/${GEM5_TARGET}"
}

function clean_gem5 {
   rm -rf ${GEM5_BUILD}/${GEM5_TARGET}
}


## mytest
MYTEST_SRC=firmware/mytest
function build_mytest {
   run_cmd "cd $SCRIPT_DIR/${MYTEST_SRC}"
   run_cmd "make"
}


function clean_mytest {
   run_cmd "cd $SCRIPT_DIR/${MYTEST_SRC}"
   run_cmd "make clean"
}


function run_mytest {
   run_cmd "cd $SCRIPT_DIR/${MYTEST_SRC}"
   run_cmd "make run"
}


function gdb_mytest {
   run_cmd "cd $SCRIPT_DIR/${MYTEST_SRC}"
   run_cmd "make gdb"
}


function build {
   build_glib $@
   build_riscv $@
   build_riscv_make $@
   build_riscv_qemu $@
   build_gem5 $@
   build_mytest $@
}

function clean {
  clean_riscv
  clean_gem5
}

$CMD $@
