#include "runTimeObject.h"
#include <fstream>
#include <string>

void runTimeObject::runProgram()
{
if(byteCodeStart == NULL)
{
std::cout << "Error: Bytecode file could not be accessed or read by this program. Please check your permissions and file names, and then try running again.\nUsage: Lobster myFile.lobster\n";
exit(1);
}

bool debugger = false;

Op_Type exitType;
Op_Type secondOpType;
Op_Code myCode;
Builtin_Function myFuncType;
UserDefinedObject* newUserObj = NULL;
varObj* myNewVarObj;
long long firstInt;
double firstDouble;
bool firstBool;
unsigned char firstChar;

long long** tempPointer = NULL;
long long secondInt;
double secondDouble;
bool secondBool;
unsigned char secondChar;

long long resultInt;
double resultDouble;
bool resultBool;
unsigned char resultChar;


unsigned char firstRegOffset;
unsigned char secondRegOffset;
unsigned char thirdRegOffset;
int nextSpot = 0;
Op_Type firstOpType;
Source_Location firstLocation;
Source_Location secondLocation;
Source_Location thirdLocation;

int intLit;
double doubleLit;
bool boolLit;
char charLit;
long long regContents;
std::string tempString;
while(true)
{
myCode = *((Op_Code*)&byteCodeStart[pc]);

switch(myCode)
{
case GLOBAL_DECLARE_OP:
firstInt = readInt(byteCodeStart, pc + 1);
(*globalFrame).addVariable(firstInt);
pc += BYTES_PER_LINE;
break;

case STACK_VARIABLE_DECLARE:
firstInt = readInt(byteCodeStart, pc + 1);
callStack.addVariable(firstInt);
pc += BYTES_PER_LINE;
break;


case PIPELINE_DECLARE:
firstInt = readInt(byteCodeStart, pc + 1);
(*pipeLineMap)[firstInt] = PipeLineStruct();
pc += BYTES_PER_LINE;
break;

case PIPELINE_ADD_FUNCTION:
firstInt = readInt(byteCodeStart, pc + 1);
secondInt = readInt(byteCodeStart, pc + 5);
(*pipeLineMap)[firstInt].functionList.push_back(secondInt);
pc += BYTES_PER_LINE;
break;

////void writeAssignment(unsigned char regMemVal, Op_Type leftType, Source_Location rightSourceLocation, void* inputArg)
case ASSIGNMENT_OP:
firstRegOffset = byteCodeStart[pc + 1];
firstOpType = *((Op_Type*)(&byteCodeStart[pc + 2]));
firstLocation = *((Source_Location*) (&byteCodeStart[pc + 3]));

	//{REGISTER_VAL, REGISTER_MEM_ADDR, LITERAL_L, CODE_LOCATION };
	switch(firstLocation)
	{
		case REGISTER_VAL:
		case REGISTER_MEM_ADDR:
		secondRegOffset = byteCodeStart[pc + 4];
		regContents = registerArray[secondRegOffset];

		switch(firstOpType)
		{
		case USER_OBJ_T:
		case ARRAY_T:
		case LIST_T:
		case QUEUE_T:
		*((long long**)(registerArray[firstRegOffset])) = *((long long**)regContents);
		break;
		
		default:
		*((long long *)(registerArray[firstRegOffset])) = regContents;
		break;
		}
		break;
	
		case LITERAL_L:
			//INTEGER_T, CHAR_T, BOOLEAN_T, DOUBLE_T, STRING_T, ARRAY_T, LIST_T, DICT_T, QUEUE_T, USER_OBJ_T, NULL_T 
			switch(firstOpType)
			{
				case DOUBLE_T:
				doubleLit = readDouble(byteCodeStart, pc + 4);
				*((double*)(registerArray[firstRegOffset])) = doubleLit;
				break;

				case CHAR_T:
			        *((char*)(registerArray[firstRegOffset])) = byteCodeStart[pc + 4];
				break;
	
				case BOOLEAN_T:
				boolLit = readBoolean(byteCodeStart, pc + 4);
				*((bool*)(registerArray[firstRegOffset])) = boolLit;
				break;

				case NULL_T:
				*((long long*)(registerArray[firstRegOffset])) = 0;
				break;

				default:
				intLit = readInt(byteCodeStart, pc + 4);
                                *((long long*)(registerArray[firstRegOffset])) = intLit;
                                break;	
			}
		break;

		case CODE_LOCATION:
		firstInt = readInt(byteCodeStart, pc + 4);
		*((long long*)(registerArray[firstRegOffset])) = firstInt;
		break;
	}

pc += BYTES_PER_LINE;
break;

case BUILTIN_OBJECT_CREATE_CODE:
firstOpType = *((Op_Type*)&byteCodeStart[pc + 1]);
secondOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
registerArray[RETURN_REGISTER_NUM] = (long long) builtinCreateFunc(firstOpType, secondOpType);
pc += BYTES_PER_LINE;
break;

case JUMP_TO:
firstInt = readInt(byteCodeStart, pc + 1);
pc = firstInt;
break;


case NEW_USER_OBJ:
case CALL_NON_MEMBER_FUNCTION:
case CALL_MEMBER_FUNCTION:
firstInt = readInt(byteCodeStart, pc + 1);
functionReturnLocations.push(pc + BYTES_PER_LINE);
pc = firstInt;
callStack.addBlock();
break;


//void writeCallBuiltinFunctionInstruction(unsigned char regWithObjAddr, Op_Type objType, Op_Type innerType, Builtin_Function funcName)
case CALL_BUILTIN_FUNCTION:
firstRegOffset = byteCodeStart[pc + 1];
firstOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
secondOpType = *((Op_Type*)&byteCodeStart[pc + 3]);
myFuncType = *((Builtin_Function*)&byteCodeStart[pc + 4]);
builtinFunctionExecute(firstRegOffset, firstOpType, secondOpType, myFuncType);
pc += BYTES_PER_LINE;
break;

case CONVERT_INT_TO_DOUBLE:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
if(debugger) std::cout << "firstReg contents: " << registerArray[firstRegOffset] << " secondReg contents: " << registerArray[secondRegOffset] <<  " Also, firstRegOffset = " << (int) firstRegOffset << " and secondRegOffset = " << (int) secondRegOffset << std::endl;
if(debugger) std::cout << "treated as an int, registerArray[secondRegOffsrt] == " << *((long long*)&registerArray[secondRegOffset]) << std::endl;
*((double*)&registerArray[firstRegOffset]) = *((long long*)&registerArray[secondRegOffset]);
if(debugger) std::cout << "after conversion, registerArray[firstRegOffset] as a double is " << *((double*)&registerArray[firstRegOffset]) << std::endl;
pc += BYTES_PER_LINE;
break;

case CONVERT_INT_TO_CHAR:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((unsigned char*)&registerArray[firstRegOffset]) = *((long long*)&registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;

case CONVERT_DOUBLE_TO_INT:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((long long*)&registerArray[firstRegOffset]) = *((double*)&registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;

case CONVERT_DOUBLE_TO_CHAR:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((unsigned char*)&registerArray[firstRegOffset]) = *((double*)&registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;

case CONVERT_CHAR_TO_INT:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((long long*)&registerArray[firstRegOffset]) = *((unsigned char*)&registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;

case CONVERT_CHAR_TO_DOUBLE:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((double*)&registerArray[firstRegOffset]) = *((unsigned char*)&registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;


//void writeSetFunctionArgumentInstruction(unsigned char functionArgRegister, Op_Type sourceType, Source_Location sourceLocation, void* sourceArg)
case SET_FUNCTION_ARGUMENT:
firstRegOffset = byteCodeStart[pc + 1];
firstOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
firstLocation = *((Source_Location*)&byteCodeStart[pc + 3]);
 switch(firstLocation)
        {
                case REGISTER_VAL:
                case REGISTER_MEM_ADDR:
                secondRegOffset = byteCodeStart[pc + 4];
                registerArray[firstRegOffset] = registerArray[secondRegOffset];
                break;

                case LITERAL_L:
                        //INTEGER_T, CHAR_T, BOOLEAN_T, DOUBLE_T, STRING_T, ARRAY_T, LIST_T, DICT_T, QUEUE_T, USER_OBJ_T, NULL_T 
                        switch(firstOpType)
                        {
                                case DOUBLE_T:
                                *((double*)&registerArray[firstRegOffset]) = readDouble(byteCodeStart, pc + 4);
                                break;

                                case CHAR_T:
                                *((unsigned char*)&registerArray[firstRegOffset]) = byteCodeStart[pc + 4];
                                break;

                                case BOOLEAN_T:
                                boolLit = readBoolean(byteCodeStart, pc + 4);
                                *((bool*)&registerArray[firstRegOffset]) = boolLit;
                                break;

                                case NULL_T:
                                registerArray[firstRegOffset] = 0;
                                break;

                                default:
                                registerArray[firstRegOffset] = readInt(byteCodeStart, pc + 4);
                                break;
                        }
                break;

                case CODE_LOCATION:
                registerArray[firstRegOffset] = readInt(byteCodeStart, pc + 4);
                break;
        }

pc += BYTES_PER_LINE;
break;

case SAVE_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
restorationStacks[firstRegOffset].push(registerArray[firstRegOffset]);
pc += BYTES_PER_LINE;
break;

case RESTORE_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
registerArray[firstRegOffset] = restorationStacks[firstRegOffset].top();
restorationStacks[firstRegOffset].pop();
pc += BYTES_PER_LINE;
break;

case COPY_REGISTER_TO_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
registerArray[firstRegOffset] = registerArray[secondRegOffset];
pc += BYTES_PER_LINE;
break;

case COPY_INT_LITERAL_TO_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
intLit = readInt(byteCodeStart, pc + 2);
registerArray[firstRegOffset] = intLit;
pc += BYTES_PER_LINE;
break;

case COPY_CHAR_LITERAL_TO_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
charLit = byteCodeStart[pc + 2];
*((unsigned char*)&registerArray[firstRegOffset]) = charLit;
pc += BYTES_PER_LINE;
break;

case COPY_DOUBLE_LITERAL_TO_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
doubleLit = readDouble(byteCodeStart, pc + 2);
*((double*)&registerArray[firstRegOffset]) = doubleLit;
pc += BYTES_PER_LINE;
break;

case COPY_BOOLEAN_LITERAL_TO_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
boolLit = readBoolean(byteCodeStart, pc + 2);
*((bool*)&registerArray[firstRegOffset]) = boolLit;
pc += BYTES_PER_LINE;
break;

case COPY_STRING_LOCATION_TO_REGISTER:
firstRegOffset = byteCodeStart[pc + 1];
intLit = readInt(byteCodeStart, pc + 2);
registerArray[firstRegOffset] = intLit;
pc += BYTES_PER_LINE;
break;

case RETURN_FROM_FUNCTION:
pc = functionReturnLocations.top();
functionReturnLocations.pop();
callStack.deleteBlock();
break;

//DEST_REG, IDENTIFIER_NUMBER
case MOVE_STACK_ADDR_TO_REG:
firstRegOffset = byteCodeStart[pc + 1];
intLit = readInt(byteCodeStart, pc + 2);
registerArray[firstRegOffset] = (long long)(callStack.getAddressOfVariable(intLit));
pc += BYTES_PER_LINE;
break;

case MOVE_GLOBAL_ADDR_TO_REG:
firstRegOffset = byteCodeStart[pc + 1];
intLit = readInt(byteCodeStart, pc + 2);
registerArray[firstRegOffset] = (long long)((*globalFrame).getAddressOfVariable(intLit));
pc += BYTES_PER_LINE;
break;

case GET_MEMBER_VAR_ADDR_OF_OBJ:
//void writeGetMemberVarAddrOfObjInstruction(unsigned char destReg, unsigned char objReg, unsigned int offsetIntoObject)
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
//if(debugger)
//std::cout << "Base Address of object - " << registerArray[secondRegOffset] << std::endl;
intLit = readInt(byteCodeStart, pc + 3);
//if(debugger)
//std::cout << "Offset: " << intLit << std::endl;
if(*((long long*)registerArray[secondRegOffset]) == 0)
{
std::cout << "Error: An exception has occured: Member variable was accessed without first calling the constructor for this object. In bytecode, this error occured on line " << pc / BYTES_PER_LINE << std::endl;
exit(1);
}
*((long long**)&registerArray[firstRegOffset]) = *((long long**)registerArray[secondRegOffset]) + intLit;
if(debugger) std::cout << "In getAddressOfMemberVariable. Address stored in register is " << *((long long**)&registerArray[firstRegOffset]) << std::endl;
//if(debugger)
//std::cout << "Address of member variable - " << registerArray[firstRegOffset] << std::endl;
pc += BYTES_PER_LINE;
break;

case ADD_MEMBER_VARS_TO_OBJ:
intLit = readInt(byteCodeStart, pc + 1);
newUserObj = new UserDefinedObject(intLit);
myNewVarObj = new varObj();
myNewVarObj->myVal = (long long)(newUserObj->internalArray);
if(debugger) std::cout << "Address of myNewVarObj->myVal - " << &(myNewVarObj->myVal) << " Treating myVal as a pointer, it equals - " << ((long long**)myNewVarObj->myVal) << " and the address of the internalArray is " <<  newUserObj->internalArray << " value of myNewVarObj->myVal = " << myNewVarObj->myVal << std::endl;
registerArray[THIS_REGISTER_NUM] = (long long) &(myNewVarObj->myVal);
if(debugger) std::cout << "Value of THIS register is now " << *((long long**)&registerArray[THIS_REGISTER_NUM]) << std::endl;
memoryLock->lock();
memoryAllocationList->push_front( (long long*)newUserObj);
memoryAllocationList->push_front( (long long*) myNewVarObj);
memoryLock->unlock();
pc += BYTES_PER_LINE;
break;


case DEREFERENCE_BOOLEAN:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((bool*)&registerArray[firstRegOffset]) = *((bool*)registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;

case DEREFERENCE_INTEGER:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
registerArray[firstRegOffset] = *((long long*)registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;

case DEREFERENCE_DOUBLE:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((double*)&registerArray[firstRegOffset]) = *((double*)registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;

case DEREFERENCE_CHAR:
firstRegOffset = byteCodeStart[pc + 1];
secondRegOffset = byteCodeStart[pc + 2];
*((unsigned char*)&registerArray[firstRegOffset]) = *((unsigned char*)registerArray[secondRegOffset]);
pc += BYTES_PER_LINE;
break;


case PLUS_EQUALS_OP:
case MINUS_EQUALS_OP:
case TIMES_EQUALS_OP:
case DIVIDE_EQUALS_OP:
case MOD_EQUALS_OP:
case POW_EQUALS_OP:
//void writeCompoundAssignmentInstruction(tokenType typeOfCompound, unsigned char sourceReg, Op_Type leftType, Source_Location rightLocation, void* rightArg)
firstRegOffset = byteCodeStart[pc + 1];
firstOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
secondLocation = *((Source_Location*)&byteCodeStart[pc + 3]);

/*
if(debugger)
{
std::cout << "In compound assignment OP: firstRegOffset = " << firstRegOffset << " firstOpType = " << firstOpType << " and secondLocation = " << secondLocation << std::endl;
}
*/
switch(firstOpType)
{
case DOUBLE_T:
parseArg(secondLocation, firstOpType, pc + 4, firstDouble);
break;

case CHAR_T:
parseArg(secondLocation, firstOpType, pc + 4, firstChar);
break;

case BOOLEAN_T:
parseArg(secondLocation, firstOpType, pc + 4, firstBool);
break;

default:
parseArg(secondLocation, firstOpType, pc + 4, firstInt);
break;
}

switch(myCode)
{
case PLUS_EQUALS_OP:
	switch(firstOpType)
	{
	case DOUBLE_T:
	*((double*)registerArray[firstRegOffset]) += firstDouble;
	break;

	case CHAR_T:
	*((unsigned char*)registerArray[firstRegOffset]) += firstChar;
	break;

	case BOOLEAN_T:
	*((bool*)registerArray[firstRegOffset]) += firstBool;
	break;

	default:
	/*
	if(debugger)
	{
	std::cout << "in Plus_Equals_Op: value of address is " << registerArray[firstRegOffset] << " and var val is " << *((long long*)registerArray[firstRegOffset]) << std::endl;

	}
*/
	*((long long*)(registerArray[firstRegOffset])) = (*((long long*)(registerArray[firstRegOffset])) + firstInt);
/*
	if(debugger)
	{
	std::cout << "after PLUS_EQUALS_OP, value of address is " << registerArray[firstRegOffset] << " and var val is " << *((long long*)registerArray[firstRegOffset]) << " and firstInt = " << firstInt << std::endl;
	}
*/
	break;
	}
break;

case MINUS_EQUALS_OP:
	switch(firstOpType)
	{
	case DOUBLE_T:
	*((double*)registerArray[firstRegOffset]) -= firstDouble;
	break;

	case CHAR_T:
	*((unsigned char*)registerArray[firstRegOffset]) -= firstChar;
	break;

	case BOOLEAN_T:
	*((bool*)registerArray[firstRegOffset]) -= firstBool;
	break;

	default:
	*((long long*)registerArray[firstRegOffset]) -= firstInt;
	break;
	}
break;


case TIMES_EQUALS_OP:
	switch(firstOpType)
	{
	case DOUBLE_T:
	*((double*)registerArray[firstRegOffset]) *= firstDouble;
	break;

	case CHAR_T:
	*((unsigned char*)registerArray[firstRegOffset]) *= firstChar;
	break;

	case BOOLEAN_T:
	*((bool*)registerArray[firstRegOffset]) *= firstBool;
	break;

	default:
	*((long long*)registerArray[firstRegOffset]) *= firstInt;
	break;
	}
break;

case DIVIDE_EQUALS_OP:
	switch(firstOpType)
	{
	case DOUBLE_T:
	*((double*)registerArray[firstRegOffset]) /= firstDouble;
	break;

	case CHAR_T:
	*((unsigned char*)registerArray[firstRegOffset]) /= firstChar;
	break;

	case BOOLEAN_T:
	*((bool*)registerArray[firstRegOffset]) /= firstBool;
	break;

	default:
	*((long long*)registerArray[firstRegOffset]) /= firstInt;
	break;

	}
break;

case MOD_EQUALS_OP:
	switch(firstOpType)
	{
	case CHAR_T:
	*((unsigned char*)registerArray[firstRegOffset]) %= firstChar;
	break;

	case INTEGER_T:
	*((long long*)registerArray[firstRegOffset]) %= firstInt;
	break;

	default:
	std::cout << "Error: mod-equals can only be applied to types char and int\n";
	exit(1);
	break;
	}
break;

case POW_EQUALS_OP:
	switch(firstOpType)
	{
	case CHAR_T:
	*((unsigned char*)registerArray[firstRegOffset]) = std::pow( *((unsigned char*)registerArray[firstRegOffset]), firstChar);
	break;

	case BOOLEAN_T:
	std::cout << "Error: Booleans can't be applied in pow operator\n";
	exit(1);
	break;

	case DOUBLE_T:
	*((double*)registerArray[firstRegOffset]) = std::pow( *((double*)registerArray[firstRegOffset]), firstDouble);
	break;

	default:
	*((long long*)registerArray[firstRegOffset]) = std::pow( *((long long*)registerArray[firstRegOffset]), firstInt);
	break;
	}
break;

default:
std::cout << "Error: Unknown op in compound assignment\n";
exit(1);
break;
}

pc += BYTES_PER_LINE;
break;


case LOGICAL_AND_EQUALS_OP:
firstRegOffset = byteCodeStart[pc + 1];
firstOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
secondLocation = *((Source_Location*)&byteCodeStart[pc + 3]);
parseArg(secondLocation, firstOpType, 4, firstBool);
*((bool*)registerArray[firstRegOffset]) =  *((bool*)registerArray[firstRegOffset]) && firstBool;
pc += BYTES_PER_LINE;
break;

case LOGICAL_OR_EQUALS_OP:
firstRegOffset = byteCodeStart[pc + 1];
firstOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
secondLocation = *((Source_Location*)&byteCodeStart[pc + 3]);
parseArg(secondLocation, firstOpType, 4, firstBool);
*((bool*)registerArray[firstRegOffset]) = *((bool*)registerArray[firstRegOffset]) || firstBool;
pc += BYTES_PER_LINE;
break;


case PLUS_OP:
case MINUS_OP:
case DIVIDE_OP:
case MULTIPLY_OP:
case POW_OP:
case BITWISE_AND_OP:
case BITWISE_OR_OP:
case BITWISE_XOR_OP:
case BITSHIFT_LEFT_OP:
case BITSHIFT_RIGHT_OP:

//ARITH_INSTRUCTION_OP(1 bytes), sourceLocationDestination(1 byte), Op_Type_Of_Expr(1 byte), destReg(1 byte), sourceLocation1(1 byte), Reg/Literal/memReg1(1-4 bytes), sourceLocation2(1 byte), Reg/LiteralmemReg/Literal/Reg2(1 - 4 bytes)


firstLocation = *((Source_Location*)&byteCodeStart[pc + 1]);
firstOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
firstRegOffset = byteCodeStart[pc + 3];
secondLocation = *((Source_Location*)&byteCodeStart[pc + 4]);
nextSpot = pc + 5;

//int parseArg(Source_Location myLocation, Op_Type myType, int nextSpot, ArgType& returnVal)
switch(firstOpType)
{
case INTEGER_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstInt);
break;

case DOUBLE_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstDouble);
break;

case BOOLEAN_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstBool);
break;

case CHAR_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstChar);
break;

case NULL_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstInt);
break;

default:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstInt);
break;
}

thirdLocation = *((Source_Location*)&byteCodeStart[nextSpot]);
++nextSpot;

switch(firstOpType)
{
case INTEGER_T:
nextSpot = parseArg(thirdLocation, firstOpType, nextSpot, secondInt);
break;

case DOUBLE_T:
nextSpot = parseArg(thirdLocation, firstOpType, nextSpot, secondDouble);
break;

case BOOLEAN_T:
nextSpot = parseArg(thirdLocation, firstOpType, nextSpot, secondBool);
break;

case CHAR_T:
nextSpot = parseArg(thirdLocation, firstOpType, nextSpot, secondChar);
break;

case NULL_T:
nextSpot = parseArg(thirdLocation, firstOpType, nextSpot, secondInt);
break;

default:
nextSpot = parseArg(thirdLocation, firstOpType, nextSpot, secondInt);
break;
}

switch(firstOpType)
{
case INTEGER_T:
	switch(myCode)
	{
	case PLUS_OP:
	resultInt = firstInt + secondInt;
	break;

	case MINUS_OP:
	resultInt = firstInt - secondInt;
	break;

	case DIVIDE_OP: 
	resultInt = firstInt / secondInt;
	break;

	case MULTIPLY_OP:
	resultInt = firstInt * secondInt;
	break;

	case POW_OP:
	resultInt = std::pow(firstInt, secondInt);
	break;

	case BITWISE_AND_OP:
	resultInt = firstInt & secondInt;
	break;

	case BITWISE_OR_OP:
	resultInt = firstInt | secondInt;
	break;

	case BITWISE_XOR_OP:
	resultInt = firstInt ^ secondInt;
	break;

	case BITSHIFT_LEFT_OP:
	resultInt = firstInt << secondInt;
	break;

	case BITSHIFT_RIGHT_OP:
	resultInt = firstInt >> secondInt;
	break;
	
	default:
	std::cout << "Error: Unexpected result op in int\n";
	exit(1);
	}

	if(firstLocation == REGISTER_VAL)
		*((long long*)&registerArray[firstRegOffset]) = resultInt;
	else
		*((long long*)registerArray[firstRegOffset]) = resultInt;

	break;


case DOUBLE_T:
	switch(myCode)
	{

	case PLUS_OP:
	resultDouble = firstDouble + secondDouble;
	break;

	case MINUS_OP:
	resultDouble = firstDouble - secondDouble;
	break;

	case DIVIDE_OP:
	resultDouble = firstDouble / secondDouble;
	break;

	case MULTIPLY_OP:
	resultDouble = firstDouble * secondDouble;
	break;

	case POW_OP:
	resultDouble = std::pow(firstDouble, secondDouble);
	break;

	case BITWISE_AND_OP:
	std::cout << "Error: Bitwise and not allowed for type double\n";
	exit(1);
	break;

	case BITWISE_OR_OP:
	std::cout << "Error: Bitwise or not allowed for type double\n";
	exit(1);
	break;

	case BITWISE_XOR_OP:
	std::cout << "Error: Bitwise xor not allowed for type double\n";
	exit(1);
	break;

	case BITSHIFT_LEFT_OP:
	std::cout << "Error: Bitshift left operator not allowed for type double\n";
	exit(1);
	break;

	case BITSHIFT_RIGHT_OP:
	std::cout << "Error: Bitshift right operator not allowed for typedouble\n";
	exit(1);
	break;

	default:
	std::cout << "Error: Unexpected operator op in double\n";
	break;
	}

	if(firstLocation == REGISTER_VAL)
		*((double*)&registerArray[firstRegOffset]) = resultDouble;
	else
		*((double*)registerArray[firstRegOffset]) = resultDouble;
	break;


case BOOLEAN_T:
	switch(myCode)
	{
	case PLUS_OP:
	resultBool = firstBool + secondBool;
	break;

	case MINUS_OP:
	resultBool = firstBool - secondBool;
	break;

	case MULTIPLY_OP:
	resultBool = firstBool * secondBool;
	break;

	case DIVIDE_OP:
	resultBool = firstBool / secondBool;
	break;

	case POW_OP:
	resultBool = std::pow(firstBool, secondBool);
	break;

	case BITWISE_AND_OP:
	resultBool = firstBool & secondBool;
	break;

	case BITWISE_OR_OP:
	resultBool = firstBool | secondBool;
	break;

	case BITWISE_XOR_OP:
	resultBool = firstBool ^ secondBool;
	break;

	case BITSHIFT_LEFT_OP:
	resultBool = firstBool << secondBool;
	break;

	case BITSHIFT_RIGHT_OP:
	resultBool = firstBool >> secondBool;
	break;

	default:
	std::cout << "Error: Unexpected boolean ops\n";
	exit(1);
	}

	if(firstLocation == REGISTER_VAL)
		*((bool*)&registerArray[firstRegOffset]) = resultBool;
	else
		*((bool*)registerArray[firstRegOffset]) = resultBool;

	break;


case CHAR_T:
	switch(myCode)
	{
	case PLUS_OP:
	resultChar = firstChar + secondChar;
	break;

	case MINUS_OP:
	resultChar = firstChar - secondChar;
	break;

	case MULTIPLY_OP:
	resultChar = firstChar * secondChar;
	break;

	case DIVIDE_OP:
	resultChar = firstChar / secondChar;
	break;

	case POW_OP:
	resultChar = std::pow(firstChar, secondChar);
	break;

	case BITWISE_AND_OP:
	resultChar = firstChar & secondChar;
	break;

	case BITWISE_OR_OP:
	resultChar = firstChar | secondChar;
	break;

	case BITWISE_XOR_OP:
	resultChar = firstChar ^ secondChar;
	break;

	case BITSHIFT_LEFT_OP:
	resultChar = firstChar << secondChar;
	break;

	case BITSHIFT_RIGHT_OP:
	resultChar = firstChar >> secondChar;
	break;

	default:
	std::cout << "Error: Invalid char op\n";
	exit(1);
	}

	if(firstLocation == REGISTER_VAL)
		*((unsigned char*)&registerArray[firstRegOffset]) = resultChar;
	else
		*((unsigned char*)registerArray[firstRegOffset]) = resultChar;
	break;


default:
std::cout << "Error: Invalid types in operator expression\n";
exit(1);
}
pc += BYTES_PER_LINE;
break;


//handling boolean operators now.
case LOGICAL_AND_OP:
case LOGICAL_OR_OP:
case EQUALS_EQUALS_OP:
case NOT_EQUALS_OP:
case GREATER_THAN_OP:
case GREATER_THAN_EQUALS_OP:
case LESS_THAN_OP:
case LESS_THAN_EQUALS_OP:


firstLocation = *((Source_Location*)&byteCodeStart[pc + 1]);
firstOpType = *((Op_Type*)&byteCodeStart[pc + 2]);
firstRegOffset = byteCodeStart[pc + 3];
secondLocation = *((Source_Location*)&byteCodeStart[pc + 4]);
nextSpot = pc + 5;

switch(firstOpType)
{
case INTEGER_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstInt);
break;

case DOUBLE_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstDouble);
break;

case BOOLEAN_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstBool);
break;

case CHAR_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstChar);
break;

case NULL_T:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstInt);
break;

default:
nextSpot = parseArg(secondLocation, firstOpType, nextSpot, firstInt);
break;
}

