#ifndef CODE_GEN_PAGE
#define CODE_GEN_PAGE
//This class takes as input a parse tree and a symbol table which represent the structure of a program
//this class converts the code into the appropriate Lobster bytecode. If isVerbose is true,
//then the program code is outputted in pseudocode (which is readable to a human, but not runnable)
//Otherwise, the output of the program is in bytecode

//NOTE: all assignments and uses of string literals initially have their field for address of string literal set to be the unique identifier number of the string literal in the symbol table. After all other code is writen, the string literals of the program are outputted at the bottom of the code. Then, the CodeGenerator goes back to the top of the code and replaces all sections with the unique identifier number with the integer representing the actual starting location of the string literal.

#include <vector>
#include "ParseTree.h"
#include "SymbolTable.h"
#include <iostream>
#include "InstrAndMisc.h"
#include "lexicalAnalyzer.h"
#include <string>
#include <cstring>
#include <bitset>
#include <cstdlib>

using namespace std;


class CodeGenerator
{
public:
CodeGenerator(parseTree& newParse, SymbolTable& myTable, bool newVerbose)
{
srand(time(NULL));
myParseTree = newParse;
symbolTable = myTable;
myByteCode = std::vector<unsigned char>();
stringLiterals = std::vector<unsigned char>();
isVerbose = newVerbose;
mainLocation = -1;
lastLineSpot = -1;
lineNumber = 0;
for(int i = 0; i < 256; ++i)
{
isInUseArray[i] = false;
isNeededArray[i] = false;
contentsOfRegisters[i] = -1;
}
}

void absoluteIntegerSet(int absolutePos, int ValueToWrite)
{
bool debugger = false;
if(debugger)
std::cout << "just entered absoluteIntegerSet()\n";
unsigned char* topMostPointer = ((unsigned char*) &ValueToWrite);
unsigned char* secondMostPointer = ((unsigned char*) &ValueToWrite) + 1;
unsigned char* thirdMostPointer = ((unsigned char*) &ValueToWrite) + 2;
unsigned char* lastPointer = ((unsigned char*) &ValueToWrite) + 3;
myByteCode[absolutePos] = *topMostPointer;
myByteCode[absolutePos + 1] = *secondMostPointer;
myByteCode[absolutePos + 2] = *thirdMostPointer;
myByteCode[absolutePos + 3] = *lastPointer;
}

//myCode is an opcode which will be set on the first 2 bytes of codeArray
unsigned int opCodeSet(unsigned char codeArray[BYTES_PER_LINE], Op_Code myCode, unsigned int startPos)
{
unsigned char* myPointer = ((unsigned char*) &myCode);
codeArray[startPos] = *myPointer;

return startPos + 1;
}

//codeArray is the line of code.
//startPos is where to start writing to the array
//valueToWrite is the value to put into codeArray
unsigned int integerSet(unsigned char codeArray[BYTES_PER_LINE], unsigned int startPos, int ValueToWrite)
{
if(BYTES_PER_LINE - startPos < 4)
{
std::cout << "Error: startPos was " << startPos << std::endl;
exit(1);
}
unsigned char* topMostPointer = ((unsigned char*) &ValueToWrite);
unsigned char* secondMostPointer = ((unsigned char*) &ValueToWrite) + 1;
unsigned char* thirdMostPointer = ((unsigned char*) &ValueToWrite) + 2;
unsigned char* lastPointer = ((unsigned char*) &ValueToWrite) + 3;
codeArray[startPos] = *topMostPointer;
codeArray[startPos + 1] = *secondMostPointer;
codeArray[startPos + 2] = *thirdMostPointer;
codeArray[startPos + 3] = *lastPointer;

return startPos + 4;
}

unsigned int doubleSet(unsigned char codeArray[BYTES_PER_LINE], unsigned int startPos, float myValue)
{
if(BYTES_PER_LINE - startPos < 4)
{
std::cout << "Error: Not enough bytes left in doubleSet()\n";
exit(1);
}
unsigned char* topMostPointer = ((unsigned char*) &myValue);
unsigned char* secondMostPointer = ((unsigned char*) &myValue) + 1;
unsigned char* thirdMostPointer = ((unsigned char*) &myValue) + 2;
unsigned char* lastPointer = ((unsigned char*) &myValue) + 3;
codeArray[startPos] = *topMostPointer;
codeArray[startPos + 1] = *secondMostPointer;
codeArray[startPos + 2] = *thirdMostPointer;
codeArray[startPos + 3] = *lastPointer;

return startPos + 4;
}

//instrPos is the start position of the desired instruction to be altered
//jumpNum is the spot that the jump should be set to.
void setJump(int instrPos, int jumpNum)
{
bool debugger = false;
if(debugger)
std::cout << "WE ARE IN SETJUMP()!!!\n";
unsigned char tempByteArray[BYTES_PER_LINE];

for(int i = 0; i < BYTES_PER_LINE; ++i)
{
tempByteArray[i] = myByteCode[instrPos + i];
}

int nextSpotInLine = 1;
nextSpotInLine = integerSet(tempByteArray, nextSpotInLine, jumpNum);
fillRest(tempByteArray, nextSpotInLine);


for(int i = 0; i < BYTES_PER_LINE; ++i)
{
myByteCode[i + instrPos] = tempByteArray[i];
}

return;
}

//call on BRANCH_ON_FALSE_REG_VAL
void setBranch(int instrPos, int jumpLocation)
{
bool debugger = false;
if(debugger)
std::cout << "in setBranch() with instrPos of " << instrPos << " and jumpLocation of " << jumpLocation << std::endl;
unsigned char tempByteArray[BYTES_PER_LINE];

for(int i = 0; i < BYTES_PER_LINE; ++i)
{
tempByteArray[i] = myByteCode[i + instrPos];
if(debugger)
std::cout << bitset<8>(tempByteArray[i]) << ", ";
}

if(debugger)
std::cout << std::endl;
int nextSpotInLine = 2;
nextSpotInLine = integerSet(tempByteArray, nextSpotInLine, jumpLocation);
fillRest(tempByteArray, nextSpotInLine);

for(int i = 0; i < BYTES_PER_LINE; ++i)
{

if(debugger)
std::cout << "Value of myByteCode[i + instrPos] before assignment: " << (int)myByteCode[i + instrPos];
myByteCode[i + instrPos] = tempByteArray[i];
if(debugger)
std::cout << "\tValue of myByteCode[i + instrPos] after assignment: " << (int)myByteCode[i + instrPos] << std::endl;
}

if(debugger)
std::cout << std::endl;
return;
}

void fillRest(unsigned char codeArray[BYTES_PER_LINE], unsigned int startPos)
{
bool debugger = false;
if(debugger)
std::cout << "in fillRest() with startPos of " << startPos << "\n";
for(int i = startPos; i < (int)BYTES_PER_LINE; ++i)
{
codeArray[i] = ((unsigned char) 0);
}
}

void writeLine(unsigned char codeArray[BYTES_PER_LINE])
{

for(int i = 0; i < BYTES_PER_LINE; ++i)
{
myByteCode.push_back(codeArray[i]);
}
}

Variable* findVariable(char* myVarName)
{
return (Variable*) symbolTable.getVar(myVarName, NULL);
}

void writeVariableDeclare(char* myVarName)
{
Variable* myVar = (Variable*) symbolTable.getVar(myVarName, NULL);

if(myVar == NULL)
{
std::cout << "Error: variable with this name not found\n";
exit(1);
}

unsigned char byteArray[BYTES_PER_LINE];
int nextNum = myVar->uniqueIdentifierNum;

	if(myVar->symbolType == GLOBAL_VARIABLE)
	{
	writeGlobalDeclaration(byteArray, myVar);
	return;
	}

	else if(myVar->symbolType == FUNCTION_LOCAL_VARIABLE)
	{
	writeStackDeclareCode(nextNum);
	return;
	}

	else if(myVar->symbolType == PARAMETER)
	{
	std::cout << "writeVariableDeclare called on parameter... something went wrong somewhere\n";
	exit(1);
	}

	else if(myVar->symbolType == MEMBER_VARIABLE)
	{
	std::cout << "writeVariableDeclare called on member variable... something went wrong somewhere\n";
	exit(1);
	}
	else
	{
	std::cout << "writeVariableDeclare was called on something which wasn't even a variable! exiting now\n";
	exit(1);
	}

}

//returns the register that the address was stored in.
unsigned char writeGetVarAddressCode(char* varName, unsigned char destReg)
{
Variable* myVar = (Variable*) symbolTable.getVar(varName, NULL);
unsigned char tempByte[BYTES_PER_LINE];
if(myVar == NULL)
{
std::cout << "Error: variable named " << varName << " was not found\n";
exit(1);
}

int nextNum = myVar->uniqueIdentifierNum;

if(myVar->symbolType == GLOBAL_VARIABLE)
{
writeMoveGlobalAddrToReg(tempByte, destReg, nextNum);
isInUseArray[destReg] = true;
isNeededArray[destReg] = true;
contentsOfRegisters[destReg] = nextNum;
return destReg;
}

else if(myVar->symbolType == FUNCTION_LOCAL_VARIABLE)
{
writeMoveStackAddrToReg(destReg, nextNum);
isInUseArray[destReg] = true;
isNeededArray[destReg] = true;
contentsOfRegisters[destReg] = nextNum;
return destReg;
}

return destReg;
}


//ARITH_INSTRUCTION_OP(1 bytes), sourceLocationDestination(1 byte), Op_Type_Of_Expr(1 byte), destReg(1 byte), sourceLocation1(1 byte), Reg/Literal/memReg1(1-4 bytes), sourceLocation2(1 byte), Reg/LiteralmemReg/Literal/Reg2(1 - 4 bytes)

void arithmeticInstructionWrite(tokenType instrSymbol, Source_Location locationOutput, Op_Type resultType, char destReg, Source_Location location1, void* firstLocPointer,
Source_Location location2, void* secondLocPointer)
{
Op_Code myCode;
unsigned char codeArray[BYTES_PER_LINE];

switch(instrSymbol)
{
case PLUS:
myCode = PLUS_OP;
break;

case MINUS:
myCode = MINUS_OP;
break;

case DIVIDE:
myCode = DIVIDE_OP;
break;

case MULTIPLY:
myCode = MULTIPLY_OP;
break;

case EXPONENT:
myCode = POW_OP;
break;

case LOGICAL_AND:
myCode = LOGICAL_AND_OP;
break;

case LOGICAL_OR:
myCode = LOGICAL_OR_OP;
break;

case BITWISE_AND:
myCode = BITWISE_AND_OP;
break;

case BITWISE_OR:
myCode = BITWISE_OR_OP;
break;

case BITWISE_XOR:
myCode = BITWISE_XOR_OP;
break;

case BIT_SHIFT_LEFT:
myCode = BITSHIFT_LEFT_OP;
break;

case BIT_SHIFT_RIGHT:
myCode = BITSHIFT_RIGHT_OP;
break;

case EQUALS_EQUALS:
myCode = EQUALS_EQUALS_OP;
break;

case NOT_EQUALS:
myCode = NOT_EQUALS_OP;
break;

case GREATER_THAN:
myCode = GREATER_THAN_OP;
break;

case GREATER_THAN_EQUAL:
myCode = GREATER_THAN_EQUALS_OP;
break;

case LESS_THAN:
myCode = LESS_THAN_OP;
break;

case LESS_THAN_EQUAL:
myCode = LESS_THAN_EQUALS_OP;
break;

default:
std::cout << "Error: Invalid binary operator encountered: " << instrSymbol << std::endl;
exit(1);
break; 
}

arithmeticInstructionSet(codeArray, myCode, locationOutput, resultType, destReg,
location1, firstLocPointer, location2, secondLocPointer);
writeLine(codeArray);
return; 
}

//this function handles all arithmetic instruction setting except for comparisons of == and != with the literal NULL, which are handled elsewhere.
void arithmeticInstructionSet(unsigned char codeArray[BYTES_PER_LINE], Op_Code InstrOp, Source_Location locationOutput, Op_Type resultType, char destReg, Source_Location location1, void* firstLocPointer, Source_Location location2, void* secondLocPointer)
{
int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, InstrOp, nextSpotInLine);

codeArray[nextSpotInLine] = *((unsigned char*)&locationOutput);
++nextSpotInLine;

codeArray[nextSpotInLine] = *((unsigned char*) &resultType);
++nextSpotInLine;

codeArray[nextSpotInLine] = destReg;
++nextSpotInLine;

codeArray[nextSpotInLine] = *((unsigned char*) &location1);
++nextSpotInLine;

unsigned int tempInt = -1;
float tempDouble = -1;
char tempChar = 'Z';
bool tempBool = false;
if(location1 == REGISTER_VAL || location1 == REGISTER_MEM_ADDR)
{
codeArray[nextSpotInLine] = *((unsigned char*)firstLocPointer);
++nextSpotInLine;
}

else if(location1 == CODE_LOCATION)
{
std::cout << "No binary operation should require a code location as an argument, as this is reserved for strings, and no string has a binary op. Terminating execution now...\n";
exit(1);
}

//handling the case where location1 == LITERAL
else
{
if(resultType == INTEGER_T)
{
tempInt = *((int*)firstLocPointer);
nextSpotInLine = integerSet(codeArray, nextSpotInLine, tempInt);
} 
else if(resultType == DOUBLE_T)
{
tempDouble = *((float*)firstLocPointer);
nextSpotInLine = doubleSet(codeArray, nextSpotInLine, tempDouble);
}
else if(resultType == CHAR_T)
{
tempChar = *((unsigned char*)firstLocPointer);
codeArray[nextSpotInLine] = tempChar;
++nextSpotInLine;
}
else if(resultType == BOOLEAN_T)
{
tempBool = *((bool*) firstLocPointer);
if(tempBool == true)
codeArray[nextSpotInLine] = 1;
else
codeArray[nextSpotInLine] = 0;
++nextSpotInLine;
}

else if(resultType == NULL_T)
{
codeArray[nextSpotInLine] = 0;
++nextSpotInLine;
}

else
{
std::cout << "Error: Type besides int, double, char, bool or NULL cannot appear as a literal in an arithmetic binary expression.\n";
exit(1);
}
}

codeArray[nextSpotInLine] = *((unsigned char*)&location2);
++nextSpotInLine;

if(location2 == REGISTER_VAL || location2 == REGISTER_MEM_ADDR)
{
codeArray[nextSpotInLine] = *((unsigned char*)secondLocPointer);
++nextSpotInLine;
}

else if(location2 == CODE_LOCATION)
{
std::cout << "Error: Code location should not appear as argument in binary expression, as this is reserved for strings, and no string can use a binary operator\n";
exit(1);
}

//handling the case where a literal comes next.
else
{
if(resultType == INTEGER_T)
{
tempInt = *((int*)secondLocPointer);
nextSpotInLine = integerSet(codeArray, nextSpotInLine, tempInt);
}
else if(resultType == DOUBLE_T)
{
tempDouble = *((float*)secondLocPointer);
nextSpotInLine = doubleSet(codeArray, nextSpotInLine, tempDouble);
}
else if(resultType == CHAR_T)
{
tempChar = *((unsigned char*)secondLocPointer);
codeArray[nextSpotInLine] = tempChar;
++nextSpotInLine;
}
else if(resultType == BOOLEAN_T)
{
tempBool = *((bool*)secondLocPointer);
if(tempBool == true)
codeArray[nextSpotInLine] = 1;
else
codeArray[nextSpotInLine] = 0;
++nextSpotInLine;
}
else if(resultType == NULL_T)
{
codeArray[nextSpotInLine] = 0;
++nextSpotInLine;
}
else
{
std::cout << "Error: In binary operation with literal, literal must be of type int, double, char, boolean or NULL\n";
exit(1);
}
}


fillRest(codeArray, nextSpotInLine);
}

//enum Source_Location : unsigned char {REGISTER_VAL, REGISTER_MEM_ADDR, LITERAL_L, CODE_LOCATION };

