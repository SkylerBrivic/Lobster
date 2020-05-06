#ifndef PARAM
#define PARAM

#include "Variable.h"

class Parameter : public Variable
{
public:

Parameter(int newNum, Type* newVariableType, char* newVariableName, symbolNode* parentBlock)
: Variable(PARAMETER, newNum, newVariableType, newVariableName, parentBlock)
{
}


};

#endif
