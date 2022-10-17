#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {
  rtl_addi(s, dest, src1, 0);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  rtl_xori(s, dest, src1, -1);
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  rtl_sub(s, dest, rz, src1);
}

static inline def_rtl(sext_bit, rtlreg_t* dest, const rtlreg_t* src1, int bit) {
  // dest <- signext(src1[(bit - 1) .. 0])
  rtl_li(s, t0, 32);
  rtl_subi(s, t0, t0, bit);
  rtl_sll(s, dest, src1, t0);
  rtl_sra(s, dest, dest, t0);
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  rtl_sext_bit(s, dest, src1, width * 8);
}

static inline def_rtl(zext_bit, rtlreg_t* dest, const rtlreg_t* src1, int bit) {
  // dest <- zeroext(src1[(bit - 1) .. 0])
  rtl_li(s, t0, 32);
  rtl_subi(s, t0, t0, bit);
  rtl_sll(s, dest, src1, t0);
  rtl_srl(s, dest, dest, t0);
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  rtl_zext_bit(s, dest, src1, width * 8);
}

static inline def_rtl(msb_bit, rtlreg_t* dest, const rtlreg_t* src1, int bit) {
  // dest <- src1[bit - 1]
  rtl_li(s, t0, 32);
  rtl_subi(s, t0, t0, bit);
  rtl_sll(s, dest, src1, t0);
  rtl_srli(s, dest, dest, 31);
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  rtl_msb_bit(s, dest, src1, width * 8);
}

static inline def_rtl(slt, rtlreg_t *dest, const rtlreg_t* src1,
const rtlreg_t* src2)  {
  if ((sword_t)*dsrc1 < (sword_t)*dsrc2)
    *ddest = 1;
  else
    *ddest = 0;
  // rtl_sub(s, s0, src1, src2);
  // rtl_msb(s, s0, s0, 4);
  // rtl_mv(s, dest, s0);
}

#endif