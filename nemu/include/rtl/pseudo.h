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

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  rtl_addi(s, dest, rz, -1);
  rtl_slli(s, dest, dest, width * 8 - 1);
  // cheating temporarily
  if (*src1 >> (width * 8 - 1)) {
    rtl_or(s, dest, src1, dest);
  }
  else {
    rtl_xori(s, dest, dest, -1);
    rtl_and(s, dest, src1, dest);
  }
}

static inline def_rtl(sext_bit, rtlreg_t* dest, const rtlreg_t* src1, int bit) {
  // dest <- signext(src1[(bit - 1) .. 0])
  rtl_addi(s, dest, rz, -1);
  rtl_slli(s, dest, dest, bit - 1);
  // cheating temporarily
  if (*src1 >> (bit - 1)) {
    rtl_or(s, dest, src1, dest);
  }
  else {
    rtl_xori(s, dest, dest, -1);
    rtl_and(s, dest, src1, dest);
  }
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  rtl_addi(s, dest, rz, -1);
  rtl_slli(s, dest, dest, width * 8 - 1);
  rtl_xori(s, dest, dest, -1);
  rtl_and(s, dest, src1, dest);
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  rtl_srli(s, dest, src1, width * 8 - 1);
}
#endif
