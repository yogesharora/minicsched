/*
 * DDG.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDG.h"
#include "globals.h"

DDG::DDG(inst_t start, inst_t end) :
	startInstruction(start), endInstruction(end), highestRegister(REGS_MINSIZE),
	lowestRegister(REGS_MINSIZE)
{
	initProgramInfo();
}

DDG::~DDG()
{
}

void DDG::initProgramInfo()
{
	inst_t cur = startInstruction;
	int ctr = 0;
	int maxRegister = REGS_MINSIZE;

	while (cur!=NULL && cur!=endInstruction )
	{
		initRegisterInfo(cur, highestRegister, lowestRegister);
		cur = cur->next;
		ctr++;
	}

	noOfInstructions = ctr;
	noOfRegisters = maxRegister + 1;
}

void DDG::initRegisterInfo(inst_t instruction, int& high,
		int &low)
{
	for (int i = 0; i < 3; i++)
	{
		if (instruction->ops[i].t == op_reg)
		{
			if (instruction->ops[i].reg > high)
				high = instruction->ops[i].reg;
			else if (instruction->ops[i].reg > high)
				low = instruction->ops[i].reg;
		}
	}
}
