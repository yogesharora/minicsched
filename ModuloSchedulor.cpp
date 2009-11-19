/*
 * ModuloSchedulor.cpp
 *
 *  Created on: Nov 19, 2009
 *      Author: Yogesh Arora
 */

#include "ModuloSchedulor.h"

ModuloSchedulor::ModuloSchedulor(int del, int res, unsigned int inst, DDG& d) :
	delta(del), k(res), noOfInstructions(inst), mrt(del), ddg(d)
{
	neverScheduled = new bool[noOfInstructions];
	memset(neverScheduled, true, noOfInstructions * sizeof(bool));

	schedTime = new int[noOfInstructions];
	memset(schedTime, INVALID_SCHEDULE_TIME, noOfInstructions * sizeof(int));
}

ModuloSchedulor::~ModuloSchedulor()
{
	delete[] neverScheduled;
	delete[] schedTime;
}

bool ModuloSchedulor::iterativeSchedule()
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

		int t = calculateEarlyStart(op);
		int minTime = t;
		int maxTime = t + delta - 1;

		int timeSlot = findTimeSlot(op, minTime, maxTime);
	}
}

int ModuloSchedulor::findTimeSlot(DDGNode *op, int minTime, int maxTime)
{
	int curTime = minTime;
	int schedSlot = INVALID_SCHEDULE_TIME;
	while (schedSlot == INVALID_SCHEDULE_TIME && curTime <= maxTime)
	{
		if (resourceConflict(curTime))
			curTime++;
		else
			schedSlot = curTime;
	}

	if (schedSlot == INVALID_SCHEDULE_TIME)
	{
		int instructionNo = op->getNo();
		if (neverScheduled[instructionNo] || (minTime
				> schedTime[instructionNo]))
		{
			schedSlot = minTime;
		}
		else
		{
			schedSlot = schedTime[instructionNo] + 1;
		}
	}

	return schedSlot;
}

bool ModuloSchedulor::resourceConflict(int curTime)
{
	int time = curTime % delta;
	return mrt[time].size() < k;
}

int ModuloSchedulor::calculateEarlyStart(DDGNode *op)
{
	// does operation scheduling
	const DDGNode::PredecessorList& predecessors = op->getPredecessors();

	// TODO check this
	int tmin = schedTime[op->getNo()];

	for (DDGNode::PredecessorListConstIter iter = predecessors.begin(); iter
			!= predecessors.end(); iter++)
	{
		int predecessorInstNo = (*iter).node->getNo();
		if (schedTime[predecessorInstNo] != INVALID_SCHEDULE_TIME)
		{
			int tnew = schedTime[predecessorInstNo] + calculateDelay(delta,
					*iter);

			tmin = tmin > tnew ? tmin : tnew;
		}
	}
	return tmin;
}

int ModuloSchedulor::calculateDelay(int delta, const DDGNode::Edge &edge)
{
	return edge.edgeWeight - delta * edge.iterationDistance;
}
