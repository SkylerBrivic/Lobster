#ifndef ABS_H
#define ABS_H

#include "lexicalAnalyzer.h"
#include "Type.h"
#include <string.h>
#include <vector>
#include "InstrAndMisc.h"

using namespace std;


enum abstractType {GLOBAL_LIST, PARAME,
CLASS_DEFINITION, NON_MEMBER_FUNCTION_DEFINITION, MEMBER_VARIABLE_DECLARATION, MEMBER_FUNCTION_DEFINITION, 
PIPE_LINE_ABSTRACT, BOOLEAN_BLOCK, STATEMENT, STATEMENT_LIST, EXPRESSION};

enum ExpressionType {BINARY_EXPRESSION, UNARY_EXPRESSION, VARIABLE_EXPRESSION,
INT_LIT, DOUBLE_LIT, CHAR_LIT, BOOL_LIT, STRING_LIT, MEM_FUNC_EXPR,
NON_MEM_FUNC_EXPR, BUILTIN_MEM_FUNC, MEM_VAR_EXPR, NEW_OBJECT_EXPRESSION, NULL_EXPRESSION,
BUILTIN_CREATION_EXPRESSION, PLUS_PLUS_EXPR, MINUS_MINUS_EXPR, CONTINUE_EXPRESSION, BREAK_EXPRESSION, EMPTY_EXPRESSION_NODE,
PRINT_EXPR, GET_INT_EXPR, GET_DOUBLE_EXPR, GET_BOOLEAN_EXPR, GET_CHAR_EXPR, GET_WORD_EXPR, GET_LINE_EXPR, IS_INPUT_FAIL_FUNC_EXPR

};

class abstractSymbolNode
{
public:

abstractSymbolNode()
{
symbolType = GLOBAL_LIST;
}

abstractSymbolNode(abstractType otherType)
{
symbolType = otherType;
}

abstractType symbolType;

};

class PipeLineNode : public abstractSymbolNode
{
public:

PipeLineNode(char* myNewName)
: abstractSymbolNode(PIPE_LINE_ABSTRACT)
{
strcpy(pipeName, myNewName);
}

void addFunction(char* newFunc)
{
functionList.push_back(strdup(newFunc));
}

int getNumFunctions()
{
return functionList.size();
}

char* getFunctionNumber(int myNum)
{
return functionList[myNum];
}

char pipeName[1000];
std::vector<char*> functionList;
};

class ExpressionNode : public abstractSymbolNode
{
public:

ExpressionNode(ExpressionType newType)
: abstractSymbolNode(EXPRESSION)
{
myType = newType;
}

ExpressionType myType;
};

class PlusPlusNode : public ExpressionNode
{
public:
PlusPlusNode(ExpressionNode* newExpr)
: ExpressionNode(PLUS_PLUS_EXPR)
{
myExpr = newExpr;
}

ExpressionNode* myExpr;
};

class MinusMinusNode : public ExpressionNode
{
public:
MinusMinusNode(ExpressionNode* newExpr)
: ExpressionNode(MINUS_MINUS_EXPR)
{
myExpr = newExpr;
}

ExpressionNode* myExpr;
};