thirdLocation = *((Source_Location*)&byteCodeStart[nextSpot]);
++nextSpot;

switch(firstOpType)
{
case INTEGER_T:
parseArg(thirdLocation, firstOpType, nextSpot, secondInt);
break;

case DOUBLE_T:
parseArg(thirdLocation, firstOpType, nextSpot, secondDouble);
break;

case BOOLEAN_T:
parseArg(thirdLocation, firstOpType, nextSpot, secondBool);
break;

case CHAR_T:
parseArg(thirdLocation, firstOpType, nextSpot, secondChar);
break;

case NULL_T:
parseArg(thirdLocation, firstOpType, nextSpot, secondInt);
break;

default:
parseArg(thirdLocation, firstOpType, nextSpot, secondInt);
break;
}

switch(firstOpType)
{

case BOOLEAN_T:
	switch(myCode)
	{
	case LOGICAL_AND_OP:
	resultBool = firstBool && secondBool;
	break;

	case LOGICAL_OR_OP:
	resultBool = firstBool || secondBool;
	break;

	case EQUALS_EQUALS_OP:
	resultBool = firstBool == secondBool;
	break;

	case NOT_EQUALS_OP:
	resultBool = firstBool != secondBool;
	break;

	case LESS_THAN_OP:
	resultBool = firstBool < secondBool;
	break;

	case GREATER_THAN_OP:
	resultBool = firstBool > secondBool;
	break;

	case LESS_THAN_EQUALS_OP:
	resultBool = firstBool <= secondBool;
	break;

	case GREATER_THAN_EQUALS_OP:
	resultBool = firstBool >= secondBool;
	break;

	default:
	std::cout << "Error: Unexpected boolean operator\n";
	exit(1);
	break;
	}

	if(firstLocation == REGISTER_VAL)
		*((bool*)&registerArray[firstRegOffset]) = resultBool;
	else
		*((bool*)registerArray[firstRegOffset]) = resultBool;
	break;



case CHAR_T:
	switch(myCode)
	{
	case LOGICAL_AND_OP:
	resultBool = firstChar && secondChar;
	break;

	case LOGICAL_OR_OP:
	resultBool = firstChar || secondChar;
	break;

	case LESS_THAN_OP:
	resultBool = firstChar < secondChar;
	break;

	case LESS_THAN_EQUALS_OP:
	resultBool = firstChar <= secondChar;
	break;

	case GREATER_THAN_OP:
	resultBool = firstChar > secondChar;
	break;

	case GREATER_THAN_EQUALS_OP:
	resultBool = firstChar >= secondChar;
	break;

	case EQUALS_EQUALS_OP:
	resultBool = firstChar == secondChar;
	break;

	case NOT_EQUALS_OP:
	resultBool = firstChar != secondChar;
	break;

	default:
	std::cout << "Error: Unexpected boolean operator\n";
	exit(1);
	}

	if(firstLocation == REGISTER_VAL)
		*((bool*)&registerArray[firstRegOffset]) = resultBool;
	else
		*((bool*)registerArray[firstRegOffset]) = resultBool;
	break;	



case DOUBLE_T:
        switch(myCode)
        {
        case LOGICAL_AND_OP:
        resultBool = firstDouble && secondDouble;
        break;

        case LOGICAL_OR_OP:
        resultBool = firstDouble || secondDouble;
        break;

        case EQUALS_EQUALS_OP:
        resultBool = firstDouble == secondDouble;
        break;

        case NOT_EQUALS_OP:
        resultBool = firstDouble != secondDouble;
        break;

        case LESS_THAN_OP:
        resultBool = firstDouble < secondDouble;
        break;

        case LESS_THAN_EQUALS_OP:
        resultBool = firstDouble <= secondDouble;
        break;

        case GREATER_THAN_OP:
        resultBool = firstDouble > secondDouble;
        break;

	case GREATER_THAN_EQUALS_OP:
        resultBool = firstDouble >= secondDouble;
        break;

        default:
        std::cout << "Error: Unknown boolean operator\n";
        exit(1);
        }

        if(firstLocation == REGISTER_VAL)
                *((bool*)&registerArray[firstRegOffset]) = resultBool;
        else
                *((bool*)registerArray[firstRegOffset]) = resultBool;
        break;

case NULL_T:
case INTEGER_T:
default:
	switch(myCode)
	{
	case LOGICAL_AND_OP:
	resultBool = firstInt && secondInt;
	break;

	case LOGICAL_OR_OP:
	resultBool = firstInt || secondInt;
	break;

	case EQUALS_EQUALS_OP:
	resultBool = firstInt == secondInt;
	break;

	case NOT_EQUALS_OP:
	resultBool = firstInt != secondInt;
	break;

	case GREATER_THAN_OP:
	resultBool = firstInt > secondInt;
	break;

	case GREATER_THAN_EQUALS_OP:
	resultBool = firstInt >= secondInt;
	break;

	case LESS_THAN_OP:
	resultBool = firstInt < secondInt;
	break;

	case LESS_THAN_EQUALS_OP:
	resultBool = firstInt <= secondInt;
	break;

	default:
	std::cout << "Error: Unknown boolean op\n";
	exit(1);
	}
	if(firstLocation == REGISTER_VAL)
		*((bool*)&registerArray[firstRegOffset]) = resultBool;
	else
		*((bool*)registerArray[firstRegOffset]) = resultBool;
	break;

}
pc += BYTES_PER_LINE;
break;

