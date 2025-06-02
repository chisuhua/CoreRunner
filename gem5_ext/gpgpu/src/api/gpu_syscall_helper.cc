/*
 * Copyright (c) 2012 Mark D. Hill and David A. Wood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gpu_syscall_helper.hh"
#include "cpu/thread_context.hh"
#include "cpu/thread_state.hh"
#include "cpu/simple_thread.hh"
#include "mem/ruby/system/RubySystem.hh"
#include "mem/se_translating_port_proxy.hh"
#include "mem/translating_port_proxy.hh"
#include "sim/full_system.hh"

GPUSyscallHelper::GPUSyscallHelper(ThreadContext *_tc, gpusyscall_t* _call_params)
    : tc(_tc), sim_params_ptr((Addr)_call_params), arg_lengths(NULL),
      args(NULL), total_bytes(0), live_param(NULL)
{
    sim_params_ptr = sim_params_ptr & __POINTER_MASK__;
    if (!sim_params_ptr)
        return;
    decode_package();
}

void
GPUSyscallHelper::readBlob(Addr addr, uint8_t* p, int size, ThreadContext *tc)
{
    assert(addr == (addr & __POINTER_MASK__));

    if (FullSystem) {
        tc->getVirtProxy().readBlob(addr, p, size);
    } else {
        // TODO schi for gem5-gpu tc->getMemProxy().readBlob(addr, p, size);
        if (tc->getSystemPtr()) {
            tc->getVirtProxy().readBlob(addr, p, size);
        } else {
            memcpy((void*)p, (void*)addr, size);
        }
    }
}

void
GPUSyscallHelper::readString(Addr addr, uint8_t* p, int size, ThreadContext *tc)
{
    assert(addr == (addr & __POINTER_MASK__));

    // Ensure that the memory buffer is cleared
    memset(p, 0, size);

    // For each line in the read, grab the system's memory and check for
    // null-terminating character
    bool null_not_found = true;
    Addr curr_addr;
    int read_size;
    unsigned block_size = ruby::RubySystem::getBlockSizeBytes();
    int bytes_read = 0;
    for (; bytes_read < size && null_not_found; bytes_read += read_size) {
        curr_addr = addr + bytes_read;
        read_size = block_size;
        if (bytes_read == 0) read_size -= curr_addr % block_size;
        if (bytes_read + read_size >= size) read_size = size - bytes_read;
        readBlob(curr_addr, &p[bytes_read], read_size, tc);
        for (int index = 0; index < read_size; ++index) {
            if (p[bytes_read + index] == 0) null_not_found = false;
        }
    }

    if (null_not_found) panic("Didn't find end of string at address %x (%s)!", addr, (char*)p);
}

void
GPUSyscallHelper::writeBlob(Addr addr, uint8_t* p, int size, ThreadContext *tc, bool is_ptr)
{
    assert(addr == (addr & __POINTER_MASK__));

    if (is_ptr)
        size = __POINTER_SIZE__;
    if (FullSystem) {
        tc->getVirtProxy().writeBlob(addr, p, size);
    } else {
        // TODO schi for gem5 tc->getMemProxy().writeBlob(addr, p, size);
        if (tc->getSystemPtr()) {
            tc->getVirtProxy().writeBlob(addr, p, size);
        } else {
            memcpy((void*)addr, (void*)p, size);
        }
    }
}

void
GPUSyscallHelper::decode_package()
{
    assert(sim_params_ptr);

#if THE_ISA == ARM_ISA
    // Size of sim_params in 32-bit simulated system is 20B
    #define SIM_PARAMS_SIZE 20 // 4B each for 5 members of gpusyscall_t
    // Add 4B to keep last 64-bit pointer math from reading other stack junk
    uint8_t params_package[SIM_PARAMS_SIZE + 4];
    readBlob(sim_params_ptr, params_package, SIM_PARAMS_SIZE);
    sim_params.total_bytes = unpackData<int>(params_package, 0);
    sim_params.num_args = unpackData<int>(params_package, 4);
    sim_params.arg_lengths = unpackPointer<Addr>(params_package, 8);
    sim_params.args = unpackPointer<Addr>(params_package, 12);
    sim_params.ret = unpackPointer<Addr>(params_package, 16);
#elif THE_ISA == X86_ISA
    // NOTE: sizeof() call assumes gem5-gpu built on 64-bit machine
    readBlob(sim_params_ptr, (unsigned char*)&sim_params, sizeof(gpusyscall_t));
#else
    #error Currently gem5-gpu is only known to support x86 and ARM
#endif

    arg_lengths = new int[sim_params.num_args];
    readBlob(sim_params.arg_lengths, (unsigned char*)arg_lengths, sim_params.num_args * sizeof(int));

    args = new unsigned char[sim_params.total_bytes];
    readBlob(sim_params.args, args, sim_params.total_bytes);

}

GPUSyscallHelper::~GPUSyscallHelper()
{
    if (arg_lengths) {
        delete[] arg_lengths;
    }
    if (args) {
        delete[] args;
    }
    if (live_param) {
        delete[] live_param;
    }
}

// TODO: Make this function a template based on the requested data type in order
// to alleviate the need for the caller to cast and dereference. This function
// should return the requested type based on the template parameter.
void*
GPUSyscallHelper::getParam(int index, bool is_ptr)
{
    // Temporarily hold the requested parameter value in the live_param
    // allocation. If a previously requested parameter is sitting in the
    // live_param, delete it to make room for currently requested param
    if (live_param) {
        delete[] live_param;
    }
    size_t live_param_size = arg_lengths[index];
    if (is_ptr) {
        assert(live_param_size <= sizeof(Addr));
        live_param_size = sizeof(Addr);
    }
    live_param = new unsigned char[live_param_size];
    memset(live_param, 0, live_param_size);
    int arg_index = 0;
    for (int i = 0; i < index; i++) {
        arg_index += arg_lengths[i];
    }
    size_t offset = live_param_size - arg_lengths[index];
    for (int i = 0; i < arg_lengths[index]; i++) {
        live_param[i] = args[i+arg_index];
    }
    if (is_ptr)
        return (void*)live_param;
    else
        return (void*)&live_param[offset];
}

void
GPUSyscallHelper::setReturn(unsigned char* retValue, size_t size, bool is_ptr)
{
    writeBlob((uint64_t)sim_params.ret, retValue, size, is_ptr);
}

class AppThreadContext : public ThreadState, public ThreadContext {
public:
    /*
    using VecLane8 = VecLaneT<uint8_t, false>;
    using VecLane16 = VecLaneT<uint16_t, false>;
    using VecLane32 = VecLaneT<uint32_t, false>;
    using VecLane64 = VecLaneT<uint64_t, false>;

    using ConstVecLane8 = VecLaneT<uint8_t, true>;
    using ConstVecLane16 = VecLaneT<uint16_t, true>;
    using ConstVecLane32 = VecLaneT<uint32_t, true>;
    using ConstVecLane64 = VecLaneT<uint64_t, true>;
    */

    typedef ThreadContext::Status ThreadStatus;

    ::gem5::X86ISA::VecRegContainer vecReg;
    ::gem5::X86ISA::VecElem vecElem;
    ::gem5::X86ISA::VecPredRegContainer vecPredReg;
    ThreadStatus status_;
    gem5::System    *system_;
    AppThreadContext() : ThreadState(nullptr, 1, nullptr)
    {
        status_ = ThreadStatus();
        // system_ = new AppSystem();
    }
    BaseCPU *getCpuPtr() {return nullptr;};
    int cpuId() const {return 0;};
    uint32_t socketId() const {return 0;};
    int threadId() const {return 0;};
    void setThreadId(int id) {};
    ContextID contextId() const {return ContextID();};
    void setContextId(ContextID id) {};
    BaseMMU *getMMUPtr() {return nullptr;};
