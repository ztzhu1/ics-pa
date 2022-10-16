def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  printf("auipc: 0x%08x\n", s->pc);
  rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(addi) {
  // printf("%08x\n", vaddr_ifetch(0x80000000, 4));
  // printf("%08x\n", vaddr_ifetch(0x80000004, 4));
  // printf("%08x\n", vaddr_ifetch(0x80000008, 4));
  // printf("%08x\n", vaddr_ifetch(0x8000000c, 4));
  // printf("%08x\n", vaddr_ifetch(0x80000014, 4));

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