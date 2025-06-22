#pragma once
// #include "../libcuda/gpu-sim.h"
#include <cassert>
#include <map>
#include <string>

class KernelSymbol;
class KernelInfo;
class IAgent;

namespace libcuda {
class gpgpu_sim;
}

struct device_id {
  device_id(libcuda::gpgpu_sim *gpu) {
    m_id = 0;
    m_next = nullptr;
    m_gpgpu = gpu;
  }
  struct device_id *next() {
    return m_next;
  }
  int num_devices() const {
    if (m_next == nullptr)
      return 1;
    else
      return 1 + m_next->num_devices();
  }
  struct device_id *get_device(unsigned n) {
    assert(n < (unsigned)num_devices());
    struct device_id *p = this;
    for (unsigned i = 0; i < n; i++) p = p->m_next;
    return p;
  }
  unsigned get_id() const { return m_id; }

  libcuda::gpgpu_sim *get_gpgpu() { return m_gpgpu; }

 protected:
  libcuda::gpgpu_sim *m_gpgpu;
  unsigned m_id;
  struct device_id *m_next;
};


struct CUctx {
public:
  CUctx(device_id *gpu, std::string& umd)
      : m_gpu(gpu)
      , m_umd(umd)
  {}
#if 0
  void add_codeobject(KernelSymbol *sym, unsigned fat_cubin_handle) {
    m_codeobject[fat_cubin_handle] = sym;
  }
#endif
  device_id *get_device() { return m_gpu; }

#if 0
  void register_executable(const char *hostFun, KernelInfo *f) {
    m_executable_lookup[hostFun] = f;
  }
  KernelInfo *get_executable(const char *hostFun) {
    std::map<const char *, KernelInfo*>::iterator i =
        m_executable_lookup.find(hostFun);
    assert(i != m_executable_lookup.end());
    return i->second;
  }
#endif

  IAgent* get_agent() {
    return m_agent;
  }

  void set_agent(IAgent* agent) {
    m_agent = agent;
  }

  std::string& get_umd_name() {
      return m_umd;
  }

#if 0
  std::map<unsigned, KernelSymbol *> m_codeobject; // fab binary handle -> global symbol table
  std::map<const void*, KernelInfo *> m_executable_lookup; // unique id (CUDA app function address) -> kernel exec
#endif
  IAgent* m_agent;

  device_id *m_gpu;  // selected gpu

  // int umd_mode;     // 0: not use umd 1: umd_libcuda 2: umd_libgem5cuda 3: umd_se
  std::string m_umd;
};
