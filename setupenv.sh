PROJECT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)

export RISCV_PATH=$PROJECT_DIR/build/riscv
export GLIB_PATH=$PROJECT_DIR/build/glib/lib/x86_64-linux-gnu
export GLIB_INCLUDE=$PROJECT_DIR/build/glib/include

export QUARKTS_PATH=$PROJECT_DIR/QuarkTS

export PKG_CONFIG_PATH=$GLIB_PATH/pkgconfig:$PKG_CONFIG_PATH
export LD_LIBRARY_PATH=$GLIB_PATH:$LD_LIBRARY_PATH:$ICSC_HOME/lib
export CPATH=$GLIB_INCLUDE:$CPATH

source venv/bin/activate

export PATH=$RISCV_PATH/bin:$PATH
export LD_LIBRARY_PATH=$PROJECT_DIR/venv/lib:$LD_LIBRARY_PATH
