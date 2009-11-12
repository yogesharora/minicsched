/*
 * BasicBlockFinder.cpp
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#include "BasicBlockFinder.h"
#include <stack>

using namespace std;

BasicBlockFinder::BasicBlockFinder(inst_t startInst) :
	startInstruction(startInst)
{
}

BasicBlockFinder::~BasicBlockFinder()
{
}

void BasicBlockFinder::findBasicBlocks()
{
	stack<inst_t> lastLabelStack;
	inst_t cur = startInstruction;

	while (cur != NULL)
	{
		if(cur->op==OP_BR || cur->op==OP_BRA)
		{
			// look for the last instruction in stack
			inst_t lastLabelInstruction = lastLabelStack.top();
			char* curLabel;

			if(cur->op==OP_BR)
				curLabel = cur->ops[1].label;
			if(cur->op==OP_BRA)
				curLabel = cur->ops[0].label;

			if (lastLabelInstruction->label != NULL && strcmp(
					lastLabelInstruction->label, curLabel) == 0)
			{
				// found a basic block
				basicBlocks.push_back(new BasicBlock(lastLabelInstruction, cur));
			}
			else
			{
				lastLabelStack.push(cur);
			}
		}
		else if (cur->label != NULL)
		{
			lastLabelStack.push(cur);
		}
		cur = cur->next;
	}
}

const BasicBlockFinder::BasicBlockList& BasicBlockFinder::getBasicBlocks()
{
	return basicBlocks;
}
