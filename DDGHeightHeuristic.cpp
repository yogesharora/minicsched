/*
 * DDGHeightHeuristic.cpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Yogesh Arora
 */

#include "DDGHeightHeuristic.h"

void DDGHeightHeuristic::calculateHeightheuristic()
{
	bool changedLatency = true;
	int maxHeight = 0;
	while (changedLatency == true)
	{
		changedLatency = false;
		const DDG::DDGNodeList& instructions = ddg.getInstructions();
		for (DDG::DDGNodeListConstIter iter = instructions.begin(); iter
				< instructions.end(); iter++)
		{
			DDGNode* X = *iter;
			const DDGNode::SuccessorList &successors = X->getSuccessors();

			for (DDGNode::SuccessorListConstIter succIter = successors.begin(); succIter
					!= successors.end(); succIter++)
			{
				const DDGNode::Edge& i = *succIter;
				int tHeight;

				if (i.edgeType == DDGNode::TRUE_DEP || i.edgeType
						== DDGNode::OUT_DEP)
				{
					tHeight = i.node->getHeight() + X->getLatency();
				}
				else if (i.edgeType == DDGNode::ANTI_DEP)
				{
					tHeight = i.node->getHeight() - i.node->getLatency() + 1;
					tHeight = tHeight > 0 ? tHeight : 0;
				}

				if (tHeight > X->getHeight())
				{
					X->setHeight(tHeight);
					changedLatency = true;
				}

				if (tHeight > maxHeight)
				{
					maxHeight = tHeight;
				}
			}
		}
	}

	const DDG::DDGNodeList& instructions = ddg.getInstructions();
	for (DDG::DDGNodeListConstIter iter = instructions.begin(); iter
			< instructions.end(); iter++)
	{
		DDGNode* instruction = *iter;
		int hDash = maxHeight - instruction->getHeight() + 1;
		instruction->setHeight(hDash);
	}
}
