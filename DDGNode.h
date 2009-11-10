/*
 * DDGNode.h
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDGNODE_H_
#define DDGNODE_H_

#include <set>
#include <utility>
#include "globals.h"
#include "s3inst.h"



class DDGNode
{
public:
	typedef std::set<Register> RegisterSet;
	typedef RegisterSet::iterator RegisterSetIter;
	typedef std::pair<DDGNode*, int> DependentEdge;
	// TODO do we need to ignore duplicates here
	typedef std::set<DependentEdge> DependentSet;

	DDGNode(inst_t instruction, int no);
	virtual ~DDGNode();

	Register getDestReg() { return destReg;}
	DDGNode::RegisterSet& getSrcRegisters() { return srcReg;}
	int getNo() { return instructionNumber; }

	void addFlowDependency(DDGNode *dependent);
	void addAntiDependency(DDGNode *dependent);
	void addOutputDependency(DDGNode *dependent);


private:
	inst_t instruction;
	int instructionNumber;
	int destReg;
	bool rootNode;
	RegisterSet srcReg;
	DependentSet dependentSet;
	int latency;

	DDGNode(DDGNode &);
	void initRegisterInfo();
	int getLatency();
};

#endif /* DDGNODE_H_ */
