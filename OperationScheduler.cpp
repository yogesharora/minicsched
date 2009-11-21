/*
 * OperationScheduler.cpp
 *
 *  Created on: Nov 20, 2009
 *      Author: Yogesh Arora
 */

#include "OperationScheduler.h"

OperationScheduler::OperationScheduler(DDG& d, int kay) :
	ddg(d), k(kay)
{
	latestUseTime = new int[ddg.getNoRegisters()];
	completionTime = new int[ddg.getNoRegisters()];
}

OperationScheduler::~OperationScheduler()
{

}

int OperationScheduler::schedule()
{

}
