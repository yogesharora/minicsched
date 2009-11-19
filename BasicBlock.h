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

class BasicBlock
{
public:

	struct Schedule
	{

		DDGNode ***mrt;
		bool *neverScheduled;
		DDGNode** schedTime;
		int delta;
		int k;
		int noOfInstructions;

		Schedule(int del, int res, int inst):
			delta(del), k(res), noOfInstructions(inst)
		{
			mrt = new DDGNode**[delta];
			for(int i=0; i < delta ;i++)
			{
				mrt[i] = new DDGNode*[k];
				memset(mrt[i], 0, k*sizeof(DDGNode*));
			}

			neverScheduled = new bool[noOfInstructions];
			memset(neverScheduled, true, noOfInstructions*sizeof(bool));

			schedTime = new DDGNode*[noOfInstructions];
			memset(schedTime, 0, noOfInstructions*sizeof(DDGNode*));
		}

		Schedule::~Schedule()
		{
			delete[] neverScheduled;
			delete[] schedTime;

			for (int i = 0; i < delta; i++)
				delete mrt[i];

			delete[] mrt;
		}
	};

private:
	int resMII;
	int recMII;
	int mII;
	DDG ddg;
	int noInstructions;
	Schedule *schedule;

	void calculateMII(int k);

public:

	BasicBlock(inst_t start, inst_t end);
	~BasicBlock();

	void scheduleBlock(int k);
	bool iterativeSchedule(int delta, Schedule *schedule);
};

#endif /* BASICBLOCK_H_ */