class BinaryExpressionNode : public ExpressionNode
{
public:

BinaryExpressionNode(tokenType exprType)
:ExpressionNode(BINARY_EXPRESSION)
{
firstExpression = secondExpression = NULL;
if(exprType != PLUS && exprType != MINUS && exprType != EXPONENT
&& exprType != MULTIPLY && exprType != DIVIDE && exprType != MOD
&& exprType != LOGICAL_AND && exprType != LOGICAL_OR
&& exprType != BITWISE_AND && exprType != BITWISE_OR
&& exprType != BITWISE_XOR && exprType != LESS_THAN
&& exprType != LESS_THAN_EQUAL && exprType != GREATER_THAN
&& exprType != GREATER_THAN_EQUAL && exprType != EQUALS_EQUALS
&& exprType != NOT_EQUALS && exprType != BIT_SHIFT_LEFT
&& exprType != BIT_SHIFT_RIGHT)
{
std::cout << "Error: tokenType of " << exprType << " is not a binary operator." << std::endl;
exit(1);
}

myToken = exprType;
}

BinaryExpressionNode(tokenType exprType, ExpressionNode* firstExp, ExpressionNode* secondExp)
: ExpressionNode(BINARY_EXPRESSION)
{
firstExpression = firstExp;
secondExpression = secondExp;
if(exprType != PLUS && exprType != MINUS && exprType != EXPONENT
&& exprType != MULTIPLY && exprType != DIVIDE && exprType != MOD
&& exprType != LOGICAL_AND && exprType != LOGICAL_OR
&& exprType != BITWISE_AND && exprType != BITWISE_OR
&& exprType != BITWISE_XOR && exprType != LESS_THAN
&& exprType != LESS_THAN_EQUAL && exprType != GREATER_THAN
&& exprType != GREATER_THAN_EQUAL && exprType != EQUALS_EQUALS
&& exprType != NOT_EQUALS && exprType != BIT_SHIFT_LEFT
&& exprType != BIT_SHIFT_RIGHT)
{
std::cout << "Error: tokenType of " << exprType << " is not a binary operator." << std::endl;
exit(1);
}

myToken = exprType;
}

void setFirst(ExpressionNode* myExpr)
{
firstExpression = myExpr;
}

void setSecond(ExpressionNode* myExpr)
{
secondExpression = myExpr;
}

tokenType myToken; 
ExpressionNode* firstExpression;
ExpressionNode* secondExpression;
};

class UnaryOperationNode : public ExpressionNode
{
public:

UnaryOperationNode(tokenType otherToken)
: ExpressionNode(UNARY_EXPRESSION)
{
myExpression = NULL;
if(otherToken != NOT && otherToken != MINUS && otherToken != PLUS
&& otherToken != BITWISE_NOT)
{
std::cout << "Error: Token of type " << otherToken << " is not a valid unary expression operator" << std::endl;
exit(1);
}

myToken = otherToken;
}

UnaryOperationNode(tokenType otherToken, ExpressionNode* newExpr)
: ExpressionNode(UNARY_EXPRESSION)
{
myExpression = newExpr;
if(otherToken != NOT && otherToken != MINUS && otherToken != PLUS
&& otherToken != BITWISE_NOT)
{
std::cout << "Error: Token of type " << otherToken << " is not a valid unary expression operator" << std::endl;
exit(1);
}

myToken = otherToken;
}


tokenType myToken;
ExpressionNode* myExpression;
};


class BuiltinCreationExpression : public ExpressionNode
{
public:

BuiltinCreationExpression(Type* newType, ExpressionNode* newExprNum)
: ExpressionNode(BUILTIN_CREATION_EXPRESSION)
{

if(newType->isPrimitive == true)
{
std::cout << "Error: typename passed to BuiltinCreationExpression cannot be a primitive type." << std::endl;
exit(1);
}

fullType = newType;
numElements = newExprNum;
if(fullType->complexType == ARRAY)
strcpy(className, "Array");

else if(fullType->complexType == LIST)
strcpy(className, "List");

else if(fullType->complexType == DICT)
strcpy(className, "Dict");

else if(fullType->complexType == QUEUE)
strcpy(className, "Queue");

else
{
std::cout << "Error: Invalid typename of complex type " << getTokenType(fullType->complexType) << " was passed to the constructor BuiltinCreationExpression()" << std::endl;
exit(1);
}
}

char className[1000];
Type* fullType;
ExpressionNode* numElements;
};

class VariableNode : public ExpressionNode
{
public:

VariableNode(char* newVarName)
: ExpressionNode(VARIABLE_EXPRESSION)
{
strcpy(varName, newVarName);
}

char varName[1000];

};

class IntegerLiteralNode : public ExpressionNode
{
public:

IntegerLiteralNode(int newInt)
: ExpressionNode(INT_LIT)
{
myValue = newInt;
}

int myValue;
};

class DoubleLiteralNode : public ExpressionNode
{
public:
DoubleLiteralNode(float newDouble)
: ExpressionNode(DOUBLE_LIT)
{
myValue = newDouble;
}
float myValue;
};

class BooleanLiteralNode : public ExpressionNode
{
public:

BooleanLiteralNode(bool newBool)
: ExpressionNode(BOOL_LIT)
{
myValue = newBool;
}

bool myValue;
};

