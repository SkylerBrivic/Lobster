#ifndef PIPELINE_PAGE
#define PIPELINE_PAGE

#include "symbolNode.h"

class PipeLine : public symbolNode
{
public:

PipeLine(int newNum, symbolNode* parentBlock, char* newPipeName)
: symbolNode(PIPE_LINE_SYM, newNum, parentBlock)
{
strcpy(pipeName, newPipeName);
functionList = std::vector<char*>();
}

void addFunction(char* newFunc)
{
functionList.push_back(strdup(newFunc));
}

int numFunctions()
{
return functionList.size();
}

char* getFunctionNumber(int myNum)
{
return functionList[myNum];
}

char pipeName[1000];
std::vector<char*> functionList;

};
#endif
