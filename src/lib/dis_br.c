#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "mask.h"

// Disasseble Data Branch

int funcCounter = 1;

void dis_br(FILE *f, Instr i, const char *cond, int pc) {
  Instr offset = br_offset_mask(i);
#ifdef AEMU_TRACE
  printf("Raw Offset = %d\n", offset);
#endif

  offset <<= 2;
  if ((offset >> 25) & 0x1) {
    offset |= 0xFC000000;
  }

  int dest_addr = pc * 4 + offset + 8;

  // branch command
  fprintf(f, "b%s #0x%x\n", cond, dest_addr);
}
