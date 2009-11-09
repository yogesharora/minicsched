/*
 * DDG.h
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDG_H_
#define DDG_H_

#include "s3inst.h"

class DDG
{
	inst_t instructions;
	int noOfInstructions;
	int noOfRegisters;

	void initProgramInfo();
	int getHighestReg(inst_t instruction);

public:
	DDG(inst_t inst);
	virtual ~DDG();
};

#endif /* DDG_H_ */
