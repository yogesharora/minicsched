/*
 * ModuloSchedulor.h
 *
 *  Created on: Nov 19, 2009
 *      Author: Yogesh Arora
 */

#ifndef MODULOSCHEDULOR_H_
#define MODULOSCHEDULOR_H_

#include "DDGNode.h"
#include "DDG.h"
#include <queue>
#include <vector>
#include <list>

class ModuloSchedulor
{
	bool *neverScheduled;
	int* schedTime;
	int delta;
	unsigned int k;
	int noOfInstructions;

	struct DDGNodeSchedule
	{
		DDGNode *ddgNode;
		int iteration;

		DDGNodeSchedule(DDGNode *d, int i) : ddgNode(d), iteration(i)
		{

		}
	};

	typedef std::list<DDGNodeSchedule> Cycle;
	typedef Cycle::iterator CycleIter;
	typedef std::vector<Cycle> Mrt;

	typedef std::list<inst_t> InstCycle;
	typedef InstCycle::iterator InstCycleIter;
	typedef std::vector<InstCycle> InstructionSched;

	Mrt mrt;
	InstructionSched prolog;
	InstructionSched epilogue;
	DDG& ddg;
	char *label;

	struct DDGComp
	{
		bool operator () (DDGNode* a, DDGNode* b)
		{
			return a->getNo() > b->getNo();
		}
	};

	typedef std::priority_queue<DDGNode*, std::vector<DDGNode*>, DDGComp>
				InstructionQueue;

	InstructionQueue queue;

	int calculateEarlyStart(DDGNode *op);
	int calculateDelay(int delta, const DDGNode::Edge &edge);
	int findTimeSlot(DDGNode *op, int minTime, int maxTime);
	bool resourceConflict(int curTime);
	void schedule(DDGNode* op, int time);
	void unschedule(int instruction);
    void genProlog(int maxIteration);
    void printMrt(Mrt& table);
    void printInstruction(InstructionSched& table);

public:
	ModuloSchedulor(int del, int res, unsigned int inst, DDG& d, char* blockLabel);
	bool iterativeSchedule();
	void rotate();
	void genPrologEpilogue();
	void print();
	~ModuloSchedulor();
};

#endif /* MODULOSCHEDULOR_H_ */