unsigned int setArgNext(unsigned char codeArray[BYTES_PER_LINE], Op_Type myType, Source_Location myLocation, void* inputArg, unsigned int nextSpotInLine)
{
bool debugger = false;
if(myLocation == REGISTER_VAL || myLocation == REGISTER_MEM_ADDR)
{
codeArray[nextSpotInLine] = *((char*)inputArg);
return nextSpotInLine + 1;
}

else if(myLocation == CODE_LOCATION)
{
return integerSet(codeArray, nextSpotInLine, *((int*)inputArg));
}

//literal argument case
else
{
	if(myType == INTEGER_T)
	{
		if(debugger)
			std::cout << "in setArgNext() with inputArg of " << *((int*)inputArg) << std::endl;
		return integerSet(codeArray, nextSpotInLine, *((int*)inputArg));
	}
	
	else if(myType == DOUBLE_T)
	{
		return doubleSet(codeArray, nextSpotInLine, *((float*)inputArg));
	}

	else if(myType == CHAR_T)
	{
		codeArray[nextSpotInLine] = *((unsigned char*)inputArg);
		return nextSpotInLine + 1;
	}
	
	else if(myType == BOOLEAN_T)
	{
		if( *((bool*)inputArg) == true)
			codeArray[nextSpotInLine] = 1;
		else
			codeArray[nextSpotInLine] = 0;
		return nextSpotInLine + 1;
	}
	else if(myType == NULL_T)
	{
		codeArray[nextSpotInLine] = 0;
		return nextSpotInLine + 1;
	}
	//some kind of mem pointer type
	else
	{
		return integerSet(codeArray, nextSpotInLine, *((int*)inputArg));
	}

}

 
}

void convertDoubleToIntSet(unsigned char codeArray[BYTES_PER_LINE], char destinationReg, Source_Location secondLocation, void* inputArg)
{
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, CONVERT_DOUBLE_TO_INT, nextSpotInLine);
codeArray[nextSpotInLine] = destinationReg;
++nextSpotInLine;
nextSpotInLine = setArgNext(codeArray, DOUBLE_T, secondLocation, inputArg, nextSpotInLine);
fillRest(codeArray, nextSpotInLine);
return;
}

void convertCharToIntSet(unsigned char codeArray[BYTES_PER_LINE], char destinationReg, Source_Location secondLocation, void* inputArg)
{
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, CONVERT_CHAR_TO_INT, nextSpotInLine);
codeArray[nextSpotInLine] = destinationReg;
++nextSpotInLine;
nextSpotInLine = setArgNext(codeArray, CHAR_T, secondLocation, inputArg, nextSpotInLine);
fillRest(codeArray, nextSpotInLine);
return;
}

void convertDoubleToCharSet(unsigned char codeArray[BYTES_PER_LINE], char destinationReg, Source_Location secondLocation, void* inputArg)
{
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, CONVERT_DOUBLE_TO_CHAR, nextSpotInLine);
codeArray[nextSpotInLine] = destinationReg;
++nextSpotInLine;
nextSpotInLine = setArgNext(codeArray, DOUBLE_T, secondLocation, inputArg, nextSpotInLine);
fillRest(codeArray, nextSpotInLine);
return;
}

void convertIntToCharSet(unsigned char codeArray[BYTES_PER_LINE], char destinationReg, Source_Location secondLocation, void* inputArg)
{
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, CONVERT_INT_TO_CHAR, nextSpotInLine);
codeArray[nextSpotInLine] = destinationReg;
++nextSpotInLine;
nextSpotInLine = setArgNext(codeArray, INTEGER_T, secondLocation, inputArg, nextSpotInLine);
fillRest(codeArray, nextSpotInLine);
return;
}

void convertCharToDoubleSet(unsigned char codeArray[BYTES_PER_LINE], char destinationReg, Source_Location secondLocation, void* inputArg)
{
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, CONVERT_CHAR_TO_DOUBLE, nextSpotInLine);
codeArray[nextSpotInLine] = destinationReg;
++nextSpotInLine;
nextSpotInLine = setArgNext(codeArray, CHAR_T, secondLocation, inputArg, nextSpotInLine);
fillRest(codeArray, nextSpotInLine);
return;
}

void convertIntToDoubleSet(unsigned char codeArray[BYTES_PER_LINE], char destinationReg, Source_Location secondLocation, void* inputArg)
{
bool debugger = false;
if(debugger)
std::cout << "in convertIntToDoubleSet(), with argument of " << *((int*)inputArg) << std::endl;
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, CONVERT_INT_TO_DOUBLE, nextSpotInLine);
codeArray[nextSpotInLine] = destinationReg;
++nextSpotInLine;
nextSpotInLine = setArgNext(codeArray, INTEGER_T, secondLocation, inputArg, nextSpotInLine);
fillRest(codeArray, nextSpotInLine);
return;
}

void convertInstructionWrite(Op_Type destinationType, char destinationReg, Op_Type sourceType, Source_Location secondLocation, void* inputArg)
{
unsigned char nextLine[BYTES_PER_LINE];
convertInstructionSet(nextLine, destinationType, destinationReg, sourceType, secondLocation, inputArg);
writeLine(nextLine);
return;
}

//all conversion instructions take either a literal, a register value, or a memory address of a variable in a register and convert them into
//another value stored by value in the register referenced by destinationReg
void convertInstructionSet(unsigned char codeArray[BYTES_PER_LINE], Op_Type destinationType, char destinationReg,  Op_Type sourceType, Source_Location secondLocation, void* inputArg)
{
if(destinationType == INTEGER_T)
{

	if(sourceType == DOUBLE_T)
		convertDoubleToIntSet(codeArray, destinationReg, secondLocation, inputArg);
	else if(sourceType == CHAR_T)
		convertCharToIntSet(codeArray, destinationReg, secondLocation, inputArg);
	else
	{
		std::cout << "sourceType is " << getOpTypeString(sourceType) << std::endl;
		std::cout << "destinationType is " << getOpTypeString(destinationType) << std::endl;
		std::cout << "Invalid conversion\n";
		exit(1);
	}
return;

}

else if(destinationType == DOUBLE_T)
{
	if(sourceType == INTEGER_T)
		convertIntToDoubleSet(codeArray, destinationReg, secondLocation, inputArg);
	else if(sourceType == CHAR_T)
		convertCharToDoubleSet(codeArray, destinationReg, secondLocation, inputArg);
	else
	{
		std::cout << "sourceType is " << getOpTypeString(sourceType) << std::endl;
		std::cout << "Invalid conversion\n";
		exit(1);
	}
return;
}

else if(destinationType == CHAR_T)
{
	if(sourceType == INTEGER_T)
		convertIntToCharSet(codeArray, destinationReg, secondLocation, inputArg);
	else if(sourceType == DOUBLE_T)
		convertDoubleToCharSet(codeArray, destinationReg, secondLocation, inputArg);
	else
	{
		std::cout << "sourceType is " << getOpTypeString(sourceType) << std::endl;
		std::cout << "Invalid conversion\n";	
		exit(1);
	}
return;
}

std::cout << "Error: Conversion destination can only be int, double or char, but in this case, it was none of those\n";
std::cout << "sourceType: " << getOpTypeString(sourceType) << ", destinationType: " << getOpTypeString(destinationType) << std::endl;
exit(1);
return;
}

void writeMoveGlobalAddrToReg(unsigned char codeArray[BYTES_PER_LINE], unsigned char destinationReg, int identifierNum)
{
bool debugger = false;
int myPos = 0;
if(debugger)
std::cout << "in writeMoveGlobalAddrToReg with destReg of " << (int)destinationReg << std::endl;
myPos = opCodeSet(codeArray, MOVE_GLOBAL_ADDR_TO_REG, myPos);
codeArray[myPos] = destinationReg;
++myPos;
myPos = integerSet(codeArray, myPos, identifierNum);
fillRest(codeArray, myPos);
writeLine(codeArray);
return;
}

void writeMoveStackAddrToReg(unsigned char destinationReg, int identifierNum)
{
unsigned char codeArray[BYTES_PER_LINE];
int myPos = 0;
myPos = opCodeSet(codeArray, MOVE_STACK_ADDR_TO_REG, myPos);
codeArray[myPos] = destinationReg;
++myPos;
myPos = integerSet(codeArray, myPos, identifierNum);
fillRest(codeArray, myPos);
writeLine(codeArray);
return;
}

void writeAssignment(unsigned char regMemVal, Op_Type leftType, Source_Location rightSourceLocation, void* inputArg)
{
unsigned char codeArray[BYTES_PER_LINE];
int positionInLine = 0;
positionInLine = opCodeSet(codeArray, ASSIGNMENT_OP, positionInLine);
codeArray[positionInLine] = regMemVal;
++positionInLine;

codeArray[positionInLine] = *((unsigned char*)&leftType);
++positionInLine;
codeArray[positionInLine] = *((unsigned char*)&rightSourceLocation);
++positionInLine;

if(rightSourceLocation == REGISTER_VAL || rightSourceLocation == REGISTER_MEM_ADDR)
{
codeArray[positionInLine] = *((unsigned char*)inputArg);
++positionInLine;
}
else if(rightSourceLocation == CODE_LOCATION)
{
positionInLine = integerSet(codeArray, positionInLine, *((int*)inputArg));
}
else
{
if(leftType == DOUBLE_T)
positionInLine = doubleSet(codeArray, positionInLine, *((float*)inputArg));

else if(leftType == CHAR_T)
{
codeArray[positionInLine] = *((unsigned char*)inputArg);
++positionInLine;
}

else if(leftType == BOOLEAN_T)
{
if( *((bool*)inputArg) == true)
codeArray[positionInLine] = ((unsigned char) 1);
else
codeArray[positionInLine] = ((unsigned char) 0);
++positionInLine;
}
else if(leftType == NULL_T)
{
codeArray[positionInLine] = ((unsigned char)0);
++positionInLine;
}

else
{
positionInLine = integerSet(codeArray, positionInLine, *((int*)inputArg));
}

}

fillRest(codeArray, positionInLine);
writeLine(codeArray);
}

void writeGlobalDeclareAssign(unsigned char codeArray[BYTES_PER_LINE], Variable* myVar, ExpressionNode* rightHalf)
{
bool tempDebug = false;

if(tempDebug)
std::cout << "in writeGlobalDeclareAssign(). Calling writeGlobalDeclaration() now\n";
writeGlobalDeclaration(codeArray, myVar);

if(tempDebug)
std::cout << "in writeGlobalDeclareAssign(). Calling writeGlobalAssignment() now\n";
writeGlobalAssignment(codeArray, myVar, rightHalf);

if(tempDebug)
std::cout << "exiting writeGlobalDeclareAssign() now\n";
}

void writeGlobalDeclaration(unsigned char codeArray[BYTES_PER_LINE], Variable* myVar)
{

bool tempDebug = false;
int nextSpotInLine = 0;

if(tempDebug)
std::cout << "in writeGlobalDeclaration(). Calling opCodeSet() now\n";
nextSpotInLine = opCodeSet(codeArray, GLOBAL_DECLARE_OP, nextSpotInLine);
if(tempDebug)
std::cout << "in writeGlobalDeclaration(). Calling integerSet() now\n";
nextSpotInLine = integerSet(codeArray, nextSpotInLine, myVar->uniqueIdentifierNum);
if(tempDebug)
std::cout << "in writeGlobalDeclaration(). Calling fillRest() now\n";
fillRest(codeArray, nextSpotInLine);

if(tempDebug)
std::cout << "in writeGlobalDeclaration. Calling writeLine() now\n";
writeLine(codeArray);

if(tempDebug)
std::cout << "exiting writeGlobalDeclaration() now\n";
return;
}

//puts left variable address into register 0.
//either assigns directly to register 0, or if a conversion needs to be done, this is done in register 1, and then moved to register 0.
//ASSIGNMENT_OP, DESTINATION_MEM_ADDR_REG(1 byte), OPERAND_TYPE(1 byte), SOURCE_LOCATION_TYPE(1 byte), LITERAL/POINTER/ADDR_IN_REG(1-4 bytes)
void writeGlobalAssignment(unsigned char codeArray[BYTES_PER_LINE], Variable* myLeftVar, ExpressionNode* rightHalf)
{
int myUniqueNum = myLeftVar->uniqueIdentifierNum;
int tempInt;
float tempDouble;
char tempChar;
bool tempBool;
Op_Type myLeftOpType = getOpType(myLeftVar->variableType);
Type* tempType;
myParseTree.getType(rightHalf, &tempType, symbolTable);
Op_Type myRightOpType = getOpType(tempType);
writeMoveGlobalAddrToReg(codeArray, 0, myUniqueNum);

if(rightHalf->myType == NULL_EXPRESSION)
{
tempChar = 0;
writeAssignment(0, myRightOpType, LITERAL_L, &tempChar);
return;
}

if(rightHalf->myType == STRING_LIT)
{
int myTemp = symbolTable.getStringNum( ((StringLiteralNode*)rightHalf)->myStringPointer);
writeAssignment(0, myRightOpType, CODE_LOCATION, &myTemp);
return;
}
//in this case, a conversion must be done first.
if(myLeftOpType != myRightOpType)
{
if(rightHalf->myType == INT_LIT)
convertInstructionSet(codeArray, myLeftOpType, ((unsigned char) 1), myRightOpType, LITERAL_L, &(((IntegerLiteralNode*)rightHalf)->myValue));
else if(rightHalf->myType == DOUBLE_LIT)
convertInstructionSet(codeArray, myLeftOpType, ((unsigned char) 1), myRightOpType, LITERAL_L, &(((DoubleLiteralNode*)rightHalf)->myValue));
else if(rightHalf->myType == CHAR_LIT)
convertInstructionSet(codeArray, myLeftOpType, ((unsigned char) 1), myRightOpType, LITERAL_L, &(((CharLiteralNode*)rightHalf)->myValue)); 
else
convertInstructionSet(codeArray, myLeftOpType, ((unsigned char) 1), myRightOpType, LITERAL_L, &(((BooleanLiteralNode*)rightHalf)->myValue));
writeLine(codeArray);
tempChar = 1;
writeAssignment(0, myLeftOpType, REGISTER_VAL, &tempChar); 
return;
}

if(rightHalf->myType == INT_LIT)
writeAssignment(0, myLeftOpType, LITERAL_L, &(((IntegerLiteralNode*)rightHalf)->myValue));
else if(rightHalf->myType == DOUBLE_LIT)
writeAssignment(0, myLeftOpType, LITERAL_L, &(((DoubleLiteralNode*)rightHalf)->myValue));
else if(rightHalf->myType == CHAR_LIT)
writeAssignment(0, myLeftOpType, LITERAL_L, &(((CharLiteralNode*)rightHalf)->myValue));
else
writeAssignment(0, myLeftOpType, LITERAL_L, &(((BooleanLiteralNode*)rightHalf)->myValue));

}