case SET_PARAMETER:
firstRegOffset = byteCodeStart[pc + 1];
firstInt = readInt(byteCodeStart, pc + 2);
callStack.addVariable(firstInt);
*((long long*)(callStack.getAddressOfVariable(firstInt))) = registerArray[firstRegOffset];
pc += BYTES_PER_LINE;
break;

case BRANCH_ON_FALSE_LITERAL:
if( *((bool*)&byteCodeStart[pc + 1]) == true)
pc += BYTES_PER_LINE;
else
{
pc = readInt(byteCodeStart, pc + 2);
}
break;

case BRANCH_ON_FALSE_REG_VAL:
firstRegOffset = byteCodeStart[pc + 1];
firstInt = readInt(byteCodeStart, pc + 2);
if( *((bool*)&registerArray[firstRegOffset]) == true)
pc += BYTES_PER_LINE;
else 
pc = firstInt;
break;

case BRANCH_ON_FALSE_MEM_ADDR:
firstRegOffset = byteCodeStart[pc + 1];
firstInt = readInt(byteCodeStart, pc + 2);
if( *((bool*)(registerArray[firstRegOffset])) == true)
pc += BYTES_PER_LINE;
else
pc = firstInt;
break;


case NOT_OP:
case BITWISE_NOT_OP:
case UNARY_MINUS_OP:
//void writeUnaryOpInstruction(tokenType unaryOp, Source_Location outputLocation, unsigned char destinationReg, Op_Type inputType, Source_Location inputLocation, void* inputArg)
firstLocation = *((Source_Location*)&byteCodeStart[pc + 1]);
firstRegOffset = byteCodeStart[pc + 2];
firstOpType = *((Op_Type*)&byteCodeStart[pc + 3]);
secondLocation = *((Source_Location*)&byteCodeStart[pc + 4]);

