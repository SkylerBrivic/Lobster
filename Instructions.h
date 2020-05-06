#ifndef INSTR_PAGE
#define INSTR_PAGE

//<---- REGISTER FORMAT --->
//There are 256 registers numbered 0 - 255. Registers 0 - 222 can store memory addresses, integer types, or floating point numbers.
//Registers 223 - 253 store arguments to function calls (eithe member functions or non member functions)
//Register 254 stores the return value of a function call (which can be a memory address, an integer, or floating point number)
//Register 255 is the "this" register, which stores the memory address of an object that has a member function called on it.
#define NUMBER_OF_GENERAL_REGISTERS 223
#define RETURN_REGISTER_NUM 254
#define THIS_REGISTER_NUM 255
#define BYTES_PER_LINE 15
#include <iostream>
#include <string.h>
#include <string>
using namespace std;

enum Op_Type : unsigned char {INTEGER_T, CHAR_T, BOOLEAN_T, DOUBLE_T,
STRING_T, ARRAY_T, LIST_T, DICT_T, QUEUE_T, USER_OBJ_T, NULL_T };

enum Source_Location : unsigned char {REGISTER_VAL, REGISTER_MEM_ADDR, LITERAL_L, CODE_LOCATION };

enum Builtin_Function : unsigned char {GET, PUT, FIND, CONTAINS, AT, SORT, LENGTH, UPDATE, PUSH, POP, FRONT};

enum Op_Code : unsigned char {
GLOBAL_DECLARE_OP, STACK_VARIABLE_DECLARE, PIPELINE_DECLARE, 

PIPELINE_ADD_FUNCTION, 

ASSIGNMENT_OP, 

NEW_USER_OBJ, BUILTIN_OBJECT_CREATE_CODE,

JUMP_TO, 

CALL_NON_MEMBER_FUNCTION, CALL_MEMBER_FUNCTION, CALL_BUILTIN_FUNCTION,

CONVERT_INT_TO_DOUBLE, CONVERT_INT_TO_CHAR, CONVERT_DOUBLE_TO_INT, CONVERT_DOUBLE_TO_CHAR, CONVERT_CHAR_TO_INT, CONVERT_CHAR_TO_DOUBLE,

SET_FUNCTION_ARGUMENT, SAVE_REGISTER, RESTORE_REGISTER, 
COPY_REGISTER_TO_REGISTER, COPY_INT_LITERAL_TO_REGISTER, 
COPY_BOOLEAN_LITERAL_TO_REGISTER, COPY_DOUBLE_LITERAL_TO_REGISTER,
COPY_CHAR_LITERAL_TO_REGISTER, COPY_STRING_LOCATION_TO_REGISTER, RETURN_FROM_FUNCTION, 

PLUS_EQUALS_OP, MINUS_EQUALS_OP, DIVIDE_EQUALS_OP, MOD_EQUALS_OP, TIMES_EQUALS_OP, POW_EQUALS_OP, LOGICAL_AND_EQUALS_OP, LOGICAL_OR_EQUALS_OP,

MOVE_STACK_ADDR_TO_REG, MOVE_GLOBAL_ADDR_TO_REG, GET_MEMBER_VAR_ADDR_OF_OBJ, ADD_MEMBER_VARS_TO_OBJ,

DEREFERENCE_BOOLEAN, DEREFERENCE_INTEGER, DEREFERENCE_DOUBLE, DEREFERENCE_CHAR,

PLUS_OP, MINUS_OP, DIVIDE_OP, MULTIPLY_OP, POW_OP, LOGICAL_AND_OP, LOGICAL_OR_OP, BITWISE_AND_OP, BITWISE_OR_OP, BITWISE_XOR_OP,
BITSHIFT_LEFT_OP, BITSHIFT_RIGHT_OP, EQUALS_EQUALS_OP, NOT_EQUALS_OP, GREATER_THAN_OP, GREATER_THAN_EQUALS_OP,
LESS_THAN_OP, LESS_THAN_EQUALS_OP, 

SET_PARAMETER,

BRANCH_ON_FALSE_LITERAL, BRANCH_ON_FALSE_REG_VAL, BRANCH_ON_FALSE_MEM_ADDR,

NOT_OP, BITWISE_NOT_OP, UNARY_MINUS_OP,

PRINT_OP, GET_INT_OP, GET_DOUBLE_OP, GET_CHAR_OP, GET_BOOLEAN_OP, GET_WORD_OP, GET_LINE_OP, IS_INPUT_FAIL_OP,

JUMP_TO_MAIN, EXIT_MAIN, OUT_OF_BOUNDS_FAULT
};

