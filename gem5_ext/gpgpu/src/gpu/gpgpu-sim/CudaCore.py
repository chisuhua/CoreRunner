# Copyright (c) 2011 Mark D. Hill and David A. Wood
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

from m5.defines import buildEnv
from m5.params import *
from m5.proxy import *
#from m5.objects.MemObject import MemObject
from m5.objects.ClockedObject import ClockedObject
from m5.objects.ShaderTLB import ShaderTLB

class CudaCore(ClockedObject):
    type = 'CudaCore'
    cxx_class = 'gem5::CudaCore'
    cxx_header = "gpu/gpgpu-sim/cuda_core.hh"

    inst_port = MasterPort("The instruction cache port for this SC")

    lsq_port = VectorMasterPort("the load/store queue coalescer ports")

    lsq_ctrl_port = MasterPort("The load/store queue control port")

    sys = Param.System(Parent.any, "system sc will run on")
    gpu = Param.CudaGPU(Parent.any, "The GPU this core is part of")

    itb = Param.ShaderTLB(ShaderTLB(), "Instruction TLB")

    id = Param.Int(-1, "ID of the SP")

    warp_contexts = Param.Int(48, "Number of warps possible per GPU core")