switch(firstOpType)
{
case DOUBLE_T:
parseArg(secondLocation, firstOpType, 5, firstDouble);
break;

case CHAR_T:
parseArg(secondLocation, firstOpType, 5, firstChar);
break;

case BOOLEAN_T:
parseArg(secondLocation, firstOpType, 5, firstBool);
break;

case INTEGER_T:
parseArg(secondLocation, firstOpType, 5, firstInt);
break;

default:
std::cout << "Error: Expected int, double, char or bool for unary op arg\n";
exit(1);
break;
}

switch(myCode)
{
case NOT_OP:
	switch(firstOpType)
	{
	case BOOLEAN_T:
	if(firstLocation == REGISTER_VAL)
		*((bool*)&registerArray[firstRegOffset]) = !firstBool;
	else
		*((bool*)registerArray[firstRegOffset]) = !firstBool;
	break;

	default:
	std::cout << "Error: Type of bool must be in not op\n";
	exit(1);
	break;
	}
break;

case BITWISE_NOT_OP:
	switch(firstOpType)
	{
	case INTEGER_T:
	if(firstLocation == REGISTER_VAL)
		*((long long*)&registerArray[firstRegOffset]) = ~ firstInt;
	else
		*((long long*)registerArray[firstRegOffset]) = ~ firstInt;
	break;

	case DOUBLE_T:
	std::cout << "Error: Bitwise Not operator is invalid for type DOUBLE\n";
	exit(1);	
	break;

	case CHAR_T:
	if(firstLocation == REGISTER_VAL)
		*((unsigned char*)&registerArray[firstRegOffset]) = ~ firstChar;
	else
		*((unsigned char*)registerArray[firstRegOffset]) = ~ firstChar;
	break;

	case BOOLEAN_T:
	if(firstLocation == REGISTER_VAL)
		*((bool*)&registerArray[firstRegOffset]) = ~ firstBool;
	else
		*((bool*)registerArray[firstRegOffset]) = ~ firstBool;
	break;

	default:
	std::cout << "Error: Unknown op type in bitwise not\n";
	exit(1);
	break;
	}
break;

case UNARY_MINUS_OP:
	switch(firstOpType)
	{
	case INTEGER_T:
	if(firstLocation == REGISTER_VAL)
		*((long long*)&registerArray[firstRegOffset]) = -firstInt;
	else
		*((long long*)registerArray[firstRegOffset]) = -firstInt;
	break;


	case DOUBLE_T:
	if(firstLocation == REGISTER_VAL)
		*((double*)&registerArray[firstRegOffset]) = -firstDouble;
	else
		*((double*)registerArray[firstRegOffset]) = -firstDouble;
	break;

	case CHAR_T:
	if(firstLocation == REGISTER_VAL)
		*((unsigned char*)&registerArray[firstRegOffset]) = -firstChar;
	else
		*((unsigned char*)registerArray[firstRegOffset]) = -firstChar;
	break;	

	case BOOLEAN_T:
	if(firstLocation == REGISTER_VAL)
		*((bool*)&registerArray[firstRegOffset]) = -firstBool;
	else
		*((bool*)registerArray[firstRegOffset]) = -firstBool;
	break;

	default:
	std::cout << "Error: Unknown op type encountered in unary minus op\n";
	exit(1);
	break;


	}
break;

default:
std::cout << "Error: Unknown unary op encountered\n";
exit(1);
break;


}

