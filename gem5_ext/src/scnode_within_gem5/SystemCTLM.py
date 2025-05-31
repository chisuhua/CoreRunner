# Copyright 2018 Google, Inc.
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

from m5.objects.SystemC import SystemC_ScModule
from m5.objects.Tlm import TlmTargetSocket
from m5.objects.Tlm import TlmInitiatorSocket
from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject


# This class is a subclass of sc_module, and all the special magic which makes
# that work is handled in the base classes.
class TLM_Target(SystemC_ScModule):
    type = "TLM_Target"
    cxx_class = "Target"
    cxx_header = "scnode_within_gem5/sc_tlm_target.hh"
    tsck = TlmTargetSocket(32, "TLM target socket")

    # A default memory size of 128 MiB (starting at 0) is used to
    # simplify the regressions
    range = Param.AddrRange(
        "128MiB", "Address range (potentially interleaved)"
    )
    scname = Param.String("sc name which scnose_wrapper use to create it")

    system = Param.System(Parent.any, "system")


# This class is a subclass of sc_module, and all the special magic which makes
# that work is handled in the base classes.
class TLM_Wrapper(SystemC_ScModule):
    type = "TLM_Wrapper"
    cxx_class = "TlmWrapper"
    cxx_header = "scnode_within_gem5/sc_tlm_wrapper.hh"
    tsck = TlmTargetSocket(32, "TLM target socket")
    isck = TlmInitiatorSocket(32, "TLM initiator socket")

    # A default memory size of 128 MiB (starting at 0) is used to
    # simplify the regressions
    range = Param.AddrRange(
        "128MiB", "Address range (potentially interleaved)"
    )
    scname = Param.String("sc name which scnose_wrapper use to create it")

    system = Param.System(Parent.any, "system")
