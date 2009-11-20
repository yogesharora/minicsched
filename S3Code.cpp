/*
 * S3Code.cpp
 *
 *  Created on: Nov 11, 2009
 *      Author: Yogesh Arora
 */

#include "S3Code.h"
#include "PrintUtils.h"
#include <stack>

using namespace std;

S3Code::S3Code(inst_t startInst) :
	startInstruction(startInst)
{
}

S3Code::~S3Code()
{
	for (BasicBlockIter iter = basicBlocks.begin(); iter != basicBlocks.end(); iter++)
	{
		delete *iter;
	}
}

void S3Code::findBasicBlocks()
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
				basicBlocksBoundary.push(lastLabelInstruction);
				basicBlocksBoundary.push(cur);
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

void S3Code::writeToOutput(FILE *fptr)
{
	inst_t cur = startInstruction;
	bool basicBlockBegin=false;
	BasicBlockIter basicBlockIter = basicBlocks.begin();
	while (cur != NULL)
	{
		inst_t basicBlockElement = basicBlocksBoundary.front();

		if (basicBlockBegin)
		{
			if (cur == basicBlockElement)
			{
				basicBlockBegin = false;
				basicBlocksBoundary.pop();
			}
		}
		else if (cur == basicBlockElement)
		{
			// basic block has begun
			basicBlockBegin = true;
			basicBlocksBoundary.pop();
			(*basicBlockIter)->printSchedule(fptr);
		}
		else
		{
			// its not part of basic block
			// print it
			PrintUtils::printInstruction(fptr, cur);
		}

		cur = cur->next;
	}
}

const S3Code::BasicBlockList& S3Code::getBasicBlocks()
{
	return basicBlocks;
}