//handles the definitions and initializations of global variables, the creation of pipelines, and allocating space
//for instructions to initialize Strings with string literals and addPipeLine instructions with function locations.
void GlobalCodeWrite()
{
myParseTree.moveToRoot();
symbolTable.moveToRoot();
unsigned char nextLineByteCode[BYTES_PER_LINE];
Op_Type myOperandType;
abstractSymbolNode* travelPointer = myParseTree.getRoot();
abstractSymbolNode* currentLineObj = NULL;
symbolNode* myObjInfo = NULL;
bool tempDebugVal = false;

if(tempDebugVal)
std::cout << "Entering for loop in GlobalCodeWrite()\n";
	for(int i = 0; i < ((GlobalListNode*) travelPointer)->globalStatementsList.size(); ++i)
	{
		currentLineObj = ((GlobalListNode*)travelPointer)->globalStatementsList[i];
		if(tempDebugVal)
			std::cout << "Got currentLineObj\n";
		 if(currentLineObj->symbolType == STATEMENT)
		{
			if(tempDebugVal)
				std::cout << "currentLineObj was a Statement\n";
			if( ((StatementNode*)currentLineObj)->myType == DECLARATION_STATEMENT)
			{
				if(tempDebugVal)
					std::cout << "currentLineObj was a DelarationStatement\n";
				myObjInfo = symbolTable.getVar( ((DeclarationStatement*)currentLineObj)->identifierName, NULL);
				if(myObjInfo == NULL)
				{
					std::cout << "Error: myObjInfo was NULL in GlobalCodeWrite()\n";
					exit(1);	
				} 
				if(tempDebugVal)
					std::cout << "calling writeGlobalDeclaration()\n";
				writeGlobalDeclaration(nextLineByteCode, ((Variable*)myObjInfo));

			}
			else if ( ((StatementNode*)currentLineObj)->myType == DECLARE_ASSIGN_STATEMENT)
			{
			if(tempDebugVal)
				std::cout << "currentLineObj was DeclareAssignStatement\n";
			myObjInfo = symbolTable.getVar( ((DeclareAssignStatement*)currentLineObj)->declarePortion->identifierName, NULL);
			if(myObjInfo == NULL)
			{
				std::cout << "Error: myObjInfo was NULL in GlobalCodeWrite()\n";
				exit(1);
			}
			if(tempDebugVal)
			std::cout << "Calling writeGlobalDeclareAssign() now\n";
			writeGlobalDeclareAssign(nextLineByteCode, ((Variable*)myObjInfo), ((DeclareAssignStatement*)currentLineObj)->assignedPortion);	

			}
			else if( ((StatementNode*)currentLineObj)->myType == ASSIGNMENT_STATEMENT)
			{
			if(tempDebugVal)
			std::cout << "currentLineObj was an assignment statement. Calling writeGlobalAssignment() now\n";
			writeGlobalAssignment(nextLineByteCode, ((Variable*)myObjInfo), ((DeclareAssignStatement*)currentLineObj)->assignedPortion);
			}
	else
	{
	std::cout << "Error: Declarations, declare-assigns, and assignemnts are the only statement types alowed at the global level\n";
	exit(1);
	}

		}

	else if(currentLineObj->symbolType == PIPE_LINE_ABSTRACT)
	{
	if(tempDebugVal)
		std::cout << "currentLineObj was a PipeLine Declaration\n";
	myObjInfo = symbolTable.getVar(  ((PipeLineNode*)currentLineObj)->pipeName, NULL);
	if(myObjInfo == NULL)
	{
	std::cout << "Error: Pipeline was not found in symbol table for PipeLine " << ((PipeLineNode*)currentLineObj)->pipeName << "\n";	
	exit(1);
	}
	
	if(tempDebugVal)
		std::cout << "calling writePipelineDeclareCode() now\n";
	writePipeLineDeclareCode(myObjInfo->uniqueIdentifierNum);
	//after all other code is written, the program will fill in where the 0 is with the function's code location
	for(int tempIndex = 0; tempIndex < ((PipeLineNode*)currentLineObj)->functionList.size(); ++tempIndex)
		{
		if(tempDebugVal)
			std::cout << "Now calling writePipeLineAddFunctionCode on this pipeline\n";
		writePipeLineAddFunctionCode(myObjInfo->uniqueIdentifierNum, symbolTable.getNonMemberFunction( ((PipeLineNode*)currentLineObj)->functionList[tempIndex])->uniqueIdentifierNum);	
		}
	}
}

if(tempDebugVal)
std::cout << "out of loop. Calling writeJumpToMainInstruction() now\n";
writeJumpToMainInstruction();
if(tempDebugVal)
std::cout << "exiting GlobalCodeWrite()\n";
}

void putLitNextAtEnd(unsigned char nextLineByteCode[13], unsigned int nextPos, ExpressionNode* rightExpression)
{
if(rightExpression->myType == BOOL_LIT)
{

if( ((BooleanLiteralNode*)rightExpression)->myValue == true)
nextLineByteCode[nextPos] = 1;

else
nextLineByteCode[nextPos] = 0;

fillRest(nextLineByteCode, nextPos + 1);
return;
}

else if(rightExpression->myType == CHAR_LIT)
{
nextLineByteCode[nextPos] = ((CharLiteralNode*)rightExpression)->myValue;
fillRest(nextLineByteCode, nextPos + 1);
return;
}

else if(rightExpression->myType == DOUBLE_LIT)
{
doubleSet(nextLineByteCode, nextPos, ((DoubleLiteralNode*)rightExpression)->myValue);
fillRest(nextLineByteCode, nextPos + 4);
return;
}
else if(rightExpression->myType == INT_LIT)
{
integerSet(nextLineByteCode, nextPos, ((IntegerLiteralNode*)rightExpression)->myValue);
fillRest(nextLineByteCode, nextPos + 4);
return;
}
std::cout << "Error: literal must be an int, double, char or boolean\n";
exit(1);
}


void writeStackDeclareCode(int identifierNum)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, STACK_VARIABLE_DECLARE, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, identifierNum);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writePipeLineDeclareCode(int identifierNum)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, PIPELINE_DECLARE, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, identifierNum);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

//format: PIPELINE_ADD_FUNCTION, PIPELINE_IDENTIFIER_NUM, LINE_NUMBER_OF_START_OF_FUNCTION
void writePipeLineAddFunctionCode(int pipeLineNum, int codeLocation)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, PIPELINE_ADD_FUNCTION, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, pipeLineNum);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, codeLocation);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

//note: any parameters to the constructor function must be set before the line of code created by this function is executed
void writeNewUserObjConstructorCall(int constructorCodeLocation)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, NEW_USER_OBJ, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, constructorCodeLocation);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

//this instruction has the formula BUILTIN_OBJECT_CREATE_CODE | OUTER_OP_TYPE | INNER_OP_TYPE
void writeBuiltinObjectConstructorCall(Type* objType)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, BUILTIN_OBJECT_CREATE_CODE, nextSpotInLine);
Op_Type outerType = getOpType(objType);
Op_Type innerType = getOpType(objType->nextType);
nextLine[nextSpotInLine] = *((unsigned char*)&outerType);
++nextSpotInLine;
nextLine[nextSpotInLine] = *((unsigned char*)&innerType);
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeNoArgInstruction(Op_Code instructCode)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, instructCode, nextSpotInLine);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeReturnFromFunctionInstruction()
{
writeNoArgInstruction(RETURN_FROM_FUNCTION);
return;
}


void writeExitMainInstruction(Op_Type returnOpType)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, EXIT_MAIN, nextSpotInLine);
nextLine[nextSpotInLine] = *((unsigned char*)&returnOpType);
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeOutOfBoundsFaultInstruction()
{
writeNoArgInstruction(OUT_OF_BOUNDS_FAULT);
return;
}

void writeJumpToInstruction(int codeLocation)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, JUMP_TO, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, codeLocation);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

//puts the jump to main instruction in place, with a code location of 0 value. This will be filled in with the
//correct value once the main function's code location is determined.
void writeJumpToMainInstruction()
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, JUMP_TO_MAIN, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, 0);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeNonMemberFunctionCallInstruction(int codeLocation)
{
bool parseDebug = false;
if(parseDebug)
std::cout << "Code Location: " << codeLocation << "\n";
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, CALL_NON_MEMBER_FUNCTION, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, codeLocation);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeMemberFunctionCallInstruction(int codeLocation)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, CALL_MEMBER_FUNCTION, nextSpotInLine);
nextSpotInLine = integerSet(nextLine, nextSpotInLine, codeLocation);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeSetFunctionArgumentInstruction(unsigned char functionArgRegister, Op_Type sourceType, Source_Location sourceLocation, void* sourceArg)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, SET_FUNCTION_ARGUMENT, nextSpotInLine);
nextLine[nextSpotInLine] = functionArgRegister;
++nextSpotInLine;
nextLine[nextSpotInLine] = *((unsigned char*)&sourceType);
++nextSpotInLine;
nextLine[nextSpotInLine] = *((unsigned char*)&sourceLocation);
++nextSpotInLine;
nextSpotInLine = setArgNext(nextLine, sourceType, sourceLocation, sourceArg, nextSpotInLine);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeSaveRegisterInstruction(unsigned char saveRegister)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, SAVE_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = saveRegister;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeRestoreRegisterInstruction(unsigned char restoreRegister)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, RESTORE_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = restoreRegister;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeCopyRegisterToRegisterInstruction(unsigned char destReg, unsigned char sourceReg)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, COPY_REGISTER_TO_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
nextLine[nextSpotInLine] = sourceReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeCopyIntLiteralToRegisterInstruction(unsigned char destReg, int intLit)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, COPY_INT_LITERAL_TO_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
nextSpotInLine = integerSet(nextLine, nextSpotInLine, intLit);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeCopyBooleanLiteralToRegisterInstruction(unsigned char destReg, bool boolLit)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, COPY_BOOLEAN_LITERAL_TO_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
if(boolLit == true)
nextLine[nextSpotInLine] = ((char) 1);
else
nextLine[nextSpotInLine] = ((char) 0);
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeCopyDoubleLiteralToRegisterInstruction(unsigned char destReg, float doubLit)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, COPY_DOUBLE_LITERAL_TO_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
nextSpotInLine = doubleSet(nextLine, nextSpotInLine, doubLit);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeCopyCharLiteralToRegisterInstruction(unsigned char destReg, unsigned char charLit)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, COPY_CHAR_LITERAL_TO_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
nextLine[nextSpotInLine] = charLit;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeCopyStringLocationToRegisterInstruction(unsigned char destReg, int stringLoc)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(nextLine, COPY_STRING_LOCATION_TO_REGISTER, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
nextSpotInLine = integerSet(nextLine, nextSpotInLine, stringLoc);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return; 
}

//TODO: Continue working here:

