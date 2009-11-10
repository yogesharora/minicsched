/*
 * DDG.h
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDG_H_
#define DDG_H_

#include <vector>
#include "s3inst.h"
#include "DDGNode.h"

class DDG
{
	inst_t startInstruction;
	inst_t endInstruction;
	int noOfInstructions;
	int noOfRegisters;
	int highestRegister;
	std::vector<DDGNode*> graph;

	void initProgramInfo();
	int getMaxUsedRegister(inst_t instruction);

	DDG(DDG&);
public:
	DDG(inst_t start, inst_t end);
	virtual ~DDG();
};

#endif /* DDG_H_ */
