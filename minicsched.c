#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3inst.h"
#include "cmdline.h"
#include "S3Code.h"
#include "BasicBlock.h"
#include "PrintUtils.h"

extern int num_errors;
extern int yyerror(...);
extern "C" int yywrap();
extern int yyparse();
extern int cmdlex();

void c_optimize(void);
void codegen_entry(FILE *fptr);
void codegen_exit(FILE *fptr);
void find_function(void);
void print_inst(FILE*, inst_t);
void print_list(FILE*, inst_t);

inst_t instList; /* list of instructions found by parser */

extern int k;

int main(int argc, char **argv)
{
	arglim = argv + argc;
	targv = argv + 1;

	cmdlex();

	if (outfile == NULL)
		outfile = strdup("out.asm");

	if (infile[0])
	{
		c_optimize();
	}

	return 0;
}

void c_optimize()
{
	/* file pointer to dump output code */
	FILE *fptr = fopen(outfile, "w");

	codegen_entry(fptr);

	yywrap();
	yyparse();

	if (num_errors > 0)
		return;

	if (verbose)
		PrintUtils::printInstructionList(stdout, instList);

	find_function(); /* remove extra instructions needed for simulation */

	/************************************************************************/
	/************************************************************************/
	/************************************************************************/
	/************************************************************************/
	/*    Call your implementation from here                                */
	S3Code s3Code(instList);
	s3Code.findBasicBlocks();
	const S3Code::BasicBlockList& basicBlockList = s3Code.getBasicBlocks();
	if(k==0)
		k=DEFAULT_K_VALUE;
	for(S3Code::BasicBlockConstIter iter=basicBlockList.begin();
			iter != basicBlockList.end();
			iter++)
	{
		if(verbose)
			(*iter)->print();
		(*iter)->scheduleBlock(k);

		if(verbose)
			(*iter)->printSchedule(stdout);
	}

	/* Find single basic block loops and perform Iterative Modulo Scheduling */

	/************************************************************************/
	/************************************************************************/
	/************************************************************************/
	/************************************************************************/

	// dump code to file
	s3Code.writeToOutput(fptr);

	codegen_exit(fptr);
	fclose(fptr); /* close file */
}

/**************************************************************************
 *   Support for generating code
 */

void codegen_entry(FILE *fptr)
{
	fprintf(fptr, "\t.ORIG x2000\n");
	fprintf(fptr, "\tJSR main\n");
	fprintf(fptr, "\tHALT\n");
}

void codegen_exit(FILE *fptr)
{
	fprintf(fptr, "\t.END\n");
}

void find_function()
{
	/* Need to remove first three instructions */
	inst_t tmp, tmp1, tmp2;
	inst_t oldhead;
	int found = 0;

	if (instList->op == OP_ORIG)
	{
		tmp = instList->next;
		if (tmp->op == OP_JSR && strcmp(tmp->ops[0].label, "main") == 0)
		{
			tmp = tmp->next;
			if (tmp->op == OP_HALT)
			{
				found = 1;
				tmp = tmp->next;
				while (instList != tmp)
				{
					oldhead = instList;
					instList = instList->next;
					free(oldhead);
				}
			}
		}
	}

	if (!found)
	{
		printf("Warning: Beginning of input not in the expected format!\n");
	}

	/* Remove last instruction: END */
	tmp = instList;
	tmp1 = tmp;
	tmp2 = tmp1;
	while (tmp != NULL)
	{
		tmp2 = tmp1;
		tmp1 = tmp;
		tmp = tmp->next;
	}

	if (tmp1->op == OP_END && tmp2 != tmp1)
	{
		tmp2->next = NULL;
		free(tmp1);
	}
	else
	{
		/*printf("Warning: Did not find .END.\n");*/
	}
}