void writePrintInstruction(Op_Type argType, unsigned char sourceReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, PRINT_OP, nextSpotInLine);
nextLine[nextSpotInLine] = *((unsigned char*)&argType);
++nextSpotInLine;
nextLine[nextSpotInLine] = sourceReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeGetIntInstruction(unsigned char destReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, GET_INT_OP, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeGetDoubleInstruction(unsigned char destReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, GET_DOUBLE_OP, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeGetCharInstruction(unsigned char destReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, GET_CHAR_OP, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeGetBooleanInstruction(unsigned char destReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, GET_BOOLEAN_OP, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeGetWordInstruction(unsigned char destReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, GET_WORD_OP, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeGetLineInstruction(unsigned char destReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, GET_LINE_OP, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeIsInputFailInstruction(unsigned char destReg)
{
int nextSpotInLine = 0;
unsigned char nextLine[BYTES_PER_LINE];
nextSpotInLine = opCodeSet(nextLine, IS_INPUT_FAIL_OP, nextSpotInLine);
nextLine[nextSpotInLine] = destReg;
++nextSpotInLine;
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}


//ex. Assume we have the line x += 5; and x is an integer and x's address is in register 4. Then, we would have the following instruction:
//PLUS_EQUALS_OP, 4, INTEGER_T, LITERAL_L, 5
//for x += y: assume the same as above, but y's address is in register 6. Then...
//PLUS_EQUALS_OP, 4, INTEGER_T, REGISTER_MEM_ADDR, 6
//for x += (y + 4), where the value of (y + 4) is stored in register 11, we have...
//PLUS_EQUALS_OP, 4, INTEGER_T, REGISTER_VAL, 11
void writeCompoundAssignmentInstruction(tokenType typeOfCompound, unsigned char sourceReg, Op_Type leftType, Source_Location rightLocation, void* rightArg)
{
unsigned char nextLine[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
bool debugger = false;

if(debugger)
std::cout << " in writeCompoundAssignmentInstruction with sourceReg of " << ((int)sourceReg) << ", Op_Type of " << getOpTypeString(leftType) << ", right SourceLocation of " << getSourceLocation(rightLocation) << std::endl;
switch(typeOfCompound)
{
case PLUS_EQUALS:
nextSpotInLine = opCodeSet(nextLine, PLUS_EQUALS_OP, nextSpotInLine);
break;

case MINUS_EQUALS:
nextSpotInLine = opCodeSet(nextLine, MINUS_EQUALS_OP, nextSpotInLine);
break;

case DIVIDE_EQUALS:
nextSpotInLine = opCodeSet(nextLine, DIVIDE_EQUALS_OP, nextSpotInLine);
break;

case MOD_EQUALS:
nextSpotInLine = opCodeSet(nextLine, MOD_EQUALS_OP, nextSpotInLine);
break;

case TIMES_EQUALS:
nextSpotInLine = opCodeSet(nextLine, TIMES_EQUALS_OP, nextSpotInLine);
break;

case POW_EQUALS:
nextSpotInLine = opCodeSet(nextLine, POW_EQUALS_OP, nextSpotInLine);
break;

default:
std::cout << "Error: Invalid compound operator type encountered: " << typeOfCompound << std::endl;
exit(1);
break;
}

nextLine[nextSpotInLine] = sourceReg;
++nextSpotInLine;

nextLine[nextSpotInLine] = *((unsigned char*)&leftType);
++nextSpotInLine;

nextLine[nextSpotInLine] = *((unsigned char*)&rightLocation);
++nextSpotInLine;

nextSpotInLine = setArgNext(nextLine, leftType, rightLocation, rightArg, nextSpotInLine);
fillRest(nextLine, nextSpotInLine);
writeLine(nextLine);
return;
}

void writeGetMemberVarAddrOfObjInstruction(unsigned char destReg, unsigned char objReg, unsigned int offsetIntoObject)
{
unsigned char codeArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;

nextSpotInLine = opCodeSet(codeArray, GET_MEMBER_VAR_ADDR_OF_OBJ, nextSpotInLine);
codeArray[nextSpotInLine] = destReg;
++nextSpotInLine;

codeArray[nextSpotInLine] = objReg;
++nextSpotInLine;

nextSpotInLine = integerSet(codeArray, nextSpotInLine, offsetIntoObject);
fillRest(codeArray, nextSpotInLine);
writeLine(codeArray);
return;
}

//this instruction has the form ADD_MEMBER_VARS_TO_OBJ NUMBER_VARS(INTEGER)
//the instruction allocates space for a new object with enough space to hold
//numVars worth of variables. It then puts the address of this variable into the THIS register.
void writeAddMemberVarsToObjInstruction(unsigned int numVars)
{
unsigned char codeArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;

isInUseArray[THIS_REGISTER_NUM] = true;
isNeededArray[THIS_REGISTER_NUM] = true;
contentsOfRegisters[THIS_REGISTER_NUM] = -1;
nextSpotInLine = opCodeSet(codeArray, ADD_MEMBER_VARS_TO_OBJ, nextSpotInLine);
nextSpotInLine = integerSet(codeArray, nextSpotInLine, numVars);
fillRest(codeArray, nextSpotInLine);
writeLine(codeArray);
return;
}

void writeDereferenceInstructionType(Op_Code myOp, unsigned char destReg, unsigned char sourceAddrReg)
{
unsigned char codeArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;

nextSpotInLine = opCodeSet(codeArray, myOp, nextSpotInLine);
codeArray[nextSpotInLine] = destReg;
++nextSpotInLine;
codeArray[nextSpotInLine] = sourceAddrReg;
++nextSpotInLine;
fillRest(codeArray, nextSpotInLine);
writeLine(codeArray);
return;
}

void writeDereferenceBooleanInstruction(unsigned char destReg, unsigned char sourceAddrReg)
{
writeDereferenceInstructionType(DEREFERENCE_BOOLEAN, destReg, sourceAddrReg);
return;
}

void writeDereferenceIntegerInstruction(unsigned char destReg, unsigned char sourceAddrReg)
{
writeDereferenceInstructionType(DEREFERENCE_INTEGER, destReg, sourceAddrReg);
return;
}

void writeDereferenceDoubleInstruction(unsigned char destReg, unsigned char sourceAddrReg)
{
writeDereferenceInstructionType(DEREFERENCE_DOUBLE, destReg, sourceAddrReg);
return;
}

void writeDereferenceCharInstruction(unsigned char destReg, unsigned char sourceAddrReg)
{
writeDereferenceInstructionType(DEREFERENCE_CHAR, destReg, sourceAddrReg);
return;
}


void writeUnaryOpInstruction(tokenType unaryOp, Source_Location outputLocation, unsigned char destinationReg, Op_Type inputType, Source_Location inputLocation, void* inputArg)
{
unsigned char codeArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
Op_Code myOpCode;

switch(unaryOp)
{
case NOT:
myOpCode = NOT_OP;
break;

case BITWISE_NOT:
myOpCode = BITWISE_NOT_OP;
break;

case MINUS:
myOpCode = UNARY_MINUS_OP;
break;

default:
std::cout << "Error: Invalid unary operator encountered: " << unaryOp << std::endl;
exit(1);
break;
}
nextSpotInLine = opCodeSet(codeArray, myOpCode, nextSpotInLine);
codeArray[nextSpotInLine] = *((unsigned char*)&outputLocation);
++nextSpotInLine;

codeArray[nextSpotInLine] = destinationReg;
++nextSpotInLine;

codeArray[nextSpotInLine] = *((unsigned char*)&inputType);
++nextSpotInLine;

codeArray[nextSpotInLine] = *((unsigned char*)&inputLocation);
++nextSpotInLine;

nextSpotInLine = setArgNext(codeArray, inputType, inputLocation, inputArg, nextSpotInLine);
fillRest(codeArray, nextSpotInLine);
writeLine(codeArray);
return; 
} 

//these instructions have the form OP_CODE, REG/LIT, codeLocation 
//codeLocation is jumped to if Reg/LIT evaluates to false. Otherwise,
//the next instruction is executed like normal.
//for now, the code location is zeroes out, and will be filled in later.
//BRANCH_ON_FALSE_LITERAL, BRANCH_ON_FALSE_REG_VAL, BRANCH_ON_FALSE_MEM_ADDR,
void writeBranchOnFalseLiteralInstruction(bool myBool)
{
unsigned char codeArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, BRANCH_ON_FALSE_LITERAL, nextSpotInLine);
if(myBool)
codeArray[nextSpotInLine] = 1;
else
codeArray[nextSpotInLine] = 0;
++nextSpotInLine;
fillRest(codeArray, nextSpotInLine);
writeLine(codeArray);
return;
}

void writeBranchOnFalseRegValInstruction(unsigned char registerNum)
{
unsigned char codeArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, BRANCH_ON_FALSE_REG_VAL, nextSpotInLine);
codeArray[nextSpotInLine] = registerNum;
++nextSpotInLine;
fillRest(codeArray, nextSpotInLine);
writeLine(codeArray);
return;
}

void writeBranchOnFalseMemAddrInstruction(unsigned char registerNum)
{
unsigned char codeArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(codeArray, BRANCH_ON_FALSE_REG_VAL, nextSpotInLine);
codeArray[nextSpotInLine] = registerNum;
++nextSpotInLine;
fillRest(codeArray, nextSpotInLine);
writeLine(codeArray);
return; 
}

void writeSetParameterInstruction(unsigned char regNum, int uniqueIdentifierNum)
{
unsigned char byteArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(byteArray, SET_PARAMETER, nextSpotInLine);
byteArray[nextSpotInLine] = regNum;
++nextSpotInLine;
nextSpotInLine = integerSet(byteArray, nextSpotInLine, uniqueIdentifierNum);
fillRest(byteArray, nextSpotInLine);
writeLine(byteArray);
return;
}

void writeCallBuiltinFunctionInstruction(unsigned char regWithObjAddr, Op_Type objType, Op_Type innerType, Builtin_Function funcName)
{
unsigned char byteArray[BYTES_PER_LINE];
unsigned int nextSpotInLine = 0;
nextSpotInLine = opCodeSet(byteArray, CALL_BUILTIN_FUNCTION, nextSpotInLine);
byteArray[nextSpotInLine] = regWithObjAddr;
++nextSpotInLine;
byteArray[nextSpotInLine] = *((unsigned char*)&objType);
++nextSpotInLine;
byteArray[nextSpotInLine] = *((unsigned char*)&innerType);
++nextSpotInLine;
byteArray[nextSpotInLine] = *((unsigned char*)&funcName);
++nextSpotInLine;
fillRest(byteArray, nextSpotInLine);
writeLine(byteArray);
return;
}


//first, we make a copy of all registers in use, and write the code
//to store these registers for when the program actually runs.
//then, we call writeExpressionPortion() on each function argument.
//at the end, the registers are restored to their state before the function call
void functionParameterSet(FunctionExpression* myExpression, bool isMember)
{
/*if(isMember && myExpression->myType != BUILTIN_MEM_FUNC && !symbolTable.isConstructor( symbolTable.getMemberFunctionOfObject( ((MemberFunctionExpression*)myExpression)->objectName, ((FunctionExpression*)myExpression)->funcName)))
{
isInUseArray[THIS_REGISTER_NUM]
}
*/
bool inUseCopy[256];
bool isNeededCopy[256];
int registerContentsCopy[256];
int myCodeLocation = -1;

Type* realType = NULL;
Type* actualType = NULL;

Op_Type myRealOp, myActualOp;
for(int i = 0; i < 256; ++i)
{
if(i == RETURN_REGISTER_NUM)
continue;
inUseCopy[i] = isInUseArray[i];
isNeededCopy[i] = isNeededArray[i];
registerContentsCopy[i] = contentsOfRegisters[i];
}

//writing the code to save these registers. From here on out, all registers will be set to not used, so that every register is free
for(unsigned int i = 0; i < 256; ++i)
{
if(i == RETURN_REGISTER_NUM)
continue;
if(isInUseArray[i] == true)
writeSaveRegisterInstruction( ((unsigned char) i));
}

for(int i = 0; i < 256; ++i)
{
isInUseArray[i] = false;
}


//now, writing the code to actually set the register arguments for the function call...

unsigned char myTempRegister = 0;
for(int i = 0; i < myExpression->argumentList.size(); ++i)
{
myTempRegister = writeExpressionPortion( myExpression->argumentList[i]);
isNeededArray[NUMBER_OF_GENERAL_REGISTERS + i] = true;
isInUseArray[NUMBER_OF_GENERAL_REGISTERS + i] = true;
contentsOfRegisters[NUMBER_OF_GENERAL_REGISTERS + i] = -1;
//checking to see if type conversion of argument is needed here to match function parameters.
if(isMember && myExpression->myType != BUILTIN_MEM_FUNC)
realType = symbolTable.getTypeOfMemberFunctionParameter( ((MemberFunctionExpression*)myExpression)->objectName, myExpression->funcName, i);
else if(myExpression->myType != BUILTIN_MEM_FUNC)
realType = symbolTable.getTypeOfNonMemberFunctionParameter(myExpression->funcName, i);
else
{
realType = symbolTable.getExpectedTypeOfBuiltinParameter( ((BuiltinMemberFunctionExpression*)myExpression)->objName, ((BuiltinMemberFunctionExpression*)myExpression)->functionName, i);
}


myRealOp = getOpType(realType);

myParseTree.getType(myExpression->argumentList[i], &actualType, symbolTable);
myActualOp = getOpType(actualType);

//in this case, a conversion must occur. We will store the result in myTempRegister to save space, and overwrite what is there.
if(myRealOp != myActualOp && myActualOp != NULL_T)
{
 // void convertInstructionWrite(Op_Type destinationType, char destinationRe     g, Op_Type sourceType, Source_Location secondLocation, void* inputArg)
convertInstructionWrite(myRealOp, myTempRegister, myActualOp, REGISTER_VAL, &myTempRegister);
}

writeCopyRegisterToRegisterInstruction( NUMBER_OF_GENERAL_REGISTERS + i, myTempRegister);
isNeededArray[myTempRegister] = false;
}

//now calling the actual function:
if(myExpression->myType == BUILTIN_MEM_FUNC)
{
myTempRegister = getAddressRoutine( ((BuiltinMemberFunctionExpression*)myExpression)->objName);
//void writeCallBuiltinFunctionInstruction(unsigned char regWithObjAddr, Op_Type objType, Builtin_Function funcName)
actualType = symbolTable.getTypeOfVariable( ((BuiltinMemberFunctionExpression*)myExpression)->objName);
myActualOp = getOpType(actualType);
writeCallBuiltinFunctionInstruction(myTempRegister, myActualOp, getOpType(actualType->nextType), ((BuiltinMemberFunctionExpression*)myExpression)->functionName);
isNeededArray[myTempRegister] = false;
}

else if(myExpression->myType == NON_MEM_FUNC_EXPR)
{
myCodeLocation = symbolTable.getNonMemberFunction( ((NonMemberFunctionExpression*)myExpression)->funcName)->firstInstructionInCode;
writeNonMemberFunctionCallInstruction(myCodeLocation);
}

else if(myExpression->myType == MEM_FUNC_EXPR)
{
if( symbolTable.isTypeDefined(((MemberFunctionExpression*)myExpression)->objectName))
{
myCodeLocation = symbolTable.getConstructorOfClass( ((MemberFunctionExpression*)myExpression)->objectName)->firstInstructionInCode;
}
else
{
myCodeLocation = symbolTable.getMemberFunctionOfObject( ((MemberFunctionExpression*)myExpression)->objectName, ((FunctionExpression*)myExpression)->funcName)->firstInstructionInCode;
moveAddressToThisRegister( ((MemberFunctionExpression*)myExpression)->objectName);
}
writeMemberFunctionCallInstruction(myCodeLocation);
}

else if(myExpression->myType == NEW_OBJECT_EXPRESSION)
{
myCodeLocation = symbolTable.getConstructorOfClass( ((NewObjectExpression*)myExpression)->className)->firstInstructionInCode;
writeMemberFunctionCallInstruction(myCodeLocation);
}

else
{
std::cout << "Error: Unknown function type encountered in setParameter()\n";
exit(1);
}


//now restoring all registers:
for(int i = 0; i < 256; ++i)
{
if(i == RETURN_REGISTER_NUM)
continue;

if(inUseCopy[i] == true)
writeRestoreRegisterInstruction( ((unsigned char) i));
}

for(int i = 0; i < 256; ++i)
{
if(i == RETURN_REGISTER_NUM)
continue;
isInUseArray[i] = inUseCopy[i];
isNeededArray[i] = isNeededCopy[i];
contentsOfRegisters[i] = registerContentsCopy[i];
}

} 

void writeBreakCode(BooleanBlockNode* myAbsBlock)
{
abstractSymbolNode* travelPointer = myAbsBlock;
int numBlocksBack = 0;
while( ((BooleanBlockNode*)travelPointer)->myType != FOR_BLOCK && ((BooleanBlockNode*)travelPointer)->myType != WHILE_BLOCK)
{
++numBlocksBack;
travelPointer = ((BooleanBlockNode*)travelPointer)->parentBlock;
}

symbolNode* mySymbolNode = symbolTable.getBreakInstructionNode(numBlocksBack);

if( ((BooleanBlockNode*)travelPointer)->myType == FOR_BLOCK)
{
writeJumpToInstruction(mySymbolNode->firstInstructionInCode + (2 * BYTES_PER_LINE));
return;
}
else
{
writeJumpToInstruction(mySymbolNode->firstInstructionInCode + BYTES_PER_LINE);
return;
}
}

void writeContinueCode(BooleanBlockNode* myAbsBlock)
{
abstractSymbolNode* travelPointer = myAbsBlock;
int numBlocksBack = 0;
while( ((BooleanBlockNode*)travelPointer)->myType != FOR_BLOCK && ((BooleanBlockNode*)travelPointer)->myType != WHILE_BLOCK)
{
++numBlocksBack;
travelPointer = ((BooleanBlockNode*)travelPointer)->parentBlock;
}

symbolNode* mySymbolNode = symbolTable.getContinueInstructionNode(numBlocksBack);

if( ((BooleanBlockNode*)travelPointer)->myType == FOR_BLOCK)
{
writeJumpToInstruction(mySymbolNode->firstInstructionInCode + BYTES_PER_LINE );
return;
}
else
{
writeJumpToInstruction(getFirstBranchInstruct(mySymbolNode->firstInstructionInCode));
return;
}
}


//writes the needed instructions to move the address of var varName into the this register
void moveAddressToThisRegister(char* varName)
{
if(strcmp(varName, "this") == 0)
return;

Variable* mySymNode = (Variable*) symbolTable.variableFindTotal(varName);
int uniqueIdentifierNum = mySymNode->uniqueIdentifierNum;
unsigned char tempBytes[BYTES_PER_LINE];

if(mySymNode->symbolType == GLOBAL_VARIABLE)
{
writeMoveGlobalAddrToReg(tempBytes, THIS_REGISTER_NUM, uniqueIdentifierNum);
return;
}

else if(mySymNode->symbolType == FUNCTION_LOCAL_VARIABLE || mySymNode->symbolType == PARAMETER)
{
writeMoveStackAddrToReg(THIS_REGISTER_NUM, uniqueIdentifierNum);
return;
}

int offset = -1;
offset = symbolTable.getIndexOfMemberVar( ((Member_Variable*)mySymNode), varName);
contentsOfRegisters[THIS_REGISTER_NUM] = -1;
writeGetMemberVarAddrOfObjInstruction(THIS_REGISTER_NUM, THIS_REGISTER_NUM, offset);
return;
}


//returns the register which contains the value that the expression evaluates to.
unsigned char writeExpressionPortion(ExpressionNode* myExpression)
{
Type* leftType = NULL;
Type* rightType = NULL;
Op_Type leftOp;
Op_Type rightOp;

Type VoidType = Type((char*)"void");
unsigned char returnRegister = 0;

unsigned char firstReg = 0;
unsigned char secondReg = 0;
unsigned char memberVarReg = 0;

// void convertInstructionWrite(Op_Type destinationType, char destinationReg, Op_Type sourceType, Source_Location secondLocation, void* inputArg)
if(myExpression->myType == INT_LIT)
{
	returnRegister = registerIndexFind();
	isInUseArray[returnRegister] = true;
	isNeededArray[returnRegister] = true;
	contentsOfRegisters[returnRegister] = -1;
	
	writeCopyIntLiteralToRegisterInstruction(returnRegister, ((IntegerLiteralNode*)myExpression)->myValue);
	return returnRegister;	
}
else if(myExpression->myType == DOUBLE_LIT)
{
	returnRegister = registerIndexFind();
	isInUseArray[returnRegister] = true;
	isNeededArray[returnRegister] = true;
	contentsOfRegisters[returnRegister] = -1;

	writeCopyDoubleLiteralToRegisterInstruction(returnRegister, ((DoubleLiteralNode*)myExpression)->myValue);
	return returnRegister;
}

else if(myExpression->myType == CHAR_LIT)
{
	returnRegister = registerIndexFind();
	isInUseArray[returnRegister] = true;
	isNeededArray[returnRegister] = true;
	contentsOfRegisters[returnRegister] = -1;

	writeCopyCharLiteralToRegisterInstruction(returnRegister, ((CharLiteralNode*)myExpression)->myValue);
	return returnRegister;
}

else if(myExpression->myType == BOOL_LIT)
{
	returnRegister = registerIndexFind();
	isInUseArray[returnRegister] = true;
	isNeededArray[returnRegister] = true;
	contentsOfRegisters[returnRegister] = -1;

	//no other type is convertable to boolean
	writeCopyBooleanLiteralToRegisterInstruction(returnRegister, ((BooleanLiteralNode*)myExpression)->myValue);
	return returnRegister;

}

else if(myExpression->myType == NULL_EXPRESSION)
{
	returnRegister = registerIndexFind();
	isInUseArray[returnRegister] = true;
	isNeededArray[returnRegister] = true;
	contentsOfRegisters[returnRegister] = -1;
	writeCopyIntLiteralToRegisterInstruction(returnRegister, 0);
	return returnRegister;
}

//void writeUnaryOpInstruction(tokenType unaryOp, Source_Location outputLocation, unsigned char destinationReg, Op_Type inputType, Source_Location inputLocation, void* inputArg)
else if(myExpression->myType == UNARY_EXPRESSION)
{
firstReg = writeExpressionPortion( ((UnaryOperationNode*)myExpression)->myExpression);
myParseTree.getType( ((UnaryOperationNode*)myExpression)->myExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);
returnRegister = registerIndexFind();
isInUseArray[returnRegister] = true;
isNeededArray[returnRegister] = true;
contentsOfRegisters[returnRegister] = -1;
writeUnaryOpInstruction(   ((UnaryOperationNode*)myExpression)->myToken, REGISTER_VAL, returnRegister, leftOp, REGISTER_VAL, &firstReg);
isNeededArray[firstReg] = false;
return returnRegister;
}

else if(myExpression->myType == BINARY_EXPRESSION)
{
firstReg = writeExpressionPortion( ((BinaryExpressionNode*)myExpression)->firstExpression);
myParseTree.getType( ((BinaryExpressionNode*)myExpression)->firstExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);

secondReg = writeExpressionPortion( ((BinaryExpressionNode*)myExpression)->secondExpression);
myParseTree.getType( ((BinaryExpressionNode*)myExpression)->secondExpression, &rightType, symbolTable);
rightOp = getOpType(rightType);

if(leftOp != rightOp)
{
if(leftOp == DOUBLE_T)
{
convertInstructionWrite(DOUBLE_T, secondReg, rightOp, REGISTER_VAL, &secondReg);
}

else if(rightOp == DOUBLE_T)
{
convertInstructionWrite(DOUBLE_T, firstReg, leftOp, REGISTER_VAL, &firstReg);
}

else if(leftOp == INTEGER_T)
{
convertInstructionWrite(INTEGER_T, secondReg, rightOp, REGISTER_VAL, &secondReg);
}

else if(rightOp == INTEGER_T)
{
convertInstructionWrite(INTEGER_T, firstReg, leftOp, REGISTER_VAL, &firstReg);
}

else if(leftOp == CHAR_T)
{
convertInstructionWrite(CHAR_T, secondReg, rightOp, REGISTER_VAL, &secondReg);
}

else if(rightOp == CHAR_T)
{
convertInstructionWrite(CHAR_T, firstReg, leftOp, REGISTER_VAL, &firstReg);
}
else
{
std::cout << "Error: Unexpected types encountered in binary arithmetic code generation\n";
exit(1);
}
}


returnRegister = registerIndexFind();
isInUseArray[returnRegister] = true;
isNeededArray[returnRegister] = true;
contentsOfRegisters[returnRegister] = -1;

myParseTree.getType(myExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);
//void arithmeticInstructionWrite(tokenType instrSymbol, Source_Location locationOutput, Op_Type resultType, char destReg, Source_Location location1, void* firstLocPointer, Source_Location location2, void* secondLocPointer)
arithmeticInstructionWrite( ((BinaryExpressionNode*)myExpression)->myToken, REGISTER_VAL, leftOp, returnRegister, REGISTER_VAL, &firstReg, REGISTER_VAL, &secondReg);
isNeededArray[firstReg] = false;
isNeededArray[secondReg] = false;
return returnRegister;
}

else if(myExpression->myType == EMPTY_EXPRESSION_NODE)
{
writeJumpToInstruction(myByteCode.size() + BYTES_PER_LINE);
return RETURN_REGISTER_NUM;
}

else if(myExpression->myType == VARIABLE_EXPRESSION)
{
firstReg = getAddressRoutine( ((VariableNode*)myExpression)->varName);
secondReg = registerIndexFind();
isInUseArray[secondReg] = true;
isNeededArray[secondReg] = true;
contentsOfRegisters[returnRegister] = -1;
myParseTree.getType(myExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);

//void writeDereferenceBooleanInstruction(unsigned char destReg, unsigned char sourceAddrReg)
if(leftOp == INTEGER_T)
{
writeDereferenceIntegerInstruction(secondReg, firstReg);
}
else if(leftOp == DOUBLE_T)
{
writeDereferenceDoubleInstruction(secondReg, firstReg);
}
else if(leftOp == CHAR_T)
{
writeDereferenceCharInstruction(secondReg, firstReg);
}
else if(leftOp == BOOLEAN_T)
{
writeDereferenceBooleanInstruction(secondReg, firstReg);
}
else
{
writeCopyRegisterToRegisterInstruction(secondReg, firstReg);
}

isNeededArray[firstReg] = false;
return secondReg;
}

else if(myExpression->myType == PLUS_PLUS_EXPR || myExpression->myType == MINUS_MINUS_EXPR)
{
ExpressionNode* innerExpression = NULL;
if(myExpression->myType == PLUS_PLUS_EXPR)
innerExpression = ((PlusPlusNode*)myExpression)->myExpr;
else
innerExpression = ((MinusMinusNode*)myExpression)->myExpr;

if(innerExpression->myType == MEM_VAR_EXPR)
{
myParseTree.getType(innerExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);
if(strcmp( ((MemberVarExpression*)innerExpression)->objectName, "this") == 0)
{
firstReg = getAddressRoutine( ((MemberVarExpression*)innerExpression)->varName);
   //void writeCompoundAssignmentInstruction(tokenType typeOfCompound, unsigned char sourceReg, Op_Type leftType, Sour     ce_Location rightLocation, void* rightArg)
}
else
{
memberVarReg = getAddressRoutine( ((MemberVarExpression*)innerExpression)->objectName);
firstReg =     getMemberVarOfObj(memberVarReg, ((MemberVarExpression*)innerExpression)->objectName, ((MemberVarExpression*)innerExpression)->varName);
isNeededArray[memberVarReg] = false;
}
int tempI = 1;
if(myExpression->myType == PLUS_PLUS_EXPR)
writeCompoundAssignmentInstruction(PLUS_EQUALS, firstReg, leftOp, LITERAL_L, &tempI);
else
writeCompoundAssignmentInstruction(MINUS_EQUALS, firstReg, leftOp, LITERAL_L, &tempI); 

return firstReg;
}

//in this case, innerExpression is a regular variable
else
{
firstReg = getAddressRoutine( ((VariableNode*)innerExpression)->varName);
myParseTree.getType(innerExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);
int tempI = 1;
if(myExpression->myType == PLUS_PLUS_EXPR)
writeCompoundAssignmentInstruction(PLUS_EQUALS, firstReg, leftOp, LITERAL_L, &tempI);
else
writeCompoundAssignmentInstruction(MINUS_EQUALS, firstReg, leftOp, LITERAL_L, &tempI);

return firstReg;
}


}

else if(myExpression->myType == MEM_VAR_EXPR)
{
if( strcmp( ((MemberVarExpression*)myExpression)->objectName, "this") == 0)
{
firstReg = getAddressRoutine( ((MemberVarExpression*)myExpression)->varName);
secondReg = registerIndexFind();
isInUseArray[secondReg] = true;
isNeededArray[secondReg] = true;
contentsOfRegisters[secondReg] = -1;
myParseTree.getType(myExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);

if(leftOp == INTEGER_T)
{
writeDereferenceIntegerInstruction(secondReg, firstReg);
}
else if(leftOp == DOUBLE_T)
writeDereferenceDoubleInstruction(secondReg, firstReg);

else if(leftOp == CHAR_T)
writeDereferenceCharInstruction(secondReg, firstReg);

else if(leftOp == BOOLEAN_T)
writeDereferenceBooleanInstruction(secondReg, firstReg);

else
writeCopyRegisterToRegisterInstruction(secondReg, firstReg);

isNeededArray[firstReg] = false;
return secondReg;

}
else
{
memberVarReg = getAddressRoutine( ((MemberVarExpression*)myExpression)->objectName);
firstReg = 	getMemberVarOfObj(memberVarReg, ((MemberVarExpression*)myExpression)->objectName, ((MemberVarExpression*)myExpression)->varName);
secondReg = registerIndexFind();
isInUseArray[secondReg] = true;
isNeededArray[secondReg] = true;
contentsOfRegisters[secondReg] = -1;
myParseTree.getType(myExpression, &leftType, symbolTable);
leftOp = getOpType(leftType);
if(leftOp == INTEGER_T)
{
writeDereferenceIntegerInstruction(secondReg, firstReg);
}
else if(leftOp == DOUBLE_T)
{
writeDereferenceDoubleInstruction(secondReg, firstReg);
}
else if(leftOp == CHAR_T)
{
writeDereferenceCharInstruction(secondReg, firstReg);
}
else if(leftOp == BOOLEAN_T)
{
writeDereferenceBooleanInstruction(secondReg, firstReg);
}
else
writeCopyRegisterToRegisterInstruction(secondReg, firstReg);

isNeededArray[memberVarReg] = false;
isNeededArray[firstReg] = false;
return secondReg;
}

}

else if(myExpression->myType == STRING_LIT)
{
firstReg = registerIndexFind();
int myNum = symbolTable.getStringNum( ((StringLiteralNode*)myExpression)->myStringPointer);
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = myNum;
writeCopyStringLocationToRegisterInstruction(firstReg, myNum);
return firstReg;
}

else if(myExpression->myType == NON_MEM_FUNC_EXPR || myExpression->myType == MEM_FUNC_EXPR || myExpression->myType == NEW_OBJECT_EXPRESSION || myExpression->myType == BUILTIN_MEM_FUNC)
{
symbolNode* myNode = NULL;
if(myExpression->myType == NON_MEM_FUNC_EXPR)
{
myNode = symbolTable.getNonMemberFunction( ((NonMemberFunctionExpression*)myExpression)->funcName, NULL);
//this writes the code to call the function, too.
functionParameterSet( ((FunctionExpression*)myExpression), false);


if( *(((Function*)myNode)->functionReturnType) != VoidType)
{
firstReg = registerIndexFind();
isInUseArray[firstReg] = true;
isNeededArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeCopyRegisterToRegisterInstruction(firstReg, RETURN_REGISTER_NUM);
isNeededArray[RETURN_REGISTER_NUM] = false;
return firstReg;
}

else
{
isNeededArray[RETURN_REGISTER_NUM] = false;
isInUseArray[RETURN_REGISTER_NUM] = false;
return RETURN_REGISTER_NUM;
}

}

else if(myExpression->myType == MEM_FUNC_EXPR)
{
myNode = symbolTable.getMemberFunctionOfObject( ((MemberFunctionExpression*)myExpression)->objectName, ((FunctionExpression*)myExpression)->funcName);
functionParameterSet( ((FunctionExpression*)myExpression), true);


if(*( ((Function*)myNode)->functionReturnType) != VoidType)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeCopyRegisterToRegisterInstruction(firstReg, RETURN_REGISTER_NUM);
isInUseArray[RETURN_REGISTER_NUM] = false;
isNeededArray[RETURN_REGISTER_NUM] = false;
return firstReg;
}
else
{
isInUseArray[RETURN_REGISTER_NUM] = false;
isNeededArray[RETURN_REGISTER_NUM] = false;
return RETURN_REGISTER_NUM;
}

}

else if(myExpression->myType == BUILTIN_MEM_FUNC)
{
functionParameterSet( ((FunctionExpression*)myExpression), true);

if( ((BuiltinMemberFunctionExpression*)myExpression)->functionName != PUT && ((BuiltinMemberFunctionExpression*)myExpression)->functionName != SORT && ((BuiltinMemberFunctionExpression*)myExpression)->functionName != UPDATE)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeCopyRegisterToRegisterInstruction(firstReg, RETURN_REGISTER_NUM);
return firstReg;
}
isInUseArray[RETURN_REGISTER_NUM] = false;
isNeededArray[RETURN_REGISTER_NUM] = false;
return RETURN_REGISTER_NUM;
}

//handling newObjectExpression
else
{
myNode = symbolTable.getConstructorOfClass( ((NewObjectExpression*)myExpression)->className);
functionParameterSet( ((NewObjectExpression*)myExpression)->myFuncExpression, true);
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeCopyRegisterToRegisterInstruction(firstReg, RETURN_REGISTER_NUM);
isInUseArray[RETURN_REGISTER_NUM] = false;
isNeededArray[RETURN_REGISTER_NUM] = false;
return firstReg;
}
}


else if(myExpression->myType == BUILTIN_CREATION_EXPRESSION)
{
if( ((BuiltinCreationExpression*)myExpression)->numElements != NULL)
{
firstReg = writeExpressionPortion( ((BuiltinCreationExpression*)myExpression)->numElements);
writeCopyRegisterToRegisterInstruction(NUMBER_OF_GENERAL_REGISTERS, firstReg);
isNeededArray[firstReg] = false;
}
writeBuiltinObjectConstructorCall(((BuiltinCreationExpression*)myExpression)->fullType);
firstReg = registerIndexFind();
isInUseArray[firstReg] = true;
isNeededArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeCopyRegisterToRegisterInstruction(firstReg, RETURN_REGISTER_NUM);
isInUseArray[RETURN_REGISTER_NUM] = false;
isNeededArray[RETURN_REGISTER_NUM] = false;
return firstReg;
}

else if(myExpression->myType == PRINT_EXPR)
{

if( ((FunctionExpression*)myExpression)->argumentList.size() == 0)
{
std::cout << "Error: print() requires at least 1 argument\n";
exit(1);
}
	for(int i = 0; i < ((FunctionExpression*)myExpression)->argumentList.size(); ++i)
	{
	firstReg = writeExpressionPortion( ((FunctionExpression*)myExpression)->argumentList[i]);
        myParseTree.getType( ((FunctionExpression*)myExpression)->argumentList[i], &leftType, symbolTable);
	leftOp = getOpType(leftType);	
        writePrintInstruction(leftOp, firstReg);
	isNeededArray[firstReg] = false;	
	}
	return RETURN_REGISTER_NUM;

}

else if(myExpression->myType == GET_INT_EXPR)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeGetIntInstruction(firstReg);
return firstReg;
} 

else if(myExpression->myType == GET_DOUBLE_EXPR)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeGetDoubleInstruction(firstReg);
return firstReg;
}

else if(myExpression->myType == GET_BOOLEAN_EXPR)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeGetBooleanInstruction(firstReg);
return firstReg;
}

else if(myExpression->myType == GET_CHAR_EXPR)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeGetCharInstruction(firstReg);
return firstReg;
}

