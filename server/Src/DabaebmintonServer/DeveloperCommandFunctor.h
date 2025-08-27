#pragma once

#include "Level.h"

class DeveloperCommandFunctor
{
public:
	DeveloperCommandFunctor(vector<Level>& levels);

	void operator()();
	
private:
	vector<Level>& _levels;
};

