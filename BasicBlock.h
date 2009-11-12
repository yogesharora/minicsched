/*
 * BasicBlock.h
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#ifndef BASICBLOCK_H_
#define BASICBLOCK_H_

#include "s3inst.h"

class BasicBlock
{
public:
	BasicBlock(inst_t start, inst_t end);
	virtual ~BasicBlock();
};

#endif /* BASICBLOCK_H_ */
