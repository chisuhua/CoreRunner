#include "ptx_ir.h"
#include "ptx_parser.h"

typedef void *yyscan_t;
class ptx_recognizer;
#include "ptx.tab.h"

#include "../../libcuda/gpgpu_context.h"
#include "../../libcuda/gpgpu_context.h"
#include "../../opu/coasm/coasm_define.h"

namespace libcuda {

void coasm_not_impl(const ptx_instruction *pI) {
  printf(
      "GPGPU-Sim PTX: ERROR (%s:%u) instruction \"%s\" not (yet) implemented\n",
      pI->source_file(), pI->source_line(), pI->get_opcode_cstr());
  abort();
}

void print_tcc(function_info *finfo, const operand_info &operand, FILE* fp) {
  fprintf(fp, "\t%s", finfo->get_coasm_tcc(operand).c_str());
}

void print_src(function_info *finfo, const operand_info &src, FILE* fp, uint32_t size=0, unsigned i_type = U32_TYPE) {
  if (src.is_builtin())
      fprintf(fp, "\%build_in%u", src.get_int());
  else if (src.is_literal())
      fprintf(fp, "0x%x", src.get_literal_value());
  else {
    if (size == 0) {
      if (src.is_vector()) {
        size = src.get_vect_nelem();
      } else {
        switch (i_type) {
          case PRED_TYPE:
            return print_tcc(finfo, src, fp);
          case S64_TYPE:
          case U64_TYPE:
          case F64_TYPE:
          case FF64_TYPE: {
            size = 2; break;
          }
          default:
          size = 1; break;
        }
      }
    }
    // fprintf(fp, "v[%u,%u]", src.reg_num(), src.reg_num()+size-1);
    fprintf(fp, "%s", finfo->get_coasm_reg(src, size).c_str());
  }
}

void print_dst(function_info *finfo, const operand_info &dst, FILE* fp, uint32_t size=0) {
  if (size == 0) {
    if (dst.is_vector()) {
      size = dst.get_vect_nelem();
    } else {
      size = 1;
    }
  }

    // fprintf(fp, "v%u", dst.reg_num());
    // fprintf(fp, "v[%u,%u]", dst.reg_num(), dst.reg_num()+size-1);
    if (dst.is_reg()) {
      fprintf(fp, "%s", finfo->get_coasm_reg(dst, size).c_str());
    } else if (dst.is_vector()) {
      unsigned nelem = dst.get_vect_nelem();
      fprintf(fp, "%s", finfo->get_coasm_reg(dst, nelem).c_str());
    } else if(dst.is_memory_operand()) {
      fprintf(fp, "%s", finfo->get_coasm_reg(dst, 2).c_str());
    } else {
      fprintf(fp, "FIXME on dst operand: %s\n", __FUNCTION__);
    }
}


void print_type_1op(const char* op, function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned dst_size =0 , unsigned src1_size = 0) {
    size_t size;
    int t;
    unsigned i_type = pI->get_type();
    if (i_type == PRED_TYPE) {
      fprintf(fp, "s_%s_b32\t", op);
    } else {
      type_info_key::type_decode(pI->get_type(), size, t);
      switch(t) {
        case 1: fprintf(fp, "v_%s_i%d\t", op, size); break;
        case 0: fprintf(fp, "v_%s_u%d\t", op, size); break;
        case -1: fprintf(fp, "v_%s_f%d\t", op, size); break;
        case 2: fprintf(fp, "s_%s_t%d\t", op, size); break;  // PRED
        case 3:
        default:
            printf("ERROR ** type_decode() does not know about \"%s\"\n", decode_token(pI->get_type()));
            assert(0);
      }
    }

    if (dst_size == 0) {
      if (pI->dst().is_vector()) {
        dst_size = pI->dst().get_vect_nelem();
      } else {
        switch (i_type) {
          case PRED_TYPE:
            return print_tcc(finfo, pI->dst(), fp);
          case S64_TYPE:
          case U64_TYPE:
          case F64_TYPE:
          case FF64_TYPE: {
            dst_size = 2; break;
          }
          default:
          dst_size = 1; break;
        }
      }
    }

    print_dst(finfo, pI->dst(), fp, dst_size);
}

void print_type_2op(const char* op, function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned dst_size=0, unsigned src1_size=0, unsigned src2_size=0) {
    print_type_1op(op, finfo, pI, fp, dst_size, src1_size);
    fprintf(fp, ",\t");

    print_src(finfo, pI->src1(), fp, src1_size, pI->get_type());
}

void print_type_3op(const char* op, function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned dst_size=0, unsigned src1_size=0, unsigned src2_size=0, unsigned src3_size=0) {
    print_type_2op(op, finfo, pI, fp, dst_size, src1_size, src2_size);
    fprintf(fp, ",\t");
    print_src(finfo, pI->src2(), fp, src2_size, pI->get_type());
}

void print_type_4op(const char* op, function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned dst_size=0, unsigned src1_size=0, unsigned src2_size=0, unsigned src3_size=0) {
    print_type_3op(op, finfo, pI, fp, dst_size, src1_size, src2_size);
    fprintf(fp, ",\t");
    print_src(finfo, pI->src3(), fp, src3_size, pI->get_type());
}

void print_type_5op(const char* op, function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned dst_size=0, unsigned src1_size=0, unsigned src2_size=0, unsigned src3_size=0, unsigned src4_size=0) {
    print_type_4op(op, finfo, pI, fp, dst_size, src1_size, src2_size, src3_size);
    fprintf(fp, ",\t");
    print_src(finfo, pI->src4(), fp, src4_size, pI->get_type());
}

void print_type_op(const char* op, function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned dst_size=0, unsigned src1_size=0, unsigned src2_size=0, unsigned src3_size=0, unsigned src4_size=0) {
    if (pI->get_num_operands() == 1) print_type_1op(op, finfo, pI, fp, dst_size);
    else if (pI->get_num_operands() == 2) print_type_2op(op, finfo, pI, fp, dst_size, src1_size);
    else if (pI->get_num_operands() == 3) print_type_3op(op, finfo, pI, fp, dst_size, src1_size, src2_size);
    else if (pI->get_num_operands() == 4) print_type_4op(op, finfo, pI, fp, dst_size, src1_size, src2_size, src3_size);
    else if (pI->get_num_operands() == 5) print_type_5op(op, finfo, pI, fp, dst_size, src1_size, src2_size, src3_size, src4_size);
    else assert(0);
}

void print_1op(function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned size = 1) {
    print_dst(finfo, pI->dst(), fp, size);
    fprintf(fp, ",\t");
    print_src(finfo, pI->src1(), fp, size);
}

void print_2op(function_info *finfo, const ptx_instruction *pI, FILE* fp, unsigned size = 1) {
    print_1op(finfo, pI, fp, size);
    fprintf(fp, ",\t");
    print_src(finfo, pI->src2(), fp, size);
}


void print_bra(function_info *finfo, const ptx_instruction *pI, const operand_info &target, FILE* fp) {
  unsigned addr = finfo->get_label_addr(target.name());
  ptx_instruction *target_pI = finfo->get_target_pI(addr);
  basic_block_t *target_bb = target_pI->get_bb();

  if (pI->has_pred()) {
     if (pI->get_pred_neg()) {
        fprintf(fp, "t_cbranch_tccz ");
     } else {
        fprintf(fp, "t_cbranch_tccnz ");
     }
     const operand_info &p = pI->get_pred();
     fprintf(fp, "%s", finfo->get_coasm_tcc(p).c_str());
     fprintf(fp, ",\tbb_%02u", target_bb->bb_id);
  } else {
     fprintf(fp, "s_branch  bb_%02u", target_bb->bb_id);
  }
}

void abs_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("abs", finfo, pI, fp);
}

void addp_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
  // PTXPlus add instruction with carry (carry is kept in a predicate) register
}

void add_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t src1_data, src2_data, data;
  int overflow = 0;
  int carry = 0;

  const operand_info &dst = pI->dst();  // get operand info of sources and destination

  const operand_info &src1 = pI->src1();  // use them to determine that they are of type 'register'
  const operand_info &src2 = pI->src2();

  unsigned i_type = pI->get_type();
  // src1_data = thread->get_operand_value(src1, dst, i_type, thread, 1);
  // src2_data = thread->get_operand_value(src2, dst, i_type, thread, 1);

  unsigned rounding_mode = pI->rounding_mode();
  /* int orig_rm = fegetround();
  switch (rounding_mode) {
    case RN_OPTION:
      break;
    case RZ_OPTION:
      fesetround(FE_TOWARDZERO);
      break;
    default:
      assert(0);
      break;
  }*/

  unsigned size = 1;
  // performs addition. Sets carry and overflow if needed.
  switch (i_type) {
    case PRED_TYPE:
      fprintf(fp, "v_add_tcc");
      size = 0;
    case S8_TYPE:
      fprintf(fp, "v_add_i8");
      // data.s64 = (src1_data.s64 & 0x0000000FF) + (src2_data.s64 & 0x0000000FF);
      // if (((src1_data.s64 & 0x80) - (src2_data.s64 & 0x80)) == 0) {
      //   overflow = ((src1_data.s64 & 0x80) - (data.s64 & 0x80)) == 0 ? 0 : 1;
      // }
      // carry = (data.u64 & 0x000000100) >> 8;
      break;
    case S16_TYPE:
      fprintf(fp, "v_add_i16");
      // data.s64 = (src1_data.s64 & 0x00000FFFF) + (src2_data.s64 & 0x00000FFFF);
      // if (((src1_data.s64 & 0x8000) - (src2_data.s64 & 0x8000)) == 0) {
      //   overflow =
      //       ((src1_data.s64 & 0x8000) - (data.s64 & 0x8000)) == 0 ? 0 : 1;
      // }
      // carry = (data.u64 & 0x000010000) >> 16;
      break;
    case S32_TYPE:
      fprintf(fp, "v_add_i32");
      // data.s64 = (src1_data.s64 & 0x0FFFFFFFF) + (src2_data.s64 & 0x0FFFFFFFF);
      // if (((src1_data.s64 & 0x80000000) - (src2_data.s64 & 0x80000000)) == 0) {
      //   overflow = ((src1_data.s64 & 0x80000000) - (data.s64 & 0x80000000)) == 0
      //                  ? 0
      //                  : 1;
      // }
      // carry = (data.u64 & 0x100000000) >> 32;
      break;
    case S64_TYPE:
      size = 2;
      fprintf(fp, "v_add_i64");
      // data.s64 = src1_data.s64 + src2_data.s64;
      break;
    case U8_TYPE:
      fprintf(fp, "v_add_u8");
      // data.u64 = (src1_data.u64 & 0xFF) + (src2_data.u64 & 0xFF);
      // carry = (data.u64 & 0x100) >> 8;
      break;
    case U16_TYPE:
      fprintf(fp, "v_add_u16");
      // data.u64 = (src1_data.u64 & 0xFFFF) + (src2_data.u64 & 0xFFFF);
      // carry = (data.u64 & 0x10000) >> 16;
      break;
    case U32_TYPE:
      fprintf(fp, "v_add_u32");
      // data.u64 = (src1_data.u64 & 0xFFFFFFFF) + (src2_data.u64 & 0xFFFFFFFF);
      // carry = (data.u64 & 0x100000000) >> 32;
      break;
    case U64_TYPE:
      size = 2;
      fprintf(fp, "v_add_u64");
      // data.u64 = src1_data.u64 + src2_data.u64;
      break;
    case F16_TYPE:
      fprintf(fp, "v_add_f16");
      // data.f16 = src1_data.f16 + src2_data.f16;
      break;  // assert(0); break;
    case F32_TYPE:
      fprintf(fp, "v_add_f32");
      // data.f32 = src1_data.f32 + src2_data.f32;
      break;
    case F64_TYPE:
    case FF64_TYPE:
      size = 2;
      fprintf(fp, "v_add_f64");
      // data.f64 = src1_data.f64 + src2_data.f64;
      break;
    default:
      break;
  }
  fprintf(fp, "\t"); print_dst(finfo, dst, fp, size);
  fprintf(fp, ",\t"); print_src(finfo, src1, fp, size, i_type);
  fprintf(fp, ",\t"); print_src(finfo, src2, fp, size, i_type);
  // thread->set_operand_value(dst, data, i_type, thread, pI, overflow, carry);
}

void addc_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}

void and_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {

  //unsigned i_type = pI->get_type();

  // if (i_type != PRED_TYPE || i_type != U64_TYPE) {
  //    printf("Check and_impl_coasm on type %d\n", i_type);
  //    // assert(0)
  //}
  // the way ptxplus handles predicates: 1 = false and 0 = true
  /*
  if (i_type == PRED_TYPE)
    data.pred = ~(~(src1_data.pred) & ~(src2_data.pred));
  else
    data.u64 = src1_data.u64 & src2_data.u64;
    */

  print_type_3op("and", finfo, pI, fp);
}

void andn_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
#if 0
  unsigned i_type = pI->get_type();

  switch (i_type) {
    case B16_TYPE:
      // src2_data.u16 = ~src2_data.u16;
      break;
    case B32_TYPE:
      // src2_data.u32 = ~src2_data.u32;
      break;
    case B64_TYPE:
      // src2_data.u64 = ~src2_data.u64;
      break;
    default:
      printf("Execution error: type mismatch with instruction\n");
      assert(0);
      break;
  }
#endif
  print_type_3op("andn", finfo, pI, fp);

}

