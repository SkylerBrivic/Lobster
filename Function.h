#ifndef FUN
#define FUN

#include "symbolNode.h"
#include "Parameter.h"
#include "Type.h"

class Function : public symbolNode
{
public:

Function(SymbolType newType, int newNum, Type* newReturnType, char* newName, symbolNode* parentBlock)
: symbolNode(newType, newNum, parentBlock)
{
functionReturnType = newReturnType;
strcpy(functionName, newName);
parameterList = vector<Parameter*>();
}

void addParameter(Parameter* newParam)
{
parameterList.push_back(newParam);
}



Type* functionReturnType;
char functionName[1000];
vector<Parameter*> parameterList;



};
#endif
