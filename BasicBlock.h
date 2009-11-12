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

class BasicBlock
{
	int resMII;
	int recMII;
	int mII;
	DDG ddg;

public:
	BasicBlock(inst_t start, inst_t end);
	virtual ~BasicBlock();
	void calculateMII(int k);
};

#endif /* BASICBLOCK_H_ */
