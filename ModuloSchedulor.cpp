/*
 * ModuloSchedulor.cpp
 *
 *  Created on: Nov 19, 2009
 *      Author: Yogesh Arora
 */

#include "ModuloSchedulor.h"
#include "PrintUtils.h"

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
		DDGNode *ddgNode= *iter;
		if(iter == instructions.begin())
		{
			loopLabel = ddgNode->getInstruction()->label;
		}
		queue.push(ddgNode);
	}

}

ModuloSchedulor::~ModuloSchedulor()
{
	delete[] neverScheduled;
	delete[] schedTime;
}

void ModuloSchedulor::rotate()
{
	// branch instruction is last instruction
	// see where is it in schedule
	int branchSchedTime = schedTime[noOfInstructions-1] % delta;
	// find the maximum
	int max = 0;
	for(int i=0;i<noOfInstructions;i++)
	{
		max = schedTime[i] % delta > max ?  schedTime[i] % delta : max;
	}

	if (branchSchedTime < max)
	{
		// TODO take care of labels
		// TODO take care of iteration numbers
		for (int j = 0; j < max-branchSchedTime; j++)
		{
			Cycle lastCycle = mrt[0];
			Cycle tempCycle;
			for (int i = 1; i <= max; i++)
			{
				Cycle tempCycle = mrt[i];
				mrt[i] = lastCycle;
				lastCycle = tempCycle;
			}
			mrt[0] = lastCycle;
		}
	}
}

void ModuloSchedulor::print()
{
	for(int i=0;i<delta;i++)
	{
		Cycle &cycle = mrt[i];
		for (CycleIter iter = cycle.begin(); iter != cycle.end(); iter++)
		{
			DDGNode* ddgNode = iter->ddgNode;
			PrintUtils::printInstruction(stdout, ddgNode->getInstruction(), true);
			printf(".");
		}
		printf("\n");
	}
	printf("\n");
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
			queue.push(iter->ddgNode);
		}
		cycle.clear();
	}

	mrt[moduloTimeSlot].push_back(DDGNodeSchedule(op, timeSlot/delta));
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
		if (iter->ddgNode->getNo() == instruction)
		{
			cycle.erase(iter);
			queue.push(iter->ddgNode);
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
