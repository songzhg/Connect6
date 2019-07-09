#pragma once
#include "AbstractAI.h"
#include "VCDT.h"
#include "VCST.h"
#include "Solver.h"
#include "AI.h"
#include <string>
using namespace std;

class AIFactory
{
public:

	AIFactory(void)
	{
	}

	~AIFactory(void)
	{
	}

	AbstractAI* get_AI_Instance(CString sType);
};