else if(myExpression->myType == GET_WORD_EXPR)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeGetWordInstruction(firstReg);
return firstReg;
}

else if(myExpression->myType == GET_LINE_EXPR)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeGetLineInstruction(firstReg);
return firstReg;
}

else if(myExpression->myType == IS_INPUT_FAIL_FUNC_EXPR)
{
firstReg = registerIndexFind();
isNeededArray[firstReg] = true;
isInUseArray[firstReg] = true;
contentsOfRegisters[firstReg] = -1;
writeIsInputFailInstruction(firstReg);
return firstReg;
}

return RETURN_REGISTER_NUM;
}


unsigned char randNumGenerator()
{
return ((unsigned char)(rand() % NUMBER_OF_GENERAL_REGISTERS));
}

void processClassDefinitionCode(abstractSymbolNode* classNode)
{
bool debugger = false;
if(debugger)
std::cout << "entered parseClassDefinitionCode()\n";
symbolTable.enterClass(  ((ClassDefinitionNode*)classNode)->className );
for(int i = 0; i < ((ClassDefinitionNode*)classNode)->memberFunctionList.size(); ++i)
{
processMemberFunctionDefinition( ((ClassDefinitionNode*)classNode)->memberFunctionList[i]);
}
symbolTable.backtrackOne();
}


