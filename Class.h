#ifndef CLASS_PAGE
#define CLASS_PAGE

#include "symbolNode.h"
#include "Member_Variable.h"
#include "Member_Function.h"

class Class : public symbolNode
{
public:

Class(int newNum, char* newClassName, symbolNode* parent)
:symbolNode(CLASS, newNum, parent)
{
strcpy(className, newClassName);
MemberVariableList = vector<Member_Variable*>();
MemberFunctionList = vector<Member_Function*>();
}

void addMemberVariable(Member_Variable* newVar)
{
MemberVariableList.push_back(newVar);
}

void addMemberFunction(Member_Function* newFunction)
{
MemberFunctionList.push_back(newFunction);
}

char className[1000];
vector<Member_Variable*> MemberVariableList;
vector<Member_Function*> MemberFunctionList;


};
#endif