// atom_impl_coasm will now result in a callback being called in mem_ctrl_pop
// (gpu-sim.c)
void atom_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  // SYNTAX
  // atom.space.operation.type d, a, b[, c]; (now read in callback)
  unsigned m_atomic_spec = pI->get_atomic();
  switch (m_atomic_spec) {
    case ATOMIC_AND:
        print_type_3op("atom_and", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_OR:
        print_type_3op("atom_or", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_XOR:
        print_type_3op("atom_xor", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_CAS:
        print_type_3op("atom_cas", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_EXCH:
        print_type_3op("atom_exch", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_ADD:
        print_type_3op("atom_add", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_INC:
        print_type_3op("atom_inc", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_DEC:
        print_type_3op("atom_dec", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_MIN:
        print_type_3op("atom_min", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    case ATOMIC_MAX:
        print_type_3op("atom_max", finfo, pI, fp, 0, 2);  // src1 is address
        break;
    default:
        printf("Execution error: atom unknown\n");
        assert(0);
  }
  memory_space_t space = pI->get_space();
  if (space == undefined_space || space == generic_space) {
      fprintf(fp, "memspace:flat");
  } else if (space == global_space) {
      fprintf(fp, "memspace:global");
  } else if (space == shared_space) {
      fprintf(fp, "memspace:global");
  } else {
      printf("Execution error: atom memspace\n");
      assert(0);
  }
}

void bar_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  unsigned bar_op = pI->barrier_op();
  unsigned red_op = pI->get_atomic();
  // unsigned ctaid = thread->get_cta_uid();
  const operand_info &dst = pI->dst();
  assert(dst.is_literal());
  unsigned bar_id = dst.get_literal_value();
  finfo->get_bar_used().insert(bar_id);

  switch (bar_op) {
    case SYNC_OPTION: {
      if (pI->get_num_operands() == 2) {
        print_type_2op("bar_sync", finfo, pI, fp);  // bar_sync bar_id, bar_count
      } else if (pI->get_num_operands() == 1) {
        fprintf(fp, "bar_sync 0x%x\n", bar_id);  // bar_sync bar_id
      } else {
        fprintf(fp, "FIXME bar_sync op on sync_option %s\n", __FUNCTION__);  // bar_sync bar_id
      }
      break;
    }
    case ARRIVE_OPTION: {
      print_type_1op("bar_arrive", finfo, pI, fp);  // bar_sync bar_id, bar_count
      break;
    }
    case RED_OPTION: {
      if (pI->get_num_operands() > 3) {
        switch (red_op) {
          case ATOMIC_POPC:
            print_type_3op("bar_red_popc", finfo, pI, fp);  // bar_sync bar_id, bar_id, bar_count, pred
            break;
          case ATOMIC_AND:
            print_type_3op("bar_red_and", finfo, pI, fp);  // bar_sync bar_id, bar_id, bar_count, pred
            break;
          case ATOMIC_OR:
            print_type_3op("bar_red_or", finfo, pI, fp);  // bar_sync bar_id, bar_id, bar_count, pred
            break;
          default:
            abort();
            break;
        }
        print_tcc(finfo, pI->src3(), fp);
      } else {
        switch (red_op) {
          case ATOMIC_POPC:
            print_type_2op("bar_red_popc", finfo, pI, fp);  // bar_sync bar_id, bar_id, bar_count, pred
            break;
          case ATOMIC_AND:
            print_type_2op("bar_red_popc", finfo, pI, fp);  // bar_sync bar_id, bar_id, bar_count, pred
            break;
          case ATOMIC_OR:
            print_type_2op("bar_red_popc", finfo, pI, fp);  // bar_sync bar_id, bar_id, bar_count, pred
            break;
          default:
            abort();
            break;
        }
        print_tcc(finfo, pI->src2(), fp);
      }
      break;
    }
    default:
      abort();
      break;
  }

}

void bfe_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("bfe", finfo, pI, fp);
}

void bfi_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("bfi", finfo, pI, fp);
  // it need 5op
  // assert(0);
}

void bfind_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
    // leading 1
  print_type_op("bfind", finfo, pI, fp);
}

void bra_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {

  const operand_info &target = pI->dst();
  print_bra(finfo, pI, target, fp);
}

void brx_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  // TODO check brx difference
  bra_impl_coasm(finfo, pI, fp);
}

void break_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
  // const operand_info &target = thread->pop_breakaddr();
  // print_bra(finfo, pI, target, fp);
}

void breakaddr_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
    /*
  const operand_info &target = pI->dst();
  thread->push_breakaddr(target);
  assert(
      pI->has_pred() ==
      false);  // pdom analysis cannot handle if this instruction is predicated
      */
}

void brev_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("brev", finfo, pI, fp);
}
void brkpt_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}


void mma_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  int i, j, k, thrd;
  int row, col, offset;

  unsigned type = pI->get_type();
  unsigned type2 = pI->get_type2();
  int tid;
  const operand_info &dst = pI->operand_lookup(0);

  float temp;
  half temp2;

  if ((type == F16_TYPE) && (type2 == F16_TYPE))
        fprintf(fp, "wmma_f16_f16"); // matrix_d[i][j].f16 += matrix_c[i][j].f16;
  else if ((type == F32_TYPE) && (type2 == F16_TYPE)) {
        fprintf(fp, "wmma_f32_f16");/*
        temp2 = matrix_d[i][j].f16 + matrix_c[i][j].f16;
        temp = temp2;
        matrix_d[i][j].f32 = temp;*/
  } else if ((type == F16_TYPE) && (type2 == F32_TYPE)) {
        fprintf(fp, "wmma_f16_f32");/*
        temp = matrix_d[i][j].f16;
        temp += matrix_c[i][j].f32;
        matrix_d[i][j].f16 = half(temp);*/
  } else {
        fprintf(fp, "wmma_f32_f32");/*
        temp = matrix_d[i][j].f16;
        temp += matrix_c[i][j].f32;
        matrix_d[i][j].f32 = temp;*/
  }

  for (int operand_num = 0; operand_num < pI->get_num_operands(); operand_num++) {
    const operand_info &src = pI->operand_lookup(operand_num);
    if (operand_num != 0) fprintf(fp, ",");
    if (src.is_reg()) {
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, 1).c_str());
    } else if (src.is_memory_operand()) {
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, 2).c_str());
    } else {
      unsigned nelem = src.get_vect_nelem();
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, nelem).c_str());
    }
  }

  unsigned a_layout = pI->get_wmma_layout(0);
  unsigned b_layout = pI->get_wmma_layout(1);
  fprintf(fp, "\t%% a_layout:%s, ", (a_layout == ROW) ? "row" :"col");
  fprintf(fp, "b_layout:%s", (b_layout == ROW) ? "row" :"col");
  // FIXME on wmma_type
}

void call_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  static unsigned call_uid_next = 1;

  const operand_info &target = pI->func_addr();
  assert(target.is_function_address());

  const symbol *func_addr = target.get_symbol();
  function_info *target_func = func_addr->get_pc();

  if (target_func->is_pdom_set()) {
    printf("GPGPU-Sim PTX: PDOM analysis already done for %s \n",
           target_func->get_name().c_str());
  } else {
    printf("GPGPU-Sim PTX: finding reconvergence points for \'%s\'...\n",
           target_func->get_name().c_str());
    /*
     * Some of the instructions like printf() gives the gpgpusim the wrong
     * impression that it is a function call. As printf() doesnt have a body
     * like functions do, doing pdom analysis for printf() causes a crash.
     */
    if (target_func->get_function_size() > 0) target_func->do_pdom();
    target_func->set_pdom();
  }

  // check that number of args and return match function requirements
  if (pI->has_return() ^ target_func->has_return()) {
    printf(
        "GPGPU-Sim PTX: Execution error - mismatch in number of return values "
        "between\n"
        "               call instruction and function declaration\n");
    abort();
  }
  unsigned n_return = target_func->has_return();
  unsigned n_args = target_func->num_args();
  unsigned n_operands = pI->get_num_operands();

  if (n_operands != (n_return + 1 + n_args)) {
    printf(
        "GPGPU-Sim PTX: Execution error - mismatch in number of arguements "
        "between\n"
        "               call instruction and function declaration\n");
    abort();
  }


  // handle intrinsic functions
  std::string fname = target_func->get_name();
  fprintf(fp, "s_call  %02u", fname.c_str());
  if (fname == "vprintf") {
    // TODO gpgpusim_cuda_vprintf(pI, thread, target_func);
    return;
  }
  else if (fname == "cudaGetParameterBufferV2") {
    // target_func->gpgpu_ctx->device_runtime->gpgpusim_cuda_getParameterBufferV2( pI, thread, target_func);
    return;
  } else if (fname == "cudaLaunchDeviceV2") {
    // target_func->gpgpu_ctx->device_runtime->gpgpusim_cuda_launchDeviceV2( pI, thread, target_func);
    return;
  } else if (fname == "cudaStreamCreateWithFlags") {
    // target_func->gpgpu_ctx->device_runtime->gpgpusim_cuda_streamCreateWithFlags( pI, thread, target_func);
    return;
  }
/*
  // read source arguements into register specified in declaration of function
  arg_buffer_list_t arg_values;
  copy_args_into_buffer_list(pI, thread, target_func, arg_values);

  // record local for return value (we only support a single return value)
  const symbol *return_var_src = NULL;
  const symbol *return_var_dst = NULL;
  if (target_func->has_return()) {
    return_var_dst = pI->dst().get_symbol();
    return_var_src = target_func->get_return_var();
  }

  gpgpu_sim *gpu = thread->get_gpu();
  unsigned callee_pc = 0, callee_rpc = 0;
  if (gpu->simd_model() == POST_DOMINATOR) {
    thread->get_core()->get_pdom_stack_top_info(thread->get_hw_wid(),
                                                &callee_pc, &callee_rpc);
    assert(callee_pc == thread->get_pc());
  }

  thread->callstack_push(callee_pc + pI->inst_size(), callee_rpc,
                         return_var_src, return_var_dst, call_uid_next++);

  copy_buffer_list_into_frame(thread, arg_values);
*/
}

// Ptxplus version of call instruction. Jumps to a label not a different Kernel.
void callp_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  static unsigned call_uid_next = 1;

  const operand_info &target = pI->dst();
  fprintf(fp, "call %s", target.name());
}

void clz_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("clz", finfo, pI, fp);
}

void cnot_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("cnot", finfo, pI, fp);
}

void cos_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("cos", finfo, pI, fp);

  unsigned i_type = pI->get_type();

  switch (i_type) {
    case F32_TYPE:
      // d.f32 = cos(a.f32);
      break;
    default:
      printf("Execution error: type mismatch with instruction\n");
      assert(0);
      break;
  }
}

void chop(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
               int rounding_mode, int saturation_mode, FILE* fp) {
      fprintf(fp, "_chop_b%u_b%u", to_width, from_width);
  /*
  switch (to_width) {
    case 8:
      // x.mask_and(0, 0xFF);
      break;
    case 16:
      x.mask_and(0, 0xFFFF);
      break;
    case 32:
      x.mask_and(0, 0xFFFFFFFF);
      break;
    case 64:
      break;
    default:
      assert(0);
  }
  return x;
  */
}

void sext(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
               int rounding_mode, int saturation_mode, FILE* fp) {
  // x = chop(x, 0, from_width, 0, rounding_mode, saturation_mode);
      fprintf(fp, "_sext_i%u_i%u", to_width, from_width);
/*
  switch (from_width) {
    case 8:
      fprintf(fp, "_sext_i32_i8");
      // if (x.get_bit(7)) x.mask_or(0xFFFFFFFF, 0xFFFFFF00);
      break;
    case 16:
      fprintf(fp, "_sext_i32_i16");
      // if (x.get_bit(15)) x.mask_or(0xFFFFFFFF, 0xFFFF0000);
      break;
    case 32:
      fprintf(fp, "_sext_i32_i32");
      // if (x.get_bit(31)) x.mask_or(0xFFFFFFFF, 0x00000000);
      break;
    case 64:
      break;
    default:
      assert(0);
  }
*/
  // return x;
}

// sign extend depending on the destination register size - hack to get
// SobelFilter working in CUDA 4.2
void sexd(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
               int rounding_mode, int saturation_mode, FILE* fp) {
  // x = chop(x, 0, from_width, 0, rounding_mode, saturation_mode);
      fprintf(fp, "_sext_i%u_i%u", to_width, from_width);
  /*
  switch (to_width) {
    case 8:
      fprintf(fp, "_sext_i32_i8");
      // if (x.get_bit(7)) x.mask_or(0xFFFFFFFF, 0xFFFFFF00);
      break;
    case 16:
      fprintf(fp, "_sext_i32_i16");
      // if (x.get_bit(15)) x.mask_or(0xFFFFFFFF, 0xFFFF0000);
      break;
    case 32:
      fprintf(fp, "_sext_i32_i32");
      // if (x.get_bit(31)) x.mask_or(0xFFFFFFFF, 0x00000000);
      break;
    case 64:
      break;
    default:
      assert(0);
  }
  */
  // return x;
}

void zext(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
               int rounding_mode, int saturation_mode, FILE* fp) {
  // return chop(x, 0, from_width, 0, rounding_mode, saturation_mode);
  fprintf(fp, "_zext_b%u_b%u", to_width, from_width);
}
/*
int saturatei(int a, int max, int min) {
  if (a > max)
    a = max;
  else if (a < min)
    a = min;
  return a;
}

unsigned int saturatei(unsigned int a, unsigned int max) {
  if (a > max) a = max;
  return a;
}
*/