void processStatementCode(abstractSymbolNode* mySymbolNode)
{

ExpressionNode* myExpression = NULL;
StatementNode* myStatement = NULL;
unsigned char LValReg = 0;
unsigned char firstTermReg = 0;
unsigned char secondTermReg = 0;
unsigned char memRegAddr = 0;
Type* firstType = NULL;
Type* secondType = NULL;
Op_Type firstOpType, secondOpType;
Type NullType = Type((char*)"NULL");
vector<unsigned char> isNeededsToFree = std::vector<unsigned char>();

	if(mySymbolNode->symbolType == EXPRESSION)
	{
		myExpression = ((ExpressionNode*)mySymbolNode);
		firstTermReg = writeExpressionPortion(myExpression);
		isNeededArray[firstTermReg] = false;
	}

	else if(mySymbolNode->symbolType == STATEMENT)
	{
		myStatement = ((StatementNode*)mySymbolNode);

		if(myStatement->myType == DECLARATION_STATEMENT)
		{
		writeVariableDeclare( ((DeclarationStatement*)myStatement)->identifierName);	
		}
		else if(myStatement->myType == DECLARE_ASSIGN_STATEMENT)
		{
		writeVariableDeclare( ((DeclareAssignStatement*)myStatement)->declarePortion->identifierName);
		firstType = symbolTable.getTypeOfVariable(  ((DeclareAssignStatement*)myStatement)->declarePortion->identifierName);
		secondTermReg = writeExpressionPortion(((DeclareAssignStatement*)myStatement)->assignedPortion);
		myParseTree.getType(  ((DeclareAssignStatement*)myStatement)->assignedPortion, &secondType, symbolTable);
		firstOpType = getOpType(firstType);
		secondOpType = getOpType(secondType);
		//if this is true, then a conversion must be done. The result of this will be stored in secondTermReg
		if(*secondType != NullType && firstOpType != secondOpType) 
		{
		//void convertInstructionWrite(Op_Type destinationType, char destinationReg, Op_Type sourceType, Source_Location secondLocation, void* inputArg)
		convertInstructionWrite(firstOpType, secondTermReg, secondOpType, REGISTER_VAL, &secondTermReg);
		}			
		// void writeAssignment(unsigned char regMemVal, Op_Type leftType, Source_Location rightSourceLocation, void* inputArg) 
		firstTermReg = getAddressRoutine( ((DeclareAssignStatement*)myStatement)->declarePortion->identifierName);
		writeAssignment(firstTermReg, firstOpType, REGISTER_VAL, &secondTermReg);
		isNeededArray[firstTermReg] = false;
		isNeededArray[secondTermReg] = false;
	}

	else if(myStatement->myType == ASSIGNMENT_STATEMENT)
	{
	secondTermReg = writeExpressionPortion( ((AssignmentStatement*)myStatement)->rightValue);

	if(  ((AssignmentStatement*)myStatement)->leftValue->myType == VARIABLE_EXPRESSION)
	{
	firstTermReg = getAddressRoutine(  ((VariableNode*)(((AssignmentStatement*)myStatement)->leftValue))->varName);
	}		
	else if( ((AssignmentStatement*)myStatement)->leftValue->myType == MEM_VAR_EXPR)
	{
	MemberVarExpression* memExpr = ((MemberVarExpression*) (((AssignmentStatement*)myStatement)->leftValue));
	memRegAddr = getAddressRoutine(memExpr->objectName);
	firstTermReg = getMemberVarOfObj(memRegAddr, memExpr->objectName, memExpr->varName);
	isNeededArray[memRegAddr] = false;
	}

	else
	{
	std::cout << "Error: L-Value must contain a variable of some sort\n";
	exit(1);
	}
myParseTree.getType( ((AssignmentStatement*)myStatement)->leftValue, &firstType, symbolTable);
myParseTree.getType( ((AssignmentStatement*)myStatement)->rightValue, &secondType, symbolTable);
firstOpType = getOpType(firstType);
secondOpType = getOpType(secondType);

	if(*secondType != NullType && firstOpType != secondOpType)
	{
	convertInstructionWrite(firstOpType, secondTermReg, secondOpType, REGISTER_VAL, &secondTermReg);
	}
	writeAssignment(firstTermReg, firstOpType, REGISTER_VAL, &secondTermReg);
	isNeededArray[firstTermReg] = false;
	isNeededArray[secondTermReg] = false;
	}

	else if(myStatement->myType == COMPOUND_ASSIGNMENT_STATEMENT)
	{
	secondTermReg = writeExpressionPortion( ((CompoundAssignmentStatement*)myStatement)->rightHalf);
	if(  ((CompoundAssignmentStatement*)myStatement)->leftHalf->myType == VARIABLE_EXPRESSION)
	{
	firstTermReg = getAddressRoutine( ((VariableNode*) ((CompoundAssignmentStatement*)myStatement)->leftHalf)->varName);
	}
	else if( ((CompoundAssignmentStatement*)myStatement)->leftHalf->myType == MEM_VAR_EXPR)
   	{
        MemberVarExpression* memExpr = ((MemberVarExpression*) (((CompoundAssignmentStatement*)myStatement)->leftHalf));
	memRegAddr = getAddressRoutine(memExpr->objectName);
	firstTermReg = getMemberVarOfObj(memRegAddr, memExpr->objectName, memExpr->varName);
	isNeededArray[memRegAddr] = false;
	}
	else
	{
	std::cout << "Error: Only variables and member variables can appear on the left side of a compound assignment statement\n";
	exit(1);
	}
myParseTree.getType( ((CompoundAssignmentStatement*)myStatement)->leftHalf, &firstType, symbolTable);
myParseTree.getType( ((CompoundAssignmentStatement*)myStatement)->rightHalf, &secondType, symbolTable);
firstOpType = getOpType(firstType);
secondOpType = getOpType(secondType);
	
	if(*secondType != NullType && firstOpType != secondOpType)
        {
 	convertInstructionWrite(firstOpType, secondTermReg, secondOpType, REGISTER_VAL, &secondTermReg);
	}

	//void writeCompoundAssignmentInstruction(tokenType typeOfCompound, unsigned char sourceReg, Op_Type leftType, Source_Location rightLocation, void* rightArg)
	writeCompoundAssignmentInstruction( ((CompoundAssignmentStatement*)myStatement)->compoundOperator, firstTermReg, firstOpType, REGISTER_VAL, &secondTermReg);
	isNeededArray[firstTermReg] = false;
	isNeededArray[secondTermReg] = false;

}

else if(myStatement->myType == EXPRESSION_STATEMENT)
{
firstTermReg = writeExpressionPortion( ((ExpressionStatement*)myStatement)->myExpression);
isNeededArray[firstTermReg] = false;
}

else if(myStatement->myType == FUNCTION_CALL_STATEMENT)
{
firstTermReg = writeExpressionPortion(  ((FunctionCallStatement*)myStatement)->myStatement);
isNeededArray[firstTermReg] = false;
}

else if(myStatement->myType == RETURN_STATEMENT)
{
firstTermReg = writeExpressionPortion( ((ReturnStatement*)myStatement)->myStatement);
firstType = symbolTable.getFunctionReturnType(symbolTable.getFuncNode());
myParseTree.getType( ((ReturnStatement*)myStatement)->myStatement, &secondType, symbolTable);
firstOpType = getOpType(firstType);
secondOpType = getOpType(secondType);

if(*secondType != NullType && firstOpType != secondOpType)
{
convertInstructionWrite(firstOpType, (unsigned char)RETURN_REGISTER_NUM, secondOpType, REGISTER_VAL, &firstTermReg);
}
else
writeCopyRegisterToRegisterInstruction( (unsigned char) RETURN_REGISTER_NUM, firstTermReg);

if(symbolTable.isMemberFunc() || strcmp(symbolTable.getFuncNode()->functionName, "main") != 0)
writeReturnFromFunctionInstruction();
else
writeExitMainInstruction(mainType);
isNeededArray[firstTermReg] = false;
isNeededArray[RETURN_REGISTER_NUM] = false;

}
else
{
std::cout << "Error: Undefined statement type encountered\n";
exit(1);
}

}

	else
	{
	std::cout << "Error:\tprocessStatementCode called on abstractSymbolNode which was neither an expression nor a statement.\n" << std::endl;
	}

}

void processNonMemberFunctionDefinition(abstractSymbolNode* funcNode)
{
symbolTable.enterNonMemberFunction( ((FunctionDefinitionNode*)funcNode)->funcName);
processFunctionDefinition(funcNode, false);
symbolTable.backtrackOne();
}

void processMemberFunctionDefinition(abstractSymbolNode* funcNode)
{
symbolTable.enterMemberFunction( ((FunctionDefinitionNode*)funcNode)->funcName);
processFunctionDefinition(funcNode, true);
symbolTable.backtrackOne();
}

//helper function that parses the body of a loop statement or else statement (really, just a loop that keeps readins statements until the block ends. This function does not alter the symbol table unless another block is entered, in which
//case, parseBlockCode will be entered with the correct new blockNum (which will in turn alter the symbol table)
void parseBlockBodyHelper(BooleanBlockNode* myBlockParse)
{
int newBlockNum = 0;

for(int i = 0; i < myBlockParse->blockStatementsList.size(); ++i)
{

if(myBlockParse->blockStatementsList[i]->symbolType != BOOLEAN_BLOCK)
{
StatementNode* myStat = (StatementNode*) myBlockParse->blockStatementsList[i];
if(myStat->myType == EXPRESSION_STATEMENT && ((ExpressionStatement*)myStat)->myExpression->myType == BREAK_EXPRESSION)
writeBreakCode(myBlockParse);

else if(myStat->myType == EXPRESSION_STATEMENT && ((ExpressionStatement*)myStat)->myExpression->myType == CONTINUE_EXPRESSION)
writeContinueCode(myBlockParse);

else
processStatementCode( myBlockParse->blockStatementsList[i]);
}

else
{
processBlockCode((BooleanBlockNode*) myBlockParse->blockStatementsList[i], newBlockNum);
++newBlockNum;
}

}

}

//this is a helper function which takes as input a line of code which is the start location of a for loop or while looop
//it then returns the spot in the code of the first branch instruction encountered
int getFirstBranchInstruct(int startLocation)
{

Op_Code myCode;
int myLocation = startLocation;
while(myLocation < myByteCode.size())
{
myCode = *((Op_Code*)&myByteCode[myLocation]);
if(myCode == BRANCH_ON_FALSE_LITERAL || myCode == BRANCH_ON_FALSE_REG_VAL || myCode == BRANCH_ON_FALSE_MEM_ADDR)
{
return myLocation;
}
myLocation += BYTES_PER_LINE;
}

return -1;
}

