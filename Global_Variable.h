#ifndef GLOB_VAR_PAGE
#define GLOB_VAR_PAGE

#include "Variable.h"

class Global_Variable : public Variable
{
Global_Variable(int newNum, Type* newType, char* newName, symbolNode* parentBlock)
{
Variable(GLOBAL_VARIABLE, newNum, newType, newName, parentBlock);
}


};
#endif
