#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(decoding.jmp_eip);

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = decoding.opcode & 0xf;
  rtl_setcc(&t0, cc);
  rtl_li(&t1, 0);
  rtl_jrelop(RELOP_NE, &t0, &t1, decoding.jmp_eip);
  print_asm("j%s %x", get_cc_name(cc), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  rtl_jr(&id_dest->val);
  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  if (id_dest->width == 4)
  {
    t0 = *eip;
    rtl_push(&t0);
    rtl_j(decoding.jmp_eip);
  }
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  // rtl_pop(&decoding.jmp_eip);
  // rtl_j(decoding.jmp_eip);
  // decoding.is_jmp = 1;
  rtl_pop(&t0);
  rtl_j(t0);
  print_asm("ret");
}

make_EHelper(call_rm) {
  //decoding.is_jmp = 1;
  //decoding.jmp_eip = id_dest->val;
  rtl_push(&decoding.seq_eip);
  rtl_j(id_dest->val);
  print_asm("call *%s", id_dest->str);
}
