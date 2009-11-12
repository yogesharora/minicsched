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
	maxReg(INVALID_REG), minReg(INVALID_REG)
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
					defInst[srcRegIndex]->addFlowDependency(node);
				}
			}

			// output dependency
			if (nodeDestReg != INVALID_REG && defInst[nodeDestRegIndex] != NULL)
			{
				defInst[nodeDestRegIndex]->addOutputDependency(node);
			}

			// anti dependency
			if (nodeDestReg != INVALID_REG)
			{
				DDGNodeSet &nodeUseInst = useInst[nodeDestRegIndex];
				for (DDGNodeSetIter iter = nodeUseInst.begin(); iter
						!= nodeUseInst.end(); iter++)
				{
					(*iter)->addAntiDependency(node);
				}
			}

			for (DDGNode::RegisterSetIter iter = srcRegisters.begin(); iter
					!= srcRegisters.end(); iter++)
			{
				Register srcReg = *iter;
				int srcRegIndex = srcReg - minReg;
				useInst[srcRegIndex].insert(node);
			}

			if (nodeDestReg != INVALID_REG)
			{
				useInst[nodeDestRegIndex].clear();
				defInst[nodeDestRegIndex] = node;
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
		calcMaxMinRegisters(cur);

		graph.push_back(new DDGNode(cur, ctr));
		cur = cur->next;
		ctr++;
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
		}
	}

	delete[] traversalInfo;
	return maxCycleLength;
}