//NOTE: Format of binary arithmetic instructions:
//result always stored by value in a register for these operations, although compound assignment operators can put values directly in a memory address.
//ARITH_INSTRUCTION_OP(2 bytes), Op_Type_Of_Expr(1 byte), destReg(1 byte), sourceLocation1(1 byte), Reg/Literal/memReg1(1-4 bytes), sourceLocation2(1 byte), Reg/Literal/memReg2(1- 4 bytes)

string getSourceLocation(Source_Location myLoc)
{
if(myLoc == REGISTER_VAL)
return string("REGISTER_VAL");

else if(myLoc == REGISTER_MEM_ADDR)
return string("REGISTER_MEM_ADDR");

else
return string("LITERAL");
}


string getBuiltinFunctionString(Builtin_Function myFunc)
{
switch(myFunc)
{
case GET:
return string("GET");

case AT:
return string("AT");

case LENGTH:
return string("LENGTH");

case SORT:
return string("SORT");

case UPDATE:
return string("UPDATE");

case PUT:
return string("PUT");

case FIND:
return string("FIND");

case CONTAINS:
return string("CONTAINS");

case PUSH:
return string("PUSH");

case POP:
return string("POP");

case FRONT:
return string("FRONT");

default:
std::cout << "Error: Unknown builtin-function encountered: " << myFunc << std::endl;
exit(1);
return string("");


}
}


