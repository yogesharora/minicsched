/*
 * BasicBlock.cpp
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#include "BasicBlock.h"

BasicBlock::BasicBlock(inst_t start, inst_t end) :
	ddg(start, end)
{

}

BasicBlock::~BasicBlock()
{

}

void BasicBlock::calculateMII(int k)
{
	recMII = ddg.getNoInstructions();
	resMII = ddg.getMaxCycleLength();

	printf("recMII %d\n", recMII);
	mII = recMII > resMII ? recMII : resMII;
}