void f2x(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
              int rounding_mode, int saturation_mode, FILE *fp) {
  // half mytemp;
  // half_float::half tmp_h;
  // assert( from_width == 32);
  //
  if (to_sign == 1)   // convert to 64-bit number first?
    fprintf(fp, "_f2x_i%u_f%u", to_width, from_width);
  else
    fprintf(fp, "_f2x_u%u_f%u", to_width, from_width);

  enum cudaRoundMode mode = cudaRoundZero;
  switch (rounding_mode) {
    case RZI_OPTION:
      fprintf(fp, "_rzi");
      // mode = cudaRoundZero;
      break;
    case RNI_OPTION:
      fprintf(fp, "_rni");
      // mode = cudaRoundNearest;
      break;
    case RMI_OPTION:
      fprintf(fp, "_rmi");
      // mode = cudaRoundMinInf;
      break;
    case RPI_OPTION:
      fprintf(fp, "_rpi");
      // mode = cudaRoundPosInf;
      break;
    default:
      break;
  }
/*
  ptx_reg_t y;
  if (to_sign == 1) {  // convert to 64-bit number first?
    int tmp = cuda_math::float2int(x.f32, mode);
    if ((x.u32 & 0x7f800000) == 0) tmp = 0;  // round denorm. FP to 0
    if (saturation_mode && to_width < 32) {
      tmp = saturatei(tmp, (1 << to_width) - 1, -(1 << to_width));
    }
    switch (to_width) {
      case 8:
        y.s8 = (char)tmp;
        break;
      case 16:
        y.s16 = (short)tmp;
        break;
      case 32:
        y.s32 = (int)tmp;
        break;
      case 64:
        y.s64 = (long long)tmp;
        break;
      default:
        assert(0);
        break;
    }
  } else if (to_sign == 0) {
    unsigned int tmp = cuda_math::float2uint(x.f32, mode);
    if ((x.u32 & 0x7f800000) == 0) tmp = 0;  // round denorm. FP to 0
    if (saturation_mode && to_width < 32) {
      tmp = saturatei(tmp, (1 << to_width) - 1);
    }
    switch (to_width) {
      case 8:
        y.u8 = (unsigned char)tmp;
        break;
      case 16:
        y.u16 = (unsigned short)tmp;
        break;
      case 32:
        y.u32 = (unsigned int)tmp;
        break;
      case 64:
        y.u64 = (unsigned long long)tmp;
        break;
      default:
        assert(0);
        break;
    }
  } else {
    switch (to_width) {
      case 16:
        y.f16 = half_float::half_cast<half,
                                      std::numeric_limits<float>::round_style>(
            x.f32);  // mytemp;
        break;
      case 32:
        y.f32 = float(x.f16);
        break;  // handled by f2f
      case 64:
        y.f64 = x.f32;
        break;
      default:
        assert(0);
        break;
    }
  }
  return y;
  */
}
/*
double saturated2i(double a, double max, double min) {
  if (a > max)
    a = max;
  else if (a < min)
    a = min;
  return a;
}
*/

void d2x(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
              int rounding_mode, int saturation_mode, FILE *fp) {
  assert(from_width == 64);

  if (to_sign == 1)   // convert to 64-bit number first?
    fprintf(fp, "_d2x_i%u_d%u", to_width, from_width);
  else
    fprintf(fp, "_d2x_u%u_d%u", to_width, from_width);

  double tmp;
  switch (rounding_mode) {
    case RZI_OPTION:
      fprintf(fp, "_rzi");
      // tmp = trunc(x.f64);
      break;
    case RNI_OPTION:
      fprintf(fp, "_rni");
      // tmp = nearbyint(x.f64);
      break;
    case RMI_OPTION:
      fprintf(fp, "_rmi");
      // tmp = floor(x.f64);
      break;
    case RPI_OPTION:
      fprintf(fp, "_rpi");
      // tmp = ceil(x.f64);
      break;
    default:
      // tmp = x.f64;
      break;
  }
/*
  ptx_reg_t y;
  if (to_sign == 1) {
    tmp = saturated2i(tmp, ((1 << (to_width - 1)) - 1), (1 << (to_width - 1)));
    switch (to_width) {
      case 8:
        y.s8 = (char)tmp;
        break;
      case 16:
        y.s16 = (short)tmp;
        break;
      case 32:
        y.s32 = (int)tmp;
        break;
      case 64:
        y.s64 = (long long)tmp;
        break;
      default:
        assert(0);
        break;
    }
  } else if (to_sign == 0) {
    tmp = saturated2i(tmp, ((1 << (to_width - 1)) - 1), 0);
    switch (to_width) {
      case 8:
        y.u8 = (unsigned char)tmp;
        break;
      case 16:
        y.u16 = (unsigned short)tmp;
        break;
      case 32:
        y.u32 = (unsigned int)tmp;
        break;
      case 64:
        y.u64 = (unsigned long long)tmp;
        break;
      default:
        assert(0);
        break;
    }
  } else {
    switch (to_width) {
      case 16:
        assert(0);
        break;
      case 32:
        y.f32 = x.f64;
        break;
      case 64:
        y.f64 = x.f64;  // should be handled by d2d
        break;
      default:
        assert(0);
        break;
    }
  }
  return y;
  */
}

void s2f(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
              int rounding_mode, int saturation_mode, FILE *fp) {
  // ptx_reg_t y;
  if (from_width < 64) {  // 32-bit conversion
    // y = sext(x, from_width, 32, 0, rounding_mode, saturation_mode);
    switch (to_width) {
      case 16:
        assert(0);
        break;
      case 32:
        switch (rounding_mode) {
          case RZ_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rz", to_width, from_width);
            // y.f32 = cuda_math::__int2float_rz(y.s32);
            break;
          case RN_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rn", to_width, from_width);
            // y.f32 = cuda_math::__int2float_rn(y.s32);
            break;
          case RM_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rm", to_width, from_width);
            // y.f32 = cuda_math::__int2float_rd(y.s32);
            break;
          case RP_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rp", to_width, from_width);
            // y.f32 = cuda_math::__int2float_ru(y.s32);
            break;
          default:
            break;
        }
        break;
      case 64:
        fprintf(fp, "_s2f_f%u_i%u", to_width, from_width);
        // y.f64 = y.s32;
        break;  // no rounding needed
      default:
        assert(0);
        break;
    }
  } else {
    switch (to_width) {
      case 16:
        assert(0);
        break;
      case 32:
        switch (rounding_mode) {
          case RZ_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rz", to_width, from_width);
            // y.f32 = cuda_math::__ll2float_rz(y.s64);
            break;
          case RN_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rn", to_width, from_width);
            // y.f32 = cuda_math::__ll2float_rn(y.s64);
            break;
          case RM_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rm", to_width, from_width);
            // y.f32 = cuda_math::__ll2float_rd(y.s64);
            break;
          case RP_OPTION:
            fprintf(fp, "_s2f_f%u_i%u_rm", to_width, from_width);
            // y.f32 = cuda_math::__ll2float_ru(y.s64);
            break;
          default:
            break;
        }
        break;
      case 64:
        fprintf(fp, "_s2f_f%u_i%u_rm", to_width, from_width);
        // y.f64 = y.s64;
        break;  // no internal implementation found
      default:
        assert(0);
        break;
    }
  }

  // saturating an integer to 1 or 0?
  // return y;
}

void u2f(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
              int rounding_mode, int saturation_mode, FILE *fp) {
  // ptx_reg_t y;

  if (from_width < 64) {  // 32-bit conversion
    // y = zext(x, from_width, 32, 0, rounding_mode, saturation_mode);

    switch (to_width) {
      case 16:
        assert(0);
        break;
      case 32:
        switch (rounding_mode) {
          case RZ_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rz", to_width, from_width);
            // y.f32 = cuda_math::__uint2float_rz(y.u32);
            break;
          case RN_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rn", to_width, from_width);
            // y.f32 = cuda_math::__uint2float_rn(y.u32);
            break;
          case RM_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rm", to_width, from_width);
            // y.f32 = cuda_math::__uint2float_rd(y.u32);
            break;
          case RP_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rp", to_width, from_width);
            // y.f32 = cuda_math::__uint2float_ru(y.u32);
            break;
          default:
            break;
        }
        break;
      case 64:
        fprintf(fp, "_u2f_f%u_u%u", to_width, from_width);
        // y.f64 = y.u32;
        break;  // no rounding needed
      default:
        assert(0);
        break;
    }
  } else {
    switch (to_width) {
      case 16:
        assert(0);
        break;
      case 32:
        switch (rounding_mode) {
          case RZ_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rz", to_width, from_width);
            // y.f32 = cuda_math::__ull2float_rn(y.u64);
            break;
          case RN_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rn", to_width, from_width);
            // y.f32 = cuda_math::__ull2float_rn(y.u64);
            break;
          case RM_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rm", to_width, from_width);
            // y.f32 = cuda_math::__ull2float_rn(y.u64);
            break;
          case RP_OPTION:
            fprintf(fp, "_u2f_f%u_u%u_rp", to_width, from_width);
            // y.f32 = cuda_math::__ull2float_rn(y.u64);
            break;
          default:
            break;
        }
        break;
      case 64:
        fprintf(fp, "_u2f_f%u_u%u", to_width, from_width);
        // y.f64 = y.u64;
        break;  // no internal implementation found
      default:
        assert(0);
        break;
    }
  }

  // saturating an integer to 1 or 0?
  // return y;
}

void f2f(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
              int rounding_mode, int saturation_mode, FILE *fp) {
  // ptx_reg_t y;
  if (from_width == 16) {
    // half_float::detail::uint16 val = x.u16;
    // y.f32 = half_float::detail::half2float<float>(val);
    fprintf(fp, "_f2f_f32_f16");
  } else {
    switch (rounding_mode) {
      case RZI_OPTION:
       //  y.f32 = truncf(x.f32);
        fprintf(fp, "_f2f_f32_f32_rzi");
        break;
      case RNI_OPTION:
        fprintf(fp, "_f2f_f32_f32_rni");
        // y.f32 = nearbyintf(x.f32);
        break;
      case RMI_OPTION:
        fprintf(fp, "_f2f_f32_f32_rmi");
        /*
        if ((x.u32 & 0x7f800000) == 0) {
          y.u32 = x.u32 & 0x80000000;  // round denorm. FP to 0, keeping sign
        } else {
          y.f32 = floorf(x.f32);
        }*/
        break;
      case RPI_OPTION:
        fprintf(fp, "_f2f_f32_f32_rpi");
        /*
        if ((x.u32 & 0x7f800000) == 0) {
          y.u32 = x.u32 & 0x80000000;  // round denorm. FP to 0, keeping sign
        } else {
          y.f32 = ceilf(x.f32);
        }*/
        break;
      default:
        fprintf(fp, "_f2f_f32_f32");
        /*
        if ((x.u32 & 0x7f800000) == 0) {
          y.u32 = x.u32 & 0x80000000;  // round denorm. FP to 0, keeping sign
        } else {
          y.f32 = x.f32;
        }
        */
        break;
    }
    /*
    if (isnanf(y.f32))
    {
      y.u32 = 0x7fffffff;
    } else if (saturation_mode) {
      y.f32 = cuda_math::__saturatef(y.f32);
    }
    */
  }

  // return y;
}

void d2d(unsigned from_type, unsigned from_width, unsigned to_width, int to_sign,
              int rounding_mode, int saturation_mode, FILE *fp) {
  // ptx_reg_t y;
  switch (rounding_mode) {
    case RZI_OPTION:
      fprintf(fp, "_d2d_f64_f64_rzi");
      // y.f64 = trunc(x.f64);
      break;
    case RNI_OPTION:
      fprintf(fp, "_d2d_f64_f64_rni");
      // y.f64 = nearbyint(x.f64);
      break;
    case RMI_OPTION:
      fprintf(fp, "_d2d_f64_f64_rmi");
      // y.f64 = floor(x.f64);
      break;
    case RPI_OPTION:
      fprintf(fp, "_d2d_f64_f64_rpi");
      // y.f64 = ceil(x.f64);
      break;
    default:
      fprintf(fp, "_d2d_f64_f64");
      // y.f64 = x.f64;
      break;
  }
  /*
  if (std::isnan(y.f64)) {
    y.u64 = 0xfff8000000000000ull;
  } else if (saturation_mode) {
    y.f64 = cuda_math::__saturatef(y.f64);
  }
  */
  // return y;
}

void (*g_cvt_print_coasm_fn[11][11])(unsigned from_type, unsigned from_width,
                              unsigned to_width, int to_sign, int rounding_mode,
                              int saturation_mode, FILE *fp) = {
    {NULL, sext, sext, sext, NULL, sext, sext, sext, s2f, s2f, s2f},
    {chop, NULL, sext, sext, chop, NULL, sext, sext, s2f, s2f, s2f},
    {chop, sexd, NULL, sext, chop, chop, NULL, sext, s2f, s2f, s2f},
    {chop, chop, chop, NULL, chop, chop, chop, NULL, s2f, s2f, s2f},
    {NULL, zext, zext, zext, NULL, zext, zext, zext, u2f, u2f, u2f},
    {chop, NULL, zext, zext, chop, NULL, zext, zext, u2f, u2f, u2f},
    {chop, chop, NULL, zext, chop, chop, NULL, zext, u2f, u2f, u2f},
    {chop, chop, chop, NULL, chop, chop, chop, NULL, u2f, u2f, u2f},
    {f2x, f2x, f2x, f2x, f2x, f2x, f2x, f2x, NULL, f2f, f2x},
    {f2x, f2x, f2x, f2x, f2x, f2x, f2x, f2x, f2x, f2f, f2x},
    {d2x, d2x, d2x, d2x, d2x, d2x, d2x, d2x, d2x, d2x, d2d}};

