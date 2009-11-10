/*
 * DDG.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDG.h"
#include "globals.h"

DDG::DDG(inst_t start, inst_t end) :
	startInstruction(start), endInstruction(end), highestRegister(LOWEST_REGISTER)
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

	while (cur != NULL && cur != endInstruction)
	{
		int maxRegister = getMaxUsedRegister(cur);
		if(maxRegister>highestRegister)
			highestRegister = maxRegister;

		graph.push_back(new DDGNode(cur));
		cur = cur->next;
		ctr++;
	}

	noOfInstructions = ctr;
	noOfRegisters = highestRegister + 1;
}

int DDG::getMaxUsedRegister(inst_t instruction)
{
	int high;
	for (int i = 0; i < 3; i++)
	{
		if (instruction->ops[i].t == op_reg)
		{
			if (instruction->ops[i].reg > high)
				high = instruction->ops[i].reg;
		}
	}
	return high;
}
