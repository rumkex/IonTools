#pragma once

#include "Constraint.h"

class PositiveConstraint: public Constraint
{
public:
	PositiveConstraint()
	{}
	
    virtual void iterate();
};
