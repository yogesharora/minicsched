/*
 * PrintUtils.h
 *
 *  Created on: Nov 19, 2009
 *      Author: Yogesh Arora
 */

#ifndef PRINTUTILS_H_
#define PRINTUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include "s3inst.h"

class PrintUtils
{
	static void printConditionCode(FILE *fptr, int ccode);
	static void printOperand(FILE *fptr, struct operand op, char *labelsuffix="");

public:
	static void printInstruction(FILE* fptr, inst_t i, bool cycle=false, char* labelsuffix="");
	static void printInstructionList(FILE* fptr, inst_t i);
};

#endif /* PRINTUTILS_H_ */
