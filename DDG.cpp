/*
 * DDG.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDG.h"
#include "globals.h"

DDG::DDG(inst_t start, inst_t end) :
	startInstruction(start), endInstruction(end), highestRegister(
			LOWEST_REGISTER)
{
	initProgramInfo();
	graph = new DDGNodeList(noOfInstructions);
	defInst = new DDGNode*[noOfRegisters];
	useInst = new DDGNodeSet[noOfRegisters];

	for (int i = 0; i < noOfRegisters; i++)
	{
		defInst[i]=NULL;
	}

	createDDG();
}

DDG::~DDG()
{
	delete graph;
	delete []defInst;
	delete []useInst;
}

void DDG::createDDG()
{
	for (DDGNodeListIter iter = graph->begin(); iter != graph->end(); iter++)
	{
		DDGNode *node = *iter;
		int nodeDestReg = node->getDestReg();

		// true dependency check
		DDGNode::RegisterSet srcRegistes = node->getSrcRegisters();
		for (DDGNode::RegisterSetIter iter = srcRegistes.begin(); iter
				!= srcRegistes.end(); iter++)
		{
			Register srcReg = *iter;
			if(defInst[srcReg]!=NULL)
			{
				defInst[srcReg]->addFlowDependency(node);
			}
		}

		if(nodeDestReg!=INVALID_REG && defInst[nodeDestReg]!=NULL)
		{
			defInst[nodeDestReg]->addOutputDependency(node);
		}

		if(nodeDestReg!=INVALID_REG)
		{
			DDGNodeSet &nodeUseInst =  useInst[nodeDestReg];
			for (DDGNodeSetIter iter = nodeUseInst.begin(); iter
					!= nodeUseInst.end(); iter++)
			{
				(*iter)->addAntiDependency(node);
			}
		}

		for (DDGNode::RegisterSetIter iter = srcRegistes.begin(); iter
				!= srcRegistes.end(); iter++)
		{
			Register srcReg = *iter;
			useInst[srcReg].insert(node);
		}

		if (nodeDestReg != INVALID_REG)
		{
			useInst[nodeDestReg].clear();
			defInst[nodeDestReg] = node;
		}
	}
}

void DDG::initProgramInfo()
{
	inst_t cur = startInstruction;
	int ctr = 0;

	while (cur != NULL && cur != endInstruction)
	{
		int maxRegister = getMaxUsedRegister(cur);
		if (maxRegister > highestRegister)
			highestRegister = maxRegister;

		graph->push_back(new DDGNode(cur, ctr));
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