#ifndef BUILD_PPU
    CheckerCPU *getCheckerCpuPtr() {return nullptr;};
#endif
    BaseISA *getIsaPtr() {return nullptr;};
    ::gem5::X86ISA::Decoder *getDecoderPtr() {return nullptr;};
    System *getSystemPtr() {return nullptr;};
    PortProxy &getVirtProxy() {return ThreadState::getVirtProxy();};
    void initMemProxies(ThreadContext *tc) {};
    Process *getProcessPtr() {return nullptr;};
    void setProcessPtr(Process *p) {};
    ThreadStatus status() const {return status_;};
    void setStatus(ThreadStatus new_status) {};
    void activate() {};
    void suspend() {};
    void halt() {};
    void takeOverFrom(ThreadContext *old_context) {};
    void regStats(const std::string &name) {};
    void scheduleInstCountEvent(Event *event, Tick count) {};
    void descheduleInstCountEvent(Event *event) {};
    Tick getCurrentInstCount() {return Tick();};
    Tick readLastActivate() {return Tick();};
    Tick readLastSuspend() {return Tick();};
    void copyArchRegs(ThreadContext *tc) {};
    void clearArchRegs() {};
    RegVal readIntReg(RegIndex reg_idx) const {return RegVal();};
    RegVal readFloatReg(RegIndex reg_idx) const {return RegVal();};
    const ::gem5::X86ISA::VecRegContainer& readVecReg(const RegId& reg) const {return vecReg;};
    ::gem5::X86ISA::VecRegContainer& getWritableVecReg(const RegId& reg) {return vecReg;};
    const ::gem5::X86ISA::VecElem& readVecElem(const RegId& reg) const {return vecElem;};

    const ::gem5::X86ISA::VecPredRegContainer& readVecPredReg(
            const RegId& reg) const {return vecPredReg;};
    ::gem5::X86ISA::VecPredRegContainer& getWritableVecPredReg(
            const RegId& reg) {return vecPredReg;};

    RegVal readCCReg(RegIndex reg_idx) const {return RegVal();};
    void setIntReg(RegIndex reg_idx, RegVal val) {};
    void setFloatReg(RegIndex reg_idx, RegVal val) {};
    void setVecReg(const RegId& reg, const ::gem5::X86ISA::VecRegContainer& val) {};
    void setVecElem(const RegId& reg, const ::gem5::X86ISA::VecElem& val) {};
    void setVecPredReg(const RegId& reg,
            const ::gem5::X86ISA::VecPredRegContainer& val) {};
    void setCCReg(RegIndex reg_idx, RegVal val) {};
    ::gem5::X86ISA::PCState pcState() const {return ::gem5::X86ISA::PCState();};
    void pcState(const ::gem5::X86ISA::PCState &val) {};
    void pcStateNoRecord(const ::gem5::X86ISA::PCState &val) {};
    Addr instAddr() const {return Addr();};
    Addr nextInstAddr() const {return Addr();};
    MicroPC microPC() const {return MicroPC();};
    RegVal readMiscRegNoEffect(RegIndex misc_reg) const {return RegVal();};
    RegVal readMiscReg(RegIndex misc_reg) {return RegVal();};
    void setMiscRegNoEffect(RegIndex misc_reg, RegVal val) {};
    // TODO schi add from gem5-gpu
    void setMiscRegActuallyNoEffect(int misc_reg, const RegVal &val) {};
    void setMiscReg(RegIndex misc_reg, RegVal val) {};
    RegId flattenRegId(const RegId& reg_id) const {return RegId();};
    unsigned readStCondFailures() const {return 0;};
    void setStCondFailures(unsigned sc_failures) {};
    int exit() { return 1; };
    RegVal readIntRegFlat(RegIndex idx) const {return RegVal();};
    void setIntRegFlat(RegIndex idx, RegVal val) {};
    RegVal readFloatRegFlat(RegIndex idx) const {return RegVal();};
    void setFloatRegFlat(RegIndex idx, RegVal val) {};
    const ::gem5::X86ISA::VecRegContainer&
        readVecRegFlat(RegIndex idx) const {return vecReg;};
    ::gem5::X86ISA::VecRegContainer& getWritableVecRegFlat(RegIndex idx) {return vecReg;};
    void setVecRegFlat(RegIndex idx,
            const ::gem5::X86ISA::VecRegContainer& val) {};

    const ::gem5::X86ISA::VecElem& readVecElemFlat(RegIndex idx,
            const ElemIndex& elem_idx) const {return vecElem;};
    void setVecElemFlat(RegIndex idx, const ElemIndex& elem_idx,
            const ::gem5::X86ISA::VecElem& val) {};

    const ::gem5::X86ISA::VecPredRegContainer &
        readVecPredRegFlat(RegIndex idx) const {return vecPredReg;};
    ::gem5::X86ISA::VecPredRegContainer& getWritableVecPredRegFlat(
            RegIndex idx) {return vecPredReg;};
    void setVecPredRegFlat(RegIndex idx,
            const ::gem5::X86ISA::VecPredRegContainer& val) {};

    RegVal readCCRegFlat(RegIndex idx) const {return RegVal();};
    void setCCRegFlat(RegIndex idx, RegVal val) {};
    void htmAbortTransaction(uint64_t htm_uid,
                                     HtmFailureFaultCause cause) {};
    BaseHTMCheckpointPtr& getHtmCheckpointPtr() override {
        // return std::unique_ptr<BaseHTMCheckpoint>(new BaseHTMCheckpoint());
        panic("%s not implemented.", __FUNCTION__);
    };
    void setHtmCheckpointPtr(BaseHTMCheckpointPtr cpt) {};

    bool schedule(PCEvent *e) override { return true; }
    bool remove(PCEvent *e) override { return true; }
};

ThreadContext* create_thread_context() {
    static ThreadContext *app_context_outside_gem5 = nullptr;
    if (app_context_outside_gem5 == nullptr) {
        app_context_outside_gem5 = new ::AppThreadContext();
    }
    return app_context_outside_gem5;
}

