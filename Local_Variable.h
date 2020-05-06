#ifndef LOCAL_VAR_PAGE
#define LOCAL_VAR_PAGE

#include "Variable.h"

class Local_Variable : public Variable
{
Local_Variable(int newNum, Type* varType, char* varName, symbolNode* parentBlock)
{
Variable(FUNCTION_LOCAL_VARIABLE, newNum, varType, varName, parentBlock);
}


};
#endif
