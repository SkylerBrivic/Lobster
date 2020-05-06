#ifndef INSTR_MISC
#define INSTR_MISC

#include "Instructions.h"

Op_Type getOpType(Type* myType)
{
if(myType->isPrimitive)
{
if(strcmp(myType->primitiveName, "int") == 0 || strcmp(myType->primitiveName, "INT_LIT") == 0)
return INTEGER_T;

else if(strcmp(myType->primitiveName, "double") == 0 || strcmp(myType->primitiveName, "DOUBLE_LIT") == 0)
return DOUBLE_T;

else if(strcmp(myType->primitiveName, "char") == 0 || strcmp(myType->primitiveName, "CHAR_LIT") == 0)
return CHAR_T;

else if(strcmp(myType->primitiveName, "boolean") == 0 || strcmp(myType->primitiveName, "BOOL_LIT") == 0)
return BOOLEAN_T;

else if(strcmp(myType->primitiveName, "String") == 0 || strcmp(myType->primitiveName, "STRING_LIT") == 0)
return STRING_T;

else if(strcmp(myType->primitiveName, "NULL") == 0)
return NULL_T;

else
return USER_OBJ_T;
}
else
{
if(myType->complexType == ARRAY)
return ARRAY_T;

else if(myType->complexType == LIST)
return LIST_T;

else if(myType->complexType == DICT)
return DICT_T;

else if(myType->complexType == QUEUE)
return QUEUE_T;

else
std::cout << "Error: Unknown type of " << *myType << " encountered\n";
exit(1);
return USER_OBJ_T;
}

}

#endif