class CharLiteralNode : public ExpressionNode
{
public:

CharLiteralNode(char newChar)
: ExpressionNode(CHAR_LIT)
{
myValue = newChar;
}

char myValue;
};

class StringLiteralNode : public ExpressionNode
{
public:
StringLiteralNode(char* newString)
: ExpressionNode(STRING_LIT)
{
myStringPointer = strdup(newString);
}


char* myStringPointer;
};

class MemberVarExpression : public ExpressionNode
{
public:

MemberVarExpression(char* newObjectName, char* newVarName)
: ExpressionNode(MEM_VAR_EXPR)
{
strcpy(objectName, newObjectName);
strcpy(varName, newVarName);
}

char objectName[1000];
char varName[1000];

};

class FunctionExpression : public ExpressionNode
{
public:

FunctionExpression(ExpressionType myExpr, char* newFuncName)
: ExpressionNode(myExpr)
{
strcpy(funcName, newFuncName);
argumentList = std::vector<ExpressionNode*>();
}

void addArgument(ExpressionNode* newPointer)
{
argumentList.push_back(newPointer);
}

int numArgs()
{
return argumentList.size();
}

ExpressionNode* getArgNum(int argNum)
{
return argumentList[argNum];
}

char funcName[1000];
vector<ExpressionNode*> argumentList;
};

class BuiltinMemberFunctionExpression : public FunctionExpression
{
public:
BuiltinMemberFunctionExpression(char* newObjName, char* newFuncName)
:FunctionExpression(BUILTIN_MEM_FUNC, newFuncName)
{
objName = strdup(newObjName);
if(strcmp(newFuncName, "sort") == 0)
functionName = SORT;

else if(strcmp(newFuncName, "at") == 0)
functionName = AT;

else if(strcmp(newFuncName, "contains") == 0)
functionName = CONTAINS;

else if(strcmp(newFuncName, "put") == 0)
functionName = PUT;

else if(strcmp(newFuncName, "get") == 0)
functionName = GET;

else if(strcmp(newFuncName, "length") == 0)
functionName = LENGTH;

else if(strcmp(newFuncName, "update") == 0)
functionName = UPDATE;

else if(strcmp(newFuncName, "push") == 0)
functionName = PUSH;

else if(strcmp(newFuncName, "pop") == 0)
functionName = POP;

else if(strcmp(newFuncName, "front") == 0)
functionName = FRONT;

else
{
std::cout << "Error: Undefined builtin function named " << newFuncName << "encountered\n";
exit(1);
}

}



char* objName;
Builtin_Function functionName;


};

class MemberFunctionExpression : public FunctionExpression
{
public:

MemberFunctionExpression(char* newObject, char* newFuncName) : FunctionExpression(MEM_FUNC_EXPR, newFuncName)
{
strcpy(objectName, newObject);
}

char objectName[1000];
};

class NonMemberFunctionExpression : public FunctionExpression
{
public:
NonMemberFunctionExpression(char* newFuncName)
: FunctionExpression(NON_MEM_FUNC_EXPR, newFuncName)
{}

};

class PrintExpression : public FunctionExpression
{
public:
PrintExpression()
: FunctionExpression(PRINT_EXPR, (char*)"print")
{}

};


class ContinueExpressionNode : public ExpressionNode
{
public:
ContinueExpressionNode()
: ExpressionNode(CONTINUE_EXPRESSION)
{}
};

class BreakExpressionNode : public ExpressionNode
{
public:
BreakExpressionNode()
: ExpressionNode(BREAK_EXPRESSION)
{}
};

class EmptyExpressionNode : public ExpressionNode
{
public:
EmptyExpressionNode()
: ExpressionNode(EMPTY_EXPRESSION_NODE)
{}

};

class NullExpressionNode : public ExpressionNode
{
public:
NullExpressionNode()
: ExpressionNode(NULL_EXPRESSION)
{}
};



//GET_INT_EXPR, GET_DOUBLE_EXPR, GET_BOOLEAN_EXPR, GET_CHAR_EXPR, GET_WORD_EXPR, GET_LINE_EXPR, IS_INPUT_FAIL_FUNC_EXPR

class getIntExpression : public ExpressionNode
{
public: 
getIntExpression()
: ExpressionNode(GET_INT_EXPR)
{}
};