pc += BYTES_PER_LINE;
break;

case PRINT_OP:
firstOpType = *((Op_Type*)&byteCodeStart[pc + 1]);
firstRegOffset = byteCodeStart[pc + 2];
int tempPC;
////INTEGER_T, CHAR_T, BOOLEAN_T, DOUBLE_T, STRING_T, ARRAY_T, LIST_T, DICT_T, QUEUE_T, USER_OBJ_T, NULL_T 
switch(firstOpType)
{
	case DOUBLE_T:
	std::cout << *((double*)&registerArray[firstRegOffset]);
	break;

	case CHAR_T:
	std::cout << *((unsigned char*)&registerArray[firstRegOffset]);
	break;

	case BOOLEAN_T:
	if(*((bool*)&registerArray[firstRegOffset]) == false)
		std::cout << "FALSE";
	else
		std::cout << "TRUE";
	break;
	
	case INTEGER_T:
	std::cout << registerArray[firstRegOffset];
	break;

	case NULL_T:
	std::cout << "NULL";
	break;


	case STRING_T:
	tempPC = registerArray[firstRegOffset];
	while(byteCodeStart[tempPC] != '\0')
		{
		std::cout << byteCodeStart[tempPC];
		++tempPC;
		}
	break;

	//this is some type of memory address
	default:
	if(registerArray[firstRegOffset] == 0)
		std::cout << "NULL";
	else
		std::cout << (long long*)registerArray[firstRegOffset];
	break;
}
pc += BYTES_PER_LINE;
break;

