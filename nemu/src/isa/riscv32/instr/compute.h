def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  printf("auipc: 0x%08x\n", s->pc);
  rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(jal) {
  // cheating 
  printf("jal: 0x%08x\n", cpu.pc);
  printf("%08x\n", id_src1->imm);
  if (id_src1->imm >> 19)
    *ddest = id_src1->imm | 0xFFF00000;
  else
    *ddest = id_src1->imm & 0x000FFFFF;
  printf("%08x\n", *ddest);
  for (int j = 0; j < 10; j++)
    printf("%08x\n", vaddr_ifetch(0x80000000+4*j, 4));
  // rtl_add(s, &s->dnpc, &s->pc, ddest);
  // rtl_addi(s, ddest, &s->pc, 4);
}

def_EHelper(addi) {
  printf("addi: 0x%08x\n", s->pc);
  rtl_sext_bit(s, ddest, &id_src2->imm, 12);
  printf("%d, %d\n", id_src2->imm, *ddest);
  rtl_addi(s, ddest, dsrc1, *ddest);
}