class getDoubleExpression : public ExpressionNode
{
public:
getDoubleExpression()
: ExpressionNode(GET_DOUBLE_EXPR)
{}

};

class getBooleanExpression : public ExpressionNode
{
public:
getBooleanExpression()
: ExpressionNode(GET_BOOLEAN_EXPR)
{}
};

class getCharExpression : public ExpressionNode
{
public:
getCharExpression()
: ExpressionNode(GET_CHAR_EXPR)
{}
};

class getWordExpression : public ExpressionNode
{
public:
getWordExpression()
: ExpressionNode(GET_WORD_EXPR)
{}
};

class getLineExpression : public ExpressionNode
{
public:
getLineExpression()
: ExpressionNode(GET_LINE_EXPR)
{}
};

class isInputFailFunctionExpression : public ExpressionNode
{
public:
isInputFailFunctionExpression()
: ExpressionNode(IS_INPUT_FAIL_FUNC_EXPR)
{}
};

class NewObjectExpression : public ExpressionNode
{
public:

NewObjectExpression(char* newClass, FunctionExpression* newExpr)
: ExpressionNode(NEW_OBJECT_EXPRESSION)
{
strcpy(className, newClass);
myFuncExpression = newExpr;
}

void addArgument(ExpressionNode* newArg)
{
myFuncExpression->addArgument(newArg);
}

int numArgs()
{
return myFuncExpression->numArgs();
}

ExpressionNode* getArgNum(int argNum)
{
return myFuncExpression->getArgNum(argNum);
}

char className[1000];
FunctionExpression* myFuncExpression;
};

class declarationNode : public abstractSymbolNode
{
public:

declarationNode(abstractType otherType)
: abstractSymbolNode(otherType)
{
}

declarationNode(abstractType otherType, Type* newType, char* otherName)
: abstractSymbolNode(otherType)
{
myType = newType;
strcpy(identifierName, otherName);
}

Type* myType;
char identifierName[1000];
};

class ParameterNode : public declarationNode
{
public:

ParameterNode(Type* varType, char* varName)
: declarationNode(PARAME, varType, varName)
{}

};

class FunctionDefinitionNode : public abstractSymbolNode
{
public:
FunctionDefinitionNode(abstractType mySymbolType, Type* newReturn, char* newName, abstractSymbolNode* newParent)
: abstractSymbolNode(mySymbolType)
{
myParameters = vector<ParameterNode*>();
returnType = newReturn;
strcpy(funcName, newName);
parent = newParent;
}

void addParameter(Type* identifierType, char* identifierName)
{
myParameters.push_back(new ParameterNode(identifierType, identifierName));
}

void addStatement(abstractSymbolNode* newStatement)
{
functionBody.push_back(newStatement);
}

abstractSymbolNode* getLastStatement()
{
return functionBody[functionBody.size() - 1];
}

void replaceLast(abstractSymbolNode* newLast)
{
functionBody[functionBody.size() - 1] = newLast;
}

Type* returnType;
char funcName[1000];
vector<ParameterNode*> myParameters;
vector<abstractSymbolNode*> functionBody;
abstractSymbolNode* parent;
};

class NonMemberFunctionDefinitionNode : public FunctionDefinitionNode
{
public:
NonMemberFunctionDefinitionNode(Type* returnType, char* identifierName, abstractSymbolNode* newParent)
: FunctionDefinitionNode(NON_MEMBER_FUNCTION_DEFINITION, returnType, identifierName, newParent)
{
}
};

class MemberFunctionDefinitionNode : public FunctionDefinitionNode
{
public:
MemberFunctionDefinitionNode(Type* returnType, char* identifierName, abstractSymbolNode* newParent)
: FunctionDefinitionNode(MEMBER_FUNCTION_DEFINITION, returnType, identifierName, newParent)
{
}
};

class MemberVariableDefinitionNode : public declarationNode
{
public:
MemberVariableDefinitionNode(Type* idenType, char* idenName)
: declarationNode(MEMBER_VARIABLE_DECLARATION, idenType, idenName)
{
}

};


