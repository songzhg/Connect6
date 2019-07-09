#include "stdafx.h"
#include "AIFactory.h"

AbstractAI* AIFactory::get_AI_Instance(CString sType)
{
	if (sType == "VCDT")
	{
		return new VCDT();
	}
	if (sType == "VCST")
	{
		return new VCST();
	}
	if (sType == "Solver")
	{
		return new Solver();
	}
	if (sType == "AI")
	{
		return new AI();
	}
	return NULL;
}