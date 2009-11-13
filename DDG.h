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
	int getNoInstructions() { return noOfInstructions;}
	virtual ~DDG();

private:

	struct TraversalInfo
	{
		bool visited;
		int pathLength;

		TraversalInfo()
		{
			visited = false;
			pathLength = 0;
		}
	};

	struct TraversalInfoArray
	{
		TraversalInfo *traversalInfo;
		int length;

		TraversalInfoArray(int arrayLength) : traversalInfo(NULL), length(arrayLength)
		{
			traversalInfo = new TraversalInfo[arrayLength];
		}

		TraversalInfoArray(TraversalInfoArray& source)
		{
			traversalInfo = new TraversalInfo[source.length];
			length = source.length;

			for (int i = 0; i < length; ++i)
			{
				traversalInfo[i] = source.traversalInfo[i];
			}
		}

		~TraversalInfoArray()
		{
			delete []traversalInfo;
		}
	};


	inst_t startInstruction;
	inst_t endInstruction;
	int noOfInstructions;
	int noOfRegisters;
	int maxCycleLength;
	Register maxReg;
	Register minReg;
	DDGNodeList graph;
	DDGNode** defInst;
	DDGNodeSet* useInst;

	void initProgramInfo();
	void calcMaxMinRegisters(inst_t instruction);
	void createDDG();
	void checkCycles(DDGNode* graphRoot, DDGNode* node, TraversalInfoArray infoArray);

	DDG(DDG&);
};

#endif /* DDG_H_ */
