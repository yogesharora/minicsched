/*
 * DDGNode.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: Yogesh Arora
 */

#include "DDGNode.h"
#include "s3inst.h"

DDGNode::DDGNode(inst_t instruction, int no) :
	instruction(instruction), instructionNumber(no), destReg(INVALID_REG),
	rootNode(true), latency(getLatency())
{
	initRegisterInfo();
}

DDGNode::~DDGNode()
{
}

int DDGNode::getLatency()
{
	int value;
	switch (instruction->op)
	{
		case OP_LD:
			value = 2;
			break;
		case OP_LDI:
			value = 3;
			break;
		case OP_LDR:
			value=2;
			break;
		case OP_MUL:
			value=3;
			break;
		default:
			value=1;
			break;
	}
	return value;
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
			srcReg.insert(instruction->ops[1].reg);

			if (instruction->ops[2].t == op_reg)
				srcReg.insert(instruction->ops[2].reg);
			break;
		case OP_SET :
			destReg = instruction->ops[0].reg;
			break;
		case OP_NOT :
		case OP_NOTL :
			destReg = instruction->ops[0].reg;
			srcReg.insert(instruction->ops[1].reg);
			break;
		case OP_LD :
			destReg = instruction->ops[0].reg;
			break ;
		case OP_LDI :
			destReg = instruction->ops[0].reg;
			srcReg.insert(instruction->ops[1].reg);
			break;
		case OP_LDR :
			destReg = instruction->ops[0].reg;
			srcReg.insert(instruction->ops[1].reg);
			if (instruction->ops[2].t == op_reg)
				srcReg.insert(instruction->ops[2].reg);
			break;
		case OP_ST :
			srcReg.insert(instruction->ops[0].reg);
			break;
		case OP_STI :
			srcReg.insert(instruction->ops[0].reg);
			break;
		case OP_STR :
			srcReg.insert(instruction->ops[0].reg);
			if (instruction->ops[1].t == op_reg)
				srcReg.insert(instruction->ops[1].reg);
			if (instruction->ops[2].t == op_reg)
				srcReg.insert(instruction->ops[2].reg);
			break;
		case OP_BR :
			srcReg.insert(instruction->ops[0].reg);
			break;
		case OP_LEA :
			destReg = instruction->ops[0].reg;
			break ;
		case OP_OUT :
			// TODO
			srcReg.insert(0);
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

void DDGNode::addFlowDependency(DDGNode *dependent)
{
	insertEdge(dependent, latency);
	printEdges();
}

void DDGNode::addAntiDependency(DDGNode *dependent)
{
	insertEdge(dependent, 0);
	printEdges();
}

void DDGNode::addOutputDependency(DDGNode *dependent)
{
	insertEdge(dependent, 1);
	printEdges();
}

void DDGNode::insertEdge(DDGNode* dependent, int edgeWeight)
{
	for(DependentListIter iter = dependents.begin(); iter!=dependents.end();iter++)
	{
		DependentEdge &edge = *iter;
		if (edge.first->getNo() == dependent->getNo())
		{
			// edge to same node exits
			// if the weight is more...add that edge else no need
			if(edge.second < edgeWeight)
			{
				edge.second=edgeWeight;
			}
			return;
		}
	}
	dependents.push_back(DependentEdge(dependent, edgeWeight));

}

void DDGNode::printEdges()
{
//	printf("Node No: %d ", instructionNumber);
//	for(DependentListIter iter = dependents.begin(); iter!=dependents.end();iter++)
//	{
//		DependentEdge &edge = *iter;
//		printf("(%d, %d) ",edge.first->getNo(), edge.second);
//	}
//	printf("\n");
}
