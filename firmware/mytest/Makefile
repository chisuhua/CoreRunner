this_dir := $(abspath $(lastword $(MAKEFILE_LIST)))
INC = $(addprefix -I, $(dir $(this_dir)))
#SRC = $(wildcard *.c)
#SRC = $(wildcard sec2queues.c)
#SRC = $(wildcard sec2eventflag.c)
#SRC = $(wildcard main.c)
#SRC = $(wildcard helloworld.c)
#SRC = $(wildcard verifyloop.c)
#SRC = $(wildcard baremetal.c)
#SRC = $(wildcard verifyqueue.c)
SRC = $(wildcard verifyopen.c)
LINKER_SRC = linker.ld.S
NDEBUG = 0
BAREMETAL = 1
include $(QUARKTS_PATH)/Makefile
