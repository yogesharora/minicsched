/*
 * DDGNode.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDGNode.h"
#include "s3inst.h"

DDGNode::DDGNode(inst_t instruction) :
	instruction(instruction)
{
	initRegisterInfo();
}

DDGNode::~DDGNode()
{
}

void DDGNode::initRegisterInfo()
{
	switch (instruction->op)
	{
		case OP_ADD :
		case OP_AND :
		case OP_ANDL :
		case OP_DIV :
		case OP_MUL :
		case OP_OR :
		case OP_ORL :
		case OP_SUB :
			destReg = instruction->ops[0].reg;
			srcReg.push_back(instruction->ops[1].reg);

			if (instruction->ops[2].t == op_reg)
				srcReg.push_back(instruction->ops[2].reg);
			break;
		case OP_SET :
			destReg = instruction->ops[0].reg;
			break;
		case OP_NOT :
		case OP_NOTL :
			destReg = instruction->ops[0].reg;
			srcReg.push_back(instruction->ops[1].reg);
			break;
		case OP_LD :
			destReg = instruction->ops[0].reg;
			break ;
		case OP_LDI :
			destReg = instruction->ops[0].reg;
			srcReg.push_back(instruction->ops[1].reg);
			break;
		case OP_LDR :
			destReg = instruction->ops[0].reg;
			srcReg.push_back(instruction->ops[1].reg);
			if (instruction->ops[2].t == op_reg)
				srcReg.push_back(instruction->ops[2].reg);
			break;
		case OP_ST :
			srcReg.push_back(instruction->ops[0].reg);
			break;
		case OP_STI :
			srcReg.push_back(instruction->ops[0].reg);
			break;
		case OP_STR :
			srcReg.push_back(instruction->ops[0].reg);
			srcReg.push_back(instruction->ops[1].reg);
			if (instruction->ops[2].t == op_reg)
				srcReg.push_back(instruction->ops[2].reg);
			break;
		case OP_BR :
			srcReg.push_back(instruction->ops[0].reg);
			break;
		case OP_LEA :
			destReg = instruction->ops[0].reg;
			break ;
		case OP_OUT :
			srcReg.push_back(R0);
			break;
		case OP_IN :
			destReg = R0;
			break;

		case OP_BRA :
		case OP_JSR :
		case OP_JSRR :
		case OP_JMP :
		default:
			break;
	}
}
