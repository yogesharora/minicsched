/*
 * DDG.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDG.h"
#include "globals.h"

DDG::DDG(inst_t inst):
	instructions(inst)
{
	initProgramInfo();
}

DDG::~DDG()
{
	// TODO Auto-generated destructor stub
}

void DDG::initProgramInfo()
{
	inst_t head = instructions;
	int ctr = 0;
	int maxRegister = REGS_MINSIZE;

	while (head)
	{
		int reg = getHighestReg(head);
		if (reg > maxRegister)
			maxRegister = reg;

		ctr++;
	}

	noOfInstructions = ctr;
	noOfRegisters = maxRegister + 1;
}

int DDG::getHighestReg(inst_t instruction)
{
	int highestReg = 0;
	for (int i = 0; i < 3; i++)
	{
		if (instruction->ops[i].t == op_reg)
		{
			if (instruction->ops[i].reg > highestReg)
				highestReg = instruction->ops[i].reg;
		}
	}
	return highestReg;
}
