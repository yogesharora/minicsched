/*
 * BasicBlock.h
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#ifndef BASICBLOCK_H_
#define BASICBLOCK_H_

#include "s3inst.h"
#include "DDG.h"
#include "DDGNode.h"
#include <queue>
#include <vector>
#include <list>

class BasicBlock
{
public:

	struct Schedule
	{

		bool *neverScheduled;
		int* schedTime;
		int delta;
		unsigned int k;
		int noOfInstructions;
		std::vector<std::list<DDGNode *> >mrt;

		Schedule(int del, int res, unsigned int inst):
			delta(del), k(res), noOfInstructions(inst), mrt(del)
		{
			neverScheduled = new bool[noOfInstructions];
			memset(neverScheduled, true, noOfInstructions*sizeof(bool));

			schedTime = new int[noOfInstructions];
			memset(schedTime, INVALID_SCHEDULE_TIME, noOfInstructions*sizeof(int));
		}

		Schedule::~Schedule()
		{
			delete[] neverScheduled;
			delete[] schedTime;
		}
	};

private:

	struct DDGComp
	{

		bool operator () (DDGNode* a, DDGNode* b)
		{
			return a->getNo() < b->getNo();
		}
	};

	typedef std::priority_queue<DDGNode*, std::vector<DDGNode*>, DDGComp>
			InstructionQueue;

	int resMII;
	int recMII;
	int mII;
	DDG ddg;
	int noInstructions;
	Schedule *schedule;

	void calculateMII(int k);
	bool iterativeSchedule(Schedule *schedule);
	int calculateEarlyStart(DDGNode *op, Schedule *schedule);
	int calculateDelay(int delta, const DDGNode::Edge &edge);
	int findTimeSlot(DDGNode *op, Schedule *schedule, int minTime, int maxTime);
	bool resourceConflict(int curTime, Schedule *schedule);
public:

	BasicBlock(inst_t start, inst_t end);
	~BasicBlock();

	void scheduleBlock(int k);
};

#endif /* BASICBLOCK_H_ */