#if 0
void ptx_round(ptx_reg_t &data, int rounding_mode, int type) {
  if (rounding_mode == RN_OPTION) {
    return;
  }
  switch (rounding_mode) {
    case RZI_OPTION:
      switch (type) {
        case S8_TYPE:
        case S16_TYPE:
        case S32_TYPE:
        case S64_TYPE:
        case U8_TYPE:
        case U16_TYPE:
        case U32_TYPE:
        case U64_TYPE:
          printf("Trying to round an integer??\n");
          assert(0);
          break;
        case F16_TYPE:
          data.f16 = truncf(data.f16);
          break;  // assert(0); break;
        case F32_TYPE:
          data.f32 = truncf(data.f32);
          break;
        case F64_TYPE:
        case FF64_TYPE:
          if (data.f64 < 0)
            data.f64 = ceil(data.f64);  // negative
          else
            data.f64 = floor(data.f64);  // positive
          break;
        default:
          assert(0);
          break;
      }
      break;
    case RNI_OPTION:
      switch (type) {
        case S8_TYPE:
        case S16_TYPE:
        case S32_TYPE:
        case S64_TYPE:
        case U8_TYPE:
        case U16_TYPE:
        case U32_TYPE:
        case U64_TYPE:
          printf("Trying to round an integer??\n");
          assert(0);
          break;
        case F16_TYPE:  // assert(0); break;
          data.f16 = nearbyintf(data.f16);
          break;
        case F32_TYPE:
          data.f32 = nearbyintf(data.f32);
          break;
        case F64_TYPE:
        case FF64_TYPE:
          data.f64 = round(data.f64);
          break;
        default:
          assert(0);
          break;
      }
      break;
    case RMI_OPTION:
      switch (type) {
        case S8_TYPE:
        case S16_TYPE:
        case S32_TYPE:
        case S64_TYPE:
        case U8_TYPE:
        case U16_TYPE:
        case U32_TYPE:
        case U64_TYPE:
          printf("Trying to round an integer??\n");
          assert(0);
          break;
        case F16_TYPE:
          data.f16 = floorf(data.f16);
          break;  // assert(0); break;
        case F32_TYPE:
          data.f32 = floorf(data.f32);
          break;
        case F64_TYPE:
        case FF64_TYPE:
          data.f64 = floor(data.f64);
          break;
        default:
          assert(0);
          break;
      }
      break;
    case RPI_OPTION:
      switch (type) {
        case S8_TYPE:
        case S16_TYPE:
        case S32_TYPE:
        case S64_TYPE:
        case U8_TYPE:
        case U16_TYPE:
        case U32_TYPE:
        case U64_TYPE:
          printf("Trying to round an integer??\n");
          assert(0);
          break;
        case F16_TYPE:
          data.f16 = ceilf(data.f16);
          break;  // assert(0); break;
        case F32_TYPE:
          data.f32 = ceilf(data.f32);
          break;
        case F64_TYPE:
        case FF64_TYPE:
          data.f64 = ceil(data.f64);
          break;
        default:
          assert(0);
          break;
      }
      break;
    default:
      break;
  }

  if (type == F32_TYPE) {
    if (isnanf(data.f32))
    {
      data.u32 = 0x7fffffff;
    }
  }
  if ((type == F64_TYPE) || (type == FF64_TYPE)) {
    if (std::isnan(data.f64)) {
      data.u64 = 0xfff8000000000000ull;
    }
  }
}

void ptx_saturate(ptx_reg_t &data, int saturation_mode, int type) {
  if (!saturation_mode) {
    return;
  }
  switch (type) {
    case S8_TYPE:
    case S16_TYPE:
    case S32_TYPE:
    case S64_TYPE:
    case U8_TYPE:
    case U16_TYPE:
    case U32_TYPE:
    case U64_TYPE:
      printf("Trying to clamp an integer to 1??\n");
      assert(0);
      break;
    case F16_TYPE:                           // assert(0); break;
      if (data.f16 > 1.0f) data.f16 = 1.0f;  // negative
      if (data.f16 < 0.0f) data.f16 = 0.0f;  // positive
      break;
    case F32_TYPE:
      if (data.f32 > 1.0f) data.f32 = 1.0f;  // negative
      if (data.f32 < 0.0f) data.f32 = 0.0f;  // positive
      break;
    case F64_TYPE:
    case FF64_TYPE:
      if (data.f64 > 1.0f) data.f64 = 1.0f;  // negative
      if (data.f64 < 0.0f) data.f64 = 0.0f;  // positive
      break;
    default:
      assert(0);
      break;
  }
}
#endif

void cvt_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  unsigned to_type = pI->get_type();
  unsigned from_type = pI->get_type2();
  unsigned rounding_mode = pI->rounding_mode();
  unsigned saturation_mode = pI->saturation_mode();

  //   if ( to_type == F16_TYPE || from_type == F16_TYPE )
  //      abort();

  int to_sign, from_sign;
  size_t from_width, to_width;
  unsigned src_fmt =
      type_info_key::type_decode(from_type, from_width, from_sign);
  unsigned dst_fmt = type_info_key::type_decode(to_type, to_width, to_sign);

  fprintf(fp, "v");
  // ptx_reg_t data = thread->get_operand_value(src1, dst, from_type, thread, 1);
  if (g_cvt_print_coasm_fn[src_fmt][dst_fmt] != NULL) {
    g_cvt_print_coasm_fn[src_fmt][dst_fmt](from_type, from_width, to_width, to_sign, rounding_mode, saturation_mode, fp);
    // ptx_reg_t result = g_cvt_fn[src_fmt][dst_fmt](
    //    data, from_width, to_width, to_sign, rounding_mode, saturation_mode);
    // data = result;
  }

  fprintf(fp, "\t"); print_dst(finfo, dst, fp, (to_width + 31)/ 32);
  fprintf(fp, ",\t");
  if (pI->is_neg()) {
      fprintf(fp, "-");
  }
  print_src(finfo, src1, fp, (from_width + 31) / 32);

  // thread->set_operand_value(dst, data, to_type, thread, pI);
}


void cvta_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t data;

  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  memory_space_t space = pI->get_space();
  bool to_non_generic = pI->is_to();

  unsigned i_type = pI->get_type();
  // ptx_reg_t from_addr = thread->get_operand_value(src1, dst, i_type, thread, 1);
  // addr_t from_addr_hw = (addr_t)from_addr.u64;
  // addr_t to_addr_hw = 0;
  // unsigned smid = thread->get_hw_sid();
  // unsigned hwtid = thread->get_hw_tid();

  if (to_non_generic) {
    switch (space.get_type()) {
      case shared_space:
        // to_addr_hw = generic_to_shared(smid, from_addr_hw);
        fprintf(fp, "v_cvta_flat_to_shared");
        break;
      case local_space:
        // to_addr_hw = generic_to_local(smid, hwtid, from_addr_hw);
        fprintf(fp, "v_cvta_flat_to_local");
        break;
      case global_space:
        // to_addr_hw = generic_to_global(from_addr_hw);
        fprintf(fp, "v_cvta_flat_to_global");
        break;
      default:
        abort();
    }
  } else {
    switch (space.get_type()) {
      case shared_space:
        // to_addr_hw = shared_to_generic(smid, from_addr_hw);
        fprintf(fp, "v_cvta_shared_to_flat");
        break;
      case local_space:
        fprintf(fp, "v_cvta_local_to_flat");
        // to_addr_hw = local_to_generic(smid, hwtid, from_addr_hw) +
        //            thread->get_local_mem_stack_pointer();
        break;  // add stack ptr here so that it can be passed as a pointer at
                // function call
      case global_space:
        fprintf(fp, "v_cvta_global_to_flat");
        // to_addr_hw = global_to_generic(from_addr_hw);
        break;
      default:
        abort();
    }
  }

  fprintf(fp, "\t");
  print_1op(finfo, pI, fp, 2);
// ptx_reg_t to_addr; // to_addr.u64 = to_addr_hw;
  // thread->set_reg(dst.get_symbol(), to_addr);
}

void div_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_3op("div", finfo, pI, fp);
}

void dp4a_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  printf("DP4A instruction not implemented yet");
  assert(0);
}

void ex2_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t src1_data, src2_data, data;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();

  unsigned i_type = pI->get_type();


  switch (i_type) {
    case F32_TYPE:
      fprintf(fp, "v_powf_f32\t%s, 2.0,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str());
      // data.f32 = cuda_math::__powf(2.0, src1_data.f32);
      break;
    default:
      printf("Execution error: type mismatch with instruction\n");
      assert(0);
      break;
  }

}

void exit_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  fprintf(fp, "t_exit\n");
    /*
  thread->set_done();
  thread->exitCore();
  thread->registerExit();
  */
}

void mad_def(function_info* finfo, const ptx_instruction *pI, FILE *fp,
             bool use_carry = false);

void fma_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  mad_def(finfo, pI, fp);
}

void isspacep_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t a;
  bool t = false;

  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  memory_space_t space = pI->get_space();

  // a = thread->get_reg(src1.get_symbol());
  // addr_t addr = (addr_t)a.u64;
  // unsigned smid = thread->get_hw_sid();
  // unsigned hwtid = thread->get_hw_tid();

  fprintf(fp, "v_isspace ");
  fprintf(fp, "\t%s", finfo->get_coasm_tcc(dst).c_str());
  fprintf(fp, "\t%s", finfo->get_coasm_reg(src1, 2).c_str());
  /*
  switch (space.get_type()) {
    case shared_space:
      t = isspace_shared(smid, addr);
    case local_space:
      t = isspace_local(smid, hwtid, addr);
    case global_space:
      t = isspace_global(addr);
    default:
      abort();
  }*/

  // ptx_reg_t p;
  // p.pred = t ? 1 : 0;

  // thread->set_reg(dst.get_symbol(), p);
}


unsigned dtype_size(unsigned type) {
  switch(type) {
    case S8_TYPE:
  	case B8_TYPE:
  	case U8_TYPE:
        return 1; break;
  	case S16_TYPE:
  	case U16_TYPE:
  	case B16_TYPE:
  	case F16_TYPE:
        return 2; break;
  	case S32_TYPE:
  	case B32_TYPE:
  	case U32_TYPE:
  	case F32_TYPE:
        return 4; break;
  	case S64_TYPE:
  	case B64_TYPE:
  	case U64_TYPE:
  	case F64_TYPE:
        return 8; break;
    default:
      assert("ld_exec failed on unknow type");
  }
}

void print_mspace(memory_space_t &space, FILE *fp) {
  switch (space.get_type()) {
    case global_space:
      fprintf(fp, " %% mspace:global");
      break;
    case param_space_local:
    case param_space_kernel:
      // FIXME on local
      fprintf(fp, " %% mspace:param");
      break;
    case local_space:
      fprintf(fp, " %% mspace:private");
      break;
    case tex_space:
    case surf_space:
      fprintf(fp, " %% mspace:surface");
      break;
    case shared_space:
      fprintf(fp, " %% mspace:shared");
      break;
    case const_space:
      fprintf(fp, " %% mspace:const");
      break;
    case generic_space:
      fprintf(fp, " %% mspace:flat");
      break;
    case sstarr_space:
    case param_space_unclassified:
    case undefined_space:
    default:
      abort();
  }
}

void decode_space(memory_space_t &space, const operand_info &op) {

  if (space == param_space_unclassified) {
    // need to op to determine whether it refers to a kernel param or local
    // param
    const symbol *s = op.get_symbol();
    const type_info *t = s->type();
    type_info_key ti = t->get_key();
    if (ti.is_param_kernel())
      space = param_space_kernel;
    else if (ti.is_param_local()) {
      space = param_space_local;
    }
    // mov r1, param-label
    else if (ti.is_reg()) {
      space = param_space_kernel;
    } else {
      printf("GPGPU-Sim PTX: ERROR ** cannot resolve .param space for '%s'\n",
             s->name().c_str());
      abort();
    }
  }
}