case GET_INT_OP:
firstRegOffset = byteCodeStart[pc + 1];
std::cin >> registerArray[firstRegOffset];
pc += BYTES_PER_LINE;
break;

case GET_DOUBLE_OP:
firstRegOffset = byteCodeStart[pc + 1];
std::cin >> *((double*)&registerArray[firstRegOffset]);
pc += BYTES_PER_LINE;
break;

case GET_CHAR_OP:
firstRegOffset = byteCodeStart[pc + 1];
std::cin >> *((unsigned char*)&registerArray[firstRegOffset]);
pc += BYTES_PER_LINE;
break;

case GET_BOOLEAN_OP:
firstRegOffset = byteCodeStart[pc + 1];
std::cin >> *((bool*)&registerArray[firstRegOffset]);
pc += BYTES_PER_LINE;
break;

case GET_WORD_OP:
firstRegOffset = byteCodeStart[pc + 1];
std::cin >> tempString;
registerArray[firstRegOffset] = (long long) tempString.c_str();
break;

case GET_LINE_OP:
firstRegOffset = byteCodeStart[pc + 1];
std::getline(std::cin, tempString);
registerArray[firstRegOffset] = (long long) tempString.c_str();
break;

case IS_INPUT_FAIL_OP:
firstRegOffset = byteCodeStart[pc + 1];
*((bool*)&registerArray[firstRegOffset]) = std::cin.fail();
pc += BYTES_PER_LINE;
break;

