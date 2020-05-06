#ifndef VARIABLE
#define VARIABLE

#include "symbolNode.h"
#include "Type.h"

class Variable : public symbolNode
{
public:



Variable(SymbolType newType, int newNum, Type* newVariableType, char* newVariableName, symbolNode* parentBlock)
: symbolNode(newType, newNum, parentBlock)
{
variableType = newVariableType;
strcpy(variableName, newVariableName);
}

char variableName[1000];
Type* variableType;
};

#endif
