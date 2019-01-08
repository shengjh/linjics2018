#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  // printf("Old ZF = %x t0 = %x\n",cpu.eflags.ZF, t0);
  //=printf("id_dest->value = %x  id_src = %x\n", id_dest->val, id_src->val);
  rtl_and(&t0, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t0, id_dest->width);
  // printf("New ZF = %x t0 = %x\n",cpu.eflags.ZF, t0);
  rtl_li(&t1,0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t0);
  rtl_update_ZFSF(&t0, id_dest->width);
  rtl_li(&t0,0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_li(&t0,0);
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  print_asm_template2 (xor);
}

make_EHelper(or) {
  rtl_or(&t2, &id_dest->val, &id_src->val);
  // printf("id_dest->val:%x\n", id_dest->val);
  // printf("id_src->val:%x,t2:%d\n", id_src->val, t2);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  rtl_li(&t0,0);
  rtl_set_OF(&t0);
  rtl_set_CF(&t0);
  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest, &id_dest->val);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest, &id_dest->val);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&id_dest->val, &id_dest->val, &id_src->val);
  operand_write(id_dest, &id_dest->val);
  rtl_update_ZFSF(&id_dest->val, id_dest->width);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shr);
}

make_EHelper(rol) {
	rtl_shl(&t0, &id_dest->val, &id_src->val);
	if (decoding.is_operand_size_16) {
		t3 = 0;
		rtl_addi(&t1, &t3, 16);
		rtl_sub(&t1, &t1, &id_src->val);
		rtl_shr(&t2, &id_dest->val, &t1);
	} else {
		t3 = 0;
		rtl_addi(&t1, &t3, 32);
		rtl_sub(&t1, &t1, &id_src->val);
		rtl_shr(&t2, &id_dest->val, &t1);
	}
	rtl_or(&t0, &t0, &t2);
	operand_write(id_dest, &t0);

	// unnecessary to update CF and OF in NEMU
	rtl_update_ZFSF(&t0, id_dest->width);

	print_asm_template2(shl);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_mv(&t0, &id_dest->val);
  rtl_not(&t0,&t0);
  operand_write(id_dest, &t0);
  print_asm_template1(not);
}