void ld_exec(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  bool debug = false;
  if (debug) fprintf(fp, "Debug: ld_exec:\n");
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  if (debug) fprintf(fp, "Debug: operand num %d\n", pI->get_num_operands());

  unsigned type = pI->get_type();
  unsigned dsize = dtype_size(type);

  // ptx_reg_t src1_data = thread->get_operand_value(src1, dst, type, thread, 1);
  // ptx_reg_t data;
  memory_space_t space = pI->get_space();
  unsigned vector_spec = pI->get_vector();

  decode_space(space, src1);

  _memory_space_t space_type = space.get_type();

  unsigned kernel_param_addr_in_bytes;
  if (space_type == param_space_kernel) {
      const symbol *param = src1.get_symbol();
      addr_t param_addr = param->get_address();
      kernel_param_addr_in_bytes = param_addr;
#if 0
      std::string param_name = src1.get_symbol()->name();
      if (debug) fprintf(fp, "DEBUG param str is: %s\n", param_name.c_str());
      std::string::size_type pos = param_name.rfind("param_");
      if (pos != std::string::npos) {
        std::string param_num = param_name.substr(pos + 6);
        pos = param_num.rfind("+");

        if (pos != std::string::npos) {
            // if (param_name[pos+8] == '+') {
            param_num = param_name.substr(pos + 1);
            kernel_param_num = std::atoi(param_num.c_str());
            if (debug) fprintf(fp, "Debug: %s", param_name.c_str());
        } else {
            kernel_param_num = std::atoi(param_num.c_str());
        }
      } else {
          kernel_param_num = 0;
      }
      unsigned num = kernel_param_num;
      fprintf(fp, "s_load_dwordx2 s[%d:%d], s[0:1], %d\n", num*2+2, num*2+3, num*2);
  } else {
      fprintf(fp, "FIXME on space type is not kernel\n");
#endif
  }

  fprintf(fp, "v_load");

  if (dsize == 1)
    fprintf(fp, "_u8");
  else if (dsize == 2)
    fprintf(fp, "_u16");
  else if (dsize == 4)
    fprintf(fp, "_u32");
  else if (dsize == 8)
    fprintf(fp, "_u64");
  else
    assert("ld_exec failed on unknow type");

  if (!vector_spec) {
    // fprintf(fp, "\tv%u", dst.reg_num());
    fprintf(fp, "\t%s", finfo->get_coasm_reg(dst, (dsize + 3)/4).c_str());
    // mem->read(addr, size / 8, &data.s64);
    // if (type == S16_TYPE || type == S32_TYPE) sign_extend(data, size, dst);
    // thread->set_operand_value(dst, data, type, thread, pI);
  } else {
    if (vector_spec == V2_TYPE) {  // either V3 or V4
        if (dst.is_reg()) {
            // fprintf(fp, "\tv[%u:%u]", dst.reg_num(), dst.reg_num() + 1);
            fprintf(fp, "x2\t%s", finfo->get_coasm_reg(dst, (dsize + 3)/4 * 2).c_str());
        } else if (dst.is_vector()) {
            unsigned nelem = dst.get_vect_nelem();
            unsigned dst_reg_num = dst.reg_num();
            fprintf(fp, "x2\t%s", finfo->get_coasm_reg(dst, nelem).c_str());
        } else {
            fprintf(fp, "\tFIXME on st dst %s\n", __FUNCTION__);
        }
    } else if (vector_spec == V3_TYPE || vector_spec == V4_TYPE) {  // either V3 or V4
        if (dst.is_vector()) {
            unsigned nelem = dst.get_vect_nelem();
            unsigned dst_reg_num = dst.reg_num();
            fprintf(fp, "x4\t%s", finfo->get_coasm_reg(dst, nelem).c_str());
        } else {
            fprintf(fp, "\t%s", finfo->get_coasm_reg(dst, (dsize + 3)/4 * 4).c_str());
        }
      // fprintf(fp, "\tv[%u:%u]", dst.reg_num(), dst.reg_num() + 3);
    }
  }

  if (space_type == param_space_kernel) {
    // unsigned num = kernel_param_addr_in_bytes / 4;
    // we asume each param is 4bytes align
    // assert(num * 4 == kernel_param_addr_in_bytes);
#ifdef COASM_BUILTIN_USE
    const symbol *param = src1.get_symbol();
    fprintf(fp, ",\t%s", param->name().c_str());
    finfo->get_coasm_special_sregs()[KERNEL_CTRL_BIT_PARAM_BASE] = -2;
#else
    fprintf(fp, ",\tv[%d:%d]", KERNEL_PARAM_BASE, KERNEL_PARAM_BASE+1);
#endif
  } else if (src1.is_reg()) {
    // fprintf(fp, ",\tv%u", src1.reg_num());
    fprintf(fp, ",\t%s", finfo->get_coasm_reg(src1, 2).c_str());
  } else if(src1.is_memory_operand()) {
    fprintf(fp, ",\t%s, 0x%x", finfo->get_coasm_reg(src1, 2).c_str(), src1.get_addr_offset());
  } else {
    fprintf(fp, "\tFIXME on operand src1 %s", __FUNCTION__);
  }

  print_mspace(space, fp);

  if (debug) fprintf(fp, "Debug: ld_exec_end \n");
  //    fprintf(fp, ",\t%s", finfo->get_coasm_reg(src1));
}

void ld_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ld_exec(finfo, pI, fp);
}
void ldu_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ld_exec(finfo, pI, fp);
}

void mma_st_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  size_t size;
  unsigned smid;
  int t;
  int thrd, k;
  ptx_thread_info *thread;

  const operand_info &src = pI->operand_lookup(1);
  const operand_info &src1 = pI->operand_lookup(0);
  const operand_info &src2 = pI->operand_lookup(2);

  int tid;
  unsigned type = pI->get_type();
  unsigned wmma_type = pI->get_wmma_type();
  unsigned wmma_layout = pI->get_wmma_layout(0);

  int stride;

  _memory_op_t insn_memory_op = pI->has_memory_read() ? memory_load : memory_store;


    //ptx_reg_t addr_reg = thread->get_operand_value(src1, src, type, thread, 1);
    // ptx_reg_t src2_data = thread->get_operand_value(src2, src, type, thread, 1);
    // stride = src2_data.u32;
    memory_space_t space = pI->get_space();
    _memory_space_t space_type = space.get_type();

    // hack to find correct mem space
    if (pI->source().find(".shared.") != std::string::npos) {
      space_type = shared_space;
      space = memory_space_t(space_type);
    }

    memory_space *mem = NULL;
    // addr_t addr = addr_reg.u32;

    new_addr_type mem_txn_addr[MAX_ACCESSES_PER_INSN_PER_THREAD];
    int num_mem_txn = 0;

    type_info_key::type_decode(type, size, t);
    // addr_t new_addr = addr + thread_group_offset(thrd, wmma_type, wmma_layout, type, stride) *
    //                size / 8;
    addr_t push_addr;

    if (type == F32_TYPE) {
        // mem->write(new_addr+4*acc_float_offset(k,wmma_layout,stride),size/8,&v[k].s64,thread,pI);
      fprintf(fp, "wmma_st_f32");
    } else if (type == F16_TYPE) {
      fprintf(fp, "wmma_st_f16");
    } else {
      printf("wrong wmma data type\n");
      abort();
    }

  for (int operand_num = 0; operand_num < 3; operand_num++) {
    const operand_info &src = pI->operand_lookup(operand_num);
    if (operand_num != 0) fprintf(fp, ",");
    if (src.is_reg()) {
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, 1).c_str());
    } else if (src.is_memory_operand()) {
      uint32_t size = 2;
      if (space_type == shared_space) {size = 1;}
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, size).c_str());
    } else {
      unsigned nelem = src.get_vect_nelem();
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, nelem).c_str());
    }
  }

  print_mspace(space, fp);
  fprintf(fp, ",layout:%s", (wmma_layout == ROW) ? "row" :"col");
}

void mma_ld_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  size_t size;
  int t, i;
  unsigned smid;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();

  unsigned type = pI->get_type();
  unsigned wmma_type = pI->get_wmma_type();
  unsigned wmma_layout = pI->get_wmma_layout(0);
  int tid;
  int thrd, stride;

  _memory_op_t insn_memory_op = pI->has_memory_read() ? memory_load : memory_store;

    // stride = src2_data.u32;
    memory_space_t space = pI->get_space();
    decode_space(space, src1);
    _memory_space_t space_type = space.get_type();

    // hack to find correct mem space
    if (pI->source().find(".shared.") != std::string::npos) {
      space_type = shared_space;
      space = memory_space_t(space_type);
    }

    memory_space *mem = NULL;
    // addr_t addr = src1_data.u32;

    //addr_t new_addr =
    //    addr + thread_group_offset(thrd, wmma_type, wmma_layout, type, stride) * size / 8;

    addr_t fetch_addr;

    if (wmma_type == LOAD_A) {
        fprintf(fp, "wmma_ld_a");
        // mem->read(new_addr+2*(i%4)+2*stride*4*(i/4),size/8,&data[i].s64);
    } else if (wmma_type == LOAD_B) {
        fprintf(fp, "wmma_ld_b");
    } else if (wmma_type == LOAD_C) {
        fprintf(fp, "wmma_ld_c");
    } else {
      printf("wrong wmma load type\n");
      abort();
    }

    if (type == F16_TYPE) fprintf(fp, "_f32");
    else if (type == F32_TYPE) fprintf(fp, "_f16");
    else {
      printf("wrong wmma data type\n");
      abort();
    }

  for (int operand_num = 0; operand_num < 3; operand_num++) {
    const operand_info &src = pI->operand_lookup(operand_num);
    if (operand_num != 0) fprintf(fp, ",");
    if (src.is_reg()) {
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, 1).c_str());
    } else if (src.is_memory_operand()) {
      uint32_t size = 2;
      if (space_type == shared_space) {size = 1;}
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, size).c_str());
    } else {
      unsigned nelem = src.get_vect_nelem();
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src, nelem).c_str());
    }
  }

  print_mspace(space, fp);
  fprintf(fp, ",layout:%s", (wmma_layout == ROW) ? "row" :"col");
}

void lg2_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("lg2", finfo, pI, fp);
}

void mad24_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("lg2", finfo, pI, fp);
  unsigned sat_mode = pI->saturation_mode();

  assert(!pI->is_wide());

  fprintf(fp, "sat:%d", sat_mode);
  fprintf(fp, "hilo:%d", pI->is_hi());
}

void mad_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  mad_def(finfo, pI, fp, false);
}

void madp_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  mad_def(finfo, pI, fp, true);
}

void madc_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  mad_def(finfo, pI, fp, true);
}

void mad_def(function_info *finfo, const ptx_instruction *pI, FILE *fp, bool use_carry) {
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();
  const operand_info &src3 = pI->src3();
  ptx_reg_t d, t;

  int carry = 0;
  int overflow = 0;

  unsigned i_type = pI->get_type();
  // ptx_reg_t a = thread->get_operand_value(src1, dst, i_type, thread, 1);
  // ptx_reg_t b = thread->get_operand_value(src2, dst, i_type, thread, 1);
  // ptx_reg_t c = thread->get_operand_value(src3, dst, i_type, thread, 1);

  // take the carry bit, it should be the 4th operand
  ptx_reg_t carry_bit;
  carry_bit.u64 = 0;
  /*
  if (use_carry) {
    const operand_info &carry = pI->operand_lookup(4);
    carry_bit = thread->get_operand_value(carry, dst, PRED_TYPE, thread, 0);
    carry_bit.pred &= 0x4;
    carry_bit.pred >>= 2;
  }
  */

  std::vector<std::string> sufix;
  unsigned rounding_mode = pI->rounding_mode();

  int to_sign, from_sign;
  size_t from_width, to_width;
  unsigned dst_fmt = type_info_key::type_decode(i_type, to_width, to_sign);

  uint32_t size = 1;
  uint32_t dst_size = 1;

  fprintf(fp, "v_mad");
  switch (i_type) {
    case S16_TYPE:
      // t.s32 = a.s16 * b.s16;
      if (pI->is_wide())
        fprintf(fp, "_i32_i16");
        // d.s32 = t.s32 + c.s32 + carry_bit.pred;
      else if (pI->is_hi())
        fprintf(fp, "hi_i16");
        // d.s16 = (t.s32 >> 16) + c.s16 + carry_bit.pred;
      else if (pI->is_lo())
        fprintf(fp, "lo_i16");
        // d.s16 = t.s16 + c.s16 + carry_bit.pred;
      else
        assert(0);
      // carry = ((long long int)(t.s32 + c.s32 + carry_bit.pred) & 0x100000000) >> 32;
      break;
    case S32_TYPE:
      // t.s64 = a.s32 * b.s32;
      if (pI->is_wide()) {
        dst_size = 2;
        fprintf(fp, "_i64_i32");
        // d.s64 = t.s64 + c.s64 + carry_bit.pred;
      } else if (pI->is_hi())
        fprintf(fp, "hi_i32");
        // d.s32 = (t.s64 >> 32) + c.s32 + carry_bit.pred;
      else if (pI->is_lo())
        fprintf(fp, "lo_i32");
        // d.s32 = t.s32 + c.s32 + carry_bit.pred;
      else
        assert(0);
      break;
    case S64_TYPE:
      size = 2;
      dst_size = 2;
      // t.s64 = a.s64 * b.s64;
      assert(!pI->is_wide());
      assert(!pI->is_hi());
      assert(use_carry == false);
      if (pI->is_lo())
        fprintf(fp, "lo_i64");
        // d.s64 = t.s64 + c.s64 + carry_bit.pred;
      else
        assert(0);
      break;
    case U16_TYPE:
      // t.u32 = a.u16 * b.u16;
      if (pI->is_wide())
        fprintf(fp, "_u32_u16");
        // d.u32 = t.u32 + c.u32 + carry_bit.pred;
      else if (pI->is_hi())
        fprintf(fp, "hi_u16");
        // d.u16 = (t.u32 + c.u16 + carry_bit.pred) >> 16;
      else if (pI->is_lo())
        fprintf(fp, "lo_u16");
        // d.u16 = t.u16 + c.u16 + carry_bit.pred;
      else
        assert(0);
      // carry = ((long long int)((long long int)t.u32 + c.u32 + carry_bit.pred) &
      //          0x100000000) >>
      //         32;
      break;
    case U32_TYPE:
      // t.u64 = a.u32 * b.u32;
      if (pI->is_wide()) {
        dst_size = 2;
        fprintf(fp, "_u64_u32");
        // d.u64 = t.u64 + c.u64 + carry_bit.pred;
      } else if (pI->is_hi())
        fprintf(fp, "hi_u32");
        // d.u32 = (t.u64 + c.u32 + carry_bit.pred) >> 32;
      else if (pI->is_lo())
        fprintf(fp, "lo_u32");
        // d.u32 = t.u32 + c.u32 + carry_bit.pred;
      else
        assert(0);
      break;
    case U64_TYPE:
      size = 2;
      dst_size = 2;
      // t.u64 = a.u64 * b.u64;
      assert(!pI->is_wide());
      assert(!pI->is_hi());
      assert(use_carry == false);
      if (pI->is_lo())
        fprintf(fp, "u64_u64");
        // d.u64 = t.u64 + c.u64 + carry_bit.pred;
      else
        assert(0);
      break;
    case F16_TYPE: {
      // assert(0);
      // break;
      fprintf(fp, "_f16");
      assert(use_carry == false);
      // int orig_rm = fegetround();
      switch (rounding_mode) {
        case RN_OPTION:
          sufix.push_back("rm:rn");
          break;
        case RZ_OPTION:
          sufix.push_back("rm:rz");
          // fesetround(FE_TOWARDZERO);
          break;
        default:
          assert(0);
          break;
      }
      // d.f16 = a.f16 * b.f16 + c.f16;
      if (pI->saturation_mode()) {
        sufix.push_back("sat:01");
        /*if (d.f16 < 0)
          d.f16 = 0;
        else if (d.f16 > 1.0f)
          d.f16 = 1.0f;
          */
      }
      // fesetround(orig_rm);
      break;
    }
    case F32_TYPE: {
      assert(use_carry == false);
      fprintf(fp, "_f32");
      // int orig_rm = fegetround();
      switch (rounding_mode) {
        case RN_OPTION:
          sufix.push_back("rm:rn");
          break;
        case RZ_OPTION:
          sufix.push_back("rm:rz");
          // fesetround(FE_TOWARDZERO);
          break;
        default:
          assert(0);
          break;
      }
      // d.f32 = a.f32 * b.f32 + c.f32;
      if (pI->saturation_mode()) {
        sufix.push_back("sat:01");
        /*if (d.f32 < 0)
          d.f32 = 0;
        else if (d.f32 > 1.0f)
          d.f32 = 1.0f;
          */
      }
      // fesetround(orig_rm);
      break;
    }
    case F64_TYPE:
    case FF64_TYPE: {
      size = 2;
      dst_size = 2;
      assert(use_carry == false);
      // int orig_rm = fegetround();
      fprintf(fp, "_f64");
      switch (rounding_mode) {
        case RN_OPTION:
          sufix.push_back("rm:rn");
          break;
        case RZ_OPTION:
          sufix.push_back("rm:rz");
          // fesetround(FE_TOWARDZERO);
          break;
        default:
          assert(0);
          break;
      }
      // d.f64 = a.f64 * b.f64 + c.f64;
      if (pI->saturation_mode()) {
        sufix.push_back("sat:01");
        /*if (d.f64 < 0)
          d.f64 = 0;
        else if (d.f64 > 1.0f)
          d.f64 = 1.0;
          */
      }
      // fesetround(orig_rm);
      break;
    }
    default:
      assert(0);
      break;
  }
  fprintf(fp, "\t");
  print_src(finfo, dst, fp, dst_size);
  fprintf(fp, ",\t");
  print_src(finfo, src1, fp);
  fprintf(fp, ",\t");
  print_src(finfo, src2, fp);
  fprintf(fp, ",\t");
  print_src(finfo, src3, fp);
  // thread->set_operand_value(dst, d, i_type, thread, pI, overflow, carry);
  for (int i= 0 ; i < sufix.size(); i++) {
    if (i == 0) {
        fprintf(fp, " %% ");
    } else {
        fprintf(fp, ",");
    }
    fprintf(fp, " %s ", sufix[i].c_str());
  }

}

