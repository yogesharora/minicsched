/*
 * OperationScheduler.cpp
 *
 *  Created on: Nov 20, 2009
 *      Author: Yogesh Arora
 */

#include "OperationScheduler.h"
#include "PrintUtils.h"

OperationScheduler::OperationScheduler(DDG& d, int kay, char *b) :
	ddg(d), k(kay), blockLabel(b)
{
	latestUseTime = new unsigned int[ddg.getNoRegisters()];
	completionTime = new unsigned int[ddg.getNoRegisters()];
	memset(latestUseTime, 0, ddg.getNoRegisters() * sizeof(unsigned int));
	memset(completionTime, 0, ddg.getNoRegisters() * sizeof(unsigned int));
}

OperationScheduler::~OperationScheduler()
{
	delete[] latestUseTime;
	delete[] completionTime;
}

int OperationScheduler::schedule()
{
	const DDG::DDGNodeList& instructions = ddg.getInstructions();
	for (DDG::DDGNodeListConstIter iter = instructions.begin(); iter
			< instructions.end()-1; iter++)
	{
		DDGNode* instruction = *iter;
		unsigned int t = 0;
		int nodeDestReg = instruction->getDestReg();
		int nodeDestRegIndex = nodeDestReg - ddg.getMinRegister();
		t = getEarliestSchedulTime(nodeDestReg, nodeDestRegIndex, instruction);
		scheduleInstruction(nodeDestReg, nodeDestRegIndex, t, instruction);
	}

	// now schedule the branch instruction
	scheduleBranchInstruction();

	return opSched.size();
}

void OperationScheduler::scheduleBranchInstruction()
{
	const DDG::DDGNodeList& instructions = ddg.getInstructions();
	unsigned int t = opSched.size() - 1;
	DDGNode* instruction = *(instructions.end() - 1);

	// need to consider only flow dependency for branch
	const DDGNode::RegisterSet& srcReg = instruction->getSrcRegisters();
	for (DDGNode::RegisterSetConstIter iter = srcReg.begin(); iter
			!= srcReg.end(); iter++)
	{
		int srcRegIndex = *iter - ddg.getMinRegister();
		t = completionTime[srcRegIndex] > t ? completionTime[srcRegIndex] : t;
	}

	scheduleInstruction(-1, -1, t, instruction);
}

int OperationScheduler::getEarliestSchedulTime(int nodeDestReg, int nodeDestRegIndex, DDGNode* instruction)
{
	unsigned int t = 0;
	//antidependency
	if (nodeDestReg != INVALID_REG)
	{
		int antiTime = latestUseTime[nodeDestRegIndex]
				- instruction->getLatency() + 1;
		t = antiTime > 0 ? antiTime : 0;
	}

	// flow dependency
	const DDGNode::RegisterSet& srcReg = instruction->getSrcRegisters();
	for (DDGNode::RegisterSetConstIter iter = srcReg.begin(); iter
			!= srcReg.end(); iter++)
	{
		int srcRegIndex = *iter - ddg.getMinRegister();
		t = completionTime[srcRegIndex] > t ? completionTime[srcRegIndex] : t;
	}

	// output dependency
	if (nodeDestReg != INVALID_REG)
	{
		t = completionTime[nodeDestRegIndex] > t ? completionTime[nodeDestRegIndex]
						: t;
	}
	return t;
}

void OperationScheduler::scheduleInstruction(int nodeDestReg,
		int nodeDestRegIndex, unsigned int t, DDGNode* instruction)
{
	// schedule it
	bool scheduled = false;
	if (t >= opSched.size())
	{
		for (unsigned int i = 0; i <= t; i++)
			opSched.push_back(Cycle());
	}

	while (!scheduled)
	{
		Cycle& cycle = opSched[t];
		if (cycle.size() >= k)
		{
			t = t + 1;
			if (t >= opSched.size())
			{
				opSched.push_back(Cycle());
			}
		}
		else
		{
			cycle.push_back(instruction);
			scheduled = true;
		}
	}

	// set completion times
	if (nodeDestReg != INVALID_REG)
	{
		completionTime[nodeDestRegIndex] = t + instruction->getLatency();
	}
	const DDGNode::RegisterSet& srcReg = instruction->getSrcRegisters();
	for (DDGNode::RegisterSetConstIter iter = srcReg.begin(); iter
			!= srcReg.end(); iter++)
	{
		int srcRegIndex = *iter - ddg.getMinRegister();
		latestUseTime[srcRegIndex] = t > latestUseTime[srcRegIndex] ? t
				: latestUseTime[srcRegIndex];
	}

	instruction->setDepth(t);
}

void OperationScheduler::print(FILE* fptr)
{
	fprintf(fptr,"%s:", blockLabel);
	for (unsigned int i = 0; i < opSched.size(); i++)
	{
		Cycle &cycle = opSched[i];
		for (CycleIter iter = cycle.begin(); iter != cycle.end(); iter++)
		{
			if (iter != cycle.begin())
				fprintf(fptr,".");
			DDGNode* ddgNode = *iter;
			PrintUtils::printInstruction(fptr, ddgNode->getInstruction(), true);

		}
		if(cycle.size()>0)
			fprintf(fptr,"\n");
	}
}
