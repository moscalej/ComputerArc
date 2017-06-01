/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */
// This document was modify it by
//  Alejandro Moscoso and Yonathan Haimovich
//  for the Hw 3 of Computer Arch 2017 spring
//
#include "dflow_calc.h"

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


