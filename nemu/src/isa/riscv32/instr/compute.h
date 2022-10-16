def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  printf("auipc: 0x%08x\n", s->pc);
  rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(addi) {
  printf("addi: 0x%08x\n", s->pc);
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(jal) {
  printf("jal: 0x%08x\n", s->pc);
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, &s->pc, id_src1->imm);
}

def_EHelper(jalr) {
  printf("jalr: 0x%08x\n", s->pc);
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm);
}