case JUMP_TO_MAIN:
firstInt = readInt(byteCodeStart, pc + 1);
pc = firstInt;
callStack.addBlock();
break;

case EXIT_MAIN:
callStack.deleteBlock();
exitType = *((Op_Type*)&byteCodeStart[pc + 1]);
if(exitType == NULL_T)
exit(0);
exit(registerArray[RETURN_REGISTER_NUM]);
break;

case OUT_OF_BOUNDS_FAULT:
std::cout << "Error: End of non-void function was hit without returning anything. Please check your syntax and recompile your program\n";
exit(1);
break;

default:
std::cout << "Instruction not implemented yet...\n";
break;
}

continue;
}

return;

}

int main(int argc, char** argv)
{
if(argc != 2)
{
std::cerr << "Usage: Lobster myProgram.Lobster\n";
exit(1);
}

std::ifstream inputStream(argv[1], std::ios::in);
if(! inputStream)
{
std::cerr << "Error opening file " << argv[1] << std::endl;
exit(1);
}

std::mutex* myLock = new std::mutex();
std::unordered_map<int, PipeLineStruct>* pipeMapObj = new std::unordered_map<int, PipeLineStruct>();

std::list<long long*>* memList = new std::list<long long*>();
inputStream.seekg(0, inputStream.end);
int fileSize = inputStream.tellg();
inputStream.seekg(0, inputStream.beg);
unsigned char* myByteCode = new unsigned char[fileSize];
inputStream.read((char*)myByteCode, fileSize);
inputStream.close();
stackTableStructure* myGlobalFrame = new stackTableStructure();
myGlobalFrame->addBlock();
runTimeObject mainRunTimeObject = runTimeObject(true, myGlobalFrame, memList, myLock, pipeMapObj);
mainRunTimeObject.byteCodeStart = myByteCode;
mainRunTimeObject.runProgram();
return 0;
}