class ClassDefinitionNode : public abstractSymbolNode
{
public:
ClassDefinitionNode(char* newName, abstractSymbolNode* newParent)
: abstractSymbolNode(CLASS_DEFINITION)
{
parent = newParent;
strcpy(className, newName);
memberVariableList = vector<MemberVariableDefinitionNode*>();
memberFunctionList = vector<MemberFunctionDefinitionNode*>();
}

void addMemberVar(Type* identifierType, char* identifierName)
{
memberVariableList.push_back(new MemberVariableDefinitionNode(identifierType, identifierName));
}

void addMemberFunc(Type* returnType, char* funcName)
{
memberFunctionList.push_back(new MemberFunctionDefinitionNode(returnType, funcName, this));
}

char className[1000];
abstractSymbolNode* parent;
vector<MemberVariableDefinitionNode*> memberVariableList;
vector<MemberFunctionDefinitionNode*> memberFunctionList;
};

class GlobalListNode : public abstractSymbolNode
{

public:

GlobalListNode()
: abstractSymbolNode(GLOBAL_LIST)
{
globalStatementsList = vector<abstractSymbolNode*>();
}

void addToGlobal(abstractSymbolNode* newAddition)
{
globalStatementsList.push_back(newAddition);
}

abstractSymbolNode* getLastStatement()
{
return globalStatementsList[globalStatementsList.size() - 1];
}

void replaceLast(abstractSymbolNode* newLast)
{
globalStatementsList[globalStatementsList.size() - 1] = newLast;
}

vector<abstractSymbolNode*> globalStatementsList;
};

enum StatementType {ASSIGNMENT_STATEMENT, COMPOUND_ASSIGNMENT_STATEMENT, DECLARATION_STATEMENT, DECLARE_ASSIGN_STATEMENT, EXPRESSION_STATEMENT, FUNCTION_CALL_STATEMENT, RETURN_STATEMENT};

class StatementNode : public abstractSymbolNode
{
public:


StatementNode(StatementType newType)
: abstractSymbolNode(STATEMENT)
{
myType = newType;
}

StatementType myType;
};

class DeclarationStatement : public StatementNode
{
public:

DeclarationStatement(Type* newType, char* newName)
: StatementNode(DECLARATION_STATEMENT)
{
myType = newType;
strcpy(identifierName, newName);
}

Type* myType;
char identifierName[1000];
};

class AssignmentStatement : public StatementNode
{
public:
AssignmentStatement()
: StatementNode(ASSIGNMENT_STATEMENT)
{
leftValue = rightValue = NULL;
}


AssignmentStatement(ExpressionNode* newLeft, ExpressionNode* newRight)
: StatementNode(ASSIGNMENT_STATEMENT)
{
leftValue = newLeft;
rightValue = newRight;
}
ExpressionNode* leftValue;
ExpressionNode* rightValue;
};

class CompoundAssignmentStatement : public StatementNode
{
public:
CompoundAssignmentStatement(tokenType newOperator, ExpressionNode* newLeftHalf, ExpressionNode* newRightHalf)
: StatementNode(COMPOUND_ASSIGNMENT_STATEMENT)
{
leftHalf = newLeftHalf;
rightHalf = newRightHalf;
if(newOperator != PLUS_EQUALS && newOperator != MINUS_EQUALS && newOperator != TIMES_EQUALS && newOperator != DIVIDE_EQUALS && newOperator != MOD_EQUALS)
{
std::cout << "Exception in CompoundAssignmentStatement() constructor. Unknown compound operator of type " << getTokenType(newOperator) << " encountered." << std::endl;
exit(1);
}
compoundOperator = newOperator;
}

ExpressionNode* leftHalf;
ExpressionNode* rightHalf;
tokenType compoundOperator;
};

class DeclareAssignStatement : public StatementNode
{
public:
DeclareAssignStatement(Type* newType, char* newIdenName, ExpressionNode* newExpr)
: StatementNode(DECLARE_ASSIGN_STATEMENT)
{
declarePortion = new declarationNode(STATEMENT, newType, newIdenName);
assignedPortion = newExpr;
}

void setDeclaration(declarationNode* myPointer)
{
declarePortion = myPointer;
}

declarationNode* getDeclaration()
{
return declarePortion;
}

void setAssignment(ExpressionNode* newAssigned)
{
assignedPortion = newAssigned;
}

ExpressionNode* getAssignment()
{
return assignedPortion;
}

declarationNode* declarePortion;
ExpressionNode* assignedPortion;
};

