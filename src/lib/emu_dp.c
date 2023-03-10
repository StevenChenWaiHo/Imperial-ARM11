#include <assert.h>
#ifdef AEMU_TRACE
#include <stdio.h>
#endif

#include "dp.h"
#include "emu.h"
#include "shift.h"
#include "unused.h"

typedef Instr (*EmuDpFn)(Instr, Instr, bool *);

// TODO: Always set carry
// https://edstem.org/us/courses/14690/discussion/1561818:q
static Instr emu_dp_and(Instr rn, Instr op2, UNUSED bool *c) {
  return rn & op2;
}
static Instr emu_dp_eor(Instr rn, Instr op2, UNUSED bool *c) {
  return rn ^ op2;
}
static Instr emu_dp_sub(Instr rn, Instr op2, bool *carry) {
  *carry = !(op2 > rn);
  return rn - op2;
}
static Instr emu_dp_rsb(Instr rn, Instr op2, UNUSED bool *c) {
  return op2 - rn;
}
static Instr emu_dp_add(Instr rn, Instr op2, UNUSED bool *c) {
  return rn + op2;
}
static Instr emu_dp_orr(Instr rn, Instr op2, UNUSED bool *c) {
  return rn | op2;
}
static Instr emu_dp_mov(UNUSED Instr rn, Instr op2, UNUSED bool *c) {
  return op2;
}

static EmuDpFn opcodefn[] = {
    [DP_AND] = emu_dp_and, [DP_EOR] = emu_dp_eor, [DP_SUB] = emu_dp_sub,
    [DP_RSB] = emu_dp_rsb, [DP_ADD] = emu_dp_add, [DP_TST] = emu_dp_and,
    [DP_TEQ] = emu_dp_eor, [DP_CMP] = emu_dp_sub, [DP_ORR] = emu_dp_orr,
    [DP_MOV] = emu_dp_mov};

static bool should_write(Instr i) {
  switch (i) {
  case DP_TST:
  case DP_TEQ:
  case DP_CMP:
    return false;
  default:
    return true;
  }
}

void emu_dp(CpuState *cpu, Instr instr) {
  Instr op = dp_opcode_mask(instr);
  Instr i = dp_i_mask(instr); // Immediate
  Instr s = dp_s_mask(instr); // Set Flags
  Instr rn = dp_rn_mask(instr);
  Instr rd = dp_rd_mask(instr);

  Instr op2;
  if (i) {
    op2 = operand_immediate(dp_operand2_mask(instr));
  } else {
    Instr rm = dp_operand2_rm_mask(instr);
    op2 = cpu->regs[rm];
    Instr sty = dp_operand2_shift_type_mask(instr);
    Instr sby;
    if (dp_operand2_shift_ind_mask(
            instr)) // Shift specified by a register(optional)
      sby = cpu->regs[dp_operand2_rotate_mask(instr)];
    else // Shift by a constant amount
      sby = sdt_shift_imm_mask(instr);
    op2 = emu_do_shift(op2, sby, sty);
  }

  EmuDpFn fn = opcodefn[op];
  bool carry = false;
  Instr r = fn(cpu->regs[rn], op2, &carry);

  if (should_write(op))
    cpu->regs[rd] = r;

  if (s) {
#ifdef AEMU_TRACE
    printf("Update flags\n");
#endif
    Instr cpsr = cpu->regs[REG_CPSR];
    cpsr = set_cpsr_z(cpsr, r == 0);
    cpsr = set_cpsr_n(cpsr, dp_bit_31_mask(r));
    cpsr = set_cpsr_c(cpsr, carry);
    cpu->regs[REG_CPSR] = cpsr;
  }

  //   Instr z = r == 0;
  //   Instr n = dp_bit_31_mask(r);
  // TODO: Update flags
}
