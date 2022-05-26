#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "mask.h"

// Disaseble Data Branch

int funcCounter = 1;

void dis_br(Instr i, char *cond) {
  Instr offset = dp_branch_mask(i);
  offset <<= 2;
  if ((offset >> 23) & 0x1) {
    offset |= 0xFC000000;
  }

  // branch command
  printf("b%s #%d\n", cond, offset + 8);
}