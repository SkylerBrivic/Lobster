#ifndef MEMBER_F
#define MEMBER_F

#include "Function.h"

class Member_Function : public Function
{
public:
Member_Function(int newNum, Type* returnType, char* funcName, symbolNode* parentBlock)
: Function(MEMBER_FUNCTION, newNum, returnType, funcName, parentBlock)
{
}



};

#endif
