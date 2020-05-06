#ifndef NON_MEM
#define NON_MEM

#include "Function.h"

class Non_Member_Function : public Function
{
public:
Non_Member_Function(int newNum, Type* newReturnType, char* newName, symbolNode* parentBlock)
:Function(NON_MEMBER_FUNCTION, newNum, newReturnType, newName, parentBlock)
{
}


};

#endif
