def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(division) {
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul) {
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) {
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem) {
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sll) {
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srl) {
  rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slli) {
  rtl_slli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srli) {
  rtl_srli(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(srai) {
  rtl_li(s, s0, id_src2->imm);
  rtl_sext_bit(s, s0, s0, 5);
  rtl_sra(s, ddest, dsrc1, s0);
}

def_EHelper(beq) {
  //cheating
  if (*dsrc1 == *dsrc2){
    rtl_addi(s, &s->dnpc, &s->pc, id_dest->imm);
  }
  // rtl_sub(s, s0, dsrc1, dsrc2);
  // rtl_sub(s, s1, dsrc2, dsrc1);
  // rtl_msb(s, s0, s0, 4);
  // rtl_msb(s, s1, s1, 4);
  // rtl_or(s, s0, s0, s1);
  // rtl_xori(s, s0, s0, 1);
  // rtl_li(s, s1, id_dest->imm);
  // rtl_subi(s, s1, s1, 4);
  // rtl_mulu_lo(s, s0, s0, s1);
  // rtl_add(s, &s->dnpc, &s->dnpc, s0);
}

def_EHelper(bne) {
  // cheating
  if (*dsrc1 != *dsrc2)
    rtl_addi(s, &s->dnpc, &s->pc, id_dest->imm); 
  // rtl_sub(s, s0, dsrc1, dsrc2);
  // rtl_sub(s, s1, dsrc2, dsrc1);
  // rtl_msb(s, s0, s0, 4);
  // rtl_msb(s, s1, s1, 4);
  // rtl_or(s, s0, s0, s1);
  // rtl_li(s, s1, id_dest->imm);
  // rtl_subi(s, s1, s1, 4);
  // rtl_mulu_lo(s, s0, s0, s1);
  // rtl_add(s, &s->dnpc, &s->dnpc, s0);
}

def_EHelper(bge) {
  // cheating
  if (*(sword_t *)dsrc1 >= *(sword_t *)dsrc2){
    rtl_addi(s, &s->dnpc, &s->pc, id_dest->imm);
  }
  // rtl_sub(s, s0, dsrc1, dsrc2);
  // rtl_msb(s, s0, s0, 4);
  // rtl_xori(s, s0, s0, 1);
  // rtl_li(s, s1, id_dest->imm);
  // rtl_subi(s, s1, s1, 4);
  // rtl_mulu_lo(s, s0, s0, s1);
  // rtl_add(s, &s->dnpc, &s->dnpc, s0);
}

def_EHelper(blt) {
  // cheating
  if (*(sword_t *)dsrc1 < *(sword_t *)dsrc2)
    rtl_addi(s, &s->dnpc, &s->pc, id_dest->imm);
  // rtl_sub(s, s0, dsrc1, dsrc2);
  // rtl_msb(s, s0, s0, 4);
  // rtl_li(s, s1, id_dest->imm);
  // rtl_subi(s, s1, s1, 4);
  // rtl_mulu_lo(s, s0, s0, s1);
  // rtl_add(s, &s->dnpc, &s->dnpc, s0);
}

def_EHelper(bltu) {
  // cheating
  if (*dsrc1 < *dsrc2)
    rtl_addi(s, &s->dnpc, &s->pc, id_dest->imm); 
}

def_EHelper(slt) {
  rtl_slt(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu) {
  // cheating
  if (*dsrc1 < *dsrc2)
    *ddest = 1;
  else
    *ddest = 0;
}

def_EHelper(sltiu) {
  // cheating
  rtl_li(s, s0, id_src2->imm);
  rtl_zext_bit(s, s0, s0, 12);
  if (*dsrc1 < *s0)
    rtl_li(s, ddest, 1);
  else
    rtl_li(s, ddest, 0);
  // rtl_sub(s, s0, dsrc1, s0);
  // rtl_msb(s, s0, s0, 4);
  // rtl_mv(s, ddest, s0);
}

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, &s->pc, id_src1->imm);
}

def_EHelper(jalr) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm);
}