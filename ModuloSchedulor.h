/*
 * ModuloSchedulor.h
 *
 *  Created on: Nov 19, 2009
 *      Author: Yogesh Arora
 */

#ifndef MODULOSCHEDULOR_H_
#define MODULOSCHEDULOR_H_

#include "DDGNode.h"
#include "DDG.h"
#include <queue>
#include <vector>
#include <list>

class ModuloSchedulor
{
	bool *neverScheduled;
	int* schedTime;
	int delta;
	unsigned int k;
	int noOfInstructions;
	std::vector<std::list<DDGNode *> > mrt;
	DDG& ddg;

	struct DDGComp
	{

		bool operator () (DDGNode* a, DDGNode* b)
		{
			return a->getNo() < b->getNo();
		}
	};

	typedef std::priority_queue<DDGNode*, std::vector<DDGNode*>, DDGComp>
				InstructionQueue;

	int calculateEarlyStart(DDGNode *op);
	int calculateDelay(int delta, const DDGNode::Edge &edge);
	int findTimeSlot(DDGNode *op, int minTime, int maxTime);
	bool resourceConflict(int curTime);

public:
	ModuloSchedulor(int del, int res, unsigned int inst, DDG& d);
	bool iterativeSchedule();
	~ModuloSchedulor();
};

#endif /* MODULOSCHEDULOR_H_ */
