/*
 * OperationScheduler.h
 *
 *  Created on: Nov 20, 2009
 *      Author: Yogesh Arora
 */

#ifndef OPERATIONSCHEDULER_H_
#define OPERATIONSCHEDULER_H_

#include "DDG.h"
#include "DDGNode.h"

class OperationScheduler
{
	DDG& ddg;
	int k;
	int* latestUseTime;
	int* completionTime;
	typedef std::list<DDGNode*> Cycle;
	typedef Cycle::iterator CycleIter;
	typedef std::vector<Cycle> Schedule;

public:
	OperationScheduler(DDG& d, int kay);
	~OperationScheduler();

	int schedule();

	void print(FILE* fptr);
};

#endif /* OPERATIONSCHEDULER_H_ */