void max_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t a, b, d;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();

  unsigned i_type = pI->get_type();
  // a = thread->get_operand_value(src1, dst, i_type, thread, 1);
  // b = thread->get_operand_value(src2, dst, i_type, thread, 1);

  switch (i_type) {
    case U16_TYPE:
      fprintf(fp, "v_max_u16\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.u16 = MY_MAX_I(a.u16, b.u16);
      break;
    case U32_TYPE:
      fprintf(fp, "v_max_u32\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.u32 = MY_MAX_I(a.u32, b.u32);
      break;
    case U64_TYPE:
      fprintf(fp, "v_max_u32\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1, 2).c_str(), finfo->get_coasm_reg(src2, 2).c_str());
      // d.u64 = MY_MAX_I(a.u64, b.u64);
      break;
    case S16_TYPE:
      fprintf(fp, "v_max_i16\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.s16 = MY_MAX_I(a.s16, b.s16);
      break;
    case S32_TYPE:
      fprintf(fp, "v_max_i32\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.s32 = MY_MAX_I(a.s32, b.s32);
      break;
    case S64_TYPE:
      fprintf(fp, "v_max_i64\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1, 2).c_str(), finfo->get_coasm_reg(src2, 2).c_str());
      // d.s64 = MY_MAX_I(a.s64, b.s64);
      break;
    case F32_TYPE:
      fprintf(fp, "v_max_f32\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.f32 = MY_MAX_F(a.f32, b.f32);
      break;
    case F64_TYPE:
    case FF64_TYPE:
      fprintf(fp, "v_max_f64\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1, 2).c_str(), finfo->get_coasm_reg(src2, 2).c_str());
      // d.f64 = MY_MAX_F(a.f64, b.f64);
      break;
    default:
      printf("Execution error: type mismatch with instruction\n");
      assert(0);
      break;
  }

  // thread->set_operand_value(dst, d, i_type, thread, pI);
}

void membar_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  // handled by timing simulator
}

void min_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t a, b, d;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();

  unsigned i_type = pI->get_type();
  // a = thread->get_operand_value(src1, dst, i_type, thread, 1);
  // b = thread->get_operand_value(src2, dst, i_type, thread, 1);

  switch (i_type) {
    case U16_TYPE:
      fprintf(fp, "v_min_u16\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.u16 = MY_MIN_I(a.u16, b.u16);
      break;
    case U32_TYPE:
      fprintf(fp, "v_min_u32\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.u32 = MY_MIN_I(a.u32, b.u32);
      break;
    case U64_TYPE:
      fprintf(fp, "v_min_u64\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1, 2).c_str(), finfo->get_coasm_reg(src2, 2).c_str());
      // d.u64 = MY_MIN_I(a.u64, b.u64);
      break;
    case S16_TYPE:
      fprintf(fp, "v_min_i16\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.s16 = MY_MIN_I(a.s16, b.s16);
      break;
    case S32_TYPE:
      fprintf(fp, "v_min_i32\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.s32 = MY_MIN_I(a.s32, b.s32);
      break;
    case S64_TYPE:
      fprintf(fp, "v_min_i64\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.s64 = MY_MIN_I(a.s64, b.s64);
      break;
    case F32_TYPE:
      fprintf(fp, "v_min_f32\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str(), finfo->get_coasm_reg(src2).c_str());
      // d.f32 = MY_MIN_F(a.f32, b.f32);
      break;
    case F64_TYPE:
    case FF64_TYPE:
      fprintf(fp, "v_min_f64\t%s,\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1, 2).c_str(), finfo->get_coasm_reg(src2, 2).c_str());
      // d.f64 = MY_MIN_F(a.f64, b.f64);
      break;
    default:
      printf("Execution error: type mismatch with instruction\n");
      assert(0);
      break;
  }

  // thread->set_operand_value(dst, d, i_type, thread, pI);
}


void mov_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE* fp)  {
  ptx_reg_t data;

  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  unsigned i_type = pI->get_type();
  assert(src1.is_param_local() == 0);

  if ((src1.is_vector() || dst.is_vector()) && (i_type != BB64_TYPE) &&
      (i_type != BB128_TYPE) && (i_type != FF64_TYPE)) {
    // pack or unpack operation
    unsigned nbits_to_move;
    ptx_reg_t tmp_bits;

    switch (pI->get_type()) {
      case B16_TYPE:
        nbits_to_move = 16;
        break;
      case B32_TYPE:
        nbits_to_move = 32;
        break;
      case B64_TYPE:
        nbits_to_move = 64;
        break;
      default:
        printf(
            "Execution error: mov pack/unpack with unsupported type "
            "qualifier\n");
        assert(0);
        break;
    }
    fprintf(fp, "v_mov_b%u", nbits_to_move);

    if (dst.is_vector()) {
      unsigned nelem = dst.get_vect_nelem();
      unsigned dst_reg_num = dst.reg_num();
      // fprintf(fp, "\tv[%u:%u],", dst_reg_num, dst_reg_num + nelem);
      fprintf(fp, "\t%s,", finfo->get_coasm_reg(dst, nelem).c_str());
    } else {
      // fprintf(fp, "\tv%u,", dst.reg_num());
      fprintf(fp, "\t%s,", finfo->get_coasm_reg(dst).c_str());
    }

    if (src1.is_vector()) {
      unsigned nelem = src1.get_vect_nelem();
      unsigned src1_reg_num = src1.reg_num();
      // fprintf(fp, "\tv[%u:%u]", src1_reg_num, src1_reg_num + nelem);
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src1, nelem).c_str());

    } else {
      // fprintf(fp, "\tv%u", src1.reg_num());
      fprintf(fp, "\t%s", finfo->get_coasm_reg(src1).c_str());
    }

  } else if (i_type == PRED_TYPE and src1.is_literal() == true) {
    // in ptx, literal input translate to predicate as 0 = false and 1 = true
    // we have adopted the opposite to simplify implementation of zero flags in
    // ptxplus
    fprintf(fp, "v_mov_b32\t%s, %s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str());
  } else {
    fprintf(fp, "v_mov_b32\t%s", finfo->get_coasm_reg(dst).c_str());
    if (src1.is_builtin()) {
        int buildin_id = src1.get_int();
        int dim = src1.get_addr_offset();
        if (finfo->get_coasm_buildin(buildin_id, dim).second >= 0) {
            fprintf(fp, ",\tv%d", finfo->get_coasm_buildin(buildin_id, dim).second);
        } else {
            fprintf(fp, ",\t%s", finfo->get_coasm_buildin(buildin_id, dim).first.c_str());
        }
    } else if (src1.is_literal()) {
        fprintf(fp, ",\t0x%x", src1.get_literal_value());
    } else if (src1.is_reg()) {
        fprintf(fp, ",\t%s", finfo->get_coasm_reg(src1).c_str());
    } else if (src1.is_shared()) {
        fprintf(fp, ",\t%s", src1.get_symbol()->name().c_str());
    } else {
        // FIXME
        fprintf(fp, ",\tFIXME\n");
        // fprintf(fp, ",\t%s", src1.get_name().c_str());
    }
  }
}

void mul24_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("mul24", finfo, pI, fp);

  fprintf(fp, " hilo:%d", pI->is_hi());
}

void mul_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t data;

  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();
  ptx_reg_t d, t;

  std::vector<std::string> sufix;

  unsigned i_type = pI->get_type();
  // ptx_reg_t a = thread->get_operand_value(src1, dst, i_type, thread, 1);
  // ptx_reg_t b = thread->get_operand_value(src2, dst, i_type, thread, 1);

  unsigned rounding_mode = pI->rounding_mode();

  unsigned size = 1;
  unsigned dst_size = 1;
  switch (i_type) {
    case S16_TYPE:
      // t.s32 = ((int)a.s16) * ((int)b.s16);
      if (pI->is_wide()) {
        fprintf(fp, "v_mul_i32_i16");
        dst_size = 2;
        // d.s32 = t.s32;
      } else if (pI->is_hi())
        fprintf(fp, "v_mulhi_i16");
        // d.s16 = (t.s32 >> 16);
      else if (pI->is_lo())
        fprintf(fp, "v_mullo_i16");
        // d.s16 = t.s16;
      else
        assert(0);
      break;
    case S32_TYPE:
      // t.s64 = ((long long)a.s32) * ((long long)b.s32);
      if (pI->is_wide()) {
        fprintf(fp, "v_mul_i64_i32");
        dst_size = 2;
        // d.s64 = t.s64;
      } else if (pI->is_hi())
        fprintf(fp, "v_mulhi_i32");
        // d.s32 = (t.s64 >> 32);
      else if (pI->is_lo())
        fprintf(fp, "v_mullo_i32");
        // d.s32 = t.s32;
      else
        assert(0);
      break;
    case S64_TYPE:
      size = 2;
      // t.s64 = a.s64 * b.s64;
      assert(!pI->is_wide());
      assert(!pI->is_hi());
      if (pI->is_lo())
        fprintf(fp, "v_mullo_i64");
        // d.s64 = t.s64;
      else
        assert(0);
      break;
    case U16_TYPE:
      // t.u32 = ((unsigned)a.u16) * ((unsigned)b.u16);
      if (pI->is_wide())
        fprintf(fp, "v_mul_u32_u16");
        // d.u32 = t.u32;
      else if (pI->is_lo())
        fprintf(fp, "v_mullo_u16");
        // d.u16 = t.u16;
      else if (pI->is_hi())
        fprintf(fp, "v_mulhi_u16");
        // d.u16 = (t.u32 >> 16);
      else
        assert(0);
      break;
    case U32_TYPE:
      // t.u64 = ((unsigned long long)a.u32) * ((unsigned long long)b.u32);
      if (pI->is_wide()) {
        fprintf(fp, "v_mul_u64_u32");
        dst_size = 2;
        // d.u64 = t.u64;
      }else if (pI->is_lo())
        fprintf(fp, "v_mullo_u32");
        // d.u32 = t.u32;
      else if (pI->is_hi())
        fprintf(fp, "v_mulhi_u32");
        // d.u32 = (t.u64 >> 32);
      else
        assert(0);
      break;
    case U64_TYPE:
      size = 2;
      // t.u64 = a.u64 * b.u64;
      assert(!pI->is_wide());
      assert(!pI->is_hi());
      if (pI->is_lo())
        fprintf(fp, "v_mullo_u64");
        // d.u64 = t.u64;
      else
        assert(0);
      break;
    case F16_TYPE: {
      // assert(0);
      // break;
      // int orig_rm = fegetround();
      fprintf(fp, "v_mul_f16");
      switch (rounding_mode) {
        case RN_OPTION:
          sufix.push_back("rm:rn");
          // fprintf(fp, "v_mul_f16_rn");
          break;
        case RZ_OPTION:
          //fprintf(fp, "v_mul_f16_rz");
          sufix.push_back("rm:rz");
          // fesetround(FE_TOWARDZERO);
          break;
        default:
          assert(0);
          break;
      }

      // d.f16 = a.f16 * b.f16;

      if (pI->saturation_mode()) {
        //fprintf(fp, "_sat");
        sufix.push_back("sat:01");
        /*if (d.f16 < 0)
          d.f16 = 0;
        else if (d.f16 > 1.0f)
          d.f16 = 1.0f;*/
      }
      // fesetround(orig_rm);
      break;
    }
    case F32_TYPE: {
      // int orig_rm = fegetround();
      fprintf(fp, "v_mul_f32");
      switch (rounding_mode) {
        case RN_OPTION:
          sufix.push_back("rm:rn");
          //fprintf(fp, "v_mul_f32_rn");
          break;
        case RZ_OPTION:
          sufix.push_back("rm:rz");
          //fprintf(fp, "v_mul_f32_rz");
          // fesetround(FE_TOWARDZERO);
          break;
        default:
          assert(0);
          break;
      }

      // d.f32 = a.f32 * b.f32;
      //fprintf(fp, "v_mul_f32_f32");

      if (pI->saturation_mode()) {
        sufix.push_back("sat:01");
        //fprintf(fp, "_sat");
        /*if (d.f32 < 0)
          d.f32 = 0;
        else if (d.f32 > 1.0f)
          d.f32 = 1.0f;*/
      }
      // fesetround(orig_rm);
      break;
    }
    case F64_TYPE:
    case FF64_TYPE: {
      size = 2;
      // int orig_rm = fegetround();
      fprintf(fp, "v_mul_f64");
      switch (rounding_mode) {
        case RN_OPTION:
          sufix.push_back("rm:rn");
          break;
        case RZ_OPTION:
          sufix.push_back("rm:rz");
          // fesetround(FE_TOWARDZERO);
          break;
        default:
          assert(0);
          break;
      }
      // d.f64 = a.f64 * b.f64;
      if (pI->saturation_mode()) {
        sufix.push_back("sat:01");
        /*if (d.f64 < 0)
          d.f64 = 0;
        else if (d.f64 > 1.0f)
          d.f64 = 1.0;*/
      }
      // fesetround(orig_rm);
      break;
    }
    default:
      assert(0);
      break;
  }

  if (dst_size < size) dst_size = size;

  fprintf(fp, "\t"); print_dst(finfo, dst, fp, dst_size);
  fprintf(fp, ",\t"); print_src(finfo, src1, fp, size);
  fprintf(fp, ",\t"); print_src(finfo, src2, fp, size);
  for (int i= 0 ; i < sufix.size(); i++) {
    if (i == 0) {
        fprintf(fp, " %% ");
    } else {
        fprintf(fp, ",");
    }
    fprintf(fp, " %s ", sufix[i].c_str());
  }
  // thread->set_operand_value(dst, d, i_type, thread, pI);
}

void neg_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("neg", finfo, pI, fp);
}

// nandn bitwise negates second operand then bitwise nands with the first
// operand
void nandn_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("nandn", finfo, pI, fp);
}

// norn bitwise negates first operand then bitwise ands with the second operand
void norn_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("norn", finfo, pI, fp);
}

void not_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t a, b, d;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();

  unsigned i_type = pI->get_type();
  // a = thread->get_operand_value(src1, dst, i_type, thread, 1);

  switch (i_type) {
    case PRED_TYPE:
      // fprintf(fp, "v_not_pred\tp%u,\tp%u", dst.reg_num(), src1.reg_num());
      fprintf(fp, "s_not_b32\t%s,\t%s", finfo->get_coasm_tcc(dst).c_str(), finfo->get_coasm_tcc(src1).c_str());
      // d.pred = (~(a.pred) & 0x000F);
      break;
    case B16_TYPE:
      fprintf(fp, "v_not_u16\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str());
      // d.u16 = ~a.u16;
      break;
    case B32_TYPE:
      fprintf(fp, "v_not_u32\t%s,\t%s", finfo->get_coasm_reg(dst).c_str(), finfo->get_coasm_reg(src1).c_str());
      // d.u32 = ~a.u32;
      break;
    case B64_TYPE:
      fprintf(fp, "v_not_u64\t%s,\t%s", finfo->get_coasm_reg(dst, 2).c_str(), finfo->get_coasm_reg(src1, 2).c_str());
      // fprintf(fp, "v_not_u64\tv%u,\tv%u", dst.reg_num(), src1.reg_num());
      // d.u64 = ~a.u64;
      break;
    default:
      printf("Execution error: type mismatch with instruction\n");
      assert(0);
      break;
  }

  // thread->set_operand_value(dst, d, i_type, thread, pI);
}

void or_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("or", finfo, pI, fp);
}

void orn_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("orn", finfo, pI, fp);
}

void pmevent_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void popc_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("norn", finfo, pI, fp);
}

void prefetch_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void prefetchu_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}

void prmt_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("prmt", finfo, pI, fp);

  unsigned mode = pI->prmt_op();
  unsigned i_type = pI->get_type();

  fprintf(fp, " mode:%d", mode);
}

void rcp_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("rcp", finfo, pI, fp);
}

