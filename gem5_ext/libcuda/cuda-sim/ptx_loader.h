// Copyright (c) 2009-2011, Tor M. Aamodt
// The University of British Columbia
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
// Neither the name of The University of British Columbia nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef PTX_LOADER_H_INCLUDED
#define PTX_LOADER_H_INCLUDED
#include <string>

#define PTXINFO_LINEBUF_SIZE 1024
typedef void* yyscan_t;

// extern "C" int no_of_ptx; //counter to track number of ptx files to be extracted in an application.
namespace libcuda {
class gpgpu_context;
typedef void* yyscan_t;
class ptxinfo_data {
 public:
  ptxinfo_data(gpgpu_context* ctx) { gpgpu_ctx = ctx; }
  yyscan_t scanner;
  char linebuf[PTXINFO_LINEBUF_SIZE];
  unsigned col;
  const char* g_ptxinfo_filename;
  class gpgpu_context* gpgpu_ctx;
  bool g_keep_intermediate_files;
  bool m_ptx_save_converted_ptxplus;
  void ptxinfo_addinfo();
  bool keep_intermediate_files();
  char* gpgpu_ptx_sim_convert_ptx_and_sass_to_ptxplus(
      const std::string ptx_str, const std::string sass_str,
      const std::string elf_str);
};

#if 0
extern bool g_override_embedded_ptx;

class symbol_table *gpgpu_ptx_sim_load_ptx_from_string( const char *p, unsigned source_num );
class symbol_table *gpgpu_ptx_sim_load_ptx_from_filename( const char *filename );
void gpgpu_ptxinfo_load_from_string( const char *p_for_info, unsigned source_num, unsigned sm_version=20 );
void gpgpu_ptx_info_load_from_filename( const char *filename, unsigned sm_version );
char* gpgpu_ptx_sim_convert_ptx_and_sass_to_ptxplus(const std::string ptx_str, const std::string sass_str, const std::string elf_str);
bool keep_intermediate_files();
#endif
}

#endif
