/*
 * BasicBlock.cpp
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#include "BasicBlock.h"
#include <queue>

using namespace std;

BasicBlock::BasicBlock(inst_t start, inst_t end) :
	ddg(start, end)
{

}

BasicBlock::~BasicBlock()
{

}

void BasicBlock::scheduleBlock(int k)
{
	calculateMII(k);
	int delta = mII;

	int done = false;
	while (!done)
	{
		Schedule *schedule = new Schedule(delta, k, noInstructions);
		done = iterativeSchedule(schedule);
		delta++;
	}
}

void BasicBlock::calculateMII(int k)
{
	noInstructions = ddg.getNoInstructions();
	resMII = noInstructions / k;
	recMII = ddg.getMaxCycleLength();

	printf("recMII %d\n", recMII);
	mII = recMII > resMII ? recMII : resMII;
}

bool BasicBlock::iterativeSchedule(Schedule *schedule)
{
	InstructionQueue queue;
	const DDG::DDGNodeList& instructions = ddg.getInstructions();
	for (DDG::DDGNodeListConstIter iter = instructions.begin(); iter
			!= instructions.end(); iter++)
	{
		queue.push(*iter);
	}

	while (!queue.empty())
	{
		DDGNode* op = queue.top();
		queue.pop();

		int t = calculateEarlyStart(op, schedule);
		int minTime = t;
		int maxTime = t + schedule->delta - 1;

		int timeSlot = findTimeSlot(op, schedule, minTime, maxTime);
	}
}

int BasicBlock::findTimeSlot(DDGNode *op, Schedule *schedule,
		int minTime, int maxTime)
{
	int curTime = minTime;
	int schedSlot = INVALID_SCHEDULE_TIME;
	while (schedSlot == INVALID_SCHEDULE_TIME && curTime <= maxTime)
	{
		if(resourceConflict(curTime, schedule))
			curTime++;
		else
			schedSlot = curTime;
	}

	if(schedSlot==INVALID_SCHEDULE_TIME)
	{
		int instructionNo = op->getNo();
		if (schedule->neverScheduled[instructionNo] || (minTime
				> schedule->schedTime[instructionNo]))
		{
			schedSlot = minTime;
		}
		else
		{
			schedSlot = schedule->schedTime[instructionNo] + 1;
		}
	}

	return schedSlot;
}

bool BasicBlock::resourceConflict(int curTime, Schedule *schedule)
{
	int time = curTime % schedule->delta;
	return schedule->mrt[time].size() < schedule->k;
}

int BasicBlock::calculateEarlyStart(DDGNode *op, Schedule *schedule)
{
	// does operation scheduling
	const DDGNode::PredecessorList& predecessors = op->getPredecessors();

	// TODO check this
	int tmin = schedule->schedTime[op->getNo()];

	for (DDGNode::PredecessorListConstIter iter = predecessors.begin(); iter
			!= predecessors.end(); iter++)
	{
		int predecessorInstNo = (*iter).node->getNo();
		if (schedule->schedTime[predecessorInstNo] != INVALID_SCHEDULE_TIME)
		{
			int tnew = schedule->schedTime[predecessorInstNo] +
					calculateDelay(schedule->delta, *iter);

			tmin = tmin > tnew ? tmin : tnew;
		}
	}
	return tmin;
}

int BasicBlock::calculateDelay(int delta, const DDGNode::Edge &edge)
{
	return edge.edgeWeight - delta * edge.iterationDistance;
}
