/*
 * DDGNode.h
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDGNODE_H_
#define DDGNODE_H_

#include <vector>
#include "globals.h"
#include "s3inst.h"

class DDGNode
{
	inst_t instruction;
	int destReg;
	std::vector<int> srcReg;

	void initRegisterInfo();

	DDGNode(DDGNode &);
public:
	DDGNode(inst_t instruction);
	virtual ~DDGNode();
};

#endif /* DDGNODE_H_ */
