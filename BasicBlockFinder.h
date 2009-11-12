/*
 * BasicBlockFinder.h
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#ifndef BASICBLOCKFINDER_H_
#define BASICBLOCKFINDER_H_

#include "BasicBlock.h"
#include "s3inst.h"
#include <vector>

class BasicBlockFinder
{
public:
	typedef std::vector<BasicBlock*> BasicBlockList;
	typedef BasicBlockList::iterator BasicBlockIter;
	typedef BasicBlockList::const_iterator BasicBlockConstIter;


	BasicBlockFinder(inst_t startInst);
	~BasicBlockFinder();
	void findBasicBlocks();
	const BasicBlockList& getBasicBlocks();

	BasicBlockList basicBlocks;

private:
	inst_t startInstruction;
};

#endif /* BASICBLOCKFINDER_H_ */
