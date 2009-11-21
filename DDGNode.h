/*
 * DDGNode.h
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDGNODE_H_
#define DDGNODE_H_

#include <set>
#include <list>
#include <utility>
#include "globals.h"
#include "s3inst.h"

class DDGNode
{
public:
	typedef std::set<Register> RegisterSet;
	typedef RegisterSet::iterator RegisterSetIter;
	typedef RegisterSet::const_iterator RegisterSetConstIter;

	struct Edge
	{
		DDGNode* node;
		int edgeWeight;
		int iterationDistance;
		int dependentIteration;

		Edge(DDGNode* d, int e, int mi, int di)
		{
			node = d;
			edgeWeight = e;
			iterationDistance = di - mi;
		}
	};

	typedef std::list<Edge> SuccessorList;
	typedef SuccessorList::iterator SuccessorListIter;
	typedef SuccessorList::const_iterator SuccessorListConstIter;

	typedef std::list<Edge> PredecessorList;
	typedef PredecessorList::iterator PredecessorListIter;
	typedef PredecessorList::const_iterator PredecessorListConstIter;

	DDGNode(inst_t instruction, int no);
	virtual ~DDGNode();

	Register getDestReg() { return destReg;}

	const DDGNode::RegisterSet& getSrcRegisters() { return srcReg;}

	int getNo() { return instructionNumber; }

	const SuccessorList& getSuccessors() { return sucessors; }

	const PredecessorList& getPredecessors() { return predecessors; }

	inst_t getInstruction() { return instruction;}

	void addFlowDependency(DDGNode *dependent, int myIteration, int dependentIteration);

	void addAntiDependency(DDGNode *dependent, int myIteration, int dependentIteration);

	void addOutputDependency(DDGNode *dependent, int myIteration, int dependentIteration);

	int getLatency() { return latency; }

	void setDepth(int d) { depth=d;};

	int getDepth() {  return depth; }

private:
	inst_t instruction;
	int instructionNumber;
	int destReg;
	bool rootNode;
	RegisterSet srcReg;
	SuccessorList sucessors;
	PredecessorList predecessors;
	int latency;
	int depth;

	DDGNode(DDGNode &);
	void initRegisterInfo();
	int calcLatency();
	void insertEdge(DDGNode* dependent, int edgeWeight, int myIteration, int dependentIteration);
	void printEdges();
};

#endif /* DDGNODE_H_ */
