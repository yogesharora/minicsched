/*
 * PrintUtils.cpp
 *
 *  Created on: Nov 19, 2009
 *      Author: Yogesh Arora
 */

#include "PrintUtils.h"

void PrintUtils::printConditionCode(FILE *fptr, int ccode)
{
	if (ccode & CC_N)
		fprintf(fptr, "n");
	if (ccode & CC_Z)
		fprintf(fptr, "z");
	if (ccode & CC_P)
		fprintf(fptr, "p");

	fprintf(fptr, " ");
}

void PrintUtils::printOperand(FILE *fptr, struct operand op)
{
	enum op_type t = op.t;
	switch (t)
	{
		case op_reg :
			fprintf(fptr, "R%d", op.reg);
			break;
		case op_imm :
			fprintf(fptr, "#%d", op.imm);
			break;
		case op_label :
			fprintf(fptr, "%s", op.label);
			break;
	}
}

void PrintUtils::printInstruction(FILE* fptr, inst_t i, bool cycle)
{
	if (i->label)
	{
		fprintf(fptr, "%s:", i->label);
	}

	if (i->op == OP_BR)
	{
		fprintf(fptr, "\t%s", opnames[i->op]);
		printConditionCode(fptr, i->ccode);
	}
	else
		fprintf(fptr, "\t%s ", opnames[i->op]);

	switch (i->op)
	{

		/* 3 operands */
		case OP_ADD :
		case OP_AND :
		case OP_ANDL :
		case OP_DIV :
		case OP_LDR :
		case OP_MUL :
		case OP_OR :
		case OP_ORL :
		case OP_STR :
		case OP_SUB :
			printOperand(fptr, i->ops[0]);
			fprintf(fptr, ", ");
			printOperand(fptr, i->ops[1]);
			fprintf(fptr, ", ");
			printOperand(fptr, i->ops[2]);
			break;
			/* 2 operands */
		case OP_BR :
		case OP_SET :
		case OP_ST :
		case OP_STI :
		case OP_LD :
		case OP_LDI :
		case OP_LEA :
		case OP_NOT :
		case OP_NOTL :
			printOperand(fptr, i->ops[0]);
			fprintf(fptr, ", ");
			printOperand(fptr, i->ops[1]);
			break;

			/* one operand */
		case OP_JSRR :
		case OP_BRA :
		case OP_JMP :
		case OP_JSR :
			printOperand(fptr, i->ops[0]);

		default :
			break;
	}

	if(!cycle)
		fprintf(fptr, "\n");
}
