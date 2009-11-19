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
		ModuloSchedulor *scheduler = new ModuloSchedulor(delta, k,
				noInstructions, ddg);
		done = scheduler->iterativeSchedule();
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


