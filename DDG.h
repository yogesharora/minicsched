/*
 * DDG.h
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDG_H_
#define DDG_H_

#include <vector>
#include <set>
#include "s3inst.h"
#include "DDGNode.h"

class DDG
{
public:
	typedef std::vector<DDGNode*> DDGNodeList;
	typedef DDGNodeList::iterator  DDGNodeListIter;
	typedef std::set<DDGNode*> DDGNodeSet;
	typedef DDGNodeSet::iterator DDGNodeSetIter;

	DDG(inst_t start, inst_t end);
	int getMaxCycleLength();
	virtual ~DDG();

private:

	struct TraversalInfo
	{
		bool visited;
		int length;

		TraversalInfo()
		{
			visited = false;
			length = 0;
		}
	};

	inst_t startInstruction;
	inst_t endInstruction;
	int noOfInstructions;
	int noOfRegisters;
	int maxCycleLength;
	Register highestRegister;
	DDGNodeList graph;
	DDGNode** defInst;
	DDGNodeSet* useInst;

	void initProgramInfo();
	int getMaxUsedRegister(inst_t instruction);
	void createDDG();
	int getMaxCycleLength(DDGNode* node);

	DDG(DDG&);
};

#endif /* DDG_H_ */
