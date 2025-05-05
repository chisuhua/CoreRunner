SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)

export RISCV_PATH=$SCRIPT_DIR/build/riscv
export GLIB_PATH=$SCRIPT_DIR/build/glib/lib/x86_64-linux-gnu
export GLIB_INCLUDE=$SCRIPT_DIR/build/glib/include

export PKG_CONFIG_PATH=$GLIB_PATH/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$GLIB_PATH:$LD_LIBRARY_PATH

export CPATH=$GLIB_INCLUDE:$CPATH
export PATH=$RISCV_PATH/bin:$PATH

source venv/bin/activate

export LD_LIBRARY_PATH=$SCRIPT_DIR/venv/lib:$LD_LIBRARY_PATH