void red_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}

void rem_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("rem", finfo, pI, fp);
}

void ret_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  fprintf(fp, "t_exit\n");
}

// Ptxplus version of ret instruction.
void retp_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  fprintf(fp, "t_exit\n");
}

void rsqrt_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("rsqrt", finfo, pI, fp);
}

#define SAD(d, a, b, c) d = c + ((a < b) ? (b - a) : (a - b))

void sad_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("sad", finfo, pI, fp);
}

void selp_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_3op("csel", finfo, pI, fp);
  fprintf(fp, ",");
  print_tcc(finfo, pI->src3(), fp);

  // predicate value was changed so the lowest bit being set means the zero flag
  // is set. As a result, the value of c.pred must be inverted to get proper
  // behavior
  // d = (!(c.pred & 0x0001)) ? a : b;

}

bool CmpOp(int type, ptx_reg_t a, ptx_reg_t b, unsigned cmpop, FILE* fp) {
  bool t = false;

  switch (type) {
    case B16_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_u16");
          // t = (a.u16 == b.u16);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_u16");
          // t = (a.u16 != b.u16);
          break;
        default:
          assert(0);
      }

    case B32_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_u32");
          // t = (a.u32 == b.u32);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_u32");
          // t = (a.u32 != b.u32);
          break;
        default:
          assert(0);
      }
    case B64_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_u64");
          // t = (a.u64 == b.u64);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_u64");
          // t = (a.u64 != b.u64);
          break;
        default:
          assert(0);
      }
      break;
    case S8_TYPE:
    case S16_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          // t = (a.s16 == b.s16);
          fprintf(fp, "_eq_i16");
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_i16");
          // t = (a.s16 != b.s16);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_i16");
          // t = (a.s16 < b.s16);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_i16");
          // t = (a.s16 <= b.s16);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_i16");
          // t = (a.s16 > b.s16);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_i16");
          // t = (a.s16 >= b.s16);
          break;
        default:
          assert(0);
      }
      break;
    case S32_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_i32");
          // t = (a.s32 == b.s32);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_i32");
          // t = (a.s32 != b.s32);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_i32");
          // t = (a.s32 < b.s32);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_i32");
          // t = (a.s32 <= b.s32);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_i32");
          // t = (a.s32 > b.s32);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_i32");
          // t = (a.s32 >= b.s32);
          break;
        default:
          assert(0);
      }
      break;
    case S64_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_i64");
          // t = (a.s64 == b.s64);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_i64");
          // t = (a.s64 != b.s64);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_i64");
          // t = (a.s64 < b.s64);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_i64");
          // t = (a.s64 <= b.s64);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_i64");
          // t = (a.s64 > b.s64);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_i64");
          // t = (a.s64 >= b.s64);
          break;
        default:
          assert(0);
      }
      break;
    case U8_TYPE:
    case U16_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_ge_u16");
          // t = (a.u16 == b.u16);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_u16");
          // t = (a.u16 != b.u16);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_u16");
          // t = (a.u16 < b.u16);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_u16");
          // t = (a.u16 <= b.u16);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_u16");
          // t = (a.u16 > b.u16);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_u16");
          // t = (a.u16 >= b.u16);
          break;
        case LO_OPTION:
          fprintf(fp, "_lt_u16");
          // t = (a.u16 < b.u16);
          break;
        case LS_OPTION:
          fprintf(fp, "_le_u16");
          // t = (a.u16 <= b.u16);
          break;
        case HI_OPTION:
          fprintf(fp, "_gt_u16");
          // t = (a.u16 > b.u16);
          break;
        case HS_OPTION:
          fprintf(fp, "_ge_u16");
          // t = (a.u16 >= b.u16);
          break;
        default:
          assert(0);
      }
      break;
    case U32_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_u32");
          // t = (a.u32 == b.u32);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_u32");
          // t = (a.u32 != b.u32);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_u32");
          // t = (a.u32 < b.u32);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_u32");
          // t = (a.u32 <= b.u32);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_u32");
          // t = (a.u32 > b.u32);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_u32");
          // t = (a.u32 >= b.u32);
          break;
        case LO_OPTION:
          fprintf(fp, "_lt_u32");
          // t = (a.u32 < b.u32);
          break;
        case LS_OPTION:
          fprintf(fp, "_le_u32");
          // t = (a.u32 <= b.u32);
          break;
        case HI_OPTION:
          fprintf(fp, "_gt_u32");
          // t = (a.u32 > b.u32);
          break;
        case HS_OPTION:
          fprintf(fp, "_ge_u32");
          // t = (a.u32 >= b.u32);
          break;
        default:
          assert(0);
      }
      break;
    case U64_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_u64");
          // t = (a.u64 == b.u64);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_u64");
          // t = (a.u64 != b.u64);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_u64");
          // t = (a.u64 < b.u64);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_u64");
          // t = (a.u64 <= b.u64);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_u64");
          // t = (a.u64 > b.u64);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_u64");
          // t = (a.u64 >= b.u64);
          break;
        case LO_OPTION:
          fprintf(fp, "_lt_u64");
          // t = (a.u64 < b.u64);
          break;
        case LS_OPTION:
          fprintf(fp, "_le_u64");
          // t = (a.u64 <= b.u64);
          break;
        case HI_OPTION:
          fprintf(fp, "_gt_u64");
          // t = (a.u64 > b.u64);
          break;
        case HS_OPTION:
          fprintf(fp, "_ge_u64");
          // t = (a.u64 >= b.u64);
          break;
        default:
          assert(0);
      }
      break;
    case F16_TYPE:
      assert(0);
      break;
    case F32_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_f32");
          // t = (a.f32 == b.f32) && !isNaN(a.f32) && !isNaN(b.f32);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_f32");
          // t = (a.f32 != b.f32) && !isNaN(a.f32) && !isNaN(b.f32);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_f32");
          // t = (a.f32 < b.f32) && !isNaN(a.f32) && !isNaN(b.f32);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_f32");
          // t = (a.f32 <= b.f32) && !isNaN(a.f32) && !isNaN(b.f32);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_f32");
          // t = (a.f32 > b.f32) && !isNaN(a.f32) && !isNaN(b.f32);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_f32");
          // t = (a.f32 >= b.f32) && !isNaN(a.f32) && !isNaN(b.f32);
          break;
        case EQU_OPTION:
          fprintf(fp, "_equ_f32");
          // t = (a.f32 == b.f32) || isNaN(a.f32) || isNaN(b.f32);
          break;
        case NEU_OPTION:
          fprintf(fp, "_neu_f32");
          // t = (a.f32 != b.f32) || isNaN(a.f32) || isNaN(b.f32);
          break;
        case LTU_OPTION:
          fprintf(fp, "_ltu_f32");
          // t = (a.f32 < b.f32) || isNaN(a.f32) || isNaN(b.f32);
          break;
        case LEU_OPTION:
          fprintf(fp, "_leu_f32");
          // t = (a.f32 <= b.f32) || isNaN(a.f32) || isNaN(b.f32);
          break;
        case GTU_OPTION:
          fprintf(fp, "_gtu_f32");
          // t = (a.f32 > b.f32) || isNaN(a.f32) || isNaN(b.f32);
          break;
        case GEU_OPTION:
          fprintf(fp, "_geu_f32");
          // t = (a.f32 >= b.f32) || isNaN(a.f32) || isNaN(b.f32);
          break;
        case NUM_OPTION:
          fprintf(fp, "_num_f32");
          // t = !isNaN(a.f32) && !isNaN(b.f32);
          break;
        case NAN_OPTION:
          fprintf(fp, "_nan_f32");
          // t = isNaN(a.f32) || isNaN(b.f32);
          break;
        default:
          assert(0);
      }
      break;
    case F64_TYPE:
    case FF64_TYPE:
      switch (cmpop) {
        case EQ_OPTION:
          fprintf(fp, "_eq_f64");
          // t = (a.f64 == b.f64) && !isNaN(a.f64) && !isNaN(b.f64);
          break;
        case NE_OPTION:
          fprintf(fp, "_ne_f64");
          // t = (a.f64 != b.f64) && !isNaN(a.f64) && !isNaN(b.f64);
          break;
        case LT_OPTION:
          fprintf(fp, "_lt_f64");
          // t = (a.f64 < b.f64) && !isNaN(a.f64) && !isNaN(b.f64);
          break;
        case LE_OPTION:
          fprintf(fp, "_le_f64");
          // t = (a.f64 <= b.f64) && !isNaN(a.f64) && !isNaN(b.f64);
          break;
        case GT_OPTION:
          fprintf(fp, "_gt_f64");
          // t = (a.f64 > b.f64) && !isNaN(a.f64) && !isNaN(b.f64);
          break;
        case GE_OPTION:
          fprintf(fp, "_ge_f64");
          // t = (a.f64 >= b.f64) && !isNaN(a.f64) && !isNaN(b.f64);
          break;
        case EQU_OPTION:
          fprintf(fp, "_equ_f64");
          // t = (a.f64 == b.f64) || isNaN(a.f64) || isNaN(b.f64);
          break;
        case NEU_OPTION:
          fprintf(fp, "_neu_f64");
          // t = (a.f64 != b.f64) || isNaN(a.f64) || isNaN(b.f64);
          break;
        case LTU_OPTION:
          fprintf(fp, "_ltu_f64");
          // t = (a.f64 < b.f64) || isNaN(a.f64) || isNaN(b.f64);
          break;
        case LEU_OPTION:
          fprintf(fp, "_leu_f64");
          // t = (a.f64 <= b.f64) || isNaN(a.f64) || isNaN(b.f64);
          break;
        case GTU_OPTION:
          fprintf(fp, "_gtu_f64");
          // t = (a.f64 > b.f64) || isNaN(a.f64) || isNaN(b.f64);
          break;
        case GEU_OPTION:
          fprintf(fp, "_geu_f64");
          // t = (a.f64 >= b.f64) || isNaN(a.f64) || isNaN(b.f64);
          break;
        case NUM_OPTION:
          fprintf(fp, "_num_f64");
          // t = !isNaN(a.f64) && !isNaN(b.f64);
          break;
        case NAN_OPTION:
          fprintf(fp, "_nan_f64");
          // t = isNaN(a.f64) || isNaN(b.f64);
          break;
        default:
          assert(0);
      }
      break;
    default:
      assert(0);
      break;
  }

  return t;
}

