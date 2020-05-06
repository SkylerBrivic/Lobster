#include <stack>
#include "varObj.h"
#include <unordered_map>
using namespace std;

//varObj: public: long long myVal 

class stackTableStructure
{
public:

stackTableStructure()
{
memStack = std::stack<unordered_map<int, varObj>>();
}

void addBlock()
{
memStack.push( unordered_map<int, varObj>());
}

void deleteBlock()
{
memStack.pop();
}

void addVariable(int varNum)
{
memStack.top()[varNum] = varObj();
}

void* getAddressOfVariable(int varNum)
{
return &(memStack.top()[varNum].myVal);
}

void setLongLongOfVariable(int varNum, long long varVal)
{
memStack.top()[varNum].myVal = varVal;
}
/*
void setDoubleOfVariable(int varNum, double varVal)
{
*( (double*) (&memStack.top()[varNum].myVal) = varVal;
}
*/
void setBooleanOfVariable(int varNum, bool varVal)
{
*( (bool*) (&memStack.top()[varNum].myVal)) = varVal;
}

void setCharOfVariable(int varNum, char varVal)
{
*( (char*) (&memStack.top()[varNum].myVal)) = varVal;
}
/*
void setPointerOfVariable(int varNum, long long* myPoint)
{
*( (long long**) (&memStack.top()[varNum].myVal) = myPoint;
}
*/

void copyVarAddressToRegister(int varNum, long long* myReg)
{
 *myReg =   ((long long) (&(memStack.top()[varNum].myVal)));
 return;
}

std::stack<unordered_map<int, varObj>> memStack;

};
