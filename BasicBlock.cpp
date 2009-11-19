/*
 * BasicBlock.cpp
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#include "BasicBlock.h"
#include "PrintUtils.h"
#include <queue>

using namespace std;

BasicBlock::BasicBlock(inst_t s, inst_t e) :
	start(s), end(e), ddg(start, end), finalSchedule(NULL)
{

}

BasicBlock::~BasicBlock()
{
	delete finalSchedule;
}

void BasicBlock::scheduleBlock(int k)
{
	calculateMII(k);
	int delta = mII;

	int done = false;
	while (!done)
	{
		ModuloSchedulor *scheduler = new ModuloSchedulor(delta, k,
				noInstructions, ddg);
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
	finalSchedule->rotate();
}

void BasicBlock::calculateMII(int k)
{
	noInstructions = ddg.getNoInstructions();
	resMII = noInstructions / k;
	recMII = ddg.getMaxCycleLength();

	mII = recMII > resMII ? recMII : resMII;
}

void BasicBlock::print()
{
	inst_t head = start;
	while (head!=NULL && head!=end)
	{
		PrintUtils::printInstruction(stdout, head);
		head = head->next;
	}

	if(head!=NULL)
		PrintUtils::printInstruction(stdout, head);

	fprintf(stdout, "RecMII %d, ResMII %d, MII, %d\n", recMII, resMII, mII);

	finalSchedule->print();
}