class ExpressionStatement : public StatementNode
{
public:

ExpressionStatement(ExpressionNode* newExpr)
: StatementNode(EXPRESSION_STATEMENT)
{
myExpression = newExpr;
}

void setExpression(ExpressionNode* other)
{
myExpression = other;
}
ExpressionNode* getExpression()
{
return myExpression;
}
ExpressionNode* myExpression;
};

class FunctionCallStatement : public StatementNode
{
public:

FunctionCallStatement()
: StatementNode(FUNCTION_CALL_STATEMENT)
{
myStatement = NULL;
}
ExpressionNode* getStatement()
{
return myStatement;
}

void setStatement(ExpressionNode* newStatement)
{
myStatement = newStatement;
}

ExpressionNode* myStatement;

};

class ReturnStatement : public StatementNode
{
public:
ReturnStatement(ExpressionNode* newExpr)
: StatementNode(RETURN_STATEMENT)
{
myStatement = newExpr;
}

void setStatement(ExpressionNode* newStatement)
{
myStatement = newStatement;
}

ExpressionNode* getStatement()
{
return myStatement;
}

ExpressionNode* myStatement;

};


enum boolBlockType {FOR_BLOCK, WHILE_BLOCK, DO_WHILE_BLOCK, IF_TYPE_BLOCK};
class BooleanBlockNode : public abstractSymbolNode
{
public:
bool abstractDebug;
BooleanBlockNode(boolBlockType otherType, abstractSymbolNode* otherPointer)
: abstractSymbolNode(BOOLEAN_BLOCK)
{
abstractDebug = false;
myType = otherType;
onlyOneStatement = true;
booleanCondition = NULL;
parentBlock = otherPointer;
blockStatementsList = std::vector<abstractSymbolNode*>();
}

	void setOnlyOneStatement(bool otherBool)
	{
	onlyOneStatement = otherBool;
	}

	bool isOneStatement()
	{
	return onlyOneStatement;
	}

	void addBoolCondition(ExpressionNode* otherBool)
	{
	if(abstractDebug)
	std::cout << "Adding booleanCondition in class BoolBlockNode\n";
	booleanCondition = otherBool;
	}


	ExpressionNode* getBoolCondition()
	{
	return booleanCondition;
	}

	void addStatement(abstractSymbolNode* newStatement)
	{
	if(onlyOneStatement && blockStatementsList.size() >= 1)
		{
		std::cout << "Error: extra statement was added to block which could only accomodate one statement. Terminating execution now..." << std::endl;
		exit(1);
		}
	if(abstractDebug)
	std::cout << "Adding statement in BooleanBlockNode\n";
	blockStatementsList.push_back(newStatement);
	}

	int numStatements()
	{
	return blockStatementsList.size();
	}

	abstractSymbolNode* getStatementNum(int num)
	{
	return blockStatementsList[num];
	}

	abstractSymbolNode* getLastStatement()
	{
	if(blockStatementsList.size() == 0)
		{
		if(abstractDebug)
		std::cout << "Returning boolean condition in BooleanBlockNode's getLastStatement()\n";
		return booleanCondition;
		}

	if(abstractDebug)
	std::cout << "Returning last statement in block body in BooleanBlockNode's getLastStatement()\n";
	return blockStatementsList[blockStatementsList.size() - 1];
	}

	void replaceLast(abstractSymbolNode* newLast)
	{
	if(blockStatementsList.size() == 0)
		{
		if(abstractDebug)
		std::cout << "Replacing last statement in boolean condition in BooleanBlockNode's replaceLast()\n";
		booleanCondition = ((ExpressionNode*)newLast);
		}
	else
	{
		if(abstractDebug)
		std::cout << "Replacing last statement in block body in BooleanBlockNode's replaceLast()\n";
		blockStatementsList[blockStatementsList.size() - 1] = newLast;
	}
	}

abstractSymbolNode* parentBlock;
boolBlockType myType;
bool onlyOneStatement;
ExpressionNode* booleanCondition;
std::vector<abstractSymbolNode*> blockStatementsList;
};




class WhileBlockNode : public BooleanBlockNode
{
public:
WhileBlockNode(abstractSymbolNode* other)
: BooleanBlockNode(WHILE_BLOCK, other)
{}
};

