/*
 * BasicBlock.h
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#ifndef BASICBLOCK_H_
#define BASICBLOCK_H_

#include "s3inst.h"
#include "DDG.h"
#include "DDGNode.h"
#include "ModuloSchedulor.h"
#include <queue>
#include <vector>
#include <list>

class BasicBlock
{
private:

	int resMII;
	int recMII;
	int mII;
	DDG ddg;
	int noInstructions;
	ModuloSchedulor *schedule;

	void calculateMII(int k);

public:

	BasicBlock(inst_t start, inst_t end);
	~BasicBlock();

	void scheduleBlock(int k);
};

#endif /* BASICBLOCK_H_ */
