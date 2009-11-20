/*
 * S3Code.h
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#ifndef BASICBLOCKFINDER_H_
#define BASICBLOCKFINDER_H_

#include "BasicBlock.h"
#include "s3inst.h"
#include <vector>
#include <map>

class S3Code
{
public:
	typedef std::vector<BasicBlock*> BasicBlockList;
	typedef BasicBlockList::iterator BasicBlockIter;
	typedef BasicBlockList::const_iterator BasicBlockConstIter;

	S3Code(inst_t startInst);
	~S3Code();
	void findBasicBlocks();
	const BasicBlockList& getBasicBlocks();

	BasicBlockList basicBlocks;

private:
	inst_t startInstruction;
};

#endif /* BASICBLOCKFINDER_H_ */