class DoWhileBlockNode : public BooleanBlockNode
{
public:
DoWhileBlockNode(abstractSymbolNode* other)
: BooleanBlockNode(DO_WHILE_BLOCK, other)
{}
};

class ForBlockNode : public BooleanBlockNode
{
public:
bool abstractDebug;
ForBlockNode(abstractSymbolNode* other)
: BooleanBlockNode(FOR_BLOCK, other)
{
abstractDebug = false;
initialStatement = statementAction = NULL;
}

void setInitialStatement(StatementNode* other)
{
if(abstractDebug)
std::cout << "Set initial statement in ForBlockNode\n";
initialStatement = other;
}

void setActionStatement(StatementNode* other)
{
if(abstractDebug)
std::cout << "Set action statement in ForBlockNode\n";
statementAction = other;
}

StatementNode* getInitialStatement()
{
if(abstractDebug)
std::cout << "Got initial statement in ForBlockNode\n";
return initialStatement;
}

StatementNode* getActionStatement()
{
if(abstractDebug)
std::cout << "Got action statement in ForBlockNode\n";
return statementAction;
}

abstractSymbolNode* getLastFor()
{
if(initialStatement == NULL)
{
std::cout << "Error: initialStatement in for loop was NULL. Terminating execution now." << std::endl;
exit(1);
}
else if(booleanCondition == NULL)
{
if(abstractDebug)
std::cout << "Got initial statement in getLastFor() in ForBlockNode\n";
return (abstractSymbolNode*) initialStatement;
}
else if(statementAction == NULL)
{
if(abstractDebug)
std::cout << "Got boolean condition in getLastFor() in ForBlockNode\n";
return (abstractSymbolNode*) booleanCondition;
}

else if(blockStatementsList.size() == 0)
{
if(abstractDebug)
std::cout << "Got statement action in getLastFor() in ForBlockNode\n";
return (abstractSymbolNode*) statementAction;
}
else
{
if(abstractDebug)
std::cout << "Got last statement in block body in ForBlockNode\n";
return blockStatementsList[blockStatementsList.size() - 1];
}
}

void replaceLastFor(abstractSymbolNode* newLast)
{
if(initialStatement == NULL)
{
std::cout << "Error: initialStatement in for loop was NULL. Terminating execution now." << std::endl;
exit(1);
}
else if(booleanCondition == NULL)
{
if(abstractDebug)
std::cout << "replaced initial statement in ForBlockNode in replaceLast()\n";
initialStatement = ((StatementNode*)newLast);
}

else if(statementAction == NULL)
{
if(abstractDebug)
std::cout << "replaced boolean condition in ForBlockNode in replaceLast()\n";
booleanCondition = ((ExpressionNode*)newLast);
}

else if(blockStatementsList.size() == 0)
{
if(abstractDebug)
std::cout << "replaced statement action in ForBlockNode in replaceLast()\n";
statementAction = ((StatementNode*)newLast);
}
else
{
if(abstractDebug)
std::cout << "replaced last statement in block in ForBlockNode in replaceLast()\n";
blockStatementsList[blockStatementsList.size() - 1] = newLast;
}
}

StatementNode* initialStatement;
StatementNode* statementAction;

};
enum ifType {IF_TYPE, ELIF_TYPE, ELSE_TYPE};
class IfTypeBlock : public BooleanBlockNode
{
public:
IfTypeBlock(ifType otherType, abstractSymbolNode* other)
: BooleanBlockNode(IF_TYPE_BLOCK, other)
{
myIfType = otherType;
}

ifType myIfType;
};

class IfNode : public IfTypeBlock
{
public:
IfNode(abstractSymbolNode* other)
: IfTypeBlock(IF_TYPE, other)
{
}


};

class ElseBlock : public IfTypeBlock
{
public:
ElseBlock(abstractSymbolNode* other)
: IfTypeBlock(ELSE_TYPE, other)
{
this->addBoolCondition((ExpressionNode*) new BooleanLiteralNode(true));
}
};

class ElifBlock : public IfTypeBlock
{
public:
ElifBlock(abstractSymbolNode* other)
: IfTypeBlock(ELIF_TYPE, other)
{
}

};
#endif
