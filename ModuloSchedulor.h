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
#include <cstdio>
#include <cstdlib>

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

	struct PrologEpilogueSchedule
	{
		inst_t inst;
		int iteration;

		PrologEpilogueSchedule(inst_t d, int i) :
			inst(d), iteration(i)
		{

		}

	};

	typedef std::list<DDGNodeSchedule> Cycle;
	typedef Cycle::iterator CycleIter;
	typedef std::vector<Cycle> Mrt;

	typedef std::list<PrologEpilogueSchedule> InstCycle;
	typedef InstCycle::iterator InstCycleIter;
	typedef std::vector<InstCycle> InstructionSched;
	typedef std::vector<InstructionSched> EpilogueProlog;

	Mrt mrt;
	EpilogueProlog prologs;
	EpilogueProlog epilogues;
	DDG& ddg;
	char *basicBlockLabel;

	struct DDGComp
	{
		bool operator () (DDGNode* a, DDGNode* b)
		{
			return a->getDepth() > b->getDepth();
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
    void genEpilogue(int maxIteration, int branchIterationNo);
    void printMrt(FILE* fptr, Mrt& table);
    void printInstruction(FILE* fptr, EpilogueProlog& table);
    inst_t createNewBranchInst(inst_t ddgInstruction, int i);
    inst_t createEpilogueLabelInst(inst_t ddgInstruction, int i);

public:
	ModuloSchedulor(int del, int res, unsigned int inst, DDG& d, char* blockLabel);
	bool iterativeSchedule();
	void rotate();
	void genPrologEpilogue();
	void print(FILE* fptr);
	~ModuloSchedulor();
};

#endif /* MODULOSCHEDULOR_H_ */
