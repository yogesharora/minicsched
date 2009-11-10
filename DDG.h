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
	inst_t startInstruction;
	inst_t endInstruction;
	int noOfInstructions;
	int noOfRegisters;
	int highestRegister;
	int lowestRegister;

	void initProgramInfo();
	inline void initRegisterInfo(inst_t instruction, int& high, int &low);

public:
	DDG(inst_t start, inst_t end);
	virtual ~DDG();
};

#endif /* DDG_H_ */
