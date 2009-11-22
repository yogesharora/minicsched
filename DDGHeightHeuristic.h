/*
 * DDGHeightHeuristic.h
 *
 *  Created on: Nov 21, 2009
 *      Author: Yogesh Arora
 */

#ifndef DDGHEIGHTHEURISTIC_H_
#define DDGHEIGHTHEURISTIC_H_

#include "DDG.h"

class DDGHeightHeuristic
{
	DDG& ddg;

public:
	DDGHeightHeuristic(DDG& d) :
		ddg(d)
	{
	}

	~DDGHeightHeuristic(){};

	void calculateHeightheuristic();
};

#endif /* DDGHEIGHTHEURISTIC_H_ */
