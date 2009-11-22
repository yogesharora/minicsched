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
	rootNode(true), latency(calcLatency()), height(latency)
{
	initRegisterInfo();
}

DDGNode::~DDGNode()
{
}

int DDGNode::calcLatency()
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
			srcReg.insert(R0);
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

void DDGNode::addFlowDependency(DDGNode *dependent, int myIteration, int dependentIteration)
{
	insertEdge(dependent, latency, myIteration, dependentIteration, TRUE_DEP);
	printEdges();
}

void DDGNode::addAntiDependency(DDGNode *dependent, int myIteration, int dependentIteration)
{
	insertEdge(dependent, 0, myIteration, dependentIteration, ANTI_DEP);
	printEdges();
}

void DDGNode::addOutputDependency(DDGNode *dependent, int myIteration, int dependentIteration)
{
	insertEdge(dependent, latency, myIteration, dependentIteration, OUT_DEP);
	printEdges();
}

void DDGNode::insertEdge(DDGNode* dependent, int edgeWeight, int myIteration,
		int dependentIteration, EdgeType type)
{
	sucessors.push_back(Edge(dependent, edgeWeight, myIteration, dependentIteration, type));
	dependent->predecessors.push_back(Edge(this, edgeWeight, myIteration,
			dependentIteration, type));
}

void DDGNode::printEdges()
{
//	printf("Node No: %d ", instructionNumber);
//	for(SuccessorListIter iter = sucessors.begin(); iter!=sucessors.end();iter++)
//	{
//		Edge &edge = *iter;
//		printf("(node-%d, latency-%d, iterationDistance-%d) ",
//				edge.node->getNo(), edge.edgeWeight, edge.iterationDistance);
//	}
//	printf("\n");
}
