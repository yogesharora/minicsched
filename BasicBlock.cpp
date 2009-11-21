/*
 * BasicBlock.cpp
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#include "BasicBlock.h"
#include "PrintUtils.h"
#include <queue>
#include <cmath>

using namespace std;

BasicBlock::BasicBlock(inst_t s, inst_t e) :
	start(s), end(e), ddg(start, end), finalSchedule(NULL),
	opScheduler(NULL), moduloSchedulorUsed(true)
{
	// remove the label of the block
	// and store it
	blockLabel = start->label;
	start->label = NULL;
}

BasicBlock::~BasicBlock()
{
	delete finalSchedule;
	delete opScheduler;
}

void BasicBlock::scheduleBlock(int k)
{
	calculateMII(k);
	int delta = mII;
	opScheduler = new OperationScheduler(ddg, k, blockLabel);

	int opSchedTime = opScheduler->schedule();
	int done = false;
	while (!done && opSchedTime > delta)
	{
		ModuloSchedulor *scheduler = new ModuloSchedulor(delta, k,
				noInstructions, ddg, blockLabel);
		done = scheduler->iterativeSchedule();
		if (done)
		{
			finalSchedule = scheduler;
		}
		else
		{
			delete finalSchedule;
		}
		delta++;
	}

	if (done)
	{
		finalSchedule->rotate();
		finalSchedule->genPrologEpilogue();
	}

	if(opSchedTime <= delta)
	{
		moduloSchedulorUsed = false;
	}
}

void BasicBlock::calculateMII(int k)
{
	noInstructions = ddg.getNoInstructions();
	resMII = (int)ceil((float)noInstructions / (float)k);
	recMII = ddg.getMaxCycleLength();

	mII = recMII > resMII ? recMII : resMII;
}

void BasicBlock::print()
{
	inst_t head = start;
	fprintf(stdout,";basic block start\n");
	fprintf(stdout, "%s:", blockLabel);
	while (head!=NULL && head!=end)
	{
		PrintUtils::printInstruction(stdout, head);
		head = head->next;
	}

	if(head!=NULL)
		PrintUtils::printInstruction(stdout, head);
	fprintf(stdout,";basic block end\n");
}

void BasicBlock::printSchedule(FILE* fptr)
{
	fprintf(fptr,
			";schedule start RecMii:%d, ResMii:%d, Mii:%d, Operation Schedule Size:%d\n",
			recMII, resMII, mII, opScheduler->getScheduleSize());
	if(moduloSchedulorUsed)
	{
		finalSchedule->print(fptr);
	}
	else
	{
		fprintf(fptr,";Operation Schedule is better\n");
		opScheduler->print(fptr);
	}

	fprintf(fptr,";schedule ends\n");
}
