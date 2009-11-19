/*
 * DDG.h
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDG_H_
#define DDG_H_

#include <vector>
#include "s3inst.h"
#include "DDGNode.h"

class DDG
{
public:
	typedef std::vector<DDGNode*> DDGNodeList;
	typedef DDGNodeList::iterator  DDGNodeListIter;
	typedef DDGNodeList::const_iterator  DDGNodeListConstIter;

	DDG(inst_t start, inst_t end);
	int getMaxCycleLength();
	int getNoInstructions() { return noOfInstructions;}
	const DDGNodeList& getInstructions() { return graph; }
	virtual ~DDG();

private:

	struct DDGNodeWithIteration
	{
		DDGNode* ddgNode;
		int iterationNo;

		DDGNodeWithIteration()
		{
			ddgNode = NULL;
			iterationNo = 0;
		}

		DDGNodeWithIteration(const DDGNodeWithIteration &node)
		{
			ddgNode = node.ddgNode;
			iterationNo = node.iterationNo;
		}

		DDGNodeWithIteration(DDGNode* d, int i)
		{
			ddgNode = d;
			iterationNo = i;
		}

		bool operator!=(int* value)
		{
			if (value==NULL && ddgNode != NULL)
				return true;
			else
				return false;
		}
	};

	typedef std::vector<DDGNodeWithIteration> DDGNodeSet;
	typedef DDGNodeSet::iterator DDGNodeSetIter;

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
	DDGNodeWithIteration* defInst;
	DDGNodeSet* useInst;

	void initProgramInfo();
	void calcMaxMinRegisters(inst_t instruction);
	void createDDG();
	void checkCycles(DDGNode* graphRoot, DDGNode* node, TraversalInfoArray infoArray);

	DDG(DDG&);
};

#endif /* DDG_H_ */
