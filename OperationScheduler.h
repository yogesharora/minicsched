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
	unsigned int k;
	unsigned int* latestUseTime;
	unsigned int* completionTime;
	char *blockLabel;
	typedef std::list<DDGNode*> Cycle;
	typedef Cycle::iterator CycleIter;
	typedef std::vector<Cycle> Schedule;

	Schedule opSched;
	int getEarliestSchedulTime(int nodeDestReg, int nodeDestRegIndex,
			DDGNode* instruction);
	void scheduleInstruction(int nodeDestReg, int nodeDestRegIndex,
			unsigned int t, DDGNode* instruction);
	void scheduleBranchInstruction();

public:
	OperationScheduler(DDG& d, int kay, char* blockLabel);
	~OperationScheduler();

	int schedule();
	int getScheduleSize() { return opSched.size(); }
	void print(FILE* fptr);
};

#endif /* OPERATIONSCHEDULER_H_ */
