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

	const DDG::DDGNodeList& instructions = ddg.getInstructions();
	for (DDG::DDGNodeListConstIter iter = instructions.begin(); iter
			!= instructions.end(); iter++)
	{
		queue.push(*iter);
	}

}

ModuloSchedulor::~ModuloSchedulor()
{
	delete[] neverScheduled;
	delete[] schedTime;
}

bool ModuloSchedulor::iterativeSchedule()
{
	while (!queue.empty())
	{
		DDGNode* op = queue.top();
		queue.pop();

		int t = calculateEarlyStart(op);
		int minTime = t;
		int maxTime = t + delta - 1;

		int timeSlot = findTimeSlot(op, minTime, maxTime);

		schedule(op, timeSlot);
	}

	if (queue.empty())
		return true;
	else
		return false;
}

void ModuloSchedulor::schedule(DDGNode* op, int timeSlot)
{
	const DDGNode::SuccessorList& successors = op->getSuccessors();

	for (DDGNode::SuccessorListConstIter iter = successors.begin(); iter
			!= successors.end(); iter++)
	{
		const DDGNode::Edge &successorEdge = *iter;
		int successorInstruction = successorEdge.node->getNo();
		if (schedTime[successorInstruction] != INVALID_SCHEDULE_TIME
				&& timeSlot + calculateDelay(delta, successorEdge)
						> schedTime[successorInstruction])
		{
			unschedule(successorInstruction);
		}
	}

	int moduloTimeSlot = timeSlot % delta;
	if (mrt[moduloTimeSlot].size() >= k)
	{
		Cycle &cycle = mrt[moduloTimeSlot];
		for (CycleIter iter = cycle.begin(); iter != cycle.end(); iter++)
		{
			queue.push(*iter);
		}
		cycle.clear();
	}

	mrt[moduloTimeSlot].push_back(op);
	schedTime[op->getNo()] = timeSlot;
	neverScheduled[op->getNo()] = false;
}

void ModuloSchedulor::unschedule(int instruction)
{
	int oldSchedule = schedTime[instruction] % delta;
	schedTime[instruction] = INVALID_SCHEDULE_TIME;
	Cycle &cycle = mrt[oldSchedule];

	for (CycleIter iter = cycle.begin(); iter != cycle.end(); iter++)
	{
		if ((*iter)->getNo() == instruction)
		{
			cycle.erase(iter);
			queue.push(*iter);
			break;
		}
	}
}

int ModuloSchedulor::findTimeSlot(DDGNode *op, int minTime, int maxTime)
{
	int curTime = minTime;
	int schedSlot = INVALID_SCHEDULE_TIME;
	while (schedSlot == INVALID_SCHEDULE_TIME && curTime <= maxTime)
	{
		if (!resourceConflict(curTime))
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

	if(tmin==INVALID_SCHEDULE_TIME)
		tmin = 0;

	return tmin;
}

int ModuloSchedulor::calculateDelay(int delta, const DDGNode::Edge &edge)
{
	return edge.edgeWeight - delta * edge.iterationDistance;
}
