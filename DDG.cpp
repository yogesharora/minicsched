/*
 * DDG.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDG.h"
#include "globals.h"

using namespace std;

DDG::DDG(inst_t start, inst_t end) :
	startInstruction(start), endInstruction(end), maxCycleLength(-1),
	maxReg(INVALID_REG), minReg(INVALID_REG)
{
	initProgramInfo();
	defInst = new DDGNodeWithIteration[noOfRegisters];
	useInst = new DDGNodeSet[noOfRegisters];

	createDDG();
}

DDG::~DDG()
{
	delete[] defInst;
	delete[] useInst;
	// delete all the DDGNodes
	for (DDGNodeListIter iter = graph.begin(); iter != graph.end(); iter++)
	{
			delete *iter;
	}
}

void DDG::createDDG()
{
	for (int i = 0; i < 2; i++)
	{
		for (DDGNodeListIter iter = graph.begin(); iter != graph.end(); iter++)
		{
			DDGNode *node = *iter;
			Register nodeDestReg = node->getDestReg();
			int nodeDestRegIndex = nodeDestReg - minReg;
			// true dependency check
			DDGNode::RegisterSet srcRegisters = node->getSrcRegisters();
			for (DDGNode::RegisterSetIter iter = srcRegisters.begin(); iter
					!= srcRegisters.end(); iter++)
			{
				Register srcReg = *iter;
				int srcRegIndex = srcReg - minReg;
				if (defInst[srcRegIndex] != NULL)
				{
					defInst[srcRegIndex].ddgNode->addFlowDependency(node,
							defInst[srcRegIndex].iterationNo, i);
				}
			}

			// output dependency
			if (nodeDestReg != INVALID_REG && defInst[nodeDestRegIndex] != NULL)
			{
				defInst[nodeDestRegIndex].ddgNode->addOutputDependency(node,
						defInst[nodeDestRegIndex].iterationNo, i);
			}

			// anti dependency
			if (nodeDestReg != INVALID_REG)
			{
				DDGNodeSet &nodeUseInst = useInst[nodeDestRegIndex];
				for (DDGNodeSetIter iter = nodeUseInst.begin(); iter
						!= nodeUseInst.end(); iter++)
				{
					(*iter).ddgNode->addAntiDependency(node,
							(*iter).iterationNo, i);
				}
			}

			for (DDGNode::RegisterSetIter iter = srcRegisters.begin(); iter
					!= srcRegisters.end(); iter++)
			{
				Register srcReg = *iter;
				int srcRegIndex = srcReg - minReg;
				useInst[srcRegIndex].push_back(DDGNodeWithIteration(node, i));
			}

			if (nodeDestReg != INVALID_REG)
			{
				useInst[nodeDestRegIndex].clear();
				defInst[nodeDestRegIndex] = DDGNodeWithIteration(node, i);
			}
		}
	}
}

void DDG::initProgramInfo()
{
	inst_t cur = startInstruction;
	int ctr = 0;

	while (cur != NULL)
	{
		calcMaxMinRegisters(cur);

		graph.push_back(new DDGNode(cur, ctr));
		ctr++;

		if(cur==endInstruction)
			break;

		cur = cur->next;
	}

	noOfInstructions = ctr;
	noOfRegisters = (maxReg-minReg) + 1;
}

void DDG::calcMaxMinRegisters(inst_t instruction)
{
	if(instruction->op==OP_IN || instruction->op==OP_OUT)
	{
		minReg = R0;
		if(R0 > maxReg)
			maxReg = R0;
	}
	for (int i = 0; i < 3; i++)
	{
		if (instruction->ops[i].t == op_reg)
		{
			if(minReg==INVALID_REG)
				minReg = instruction->ops[i].reg;

			if (instruction->ops[i].reg > maxReg)
				maxReg = instruction->ops[i].reg;
			else if(instruction->ops[i].reg < minReg)
				minReg = instruction->ops[i].reg;
		}
	}
}

int DDG::getMaxCycleLength()
{
	if (maxCycleLength != -1)
		return maxCycleLength;

	// else calculate new length
	// for each node find maximum cycle length which ends at it
	for (DDGNodeListIter iter = graph.begin(); iter != graph.end(); iter++)
	{
		TraversalInfoArray infoArray(noOfInstructions);
		DDGNode *root = *iter;
		checkCycles(root, root, infoArray);
	}

	return maxCycleLength;
}

void DDG::checkCycles(DDGNode* graphRoot, DDGNode* curNode, TraversalInfoArray infoArray)
{
	const DDGNode::SuccessorList& dependents = curNode->getSuccessors();
	int graphRootInstructionNo = graphRoot->getNo();
	int curNodeInstructionNo = curNode->getNo();
	TraversalInfo &curNodeInfo = infoArray.traversalInfo[curNodeInstructionNo];
	curNodeInfo.visited = true;

	for (DDGNode::SuccessorListConstIter iter = dependents.begin(); iter
			!= dependents.end(); iter++)
	{
		const DDGNode::Edge dependentEdge = *iter;
		DDGNode* dependentNode = dependentEdge.node;
		int edgeLength = dependentEdge.edgeWeight;
		int dependentNodeInstructionNo = dependentNode->getNo();
		TraversalInfo &dependentInfo = infoArray.traversalInfo[dependentNodeInstructionNo];

		int lengthFromGraphRoot = curNodeInfo.pathLength + edgeLength;

		if(dependentNodeInstructionNo == graphRootInstructionNo)
		{
			if (lengthFromGraphRoot > maxCycleLength)
			{
				maxCycleLength = lengthFromGraphRoot;
			}
		}
		else if(dependentNodeInstructionNo == curNodeInstructionNo)
		{
			if (edgeLength > maxCycleLength)
			{
				maxCycleLength = edgeLength;
			}
		}
		else if (dependentInfo.visited == false)
		{
			dependentInfo.pathLength = lengthFromGraphRoot;
			checkCycles(graphRoot, dependentNode, infoArray);
		}

	}
}
