this_dir := $(abspath $(lastword $(MAKEFILE_LIST)))
INC = $(addprefix -I, $(dir $(this_dir)))
SRC = $(wildcard *.c)
LINKER = $(wildcard *.ld)
NDEBUG = 1
include $(QUARKTS_PATH)/Makefile
