/*
 * DDG.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDG.h"
#include "globals.h"
#include <queue>

using namespace std;

DDG::DDG(inst_t start, inst_t end) :
	startInstruction(start), endInstruction(end), maxCycleLength(-1),
			highestRegister(LOWEST_REGISTER)
{
	initProgramInfo();
	defInst = new DDGNode*[noOfRegisters];
	useInst = new DDGNodeSet[noOfRegisters];

	for (int i = 0; i < noOfRegisters; i++)
	{
		defInst[i] = NULL;
	}

	createDDG();
}

DDG::~DDG()
{
	delete[] defInst;
	delete[] useInst;
}

void DDG::createDDG()
{
	for (int i = 0; i < 2; i++)
	{
		for (DDGNodeListIter iter = graph.begin(); iter != graph.end(); iter++)
		{
			DDGNode *node = *iter;
			int nodeDestReg = node->getDestReg();

			// true dependency check
			DDGNode::RegisterSet srcRegistes = node->getSrcRegisters();
			for (DDGNode::RegisterSetIter iter = srcRegistes.begin(); iter
					!= srcRegistes.end(); iter++)
			{
				Register srcReg = *iter;
				if (defInst[srcReg] != NULL)
				{
					defInst[srcReg]->addFlowDependency(node);
				}
			}

			// output dependency
			if (nodeDestReg != INVALID_REG && defInst[nodeDestReg] != NULL)
			{
				defInst[nodeDestReg]->addOutputDependency(node);
			}

			// anti dependency
			if (nodeDestReg != INVALID_REG)
			{
				DDGNodeSet &nodeUseInst = useInst[nodeDestReg];
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

		graph.push_back(new DDGNode(cur, ctr));
		cur = cur->next;
		ctr++;
	}

	noOfInstructions = ctr;
	noOfRegisters = highestRegister + 1;
}

int DDG::getMaxUsedRegister(inst_t instruction)
{
	int high = LOWEST_REGISTER;
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

int DDG::getMaxCycleLength()
{
	if (maxCycleLength != -1)
		return maxCycleLength;

	// else calculate new length
	// for each node find maximum cycle length which ends at it
	for (DDGNodeListIter iter = graph.begin(); iter != graph.end(); iter++)
	{
		int maxCycleForNode = getMaxCycleLength(*iter);
		if (maxCycleForNode > maxCycleLength)
			maxCycleLength = maxCycleForNode;
	}

	return maxCycleLength;
}

int DDG::getMaxCycleLength(DDGNode* graphRoot)
{
	typedef queue<DDGNode*> CycleQueue;
	CycleQueue queue;

	TraversalInfo *traversalInfo = new TraversalInfo[noOfInstructions];
	int graphRootInstructionNo = graphRoot->getNo();
	int maxCycleLength = -1;
	// put first element in queue
	// its visited will always be false
	queue.push(graphRoot);
	while (!queue.empty())
	{
		DDGNode* queuedNode = queue.front();
		int queuedNodeInstructionNo = queuedNode->getNo();
		queue.pop();

		// queued node is now being visited
		TraversalInfo &queuedNodeInfo = traversalInfo[queuedNode->getNo()];
		queuedNodeInfo.visited=true;

		// visit all dependents of this queue element
		const DDGNode::DependentList& dependents = queuedNode->getDependents();
		for (DDGNode::DependentListConstIter iter = dependents.begin(); iter
				!= dependents.end(); iter++)
		{
			const DDGNode::DependentEdge dependentEdge = *iter;
			DDGNode* dependentNode = dependentEdge.first;
			int edgeLength = dependentEdge.second;
			int dependentNodeInstructionNo = dependentNode->getNo();

			TraversalInfo &dependentNodeInfo = traversalInfo[dependentNodeInstructionNo];

			// if that dependent is equal to graph root node
			if (dependentNodeInstructionNo == graphRootInstructionNo)
			{
				// we have found a cycle with graph root node as header
				int lengthFromGraphRoot = dependentNodeInfo.length + edgeLength;
				if (lengthFromGraphRoot > maxCycleLength)
					maxCycleLength = lengthFromGraphRoot;
			}
			else if (!dependentNodeInfo.visited)
			{
				dependentNodeInfo.length = queuedNodeInfo.length + edgeLength;
				// add it to queue
				queue.push(dependentNode);
			}
			else if (dependentNodeInstructionNo == queuedNodeInstructionNo)
			{
				// self loop
				if (edgeLength > maxCycleLength)
						maxCycleLength = edgeLength;
			}
		}
	}

	delete[] traversalInfo;
	return maxCycleLength;
}
