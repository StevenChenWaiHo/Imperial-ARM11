#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "core.h"
#include "dis.h"
#include "mask.h"

/*
ldr Load register Rd := (address) Single Data Transfer
str Store register (address) := Rd Single Data Transfer
*/

static char *shift_name(Instr s) {
  switch (s) {
  case 0:
    return "lsl";
  case 1:
    return "lsr";
  case 2:
    return "asr";
  case 3:
    return "ror"; // TODO: ROR or RRX?
  }
  assert(0); // Invariant
}

void dis_sdt(Instr instr, char *cond) {
  Instr amode = sdt_mode_mask(instr);
  Instr ispos = sdt_u_mask(instr);
  Instr isload = sdt_l_mask(instr);
  Instr rd = sdt_rd_mask(instr);
  Instr rn = sdt_rn_mask(instr);
  Instr rm = sdt_rm_mask(instr);
  Instr offset = sdt_offset_mask(instr);

  char *iname = isload ? "ldr" : "str";
  char *sign = ispos ? "" : "-";

  // See "Arm Assembly Language: Fundamentals and Tecniques" by William Hohl

  switch (amode) {
  case 0: // 01 00, Imediate Post indexed p312
    printf("%s%s %s, [%s], #%s%d\n", iname, cond, regname[rd], regname[rn],
           sign, offset);
    break;
  case 1: // 01 01. Immediate offset p306
    if (offset)
      printf("%s%s %s, [%s, #%s%d]\n", iname, cond, regname[rd], regname[rn],
             sign, offset);
    else
      printf("%s%s %s, [%s]\n", iname, cond, regname[rd], regname[rn]);
    break;
  case 2: // 01 10, Register post indexed. p313/314
    if (std_noshift_mask(instr)) {
      printf("TODO");
      exit(-1);
    } else { // 313
      printf("%s%s %s, [%s], %s%s\n", iname, cond, regname[rd], regname[rn],
             sign, regname[rm]);
    }
    break;
  case 3:                          // 01 11 Register offset p307/308
    if (std_noshift_mask(instr)) { // 308

      Instr shift_imm = sdt_shift_imm_mask(instr);
      Instr shift = sdt_shift_mask(instr);
      printf("%s%s %s, [%s, %s%s, %s #%d]\n", iname, cond, regname[rd],
             regname[rn], sign, regname[rm], shift_name(shift), shift_imm);
    } else { // 307
      printf("%s%s %s, [%s, %s%s]\n", iname, cond, regname[rd], regname[rn],
             sign, regname[rm]);
    }
    break;
  default:
    assert(0); // Invariant
  }

  // Page 309 not in spec
}