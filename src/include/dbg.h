#ifndef AEMU_DBG_H
#define AEMU_DBG_H
#include "emu.h"

#define BREAKPOINT_NUMBER 50

void dbg(CpuState *cpu, int total_instr_no, int *instr_to_line_no);
bool terminate(CpuState *cpu, int *breakpoint, int bpt_ptr, bool step);

#endif