symbolNode* getIfParent(BooleanBlockNode* myBlockParse, int blockNumOfElse)
{
abstractSymbolNode* tempNode = NULL;
//how many blocks back to go to reach an if statement from myBlockParse.
int numBlocksBack = 0;
abstractSymbolNode* parentNode = myBlockParse->parentBlock;
//set to equal the index in the parse tree's parent block of the current boolean block
int parseIndex = 0;
bool debugger = false;

if(debugger)
std::cout << "in getIfParent() with blockNumOfElse of " << blockNumOfElse << std::endl;

if( ((IfTypeBlock*)myBlockParse)->myIfType == IF_TYPE)
return
symbolTable.getBlockNum(blockNumOfElse);


while(true)
{
	if(parentNode->symbolType == MEMBER_FUNCTION_DEFINITION || parentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
 	{
		if( ((FunctionDefinitionNode*)parentNode)->functionBody[parseIndex]->symbolType == BOOLEAN_BLOCK
		&& ((BooleanBlockNode*)   (  ((FunctionDefinitionNode*)parentNode)->functionBody[parseIndex])) == myBlockParse)
			break;

		else
		{
			++parseIndex;
			continue;
		} 
	}

	else
	{
		if( ((BooleanBlockNode*)parentNode)->blockStatementsList[parseIndex]->symbolType == BOOLEAN_BLOCK
		&& ((BooleanBlockNode*) (  ((BooleanBlockNode*)parentNode)->blockStatementsList[parseIndex])) == myBlockParse)
			break;

		else
		{
			++parseIndex;
			continue;
		}
	}


}

int i;
for( i = parseIndex - 1; i >= 0; --i)
{
	if(parentNode->symbolType == MEMBER_FUNCTION_DEFINITION || parentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
	{
		tempNode = ((FunctionDefinitionNode*)parentNode)->functionBody[i];
	}

	else
		tempNode = ((BooleanBlockNode*)parentNode)->blockStatementsList[i];

	if(tempNode->symbolType == BOOLEAN_BLOCK && ((BooleanBlockNode*)tempNode)->myType == IF_TYPE_BLOCK)
	{
		++numBlocksBack;
		if( ((IfTypeBlock*)tempNode)->myIfType == IF_TYPE)
			break;
		else
			continue;
	}

	else
	{
		std::cout << "Error: if statement must precede an else if or else statement.\n";
		exit(1);
	}
}

if(i < 0)
{
std::cout << "Error: if statement must precede an else if or else statement.\n";
exit(1);
}

return symbolTable.getBlockNum(blockNumOfElse - numBlocksBack);
}

//determines if this if statement is the last if-elif-else statement of the block
//if it is, then the original if parent is gotten, and its exit loop instrution location is set
void checkAndUpdateIfStatement(BooleanBlockNode* currentBoolBlock, int blockNum)
{
abstractSymbolNode* parentPointer = currentBoolBlock->parentBlock;
abstractSymbolNode* nextPointer = NULL;
symbolNode* ifParent = NULL;
symbolNode* currentBlockSymbol = NULL;
bool debugger = false;

if(debugger)
std::cout << "In checkAndUpdateIfStatement() with blockNum of " << blockNum << std::endl;
//in this case, this is definitely the end of the block.
if(((IfTypeBlock*)currentBoolBlock)->myIfType == ELSE_TYPE)
{
ifParent = getIfParent(currentBoolBlock, blockNum);
currentBlockSymbol = symbolTable.getBlockNum(blockNum);
setJump(ifParent->firstInstructionInCode + BYTES_PER_LINE, ((Block*)currentBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);
return;
}

//in this case, checking is this statement (which is an if or else if statement) is followed by another else if or else. Otherwise, this is the end of the block.

int blockSize = -1;
int indexOfChild = -1;

if(parentPointer->symbolType == BOOLEAN_BLOCK)
blockSize = ((BooleanBlockNode*)parentPointer)->blockStatementsList.size();

else
blockSize = ((FunctionDefinitionNode*)parentPointer)->functionBody.size();

for(int i = 0; i < blockSize; ++i)
{
if(parentPointer->symbolType == BOOLEAN_BLOCK)
{
if( ((BooleanBlockNode*)parentPointer)->blockStatementsList[i] == currentBoolBlock)
{
indexOfChild = i;
if(indexOfChild != blockSize - 1)
nextPointer = ((BooleanBlockNode*)parentPointer)->blockStatementsList[i + 1];
break;
}
}
else if(parentPointer->symbolType == NON_MEMBER_FUNCTION_DEFINITION || parentPointer->symbolType == MEMBER_FUNCTION_DEFINITION)
{
if( ((FunctionDefinitionNode*)parentPointer)->functionBody[i] == currentBoolBlock)
{
indexOfChild = i;
if(indexOfChild != blockSize - 1)
nextPointer = ((FunctionDefinitionNode*)parentPointer)->functionBody[i + 1];
break;
}
}
}

//if the index of the current bool block is at the end of this branch of the parse tree, or the statement that
//comes after this one is not a bool block, or the statement that comes after this statement is an if statement,
//then we are at the end of the if-elif-else chain. Otherwise, we are not at the end yet.
if( (indexOfChild == blockSize - 1) ||  nextPointer->symbolType != BOOLEAN_BLOCK || (nextPointer->symbolType == BOOLEAN_BLOCK
&& ((BooleanBlockNode*)nextPointer)->myType != IF_TYPE_BLOCK) || (   ((BooleanBlockNode*)nextPointer)->myType == IF_TYPE_BLOCK && ((IfTypeBlock*)nextPointer)->myIfType == IF_TYPE))
{
ifParent = getIfParent(currentBoolBlock, blockNum);
currentBlockSymbol = symbolTable.getBlockNum(blockNum);
setJump(ifParent->firstInstructionInCode + BYTES_PER_LINE, ((Block*)currentBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);
return;
} 

return;
}

void processBlockCode(BooleanBlockNode* myBlockParse, int blockNum)
{
symbolNode* myIfParent = NULL;

if(myBlockParse->myType == IF_TYPE_BLOCK &&  ( ((IfTypeBlock*)myBlockParse)->myIfType != IF_TYPE ))
myIfParent = getIfParent(myBlockParse, blockNum);

symbolTable.enterBlock(blockNum);

symbolNode* myBlockSymbol = symbolTable.getCurrent();
//if the statement is a for loop, then the initial statement is written to code before firstInstructionInCode
//otherwise, firstInstructionInCode is set to being right where it is when this function is first called.

if(myBlockParse->myType == FOR_BLOCK)
{
processStatementCode(  ((ForBlockNode*)myBlockParse)->initialStatement);
clearArrays();
myBlockSymbol->firstInstructionInCode = myByteCode.size();
}

else
myBlockSymbol->firstInstructionInCode = myByteCode.size();


if(myBlockParse->myType == FOR_BLOCK)
{
writeJumpToInstruction( myByteCode.size() + (3 * BYTES_PER_LINE));
writeJumpToInstruction(0);
writeJumpToInstruction(0);
}

else if(myBlockParse->myType == WHILE_BLOCK)
{
clearArrays();
writeJumpToInstruction(myByteCode.size() + (2 * BYTES_PER_LINE));
writeJumpToInstruction(0);
}

else if(myBlockParse->myType == IF_TYPE_BLOCK && ((IfTypeBlock*)myBlockParse)->myIfType == IF_TYPE)
{
writeJumpToInstruction(myByteCode.size() + (2 * BYTES_PER_LINE));
writeJumpToInstruction(0);
}

//this writes the code of the boolean condition evaluation and branch instruction
if(myBlockParse->myType != IF_TYPE_BLOCK || ((IfTypeBlock*)myBlockParse)->myIfType != ELSE_TYPE)
{
unsigned char boolReg;
boolReg = writeExpressionPortion(myBlockParse->booleanCondition);
writeBranchOnFalseRegValInstruction(boolReg);
isNeededArray[boolReg] = false; 
}

//this writes the code of the loop or conditional block
parseBlockBodyHelper(myBlockParse);

if(myBlockParse->myType == FOR_BLOCK)
{
//setting the location for the continue line to jump to to be the next line.
setJump( myBlockSymbol->firstInstructionInCode + (BYTES_PER_LINE), myByteCode.size());

//adding post-loop increment/statement
processStatementCode( ((ForBlockNode*)myBlockParse)->statementAction);

//setting lastInstructionInCode to be the start of the instruction right after the post loop action
((Block*)myBlockSymbol)->lastInstructionInBlock = myByteCode.size();

//line of code to loop back up to the boolean condition (the last line of the block)
writeJumpToInstruction( myBlockSymbol->firstInstructionInCode + (3 * BYTES_PER_LINE));

setJump(myBlockSymbol->firstInstructionInCode + (2 * BYTES_PER_LINE), ((Block*)myBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);

setBranch(getFirstBranchInstruct(myBlockSymbol->firstInstructionInCode), ((Block*)myBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);
clearArrays();
}

else if(myBlockParse->myType == WHILE_BLOCK)
{
clearArrays();
((Block*)myBlockSymbol)->lastInstructionInBlock = myByteCode.size();
writeJumpToInstruction(myBlockSymbol->firstInstructionInCode + (2 * BYTES_PER_LINE));
setJump(myBlockSymbol->firstInstructionInCode + BYTES_PER_LINE, ((Block*)myBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);

setBranch(getFirstBranchInstruct(myBlockSymbol->firstInstructionInCode), ((Block*)myBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);
}

else if(myBlockParse->myType == IF_TYPE_BLOCK && ((IfTypeBlock*)myBlockParse)->myIfType == IF_TYPE)
{
((Block*)myBlockSymbol)->lastInstructionInBlock = myByteCode.size();
writeJumpToInstruction(myBlockSymbol->firstInstructionInCode + BYTES_PER_LINE);
setBranch(getFirstBranchInstruct(myBlockSymbol->firstInstructionInCode), ((Block*)myBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);
}

else if(myBlockParse->myType == IF_TYPE_BLOCK && ((IfTypeBlock*)myBlockParse)->myIfType == ELIF_TYPE)
{
((Block*)myBlockSymbol)->lastInstructionInBlock = myByteCode.size();
writeJumpToInstruction(myIfParent->firstInstructionInCode + BYTES_PER_LINE);
setBranch(getFirstBranchInstruct(myBlockSymbol->firstInstructionInCode), ((Block*)myBlockSymbol)->lastInstructionInBlock + BYTES_PER_LINE);
}

//we have an else statement.
else
{
((Block*)myBlockSymbol)->lastInstructionInBlock = myByteCode.size() - BYTES_PER_LINE;
}

symbolTable.backtrackOne();

if(myBlockParse->myType == IF_TYPE_BLOCK)
checkAndUpdateIfStatement(myBlockParse, blockNum);

}

//resets the isNeededArray, isInUseArray and contents of registers arrays to being empty
void clearArrays()
{
for(int i = 0; i < 256; ++i)
{
isInUseArray[i] = isNeededArray[i] = false;
contentsOfRegisters[i] = -1;
}
}

void processFunctionDefinition(abstractSymbolNode* absFuncNode, bool isMemberFunction)
{
bool debugger = false;
if(debugger)
std::cout << "Entered processFunctionDefinition()\n";
int blockNum = 0;
bool mainDone = false;
unsigned char tempReg;
for(int i = 0; i < 256; ++i)
{
isInUseArray[i] = false;
isNeededArray[i] = false;
contentsOfRegisters[i] = -1;
}
if(isMemberFunction)
{
isNeededArray[THIS_REGISTER_NUM] = true;
isInUseArray[THIS_REGISTER_NUM] = true;
contentsOfRegisters[THIS_REGISTER_NUM] = false;
}

Type voidType = Type((char*)"void");
if(!isMemberFunction && strcmp(((FunctionDefinitionNode*)absFuncNode)->funcName, "main") == 0)
{
mainLocation = myByteCode.size();
if( *(((FunctionDefinitionNode*)absFuncNode)->returnType) == voidType)
mainType = NULL_T;
else
mainType = getOpType( ((FunctionDefinitionNode*)absFuncNode)->returnType);

}
abstractSymbolNode* tempNode = NULL;

symbolNode* funcNode = NULL;
if(!isMemberFunction)
funcNode = symbolTable.getNonMemberFunction( ((FunctionDefinitionNode*)absFuncNode)->funcName);
else
{
if(debugger)
std::cout << "Getting member function from symbol table now...";
funcNode = (Function*) symbolTable.getCurrent();
if(debugger)
std::cout << "Updating instruction location now...\n";
if(debugger && funcNode == NULL)
std::cout << "Note: funcNode is NULL!\n";
if(debugger)
std::cout << "Succesfully accesed symbol table!\n";
}

funcNode->firstInstructionInCode = myByteCode.size();
//writing the code to set the parameters (no more than 30 allowed for a function)
for(int i = 0; i < ((Function*)funcNode)->parameterList.size(); ++i)
{
writeSetParameterInstruction( ((unsigned char) i + NUMBER_OF_GENERAL_REGISTERS), 
((Function*)funcNode)->parameterList[i]->uniqueIdentifierNum);
}

//if we have a constructor, then the code to allocate the object must be written.
if(isMemberFunction && symbolTable.isConstructor(funcNode))
{
writeAddMemberVarsToObjInstruction( ((ClassDefinitionNode*)((MemberFunctionDefinitionNode*)absFuncNode)->parent)->memberVariableList.size());
}

if(debugger)
std::cout << "Made it up to main for loop of function processing\n";
int i = 0;
for(i = 0; i < ((FunctionDefinitionNode*)absFuncNode)->functionBody.size(); ++i)
{
//std::cout << i << "in processFunctionDefinition() " << std::endl;
tempNode = ((FunctionDefinitionNode*)absFuncNode)->functionBody[i];

if(tempNode->symbolType == BOOLEAN_BLOCK)
{
processBlockCode(((BooleanBlockNode*)tempNode), blockNum);
++blockNum;
}
else if(tempNode->symbolType == EXPRESSION)
{
tempReg = writeExpressionPortion((ExpressionNode*)tempNode);
isNeededArray[tempReg] = false;
}
else if(tempNode->symbolType == STATEMENT)
processStatementCode(tempNode);

else
{
std::cout << "Error: type encountered in function must be boolean block, statement, or expression\n";
exit(1);
}
}

if( isMemberFunction == false && strcmp(((FunctionDefinitionNode*)absFuncNode)->funcName, "main") == 0)
{
if(tempNode->symbolType != STATEMENT || ((StatementNode*)tempNode)->myType != RETURN_STATEMENT)
writeExitMainInstruction(mainType);
return;
}

if(isMemberFunction && symbolTable.isConstructor(funcNode))
{
writeCopyRegisterToRegisterInstruction( ((unsigned char)RETURN_REGISTER_NUM), ((unsigned char) THIS_REGISTER_NUM));
writeReturnFromFunctionInstruction();
return;
}

if( *(((FunctionDefinitionNode*)absFuncNode)->returnType) == (voidType))
{
writeReturnFromFunctionInstruction();
}

else
writeOutOfBoundsFaultInstruction();
return;

}


//the main function, which generates all of the code.
void generateCode()
{
bool debugger = false;
abstractSymbolNode* myRoot = myParseTree.getRoot();
abstractSymbolNode* tempNode = NULL;
//writing out all global assignments, pipeline creations, and global declarations
GlobalCodeWrite();

//now, looking in order for class definitions and non-member functions at the global level of the parse tree.

for(int i = 0; i < ((GlobalListNode*)myRoot)->globalStatementsList.size(); ++i)
	{
	tempNode = ((GlobalListNode*)myRoot)->globalStatementsList[i];

	//handling class definitions
	if(tempNode->symbolType == CLASS_DEFINITION)
		{
		processClassDefinitionCode(tempNode);
		}

	//handling non-member-function definitions
	else if(tempNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
		{
		processNonMemberFunctionDefinition(tempNode);
		}
	//otherwise, nothing needs to be done (i is incremented, and the loop continues)
	}

lastLineSpot = myByteCode.size();
//after this for loop terminates, all class definitions, global code, member function definitions, and non member function definitions code has been written out. Now, we need to add all string literals to the end of the code
//additionally, we have to go back and fill in the addresses for pipelines and string literals referenced in the code.
stringLit* myStringLit = NULL;
for(int i = 0; i < symbolTable.myStringLits.size(); ++i)
{
if(debugger)
std::cout << symbolTable.myStringLits.size() << " is symbolTable size\n";
myStringLit = symbolTable.myStringLits[i];
myStringLit->firstInstructionInCode = myByteCode.size();

	for(int j = 0; j < strlen(myStringLit->myStr); ++j)
	{
	myByteCode.push_back(myStringLit->myStr[j]);
	}
	myByteCode.push_back('\0');
}

//now that all strings have been added, we are going back to the start of the code to look for pipeline add function codes, jumps to main,
//and moving of code locations of strings to registers.

bool hasMain = false;
for(int i = 0; i < myByteCode.size(); i += BYTES_PER_LINE)
{
Op_Code myCode = *((Op_Code*)&myByteCode[i]);
if(myCode == JUMP_TO_MAIN)
{
if(debugger)
std::cout << "found main location\n";
hasMain = true;
if(mainLocation == -1)
{
std::cout << "Error: Program must have main function to compile\n";
exit(1);
}

absoluteIntegerSet(i + 1, mainLocation);
}

else if(myCode == PIPELINE_ADD_FUNCTION) 
{
if(debugger)
std::cout << "Updating PipeLine_Add_Function\n";
absoluteIntegerSet(i + 5, symbolTable.getNonMemberFunction(*((int*)&myByteCode[i + 5]))->firstInstructionInCode);
}

else if(myCode == COPY_STRING_LOCATION_TO_REGISTER)
{
if(debugger)
std::cout << "Updating copy_string_location_to_Register\n";
absoluteIntegerSet(i + 2, symbolTable.getStringLitNode( (*((int*)&myByteCode[i + 2])))->firstInstructionInCode);
}

else if(myCode == ASSIGNMENT_OP)
{
if(debugger)
std::cout << "checking for assignment op\n";
if( *((Source_Location*)&myByteCode[i + 3]) == CODE_LOCATION)
{
if(debugger)
{
std::cout << "updating Code_Location in Assignment op\n";
printLine( (&myByteCode[0] + i) );
}
absoluteIntegerSet(i + 4, symbolTable.getStringLitNode( *((int*)&myByteCode[i + 4]))->firstInstructionInCode);
}
}

} 
//printAll();
}

//searches for a regular register (numbered 0 - 222) to use next.
//if a register exists which is in use, not needed, and isn't storing
//an address, then one of these such addresses is chosen at random
//otherwise, a register that is not in use and not needed will attempt
//to be selected next
//if this isn't available, then a register which is in use but not needed
//will be selected.
//otherwise, no register was available for selection.
unsigned char registerIndexFind()
{
bool hasAvailableRegister = false;
bool hasUsedOpenRegister = false;
bool hasUnusedRegister = false;
unsigned char returnVal = 0;
for(int i = 0; i < NUMBER_OF_GENERAL_REGISTERS; ++i)
{
if(isNeededArray[i] == false)
{
hasAvailableRegister = true;
break;
}
}

if(!hasAvailableRegister)
{
std::cout << "Error: no register was available for this line of code.\n";
exit(1);
}

for(int i = 0; i < NUMBER_OF_GENERAL_REGISTERS; ++i)
{
if(isInUseArray[i] == true && isNeededArray[i] == false && contentsOfRegisters[i] == -1)
{
hasUsedOpenRegister = true;
break;
}
}

if(hasUsedOpenRegister == true)
{
while(true)
{
returnVal = randNumGenerator();
if(isInUseArray[returnVal] == true && isNeededArray[returnVal] == false && contentsOfRegisters[returnVal] == -1)
return returnVal;
}
}

for(int i = 0; i < NUMBER_OF_GENERAL_REGISTERS; ++i)
{
if(isInUseArray[i] == false)	
{
hasUnusedRegister = true;
break;
}
}

if(hasUnusedRegister == true)
{
while(true)
{
returnVal = randNumGenerator();
if(isInUseArray[returnVal] == false)
return returnVal;

}
}

while(true)
{
returnVal = randNumGenerator();
if(isNeededArray[returnVal] == false)
return returnVal;
}


return 0;
}

//takes as input the name of a variable.
//figures out what type of variable it is, and calls the appropriate
//functions to put the address in a register, and returns that register.
//also, sets the isUsed and isNeeded arrays appropriately.
unsigned char getAddressRoutine(char* varName)
{
if(strcmp(varName, "this") == 0)
{
isNeededArray[THIS_REGISTER_NUM] = true;
return THIS_REGISTER_NUM;
}
Variable* myVar = (Variable*) symbolTable.variableFindTotal(varName);
unsigned char returnRegister = 0;
unsigned char byteArray[BYTES_PER_LINE];

if(myVar == NULL)
{
std::cout << "in getAddressRoutine() error message since myVar was NULL\n";
std::cout << "Error: Variable " << varName << " wasn't found\n";
int* x = NULL;
std::cout << *x;
exit(1);
}
int myVarNum = myVar->uniqueIdentifierNum;


//member variables aren't stored in the list of values of registers
//if this wasn't a member var, then checking if a register already
//has the variable we are looking for in it.
	if(myVar->symbolType != MEMBER_VARIABLE)
		{
		for(int i = 0; i < NUMBER_OF_GENERAL_REGISTERS; ++i)
			{
			if(isInUseArray[i] == true && contentsOfRegisters[i] == myVarNum)
				{
				return ((unsigned char) i);
				}
			}

		if(myVar->symbolType == GLOBAL_VARIABLE)
		{
		returnRegister = registerIndexFind();
		isInUseArray[returnRegister] = true;
		isNeededArray[returnRegister] = true;
		contentsOfRegisters[returnRegister] = myVarNum;
		writeMoveGlobalAddrToReg(byteArray, returnRegister, myVarNum);
		return returnRegister;		
		}
		else if(myVar->symbolType == FUNCTION_LOCAL_VARIABLE || myVar->symbolType == PARAMETER)
		{
		returnRegister = registerIndexFind();
		isInUseArray[returnRegister] = true;
		isNeededArray[returnRegister] = true;
		contentsOfRegisters[returnRegister] = myVarNum;
		writeMoveStackAddrToReg(returnRegister, myVarNum);
		return returnRegister;
		}
		else
		{
		std::cout << "Error: Undefined variable type encountered.\n";
		exit(1);
		return 0;
		}
}

//if we're down here, then this must be a member variable access accessed without using the . operator in a member function.
//this means the address for the object must be in the THIS register.

int offset = -1;
offset = symbolTable.getIndexOfMemberVar( ((Member_Variable*)myVar), varName);
if(offset == -1)
{
std::cout << "Error: Member variable not found\n";
exit(1);
}

returnRegister = registerIndexFind();
isInUseArray[returnRegister] = true;
isNeededArray[returnRegister] = true;
contentsOfRegisters[returnRegister] = -1;
writeGetMemberVarAddrOfObjInstruction(returnRegister, THIS_REGISTER_NUM, offset);
return returnRegister;
}

unsigned char getMemberVarOfObj(unsigned char regWithObjAddr, char* objName, char* memberVarName)
{
Member_Variable* myMember = (Member_Variable*) symbolTable.getMemberVarOfObject(objName, memberVarName);

if(myMember == NULL)
{
std::cout << "Error: myMember is NULL\n";
exit(1);
}
int offset = symbolTable.findMemberVarIndex(objName, memberVarName);
unsigned char returnRegister = registerIndexFind();
isInUseArray[returnRegister] = true;
isNeededArray[returnRegister] = true;
contentsOfRegisters[returnRegister] = -1;
writeGetMemberVarAddrOfObjInstruction(returnRegister, regWithObjAddr, offset);
return returnRegister;
}

//using repeated calls to printLine, prints out all instructions in order.
//additionally, prints out the string literals at the end of the code.
void printAll()
{
unsigned char tempArray[BYTES_PER_LINE];
std::cout << std::endl;
for(int i = 0; i < lastLineSpot; i += BYTES_PER_LINE)
{
	for(int j = 0; j < BYTES_PER_LINE; ++j)
	{
	tempArray[j] = myByteCode[i + j];
	}
	std::cout << i / BYTES_PER_LINE << ".\t";
	printLine(tempArray);
}

for(int i = lastLineSpot; i < myByteCode.size(); ++i)
	std::cout << myByteCode[i];

std::cout << std::endl;
}

Op_Type readOpType(unsigned char tempLine[BYTES_PER_LINE], int offset)
{
return *((Op_Type*)&tempLine[offset]);
}

Source_Location readSourceLocation(unsigned char tempLine[BYTES_PER_LINE], int offset)
{
return *((Source_Location*)&tempLine[offset]);
}

Builtin_Function readBuiltinFunction(unsigned char tempLine[BYTES_PER_LINE], int offset)
{
return *((Builtin_Function*)&tempLine[offset]);
}

string readInt(unsigned char tempLine[BYTES_PER_LINE], int offset)
{
bool debugger = false;
int myInt = *((int*)&tempLine[offset]);

if(debugger)
std::cout << "in readInt() with int of " << myInt << std::endl;
return to_string(myInt);
}

string readDouble(unsigned char tempLine[BYTES_PER_LINE], int offset)
{
float myDoub = *((float*)&tempLine[offset]);
return to_string(myDoub);
}

string readBool(unsigned char tempLine[BYTES_PER_LINE], int offset)
{
if(tempLine[offset] != 0)
return string("TRUE");

else
return string("FALSE");

}

//returns where the next spot in the line should be read from after printing the argument
int printArg(Op_Type myType, Source_Location myLoc, unsigned char tempLine[BYTES_PER_LINE], int offset)
{
if(myLoc == REGISTER_VAL)
{
std::cout << "R" << convertCharToInt(tempLine[offset]);
return offset + 1;
}

else if(myLoc == REGISTER_MEM_ADDR)
{
std::cout << "*R" << convertCharToInt(tempLine[offset]);
return offset + 1;
}

else if(myLoc == LITERAL_L)
{

if(myType == INTEGER_T)
{
std::cout << "INT_LIT: ";
std::cout << readInt(tempLine, offset);
return offset + 4;
}

else if(myType == DOUBLE_T)
{
std::cout << "DOUBLE_LIT: ";
std::cout << readDouble(tempLine, offset);
return offset + 4;
}

else if(myType == CHAR_T)
{
std::cout << "CHAR_LIT: ";
std::cout << convertCharToInt(tempLine[offset]);
return offset + 1;
}

else if(myType == BOOLEAN_T)
{
std::cout << "BOOLEAN_LIT: ";
std::cout << readBool(tempLine, offset);
return offset + 1;
}

else if(myType == NULL_T)
{
std::cout << "NULL_LIT: ";
std::cout << "NULL";
return offset + 4;
}

else
{
std::cout << "Error: Literals must be of primitive type to be in code\n";
exit(1);
}
}

else if(myLoc == CODE_LOCATION)
{
std::cout << "CODE_LINE_NUMBER: ";
std::cout <<  stoi(readInt(tempLine, offset)) / BYTES_PER_LINE;
return offset + 4;
}

else
{
std::cout << "Error: Undefined Source_Location\n";
exit(1);
}


}

int convertCharToInt(unsigned char myChar)
{
bool debugger = false;
int myInt = 0;
myInt = myChar;
if(debugger && myChar == 0)
std::cout << "in convertCharToInt(), myChar was 0\n";
else if(debugger)
std::cout << "in convertCharToInt(), myChar was not 0\n";

return myInt;
}

//takes as input an array representing one instruction.
//prints a representation of this instruction to standard output
void printLine(unsigned char tempLine[BYTES_PER_LINE])
{
bool debugger = false;
Op_Code myOp;
Source_Location myLoc;
Op_Type myType;
unsigned char Reg;
int intLit;
float doubleLit;
char charLit;

int nextSpotInLine = 1;
myOp = *((Op_Code*)&tempLine[0]);
std::cout << getOpCodeString(myOp);

if( myOp != OUT_OF_BOUNDS_FAULT && myOp != RETURN_FROM_FUNCTION)
std::cout << ",";

std::cout << "\t";

switch(myOp)
{
case GLOBAL_DECLARE_OP:
case STACK_VARIABLE_DECLARE:
case PIPELINE_DECLARE:
std::cout << readInt(tempLine, nextSpotInLine);
break;

case PIPELINE_ADD_FUNCTION:
std::cout << "PipeNum: " << readInt(tempLine, nextSpotInLine) << ",\t";
nextSpotInLine += 4;
std::cout << "FuncNum: " << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

case ASSIGNMENT_OP:
//void writeAssignment(unsigned char regMemVal, Op_Type leftType, Source_Location rightSourceLocation, void* inputArg)
std::cout << "*R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "Op_Type: " << getOpTypeString(readOpType(tempLine, nextSpotInLine)) << ",\t";
myType = readOpType(tempLine, nextSpotInLine);

++nextSpotInLine;
myLoc = readSourceLocation(tempLine, nextSpotInLine);
++nextSpotInLine;
printArg(myType, myLoc, tempLine, nextSpotInLine);
break;

case NEW_USER_OBJ:
std::cout << "FUNCTION_LOCATION: " << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

case BUILTIN_OBJECT_CREATE_CODE:
std::cout << "OUTER TYPE: " << getOpTypeString(readOpType(tempLine, nextSpotInLine)) << "\tINNER TYPE: " << getOpTypeString(readOpType(tempLine, nextSpotInLine + 1));
break;

case JUMP_TO:
std::cout << "CODE_LINE_NUMBER: " << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

case CALL_NON_MEMBER_FUNCTION: case CALL_MEMBER_FUNCTION:
std::cout << "FUNCTION_LOCATION: " << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

//void convertIntToDoubleSet(unsigned char codeArray[BYTES_PER_LINE], char destinationReg, Source_Location secondLocation,      void* inputArg)
case CONVERT_INT_TO_DOUBLE: case CONVERT_INT_TO_CHAR:
case CONVERT_DOUBLE_TO_INT: case CONVERT_DOUBLE_TO_CHAR:
case CONVERT_CHAR_TO_INT: case CONVERT_CHAR_TO_DOUBLE:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;

if(debugger == false)
{
std::cout << "printing remaining bytes of line for convertInstruction in printLine()\n";
for(int i = 0; i < BYTES_PER_LINE; ++i)
{
std::cout << std::bitset<8>(tempLine[i]);
std::cout << ", ";
}
}
std::cout << "SOURCE_REG: R" << convertCharToInt(tempLine[nextSpotInLine]);

break;

case SET_FUNCTION_ARGUMENT:
std::cout << "DEST_REG: " << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "OP_TYPE: " << getOpTypeString(readOpType(tempLine, nextSpotInLine)) <<",\t";
myType = readOpType(tempLine, nextSpotInLine);
++nextSpotInLine;
myLoc = readSourceLocation(tempLine, nextSpotInLine);
std::cout << "prior to printArg in Set_Function_Argument of printLine()\n";
printArg(myType, myLoc, tempLine, nextSpotInLine);
break;

case SAVE_REGISTER:
case RESTORE_REGISTER:

std::cout << "R" << convertCharToInt(tempLine[nextSpotInLine]);
break;

case COPY_REGISTER_TO_REGISTER:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "SOURCE_REG: R" << convertCharToInt(tempLine[nextSpotInLine]);
break;

case COPY_INT_LITERAL_TO_REGISTER:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << readInt(tempLine, nextSpotInLine);
break;

case COPY_BOOLEAN_LITERAL_TO_REGISTER:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << readBool(tempLine, nextSpotInLine);
break;

case COPY_DOUBLE_LITERAL_TO_REGISTER:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << readDouble(tempLine, nextSpotInLine);
break;

case COPY_CHAR_LITERAL_TO_REGISTER:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << tempLine[nextSpotInLine];
break;

case COPY_STRING_LOCATION_TO_REGISTER:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "CODE_LOCATION: ";
std::cout << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

case RETURN_FROM_FUNCTION:
case OUT_OF_BOUNDS_FAULT:
break;


case EXIT_MAIN:
std::cout << "RETURN_TYPE: " << getOpTypeString(readOpType(tempLine, nextSpotInLine));
break;

case PLUS_EQUALS_OP: case MINUS_EQUALS_OP: case DIVIDE_EQUALS_OP: case MOD_EQUALS_OP:
case TIMES_EQUALS_OP: case POW_EQUALS_OP: case LOGICAL_AND_EQUALS_OP: case LOGICAL_OR_EQUALS_OP:
//void writeCompoundAssignmentInstruction(tokenType typeOfCompound, unsigned char sourceReg, Op_Type leftType, Source_Location rightLocation, void* rightArg)
std::cout << "SOURCE/DEST_REG: *R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "OP_TYPE: " << getOpTypeString(readOpType(tempLine, nextSpotInLine)) << ",\t";
myType = readOpType(tempLine, nextSpotInLine);
++nextSpotInLine;
myLoc = readSourceLocation(tempLine, nextSpotInLine);
++nextSpotInLine;
printArg(myType, myLoc, tempLine, nextSpotInLine);
break;

case MOVE_STACK_ADDR_TO_REG:
case MOVE_GLOBAL_ADDR_TO_REG:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "IDENTIFIER_NUM: " << readInt(tempLine, nextSpotInLine);
break;

case GET_MEMBER_VAR_ADDR_OF_OBJ:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "OBJECT_REG: *R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "OFFSET: " << readInt(tempLine, nextSpotInLine);
break;

case ADD_MEMBER_VARS_TO_OBJ:
std::cout << "NUMBER_OF_MEMBER_VARS: " << readInt(tempLine, nextSpotInLine);
break;

case DEREFERENCE_BOOLEAN:
case DEREFERENCE_INTEGER:
case DEREFERENCE_DOUBLE:
case DEREFERENCE_CHAR:
std::cout << "DEST_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "SOURCE_REG: *R" << convertCharToInt(tempLine[nextSpotInLine]);
break;


case PLUS_OP: case MINUS_OP: case DIVIDE_OP: case MULTIPLY_OP: case POW_OP: case LOGICAL_AND_OP:
case LOGICAL_OR_OP: case BITWISE_AND_OP: case BITWISE_OR_OP: case BITWISE_XOR_OP:
case BITSHIFT_LEFT_OP: case BITSHIFT_RIGHT_OP: case EQUALS_EQUALS_OP: case NOT_EQUALS_OP:
case GREATER_THAN_OP: case GREATER_THAN_EQUALS_OP: case LESS_THAN_OP: case LESS_THAN_EQUALS_OP:
//void arithmeticInstructionWrite(tokenType instrSymbol, Source_Location locationOutput, Op_Type resultType, char destReg, Source_Location location1, void* firstLocPointer,
//Source_Location location2, void* secondLocPointer)
std::cout << "DESTINATION: ";
myLoc = readSourceLocation(tempLine, nextSpotInLine);
++nextSpotInLine;
myType = readOpType(tempLine, nextSpotInLine);
++nextSpotInLine;
nextSpotInLine = printArg(myType, myLoc, tempLine, nextSpotInLine);
std::cout<< ",\tFirst Arg: ";
myLoc = readSourceLocation(tempLine, nextSpotInLine);
++nextSpotInLine;
nextSpotInLine = printArg(myType, myLoc, tempLine, nextSpotInLine);
std::cout <<",\tSecond Arg: ";
myLoc = readSourceLocation(tempLine, nextSpotInLine);
++nextSpotInLine;
printArg(myType, myLoc, tempLine, nextSpotInLine);
break;

case BRANCH_ON_FALSE_LITERAL:
std::cout << readBool(tempLine, nextSpotInLine) << ",\t";
++nextSpotInLine;
std::cout << "CODE_LOCATION: " << stoi(readInt(tempLine, nextSpotInLine))/ BYTES_PER_LINE;
break;

case BRANCH_ON_FALSE_REG_VAL:
std::cout << "R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "CODE_LOCATION: " << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

case BRANCH_ON_FALSE_MEM_ADDR:
std::cout << "*R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "CODE_LOCATION: " << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

case NOT_OP: case BITWISE_NOT_OP: case UNARY_MINUS_OP:
//void writeUnaryOpInstruction(tokenType unaryOp, Source_Location outputLocation, unsigned char destinationReg, Op_Type inputType, Source_Location inputLocation, void* inputArg
std::cout << "DESTINATION: ";
myLoc = readSourceLocation(tempLine, nextSpotInLine);
++nextSpotInLine;

if(myLoc == REGISTER_VAL)
std::cout << "R";
else
std::cout << "*R";

std::cout << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
myType = readOpType(tempLine, nextSpotInLine);
++nextSpotInLine;
myLoc = readSourceLocation(tempLine, nextSpotInLine);
++nextSpotInLine;
printArg(myType, myLoc, tempLine, nextSpotInLine);
break;

case JUMP_TO_MAIN:
std::cout << "CODE_LOCATION: " << stoi(readInt(tempLine, nextSpotInLine)) / BYTES_PER_LINE;
break;

case SET_PARAMETER:
std::cout << "SOURCE_REG: " << convertCharToInt(tempLine[nextSpotInLine]) << ",\t";
++nextSpotInLine;
std::cout << "IDENTIFIER_NUM: " << readInt(tempLine, nextSpotInLine);
break;

case CALL_BUILTIN_FUNCTION:
std::cout << "OBJ_REG: R" << convertCharToInt(tempLine[nextSpotInLine]) << ",\tOUTER OP_TYPE: " << getOpTypeString(readOpType(tempLine, nextSpotInLine + 1)) << ",\tINNER OP_TYPE: " <<  getOpTypeString(readOpType(tempLine, nextSpotInLine + 2)) << ",\t Function Name: " << getBuiltinFunctionString(readBuiltinFunction(tempLine, nextSpotInLine + 3));
break;

case PRINT_OP:
std::cout << "Op_Type: " << getOpTypeString(readOpType(tempLine, nextSpotInLine)) << ",\tSOURCE_REG: " << convertCharToInt(tempLine[nextSpotInLine + 1]);
break;

case GET_INT_OP:
case GET_DOUBLE_OP:
case GET_BOOLEAN_OP:
case GET_CHAR_OP:
case GET_WORD_OP:
case GET_LINE_OP:
case IS_INPUT_FAIL_OP:
std::cout << "DESTINATION_REG: " << convertCharToInt(tempLine[nextSpotInLine]);
break;


default:
std::cout << "Error: Unknown op code encountered of type: " << myOp << std::endl;
exit(1);
}

std::cout << std::endl;
return;
}


parseTree myParseTree;
SymbolTable symbolTable;
std::vector<unsigned char> myByteCode;
std::vector<unsigned char> stringLiterals;
int lineNumber;
Op_Type mainType;
int mainLocation;
bool isVerbose;
int lastLineSpot;
bool isInUseArray[256];
bool isNeededArray[256];

//is -1 if reg can't be reused - otherwise, has unique identifier number
//of the variable whose address is stored in the corresponding register.
int contentsOfRegisters[256];

};

#endif
