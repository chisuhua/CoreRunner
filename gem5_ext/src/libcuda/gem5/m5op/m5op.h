/*
 * Copyright (c) 2003-2006 The Regents of The University of Michigan
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
 *
 * Authors: Nathan Binkert
 *          Ali Saidi
 */

#ifndef __M5OP_H__
#define __M5OP_H__

#include <stdint.h>

void arm(uint64_t address);
void quiesce(void);
void quiesceNs(uint64_t ns);
void quiesceCycle(uint64_t cycles);
uint64_t quiesceTime(void);
uint64_t rpns();
void wakeCPU(uint64_t cpuid);

void m5_exit(uint64_t ns_delay);
uint64_t m5_initparam(void);
void m5_checkpoint(uint64_t ns_delay, uint64_t ns_period);
void m5_reset_stats(uint64_t ns_delay, uint64_t ns_period);
void m5_dump_stats(uint64_t ns_delay, uint64_t ns_period);
void m5_dumpreset_stats(uint64_t ns_delay, uint64_t ns_period);
uint64_t m5_readfile(void *buffer, uint64_t len, uint64_t offset);
//uint64_t m5_writefile(void *buffer, uint64_t len, uint64_t offset, const char *filename);
void m5_debugbreak(void);
void m5_switchcpu(void);
void m5_addsymbol(uint64_t addr, char *symbol);
void m5_panic(void);
void m5_work_begin(uint64_t workid, uint64_t threadid);
void m5_work_end(uint64_t workid, uint64_t threadid);
void m5_gpu(uint64_t __gpusysno, void* call_params);
void m5_opu_umd(uint64_t __gpusysno, void* call_params);
void m5_opu_kmd(uint64_t __gpusysno, void* call_params);

// These operations are for critical path annotation
void m5a_bsm(char *sm, const void *id, int flags);
void m5a_esm(char *sm);
void m5a_begin(int flags, char *st);
void m5a_end(void);
void m5a_q(const void *id, char *q, int count);
void m5a_dq(const void *id, char *q, int count);
void m5a_wf(const void *id, char *q, char *sm, int count);
void m5a_we(const void *id, char *q, char *sm, int count);
void m5a_ws(const void *id, char *q, char *sm);
void m5a_sq(const void *id, char *q, int count, int flags);
void m5a_aq(const void *id, char *q, int count);
void m5a_pq(const void *id, char *q, int count);
void m5a_l(char *lsm, const void *id, char *sm);
void m5a_identify(uint64_t id);
uint64_t m5a_getid(void);

#define M5_AN_FL_NONE   0x0
#define M5_AN_FL_BAD    0x2
#define M5_AN_FL_LINK   0x10
#define M5_AN_FL_RESET  0x20

#endif // __M5OP_H__
