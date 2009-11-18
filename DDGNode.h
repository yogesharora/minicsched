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

	struct DependentEdge
	{
		DDGNode* dependent;
		int edgeWeight;
		int iterationNo;

		DependentEdge(DDGNode* d, int e, int i)
		{
			dependent = d;
			edgeWeight = e;
			iterationNo = i;
		}
	};

	typedef std::list<DependentEdge> DependentList;
	typedef DependentList::iterator DependentListIter;
	typedef DependentList::const_iterator DependentListConstIter;

	DDGNode(inst_t instruction, int no);
	virtual ~DDGNode();

	Register getDestReg() { return destReg;}
	DDGNode::RegisterSet& getSrcRegisters() { return srcReg;}
	int getNo() { return instructionNumber; }
	const DependentList& getDependents() { return dependents; }

	void addFlowDependency(DDGNode *dependent, int iteration );
	void addAntiDependency(DDGNode *dependent, int iteration);
	void addOutputDependency(DDGNode *dependent, int iteration);


private:
	inst_t instruction;
	int instructionNumber;
	int destReg;
	bool rootNode;
	RegisterSet srcReg;
	DependentList dependents;
	int latency;

	DDGNode(DDGNode &);
	void initRegisterInfo();
	int getLatency();
	void insertEdge(DDGNode* dependent, int edgeWeight, int iteration);
	void printEdges();
};

#endif /* DDGNODE_H_ */