string getOpCodeString(Op_Code myCode)
{
switch(myCode)
{
case GLOBAL_DECLARE_OP:
return string("GLOBAL_DECLARE_OP");

case STACK_VARIABLE_DECLARE:
return string("STACK_VARIABLE_DECLARE");

case PIPELINE_DECLARE:
return string("PIPELINE_DECLARE");

case PIPELINE_ADD_FUNCTION:
return string("PIPELINE_ADD_FUNCTION");

case ASSIGNMENT_OP:
return string("ASSIGNMENT_OP");

case NEW_USER_OBJ:
return string("NEW_USER_OBJ");

case BUILTIN_OBJECT_CREATE_CODE:
return string("BUILTIN_OBJECT_CREATE_CODE");

case JUMP_TO:
return string("JUMP_TO");

case CALL_NON_MEMBER_FUNCTION:
return string("CALL_NON_MEMBER_FUNCTION");

case CALL_MEMBER_FUNCTION:
return string("CALL_MEMBER_FUNCTION");

case CALL_BUILTIN_FUNCTION:
return string("CALL_BUILTIN_FUNCTION");

case CONVERT_INT_TO_DOUBLE:
return string("CONVERT_INT_TO_DOUBLE");

case CONVERT_INT_TO_CHAR:
return string("CONVERT_INT_TO_CHAR");

case CONVERT_DOUBLE_TO_INT:
return string("CONVERT_DOUBLE_TO_INT");

case CONVERT_DOUBLE_TO_CHAR:
return string("CONVERT_DOUBLE_TO_CHAR");

case CONVERT_CHAR_TO_INT:
return string("CONVERT_CHAR_TO_INT");

case CONVERT_CHAR_TO_DOUBLE:
return string("CONVERT_CHAR_TO_DOUBLE");

case SET_FUNCTION_ARGUMENT:
return string("SET_FUNCTION_ARGUMENT");

case SAVE_REGISTER:
return string("SAVE_REGISTER");

case RESTORE_REGISTER:
return string("RESTORE_REGISTER");

case COPY_REGISTER_TO_REGISTER:
return string("COPY_REGISTER_TO_REGISTER");

case COPY_INT_LITERAL_TO_REGISTER:
return string("COPY_INT_LITERAL_TO_REGISTER");

case COPY_BOOLEAN_LITERAL_TO_REGISTER:
return string("COPY_BOOLEAN_LITERAL_TO_REGISTER");

case COPY_DOUBLE_LITERAL_TO_REGISTER:
return string("COPY_DOUBLE_LITERAL_TO_REGISTER");

case COPY_CHAR_LITERAL_TO_REGISTER:
return string("COPY_CHAR_LITERAL_TO_REGISTER");

case COPY_STRING_LOCATION_TO_REGISTER:
return string("COPY_STRING_LOCATION_TO_REGISTER");


case RETURN_FROM_FUNCTION:
return string("RETURN_FROM_FUNCTION");

case PLUS_EQUALS_OP:
return string("PLUS_EQUALS_OP");

case MINUS_EQUALS_OP:
return string("MINUS_EQUALS_OP");

case DIVIDE_EQUALS_OP:
return string("DIVIDE_EQUALS_OP");

case TIMES_EQUALS_OP:
return string("TIMES_EQUALS_OP");

case MOD_EQUALS_OP:
return string("MOD_EQUALS_OP");

case POW_EQUALS_OP:
return string("POW_EQUALS_OP");

case LOGICAL_AND_EQUALS_OP:
return string("LOGICAL_AND_EQUALS_OP");

case LOGICAL_OR_EQUALS_OP:
return string("LOGICAL_OR_EQUALS_OP");

case MOVE_STACK_ADDR_TO_REG:
return string("MOVE_STACK_ADDR_TO_REG");

case MOVE_GLOBAL_ADDR_TO_REG:
return string("MOVE_GLOBAL_ADDR_TO_REG");

case GET_MEMBER_VAR_ADDR_OF_OBJ:
return string("GET_MEMEBER_VAR_ADDR_OF_OBJ");

case ADD_MEMBER_VARS_TO_OBJ:
return string("ADD_MEMBER_VARS_TO_OBJ");

case DEREFERENCE_BOOLEAN:
return string("DEREFERENCE_BOOLEAN");

case DEREFERENCE_INTEGER:
return string("DEREFERENCE_INTEGER");

case DEREFERENCE_DOUBLE:
return string("DEREFERENCE_DOUBLE");

case DEREFERENCE_CHAR:
return string("DEREFERENCE_CHAR");

case PLUS_OP:
return string("PLUS_OP");

case MINUS_OP:
return string("MINUS_OP");

case DIVIDE_OP:
return string("DIVIDE_OP");

case MULTIPLY_OP:
return string("MULTIPLY_OP");

case POW_OP:
return string("POW_OP");

case LOGICAL_AND_OP:
return string("LOGICAL_AND_OP");

case LOGICAL_OR_OP:
return string("LOGICAL_OR_OP");

case BITWISE_AND_OP:
return string("BITWISE_AND_OP");

case BITWISE_OR_OP:
return string("BITWISE_OR_OP");

case BITWISE_XOR_OP:
return string("BITWISE_XOR_OP");

case BITSHIFT_LEFT_OP:
return string("BITSHIFT_LEFT_OP");

case BITSHIFT_RIGHT_OP:
return string("BITSHIFT_RIGHT_OP");

case EQUALS_EQUALS_OP:
return string("EQUALS_EQUALS_OP");

case NOT_EQUALS_OP:
return string("NOT_EQUALS_OP");

case GREATER_THAN_OP:
return string("GREATER_THAN_OP");

case GREATER_THAN_EQUALS_OP:
return string("GREATER_THAN_EQUALS_OP");

case LESS_THAN_OP:
return string("LESS_THAN_OP");

case LESS_THAN_EQUALS_OP:
return string("LESS_THAN_EQUALS_OP");

case NOT_OP:
return string("NOT_OP");

case BITWISE_NOT_OP:
return string("BITWISE_NOT_OP");

case UNARY_MINUS_OP:
return string("UNARY_MINUS_OP");

case SET_PARAMETER:
return string("SET_PARAMETER");

case BRANCH_ON_FALSE_LITERAL:
return string("BRANCH_ON_FALSE_LITERAL");

case BRANCH_ON_FALSE_REG_VAL:
return string("BRANCH_ON_FALSE_REG_VAL");

case BRANCH_ON_FALSE_MEM_ADDR:
return string("BRANCH_ON_FALSE_MEM_ADDR");

case JUMP_TO_MAIN:
return string("JUMP_TO_MAIN");

case EXIT_MAIN:
return string("EXIT_MAIN");

case OUT_OF_BOUNDS_FAULT:
return string("OUT_OF_BOUNDS_FAULT");

case PRINT_OP:
return string("PRINT_OP");

case GET_INT_OP:
return string("GET_INT_OP");

case GET_DOUBLE_OP:
return string("GET_DOUBLE_OP");

case GET_CHAR_OP:
return string("GET_CHAR_OP");

case GET_BOOLEAN_OP:
return string("GET_BOOLEAN_OP");

case GET_WORD_OP:
return string("GET_WORD_OP");

case GET_LINE_OP:
return string("GET_LINE_OP");

case IS_INPUT_FAIL_OP:
return string("IS_INPUT_FAIL_OP");

default:
std::cout << "Error: Invalid operator code:" << myCode << std::endl;
exit(1);
return "";
}
}

string getOpTypeString(Op_Type myType)
{
switch(myType)
{
case INTEGER_T:
return string("INTEGER_T");

case DOUBLE_T:
return string("DOUBLE_T");

case CHAR_T:
return string("CHAR_T");

case BOOLEAN_T:
return string("BOOLEAN_T");

case STRING_T:
return string("STRING_T");

case ARRAY_T:
return string("ARRAY_T");

case LIST_T:
return string("LIST_T");

case DICT_T:
return string("DICT_T");

case QUEUE_T:
return string("QUEUE_T");

case USER_OBJ_T:
return string("USER_OBJ_T");

case NULL_T:
return string("NULL_T");

default:
std::cout << "Error: unknown op type encountered: " << myType << std::endl;
exit(1);
return "";
}
}

#endif