void setp_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t a, b;

  int t = 0;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();

  assert(pI->get_num_operands() <
         4);  // or need to deal with "c" operand / boolOp

  unsigned type = pI->get_type();
  unsigned cmpop = pI->get_cmpop();
  // a = thread->get_operand_value(src1, dst, type, thread, 1);
  // b = thread->get_operand_value(src2, dst, type, thread, 1);

  fprintf(fp, "v_cmp");
  t = CmpOp(type, a, b, cmpop, fp);

  ptx_reg_t data;

  // the way ptxplus handles the zero flag, 1 = false and 0 = true
  /*
  data.pred =
      (t ==
       0);  // inverting predicate since ptxplus uses "1" for a set zero flag
       */

  fprintf(fp, "\t%s", finfo->get_coasm_tcc(dst).c_str()); // , pI->get_pred().name().c_str());
  fprintf(fp, ",\t"); print_src(finfo, src1, fp);
  fprintf(fp, ",\t"); print_src(finfo, src2, fp);
  // thread->set_operand_value(dst, data, PRED_TYPE, thread, pI);
}

void set_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t a, b;

  int t = 0;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();

  assert(pI->get_num_operands() <
         4);  // or need to deal with "c" operand / boolOp

  unsigned src_type = pI->get_type2();
  unsigned cmpop = pI->get_cmpop();

  // a = thread->get_operand_value(src1, dst, src_type, thread, 1);
  // b = thread->get_operand_value(src2, dst, src_type, thread, 1);
  fprintf(fp, "s_set");

  t = CmpOp(src_type, a, b, cmpop, fp);
  fprintf(fp, "\tp%u", dst.reg_num());
  fprintf(fp, "\t,"); print_src(finfo, src1, fp);
  fprintf(fp, "\t,"); print_src(finfo, src2, fp);
  // Take abs of first operand if needed
  /*
  if (pI->is_abs()) {
    switch (src_type) {
      case S16_TYPE:
        a.s16 = my_abs(a.s16);
        break;
      case S32_TYPE:
        a.s32 = my_abs(a.s32);
        break;
      case S64_TYPE:
        a.s64 = my_abs(a.s64);
        break;
      case U16_TYPE:
        a.u16 = a.u16;
        break;
      case U32_TYPE:
        a.u32 = my_abs(a.u32);
        break;
      case U64_TYPE:
        a.u64 = my_abs(a.u64);
        break;
      case F32_TYPE:
        a.f32 = my_abs(a.f32);
        break;
      case F64_TYPE:
      case FF64_TYPE:
        a.f64 = my_abs(a.f64);
        break;
      default:
        printf("Execution error: type mismatch with instruction\n");
        assert(0);
        break;
    }
  }

  ptx_reg_t data;
  if (isFloat(pI->get_type())) {
    data.f32 = (t != 0) ? 1.0f : 0.0f;
  } else {
    data.u32 = (t != 0) ? 0xFFFFFFFF : 0;
  }

  thread->set_operand_value(dst, data, pI->get_type(), thread, pI);
  */
}

void shfl_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("shfl", finfo, pI, fp);
}

void shl_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  ptx_reg_t a, b, d;
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();
  const operand_info &src2 = pI->src2();

  unsigned i_type = pI->get_type();
  // a = thread->get_operand_value(src1, dst, i_type, thread, 1);
  // b = thread->get_operand_value(src2, dst, i_type, thread, 1);
  fprintf(fp, "v_lshl");

  switch (i_type) {
    case B16_TYPE:
    case U16_TYPE:
      fprintf(fp, "_b16\t");
      print_dst(finfo, dst, fp, 1); fprintf(fp, ",\t");
      print_src(finfo, src1, fp, 1); fprintf(fp, ",\t");
      print_src(finfo, src2, fp, 1);
      /*if (b.u16 >= 16)
        d.u16 = 0;
      else
        d.u16 = (unsigned short)((a.u16 << b.u16) & 0xFFFF);
        */
      break;
    case B32_TYPE:
    case U32_TYPE:
      fprintf(fp, "_b32\t");
      print_dst(finfo, dst, fp, 1); fprintf(fp, ",\t");
      print_src(finfo, src1, fp, 1); fprintf(fp, ",\t");
      print_src(finfo, src2, fp, 1);
      /*if (b.u32 >= 32)
        d.u32 = 0;
      else
        d.u32 = (unsigned)((a.u32 << b.u32) & 0xFFFFFFFF);*/
      break;
    case B64_TYPE:
    case U64_TYPE:
      fprintf(fp, "_b64\t");
      print_dst(finfo, dst, fp, 2); fprintf(fp, ",\t");
      print_src(finfo, src1, fp, 2); fprintf(fp, ",\t");
      print_src(finfo, src2, fp, 2);
      /*if (b.u32 >= 64)
        d.u64 = 0;
      else
        d.u64 = (a.u64 << b.u64);*/
      break;
    default:
      printf("Execution error: type mismatch with instruction\n");
      assert(0);
      break;
  }

  // thread->set_operand_value(dst, d, i_type, thread, pI);
}

void shr_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("shr", finfo, pI, fp);
}

void sin_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("sin", finfo, pI, fp);
}

void slct_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("slct", finfo, pI, fp);
}

void sqrt_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("sqrt", finfo, pI, fp);
}

void sst_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}

void ssy_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
  // printf("Execution Warning: unimplemented ssy instruction is treated as a
  // nop\n");
  // TODO: add implementation
}


void st_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  const operand_info &dst = pI->dst();
  const operand_info &src1 = pI->src1();  // may be scalar or vector of regs
  unsigned type = pI->get_type();
  // ptx_reg_t addr_reg = thread->get_operand_value(dst, dst, type, thread, 1);
  // ptx_reg_t data;
  memory_space_t space = pI->get_space();
  unsigned vector_spec = pI->get_vector();

  memory_space *mem = NULL;
  // addr_t addr = addr_reg.u32;

  // decode_space(space, thread, dst, mem, addr);

  size_t size;
  int t;
  type_info_key::type_decode(type, size, t);
  fprintf(fp, "v_store");

  unsigned dsize = dtype_size(type);

  if (dsize == 1)
    fprintf(fp, "_u8");
  else if (dsize == 2)
    fprintf(fp, "_u16");
  else if (dsize == 4)
    fprintf(fp, "_u32");
  else if (dsize == 8)
    fprintf(fp, "_u64");
  else
    assert("ld_exec failed on unknow type");


  if (!vector_spec) {
    fprintf(fp, "\t%s", finfo->get_coasm_reg(src1).c_str());
    // data = thread->get_operand_value(src1, dst, type, thread, 1);
    // mem->write(addr, size / 8, &data.s64, thread, pI);
  } else {
    if (vector_spec == V2_TYPE) {
      fprintf(fp, "x2\t");
      print_src(finfo, src1, fp, 2); fprintf(fp, ",\t");
      // ptx_reg_t *ptx_regs = new ptx_reg_t[2];
      // thread->get_vector_operand_values(src1, ptx_regs, 2);
      // mem->write(addr, size / 8, &ptx_regs[0].s64, thread, pI);
      // mem->write(addr + size / 8, size / 8, &ptx_regs[1].s64, thread, pI);
      // delete[] ptx_regs;
    }
    if (vector_spec == V3_TYPE) {
      fprintf(fp, "x3\t");
      print_src(finfo, src1, fp, 3); fprintf(fp, ",\t");
      // ptx_reg_t *ptx_regs = new ptx_reg_t[3];
      // thread->get_vector_operand_values(src1, ptx_regs, 3);
      // mem->write(addr, size / 8, &ptx_regs[0].s64, thread, pI);
      // mem->write(addr + size / 8, size / 8, &ptx_regs[1].s64, thread, pI);
      // mem->write(addr + 2 * size / 8, size / 8, &ptx_regs[2].s64, thread, pI);
      // delete[] ptx_regs;
    }
    if (vector_spec == V4_TYPE) {
      fprintf(fp, "x4\t");
      print_src(finfo, src1, fp, 4); fprintf(fp, ",\t");
      // ptx_reg_t *ptx_regs = new ptx_reg_t[4];
      // thread->get_vector_operand_values(src1, ptx_regs, 4);
      // mem->write(addr, size / 8, &ptx_regs[0].s64, thread, pI);
      // mem->write(addr + size / 8, size / 8, &ptx_regs[1].s64, thread, pI);
      // mem->write(addr + 2 * size / 8, size / 8, &ptx_regs[2].s64, thread, pI);
      // mem->write(addr + 3 * size / 8, size / 8, &ptx_regs[3].s64, thread, pI);
      // delete[] ptx_regs;
    }
  }

  decode_space(space, dst);
  _memory_space_t space_type = space.get_type();

  uint32_t dst_size = 2;
  if (space_type == shared_space) {
    dst_size = 1;
  }

  if(dst.is_memory_operand()) {
    fprintf(fp, ",\t%s, 0x%x", finfo->get_coasm_reg(dst, dst_size).c_str(), dst.get_addr_offset());
  } else {
    fprintf(fp, "FIXME on st dst operand: %s\n", __FUNCTION__);
  }

  print_mspace(space, fp);
  // thread->m_last_effective_address = addr;
  // thread->m_last_memory_space = space;
}

void sub_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("sub", finfo, pI, fp);
}

void nop_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  // Do nothing
}

void subc_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void suld_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void sured_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void sust_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void suq_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}


void tex_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("tex", finfo, pI, fp);
}

void txq_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void trap_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void vabsdiff_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void vadd_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void vmad_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}

#define VMAX 0
#define VMIN 1
void video_mem_instruction(function_info *finfo, const ptx_instruction *pI, int op_code)
{
  return;
}

void vmax_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
   video_mem_instruction(finfo, pI, VMAX);
}
void vmin_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  video_mem_instruction(finfo, pI, VMIN);
}
void vset_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void vshl_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void vshr_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}
void vsub_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  coasm_not_impl(pI);
}

void vote_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("vote", finfo, pI, fp);

  if (pI->vote_mode() == ptx_instruction::vote_ballot) {
      fprintf(fp, " vote_mode:ballot");
  } else if (pI->vote_mode() == ptx_instruction::vote_any) {
      fprintf(fp, " vote_mode:any");
  } else if (pI->vote_mode() == ptx_instruction::vote_all) {
      fprintf(fp, " vote_mode:all");
  } else if (pI->vote_mode() == ptx_instruction::vote_uni) {
      fprintf(fp, " vote_mode:uni");
  }

  // predicate value was changed so the lowest bit being set means the zero flag
  // is set. As a result, the value of src1_data.pred must be inverted to get
  // proper behavior
}

void activemask_impl_coasm(function_info *finfo,  const ptx_instruction *pI, FILE *fp)
{
  print_type_op("activemask", finfo, pI, fp);
}

void xor_impl_coasm(function_info *finfo, const ptx_instruction *pI, FILE *fp) {
  print_type_op("xor", finfo, pI, fp);
}


}
