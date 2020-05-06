#ifndef MEM_VAR_PAGE
#define MEM_VAR_PAGE
#include "Variable.h"

class Member_Variable : public Variable
{
public:

Member_Variable(int newNum, Type* varType, char* varName, symbolNode* parentBlock)
: Variable(MEMBER_VARIABLE, newNum, varType, varName, parentBlock)
{
}


};
#endif
