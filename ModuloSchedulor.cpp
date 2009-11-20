/*
 * ModuloSchedulor.cpp
 *
 *  Created on: Nov 19, 2009
 *      Author: Yogesh Arora
 */

#include "ModuloSchedulor.h"
#include "PrintUtils.h"

ModuloSchedulor::ModuloSchedulor(int del, int res, unsigned int inst, DDG& d,
		char* blockLabel) :
	delta(del), k(res), noOfInstructions(inst), mrt(del), ddg(d), label(
			blockLabel)
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
		queue.push(ddgNode);
	}

}

ModuloSchedulor::~ModuloSchedulor()
{
	delete[] neverScheduled;
	delete[] schedTime;
}

void ModuloSchedulor::genProlog(int maxIteration)
{
	prolog.reserve(maxIteration * delta);
	for (int i = 0; i < maxIteration; i++)
	{
		for (int j = 0; j < delta; j++)
		{
			prolog.push_back(InstCycle());
			Cycle & cycle = mrt[j];
			for (CycleIter iter = cycle.begin(); iter != cycle.end(); ++iter)
			{
				if (iter->iteration <= i)
				{
					inst_t ddgOnstruction = iter->ddgNode->getInstruction();
					if(ddgOnstruction->op == OP_BR)
					{
						// need to change the branch
						inst_t inst =  new inst_d();
						*inst = *ddgOnstruction;
						inst->ccode=~inst->ccode;
						inst->ops[1].label = new char[strlen(label)+20];
						sprintf(inst->ops[1].label, "%s_EE%d", label, i);
						prolog[i*delta + j].push_back(inst);
					}
					else
					{
						prolog[i*delta + j].push_back(ddgOnstruction);
					}
				}
			}
		}

	}

}

void ModuloSchedulor::genPrologEpilogue()
{
	int maxIteration = 0;
	for (int i = 0; i < noOfInstructions; i++)
	{
		maxIteration = schedTime[i] / delta > maxIteration ? schedTime[i] / delta : maxIteration;
	}

	int branchIterationNo = schedTime[noOfInstructions-1] / delta;
    genProlog(maxIteration);
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

			// change iteration values here
			for (CycleIter iter = lastCycle.begin(); iter != lastCycle.end(); iter++)
			{
				DDGNodeSchedule &sched = *iter;
				sched.iteration++;
				schedTime[sched.ddgNode->getNo()] +=delta;
			}
			mrt[0] = lastCycle;
		}
	}
}

void ModuloSchedulor::print()
{
	fprintf(stdout, "%s:", label);
	printInstruction(prolog);
	printMrt(mrt);
	printf("\n");
}

void ModuloSchedulor::printInstruction(InstructionSched& table)
{
	for (unsigned int i = 0; i < table.size(); i++)
	{
		InstCycle &cycle = table[i];
		for (InstCycleIter iter = cycle.begin(); iter != cycle.end(); iter++)
		{
			PrintUtils::printInstruction(stdout, *iter,
					true);
		}
		if(cycle.size()>0)
			printf("\n");
	}
	printf("\n");
}

void ModuloSchedulor::printMrt(Mrt& table)
{
	fprintf(stdout, "%s_P:", label);
	for (unsigned int i = 0; i < table.size(); i++)
	{
		Cycle &cycle = table[i];
		for (CycleIter iter = cycle.begin(); iter != cycle.end(); iter++)
		{
			if (iter != cycle.begin())
				printf(".");
			DDGNode* ddgNode = iter->ddgNode;
			PrintUtils::printInstruction(stdout, ddgNode->getInstruction(),
					true, "_P");

		}
		if(cycle.size()>0)
			printf("\n");
	}
	printf("\n");
}

bool ModuloSchedulor::iterativeSchedule()
{
	int budget = 3 * noOfInstructions;
	while (!queue.empty() && budget > 0)
	{
		DDGNode* op = queue.top();
		queue.pop();

		int t = calculateEarlyStart(op);
		int minTime = t;
		int maxTime = t + delta - 1;

		int timeSlot = findTimeSlot(op, minTime, maxTime);

		schedule(op, timeSlot);

		budget--;
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
