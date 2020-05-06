#ifndef PAR_TR_H
#define PAR_TR_H

#include "lexicalAnalyzer.h"
#include <string.h>
#include <cmath>
#include <vector>
#include "SymbolTable.h"
#include "abstractSymbolNode.h"

using namespace std;

class parseTree
{
public:
bool parseDebug;

Type* stripType(Type* myArg)
{
return myArg->nextType;
}

tokenType getCompounded(tokenType inputOperator)
{
switch(inputOperator)
{
case PLUS:
return PLUS_EQUALS;

case MINUS:
return MINUS_EQUALS;

case DIVIDE:
return DIVIDE_EQUALS;

case MOD:
return MOD_EQUALS;

case MULTIPLY:
return TIMES_EQUALS;

case EXPONENT:
return POW_EQUALS;

case BITWISE_AND:
return BITWISE_AND_EQUALS;

case BITWISE_OR:
return BITWISE_OR_EQUALS;

default:
std::cout << "Error: Operator of type " << getTokenType(inputOperator) << " cannot be converted into a compound operator\n";
exit(1);
return OTHER;
}

}
bool isLiteral(Type* myType)
{

if(myType == NULL)
{
std::cout << "Error: Null type in function isLiteral() in parseTree.h" << std::endl;
exit(1);
}
if(myType->isPrimitive == false)
return false;

if(strcmp(myType->primitiveName, "BOOL_LIT") != 0 && strcmp(myType->primitiveName, "CHAR_LIT") != 0
&& strcmp(myType->primitiveName, "DOUBLE_LIT") != 0 && strcmp(myType->primitiveName, "INT_LIT") != 0 && strcmp(myType->primitiveName, "NULL") != 0)
return false;

return true;
}

bool isLastBasic(SymbolTable& symbolTable)
{
abstractSymbolNode* tempNode;
ExpressionNode* myExpression;
Type* myType = NULL;
if(currentNode->symbolType == GLOBAL_LIST)
tempNode = ((abstractSymbolNode*)((GlobalListNode*)currentNode)->getLastStatement());
else if(currentNode->symbolType == MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
tempNode = ((abstractSymbolNode*)((FunctionDefinitionNode*)currentNode)->getLastStatement());
else
tempNode = ((abstractSymbolNode*)   ((BooleanBlockNode*)currentNode)->getLastStatement());

if(tempNode->symbolType == EXPRESSION)
{
getType( ((ExpressionNode*) tempNode), &myType, symbolTable);
return isLiteral(myType);
}
else if(tempNode->symbolType == STATEMENT)
	{
	if( ((StatementNode*)tempNode)->myType == DECLARE_ASSIGN_STATEMENT)
		{
		getType( ((DeclareAssignStatement*)tempNode)->assignedPortion, &myType, symbolTable);
		return isLiteral(myType);
		}
	}

std::cout << "Error: Type was not a basic type." << std::endl;
return false;
}



parseTree()
{
parseDebug = false;
root = currentNode = new GlobalListNode();
}


void addDeclaration(Type* newType, char* newIdentifier)
{
if(currentNode->symbolType == GLOBAL_LIST)
{
((GlobalListNode*) currentNode)->addToGlobal(new DeclarationStatement(newType, newIdentifier));
}
else if(currentNode->symbolType == CLASS_DEFINITION)
{
((ClassDefinitionNode*)currentNode)->addMemberVar(newType, newIdentifier);
}
else if(currentNode->symbolType == MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
{
((FunctionDefinitionNode*)currentNode)->addStatement((abstractSymbolNode*)new DeclarationStatement(newType, newIdentifier));
}
else if(currentNode->symbolType == BOOLEAN_BLOCK)
{
((BooleanBlockNode*)currentNode)->addStatement((abstractSymbolNode*) new DeclarationStatement(newType, newIdentifier));
}
else
{
std::cout << "Error: Unexpected declaration. Declaration only allowed in global list, class definition, function definition, or boolean block " << std::endl;
exit(1);
}
}

void addDeclareAssign(Type* identifierType, char* identifierName, ExpressionNode* expression)
{
if(currentNode->symbolType == GLOBAL_LIST)
{
((GlobalListNode*)currentNode)->addToGlobal((abstractSymbolNode*) new DeclareAssignStatement(identifierType, identifierName, expression));
}
else if(currentNode->symbolType == MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
{
((FunctionDefinitionNode*)currentNode)->addStatement((abstractSymbolNode*) new DeclareAssignStatement(identifierType, identifierName, expression));
}

else if(currentNode->symbolType == BOOLEAN_BLOCK)
{
((BooleanBlockNode*)currentNode)->addStatement((abstractSymbolNode*) new DeclareAssignStatement(identifierType, identifierName, expression));
}
else
{
std::cout << "Error: Unexpected Declare-assign statement. Declare assigns only allowed in Global level, function definition, or boolean block" << std::endl;
exit(1);
}
}

void addAssignment(ExpressionNode* leftExpression, ExpressionNode* rightExpression)
{
if(currentNode->symbolType == MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
{
((FunctionDefinitionNode*)currentNode)->addStatement((abstractSymbolNode*) new AssignmentStatement(leftExpression, rightExpression));
}
else if(currentNode->symbolType == BOOLEAN_BLOCK)
{
((BooleanBlockNode*)currentNode)->addStatement((abstractSymbolNode*) new AssignmentStatement(leftExpression, rightExpression));
}
else
{
std::cout << "Error: Assignment statement only allowed in function definition or boolean block " << std::endl;
exit(1);
}
}

void addCompoundAssignment(tokenType myOperator, ExpressionNode* leftHalf, ExpressionNode* rightHalf)
{
if(currentNode->symbolType == MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
((FunctionDefinitionNode*) currentNode)->addStatement((abstractSymbolNode*) new CompoundAssignmentStatement(myOperator, leftHalf, rightHalf));

else if(currentNode->symbolType == BOOLEAN_BLOCK)
((BooleanBlockNode*)currentNode)->addStatement((abstractSymbolNode*) new CompoundAssignmentStatement(myOperator, leftHalf, rightHalf));

else
{
std::cout << "Error: Can only perform compound assignment in function body or in boolean block " << std::endl;
exit(1);
}
}


//Note: Only use this to add statements to the bodies of functions and the bodies of boolean blocks
//specifically, this can be used for return statements and function call statements
//don't use this to add boolean blocks, since it won't move down the parse tree.
void addStatement(abstractSymbolNode* myStatement)
{
if(currentNode->symbolType == MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
((FunctionDefinitionNode*) currentNode)->addStatement(myStatement);

else if(currentNode->symbolType == BOOLEAN_BLOCK)
{
if( ((BooleanBlockNode*)currentNode)->myType != FOR_BLOCK)
((BooleanBlockNode*) currentNode)->addStatement(myStatement);
else
((ForBlockNode*)currentNode)->addStatement(myStatement);
}
else
{
std::cout << "Error: Cannot use addStatement() outside of function definition or boolean block." << std::endl;
exit(1);
}
}

void addPipeLine(PipeLineNode* newPipe)
{
((GlobalListNode*)currentNode)->addToGlobal(newPipe);
}

void addClassDefinition(char* className)
{
if(parseDebug)
std::cout << "Adding class to parse tree." << std::endl;
((GlobalListNode*) currentNode)->addToGlobal(new ClassDefinitionNode(className, currentNode));
currentNode = (abstractSymbolNode*) ((GlobalListNode*) currentNode)->globalStatementsList[((GlobalListNode*) currentNode)->globalStatementsList.size() - 1];
if(parseDebug)
std::cout << "Added class to parse tree." << std::endl;
}

void addNonMemberFunction(Type* returnType, char* identifierName)
{
//std::cout << "Type of Non-member function: " << *returnType << "\tName of function: " << identifierName << std::endl;

((GlobalListNode*) currentNode)->addToGlobal(new NonMemberFunctionDefinitionNode(returnType, identifierName, currentNode));

currentNode = (abstractSymbolNode*) ((GlobalListNode*) currentNode)->globalStatementsList[((GlobalListNode*) currentNode)->globalStatementsList.size() - 1];
}

void addMemberVariable(Type* varType, char* identifierName)
{
((ClassDefinitionNode*) currentNode)->addMemberVar(varType, identifierName);
}

void addMemberFunction(Type* returnType, char* funcName)
{
((ClassDefinitionNode*) currentNode)->addMemberFunc(returnType, funcName);
currentNode = ((ClassDefinitionNode*) currentNode)->memberFunctionList[((ClassDefinitionNode*) currentNode)->memberFunctionList.size() - 1];
}

void addParameter(Type* paramType, char* paramName)
{
((FunctionDefinitionNode*) currentNode)->addParameter(paramType, paramName);
}


void addBooleanBlock(abstractSymbolNode* newBlock)
{
if(currentNode->symbolType == MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION)
{
((FunctionDefinitionNode*)currentNode)->addStatement(newBlock);
currentNode = ((FunctionDefinitionNode*)currentNode)->functionBody[((FunctionDefinitionNode*)currentNode)->functionBody.size() - 1];
}
else if(currentNode->symbolType == BOOLEAN_BLOCK)
{
((BooleanBlockNode*)currentNode)->addStatement(newBlock);
currentNode = ((BooleanBlockNode*)currentNode)->blockStatementsList[((BooleanBlockNode*)currentNode)->blockStatementsList.size() - 1];
}
else
{
std::cout << "Error: Boolean block cannot appear outside of function definition or another boolean block" << std::endl;
exit(1);
}


}

//WARNING: Only use this function to exit from boolean blocks (ex. ifs and whiles)
void endBlock()
{
currentNode = ((BooleanBlockNode*)currentNode)->parentBlock;
}

void endNonMemberFunctionDefinition()
{
currentNode = ((FunctionDefinitionNode*) currentNode)->parent;
}

void endMemberFunctionDefinition()
{
currentNode = ((FunctionDefinitionNode*) currentNode)->parent;
}

void endClassDefinition()
{
currentNode = ((ClassDefinitionNode*)currentNode)->parent;
}

void moveToRoot()
{
currentNode = root;
}

abstractSymbolNode* getCurrent()
{
return currentNode;
}

abstractSymbolNode* getRoot()
{
return root;
}
void printTreeHelper(abstractSymbolNode* tempPointer, int depth)
{
for(int i = 0; i < depth; ++i)
std::cout << "\t";


switch(tempPointer->symbolType)
{
case GLOBAL_LIST:
std::cout << "Global List:\n";
break;

case STATEMENT_LIST:
std::cout << "Statement List:\n";

case PARAME:
std::cout << "Parameter:\tType: " << (*((ParameterNode*) tempPointer)->myType) << "\tvarName: " << ((ParameterNode*) tempPointer)->identifierName << std::endl;
break;
/*
case STATEMENT:
std::cout << "Declaration\tType: " << ((DeclarationStatement*)tempPointer)->identifierType << "\tvarName: " << ((DeclarationStatement)tempPointer)->identifierName << std::endl;
break;
*/
case CLASS_DEFINITION:
std::cout << "Class:\t" << ((ClassDefinitionNode*)tempPointer)->className << std::endl;
for(int i = 0; i < ((ClassDefinitionNode*)tempPointer)->memberVariableList.size(); ++i)
printTreeHelper(((ClassDefinitionNode*)tempPointer)->memberVariableList[i], depth + 1);

for(int i = 0; i < ((ClassDefinitionNode*)tempPointer)->memberFunctionList.size(); ++i)
printTreeHelper(((ClassDefinitionNode*)tempPointer)->memberFunctionList[i], depth + 1);
break;

case NON_MEMBER_FUNCTION_DEFINITION:
std::cout << "Non-Member Function:\tReturnType: " << (*((NonMemberFunctionDefinitionNode*)tempPointer)->returnType) << "\tFuncName: " << ((NonMemberFunctionDefinitionNode*)tempPointer)->funcName;

if(((NonMemberFunctionDefinitionNode*)tempPointer)->myParameters.size() == 0)
std::cout << " No Parameters:" << std::endl;

else
{
std::cout << " Parameter List: " << std::endl;
for(int i = 0; i < ((NonMemberFunctionDefinitionNode*)tempPointer)->myParameters.size(); ++i)
printTreeHelper(((NonMemberFunctionDefinitionNode*)tempPointer)->myParameters[i], depth + 1);
}
break;

case MEMBER_VARIABLE_DECLARATION:
std::cout << "Member Variable\tType: " << (*((MemberVariableDefinitionNode*)tempPointer)->myType) << "\tvarName: " << ((MemberVariableDefinitionNode*)tempPointer)->identifierName << std::endl;
break;

case MEMBER_FUNCTION_DEFINITION:
std::cout << "Member Function\tReturn Type: " << (*((MemberFunctionDefinitionNode*)tempPointer)->returnType) << "\tFuncName: " << ((MemberFunctionDefinitionNode*)tempPointer)->funcName;

if(((MemberFunctionDefinitionNode*)tempPointer)->myParameters.size() == 0)
std::cout << " No Parameters:" << std::endl;

else
{
std::cout << " Parameter List: " << std::endl;
for(int i = 0; i < ((MemberFunctionDefinitionNode*)tempPointer)->myParameters.size(); ++i)
printTreeHelper(((MemberFunctionDefinitionNode*)tempPointer)->myParameters[i], depth + 1);
}
break;

case PIPE_LINE_ABSTRACT:
std::cout << "Pipeline named " << ((PipeLineNode*)tempPointer)->pipeName << " (";
for(int i = 0; i < ((PipeLineNode*)tempPointer)->getNumFunctions(); ++i)
{
std::cout << ((PipeLineNode*)tempPointer)->functionList[i];
if(i < ((PipeLineNode*)tempPointer)->getNumFunctions() - 1)
std::cout << "->";
}
std::cout << ")" << std::endl;
break;
 
case EXPRESSION:
	switch(((ExpressionNode*)tempPointer)->myType)
		{

		case GET_INT_EXPR:
		std::cout << "getInt()\n";
		break;

		case GET_DOUBLE_EXPR:
		std::cout << "getDouble()\n";
		break;

		case GET_BOOLEAN_EXPR:
		std::cout << "getBoolean()\n";
		break;

		case GET_CHAR_EXPR:
		std::cout << "getChar()\n";
		break;

		case GET_WORD_EXPR:
		std::cout << "getWord()\n";
		break;

		case GET_LINE_EXPR:
		std::cout << "getLine()\n";
		break;

		case IS_INPUT_FAIL_FUNC_EXPR:
		std::cout << "isInputFail()\n";
		break;		

		case BINARY_EXPRESSION:
		std::cout << "Binary Expression (" << getTokenType(((BinaryExpressionNode*)tempPointer)->myToken) << ")" << std::endl;
		printTreeHelper(((BinaryExpressionNode*)tempPointer)->firstExpression, depth + 1);
		printTreeHelper(((BinaryExpressionNode*)tempPointer)->secondExpression, depth + 1);
		break;

		case UNARY_EXPRESSION:
		std::cout << "Unary Expression (" << getTokenType(((UnaryOperationNode*)tempPointer)->myToken) << ")" << std::endl;
		printTreeHelper(((UnaryOperationNode*)tempPointer)->myExpression, depth + 1);
		break;

		case VARIABLE_EXPRESSION:
		std::cout << "Variable Expression: " << ((VariableNode*)tempPointer)->varName << std::endl;
		break;

		case INT_LIT:
		std::cout << "Integer Literal: " << ((IntegerLiteralNode*)tempPointer)->myValue << std::endl;
		break;

		case DOUBLE_LIT:
		std::cout << "Double Literal: " << ((DoubleLiteralNode*)tempPointer)->myValue << std::endl;
		break;

		case CHAR_LIT:
		std::cout << "Character Literal: " << ((CharLiteralNode*)tempPointer)->myValue << std::endl;
		break;

		case BOOL_LIT:
		std::cout << "Boolean Literal: ";
		if(((BooleanLiteralNode*)tempPointer)->myValue)
			std::cout << "TRUE";
		else
			std::cout << "FALSE";
		std::cout << std::endl;
		break;

		case STRING_LIT:
		std::cout << "String Literal: \"" << ((StringLiteralNode*)tempPointer)->myStringPointer << "\"" << std::endl;
		break;

		case MEM_FUNC_EXPR:
		std::cout << "Member Function Call: " << ((MemberFunctionExpression*)tempPointer)->objectName << "." <<
		((FunctionExpression*)tempPointer)->funcName << std::endl;
		std::cout << "Arguments: " << std::endl;

		for(int i = 0; i < ((FunctionExpression*)tempPointer)->argumentList.size(); ++i)
		{
		std::cout << i + 1 << ".\t"; printTreeHelper(((FunctionExpression*)tempPointer)->argumentList[i], depth + 1); std::cout << std::endl;
		}
		break;

		case NON_MEM_FUNC_EXPR:
		std::cout << "Non-Member Function Call: " << ((FunctionExpression*)tempPointer)->funcName << std::endl;
		std::cout << "Arguments: " << std::endl;
	
		for(int i = 0; i < ((FunctionExpression*)tempPointer)->argumentList.size(); ++i)
		{
		std::cout << i + 1 << ".\t"; printTreeHelper(((FunctionExpression*)tempPointer)->argumentList[i], depth + 1); std::cout << std::endl;
		}
		break;


		case PRINT_EXPR:
		std::cout << "print():\n";
		std::cout << "Arguments: " << std::endl;
		for(int i = 0; i < ((FunctionExpression*)tempPointer)->argumentList.size(); ++i)
			{
			std::cout << i + 1 << ".\t"; 
			printTreeHelper( ((FunctionExpression*)tempPointer)->argumentList[i], depth + 2);
			std::cout << std::endl;
			}
		break;

		case MEM_VAR_EXPR:
		std::cout << "Member Variable Expression: " << ((MemberVarExpression*)tempPointer)->objectName << "."
		<< ((MemberVarExpression*)tempPointer)->varName << std::endl;
		break;

		case NEW_OBJECT_EXPRESSION:
		std::cout << "New " << ((NewObjectExpression*)tempPointer)->className << std::endl;

		for(int i = 0; i < ((NewObjectExpression*)tempPointer)->numArgs(); ++i)
		{
		std::cout << i + 1 << "."; printTreeHelper(((NewObjectExpression*)tempPointer)->getArgNum(i), depth + 1); std::cout << std::endl;
		}
		break;

		case BUILTIN_CREATION_EXPRESSION:
		std::cout << "Builtin Creation Expression of type " << (* ((BuiltinCreationExpression*)tempPointer)->fullType);
		if(((BuiltinCreationExpression*)tempPointer)->numElements != NULL)
		{
		std::cout << "expression arguments: " << std::endl;
		printTreeHelper( ((BuiltinCreationExpression*)tempPointer)->numElements, depth + 1);
		std::cout << std::endl;
		}	
		else
		{
		std::cout << "No function arguments" << std::endl;
		}

		break;
		
		case PLUS_PLUS_EXPR:
		std::cout << "++" << std::endl;
		printTreeHelper(((PlusPlusNode*)tempPointer)->myExpr, depth + 1);
		break;

		case MINUS_MINUS_EXPR:
		std::cout << "--" << std::endl;
		printTreeHelper(((MinusMinusNode*)tempPointer)->myExpr, depth + 1);
		break;

		case EMPTY_EXPRESSION_NODE:
		std::cout << ";" << std::endl;
		break;

		case CONTINUE_EXPRESSION:
		std::cout << "Continue" << std::endl;
		break;

		case BREAK_EXPRESSION:
		std::cout << "Break" << std::endl;
		break;

		case NULL_EXPRESSION:	
		std::cout << "NULL" << std::endl;
		break;

		default:
		std::cout << "Unknown or not processed expression type encountered " << std::endl;
		break;
}
break;

//outputting statement information:
case STATEMENT:
	switch( ((StatementNode*)tempPointer)->myType)
	{
	case ASSIGNMENT_STATEMENT:
	std::cout << "Assignment Statement:" << std::endl;
	printTreeHelper( ((AssignmentStatement*)tempPointer)->leftValue, depth + 1);
	printTreeHelper( ((AssignmentStatement*)tempPointer)->rightValue, depth + 1);
	break;

	case COMPOUND_ASSIGNMENT_STATEMENT:
	std::cout << "Compound Assignment Statement of type " << getTokenType( ((CompoundAssignmentStatement*)tempPointer)->compoundOperator) << std::endl;
	printTreeHelper( ((CompoundAssignmentStatement*)tempPointer)->leftHalf, depth + 1);
	printTreeHelper( ((CompoundAssignmentStatement*)tempPointer)->rightHalf, depth + 1);
	break;

	case DECLARATION_STATEMENT:
	std::cout << "Declaration Statement\tType:" << (*((DeclarationStatement*)tempPointer)->myType)
	<< "\tVariable Name: " << ((DeclarationStatement*)tempPointer)->identifierName << std::endl;
	break;

	case DECLARE_ASSIGN_STATEMENT:
	std::cout << "Declare-assign statement\tType: " << (*((DeclareAssignStatement*)tempPointer)->declarePortion->myType) << "\tName: " << ((DeclareAssignStatement*)tempPointer)->declarePortion->identifierName << "\t\tExpression:";
	printTreeHelper( ((DeclareAssignStatement*)tempPointer)->assignedPortion, depth + 1);
	break;

	case EXPRESSION_STATEMENT:
	std::cout << "Expression Statement:" << std::endl;
	printTreeHelper( ((ExpressionStatement*)tempPointer)->myExpression, depth + 1);
	break;

	case FUNCTION_CALL_STATEMENT:
	std::cout << "Function Call Statement:" << std::endl;
	printTreeHelper( ((FunctionCallStatement*)tempPointer)->myStatement, depth + 1);
	break;

	case RETURN_STATEMENT:
	std::cout << "Return:" << std::endl;
	printTreeHelper( ((ReturnStatement*)tempPointer)->myStatement, depth + 1);
	break;

	default:
	std::cout << "Unknown type of statement encountered." << std::endl;
	break;
	}
break;


case BOOLEAN_BLOCK:
switch( ((BooleanBlockNode*)tempPointer)->myType)
	{
	case FOR_BLOCK:
	std::cout << "For block:\tInitial Statement:\t";
	printTreeHelper( ((ForBlockNode*)tempPointer)->initialStatement, 0);
	std::cout << "Boolean Condition:\t";
	printTreeHelper( ((ForBlockNode*)tempPointer)->booleanCondition, 0);
	std::cout << "Action Statement:\t";
	printTreeHelper( ((ForBlockNode*)tempPointer)->statementAction, 0);
	break;
	
	case WHILE_BLOCK:
	std::cout << "While Block:\t Condition:" << std::endl;
	printTreeHelper( ((WhileBlockNode*)tempPointer)->booleanCondition, depth + 1);
	break;

	case DO_WHILE_BLOCK:
	std::cout << "Do-While Block:\t COndition:" << std::endl;
	printTreeHelper( ((DoWhileBlockNode*)tempPointer)->booleanCondition, depth + 1);
	break;

	case IF_TYPE_BLOCK:
	switch( ((IfTypeBlock*)tempPointer)->myIfType)
	{
	case IF_TYPE:
	std::cout << "If" << std::endl;
	printTreeHelper( ((IfNode*)tempPointer)->booleanCondition, depth + 1);
	break;

	case ELIF_TYPE:
	std::cout << "Else if" << std::endl;
	printTreeHelper( ((ElifBlock*)tempPointer)->booleanCondition, depth + 1);
	break;

	case ELSE_TYPE:
	std::cout << "Else" << std::endl;
	break;
	
	default:
	std::cout << "Unknown if-type" << std::endl;
	break;
	}
}
std::cout << "Block Body:" << std::endl;
for(int i = 0; i < ((BooleanBlockNode*)tempPointer)->numStatements(); ++i)
{
printTreeHelper( ((BooleanBlockNode*)tempPointer)->blockStatementsList[i], depth + 1);
}
	break;


default: std::cout << "TODO in default case of switch:" << std::endl;
break;

}

if(tempPointer->symbolType == NON_MEMBER_FUNCTION_DEFINITION || tempPointer->symbolType == MEMBER_FUNCTION_DEFINITION)
{
if(((FunctionDefinitionNode*)tempPointer)->functionBody.size() == 0)
std::cout << "Empty function body" << std::endl;

else
{
for(int i = 0; i < ((FunctionDefinitionNode*)tempPointer)->functionBody.size(); ++i)
printTreeHelper(((FunctionDefinitionNode*)tempPointer)->functionBody[i], depth + 1);
}
}


}

void printTree()
{
std::cout << "Start of Parse Tree: " << std::endl;
if(root->symbolType == GLOBAL_LIST)
{
std::cout << "Root is a GLOBAL_LIST\n";
std::cout << "Listing children of root: " << std::endl;
}

for(int i = 0; i < ((GlobalListNode*) root)->globalStatementsList.size(); ++i)
{
printTreeHelper(((GlobalListNode*) root)->globalStatementsList[i], 1);
}

std::cout << "End of parse tree" << std::endl;

}



//DEBUGGED THROUGH HERE!

float doubleUnaryCombine(float myTerm, tokenType myToken)
{
if(myToken == PLUS)
return myTerm;

else if(myToken == MINUS)
return -1 * myTerm;

else if(myToken == NOT)
return (!myTerm);

else
{
std::cout << "Error: Unexpected operand in doubleUnaryCombine()" << std::endl;
exit(1);
}
return 0;
}
 template<typename E>
 E unaryCombine(E myTerm, tokenType myToken)
 {
  if(myToken == MINUS)
  return myTerm * -1;
  
  else if(myToken == PLUS)
  return myTerm;
  
  else if(myToken == BITWISE_NOT)
  return ~myTerm;
  
  //(in this case, the tokenType must have been the Not operator, which only applies to booleans.
  else
  {
  if(myTerm == true)
  return false;
  
  else
  return true;
  }
  
  }

float doubleBinaryCombine(float firstTerm, float secondTerm, tokenType myOperator)
{
switch(myOperator)
{
case PLUS:
return firstTerm + secondTerm;

case MINUS: 
return firstTerm - secondTerm;

case EXPONENT:
return pow(firstTerm, secondTerm);

case MULTIPLY:
return firstTerm * secondTerm;

case DIVIDE:
return firstTerm / secondTerm;

case LESS_THAN:
return firstTerm < secondTerm;

case LESS_THAN_EQUAL:
return firstTerm <= secondTerm;

case GREATER_THAN:
return firstTerm > secondTerm;

case EQUALS_EQUALS:
return firstTerm == secondTerm;

case NOT_EQUALS:
return firstTerm != secondTerm;

default:
std::cout << "Error: Invalid operand in doubleBinaryCombine() of type " << getTokenType(myOperator) << std::endl;
exit(1);
return 0;
}

return 0;
}

template<typename E>
E binaryCombine(E firstTerm, E secondTerm, tokenType myOperator)
{
switch(myOperator)
{
case PLUS:
return firstTerm + secondTerm;

case MINUS:
return firstTerm - secondTerm;

case EXPONENT:
return pow(firstTerm, secondTerm);

case MULTIPLY:
return firstTerm * secondTerm;

case DIVIDE:
return firstTerm / secondTerm;

case MOD:
return firstTerm % secondTerm;

case LOGICAL_AND:
return firstTerm && secondTerm;

case LOGICAL_OR:
return firstTerm || secondTerm;

case BITWISE_AND:
return firstTerm & secondTerm;

case BITWISE_OR:
return firstTerm | secondTerm;

case BITWISE_XOR:
return firstTerm ^ secondTerm;

case LESS_THAN:
return firstTerm < secondTerm;

case LESS_THAN_EQUAL:
return firstTerm <= secondTerm;

case GREATER_THAN:
return firstTerm > secondTerm;

case GREATER_THAN_EQUAL:
return firstTerm >= secondTerm;

case EQUALS_EQUALS:
return firstTerm == secondTerm;

case NOT_EQUALS:
return firstTerm != secondTerm;

case BIT_SHIFT_LEFT:
return firstTerm << secondTerm;

case BIT_SHIFT_RIGHT:
return firstTerm >> secondTerm;

default:
std::cout << "Error: Invalid operator in binary expression encountered in binaryCombine() of type " << getTokenType(myOperator) <<  std::endl;
exit(1);

}

return firstTerm;
}


//DEBUGGED THROUGH HERE

//converts expression node value to a double
float extractDouble(ExpressionNode* myExpression)
{
float myAnswer = -1;

if(myExpression->myType == INT_LIT)
	myAnswer = ((IntegerLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == DOUBLE_LIT)
	myAnswer = ((DoubleLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == CHAR_LIT)
	myAnswer = ((CharLiteralNode*)myExpression)->myValue;
else
	myAnswer = ((BooleanLiteralNode*)myExpression)->myValue;

return myAnswer;
}

int extractInt(ExpressionNode* myExpression)
{
int myAnswer = -1;

if(myExpression->myType == INT_LIT)
	myAnswer = ((IntegerLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == DOUBLE_LIT)
	myAnswer = ((DoubleLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == CHAR_LIT)
	myAnswer = ((CharLiteralNode*)myExpression)->myValue;

else
	myAnswer = ((BooleanLiteralNode*)myExpression)->myValue;

return myAnswer;
}

char extractChar(ExpressionNode* myExpression)
{
char myAnswer = 'Z';

if(myExpression->myType == INT_LIT)
	myAnswer = ((IntegerLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == DOUBLE_LIT)
	myAnswer = ((DoubleLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == CHAR_LIT)
	myAnswer = ((CharLiteralNode*)myExpression)->myValue;

else
	myAnswer = ((BooleanLiteralNode*)myExpression)->myValue;

return myAnswer;
}

bool extractBool(ExpressionNode* myExpression)
{
bool myAnswer = false;

if(myExpression->myType == INT_LIT)
	myAnswer = ((IntegerLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == DOUBLE_LIT)
	myAnswer = ((DoubleLiteralNode*)myExpression)->myValue;

else if(myExpression->myType == CHAR_LIT)
	myAnswer = ((CharLiteralNode*)myExpression)->myValue;

else
	myAnswer = ((BooleanLiteralNode*)myExpression)->myValue;

return myAnswer;
}

bool isLiteralNode(ExpressionNode* myExpression)
{
if(myExpression->myType == INT_LIT || myExpression->myType == CHAR_LIT
|| myExpression->myType == BOOL_LIT || myExpression->myType == DOUBLE_LIT || myExpression->myType == NULL_EXPRESSION)
return true;

return false;
}

//returns an expression node which represents the simplified evaluation of the current expression node. The function that calls this should set myExpression = to the return value of this function. If the expression cannot be simplified, then myExpression is returned
ExpressionNode* combineLikeTerms(ExpressionNode* myExpression, SymbolTable& symbolTable)
{
if(parseDebug)
std::cout << "Entered combineLikeTerms()" << std::endl;

tokenType myOperator;
int firstInt, secondInt;
float firstDouble, secondDouble;
char firstChar, secondChar;
bool firstBool, secondBool;
Type* firstType = NULL;
Type* secondType = NULL;

if(myExpression->myType == MEM_FUNC_EXPR || myExpression->myType == NON_MEM_FUNC_EXPR || myExpression->myType == BUILTIN_MEM_FUNC || myExpression->myType == PRINT_EXPR)
{
for(int i = 0; i < ((FunctionExpression*)myExpression)->argumentList.size(); ++i)
{
((FunctionExpression*)myExpression)->argumentList[i] = combineLikeTerms( ((FunctionExpression*)myExpression)->argumentList[i], symbolTable);
}
return myExpression;
}

//GET_INT_EXPR, GET_DOUBLE_EXPR, GET_BOOLEAN_EXPR, GET_CHAR_EXPR, GET_WORD_EXPR, GET_LINE_EXPR, IS_INPUT_FAIL_FUNC_EXPR
if(myExpression->myType == NULL_EXPRESSION || myExpression->myType == EMPTY_EXPRESSION_NODE || 
myExpression->myType == VARIABLE_EXPRESSION || myExpression->myType == MEM_VAR_EXPR || 
myExpression->myType == INT_LIT || myExpression->myType == DOUBLE_LIT || 
myExpression->myType == CHAR_LIT || myExpression->myType == BOOL_LIT || myExpression->myType == STRING_LIT
|| myExpression->myType == GET_INT_EXPR || myExpression->myType == GET_DOUBLE_EXPR || myExpression->myType == GET_BOOLEAN_EXPR
|| myExpression->myType == GET_CHAR_EXPR || myExpression->myType == GET_WORD_EXPR || myExpression->myType == GET_LINE_EXPR
|| myExpression->myType == IS_INPUT_FAIL_FUNC_EXPR)
return myExpression;

if(myExpression->myType == NEW_OBJECT_EXPRESSION)
{
((NewObjectExpression*)myExpression)->myFuncExpression = ((FunctionExpression*)combineLikeTerms( ((NewObjectExpression*) myExpression)->myFuncExpression, symbolTable));
return myExpression;
}

if(myExpression->myType == BUILTIN_CREATION_EXPRESSION)
{
if( ((BuiltinCreationExpression*)myExpression)->numElements != NULL)
{
((BuiltinCreationExpression*)myExpression)->numElements = ((ExpressionNode*)combineLikeTerms( ((BuiltinCreationExpression*)myExpression)->numElements, symbolTable));
}
return myExpression;
}


if(myExpression->myType == PLUS_PLUS_EXPR || myExpression->myType == MINUS_MINUS_EXPR)
{
return myExpression;
}

if(myExpression->myType == UNARY_EXPRESSION)
{
((UnaryOperationNode*)myExpression)->myExpression = combineLikeTerms( ((UnaryOperationNode*)myExpression)->myExpression, symbolTable); 
getType(((UnaryOperationNode*)myExpression)->myExpression, &firstType, symbolTable);
if(isLiteral(firstType))
{
myOperator =  ((UnaryOperationNode*)myExpression)->myToken;

if(strcmp(firstType->primitiveName, "BOOL_LIT") == 0)
{
return new BooleanLiteralNode( unaryCombine(((BooleanLiteralNode*)((UnaryOperationNode*)myExpression)->myExpression)->myValue, myOperator));
}

else if(strcmp(firstType->primitiveName, "INT_LIT") == 0)
{
return new IntegerLiteralNode(   unaryCombine( ((IntegerLiteralNode*)((UnaryOperationNode*)myExpression)->myExpression)->myValue, myOperator));
}

else if(strcmp(firstType->primitiveName, "CHAR_LIT") == 0)
{
return new CharLiteralNode(   unaryCombine( ((CharLiteralNode*)((UnaryOperationNode*)myExpression)->myExpression)->myValue, myOperator));
}

//if we're down here, the type must have been a double
else
{
return new DoubleLiteralNode(   doubleUnaryCombine( ((DoubleLiteralNode*)((UnaryOperationNode*)myExpression)->myExpression)->myValue, myOperator));
}
}
}

if(myExpression->myType == BINARY_EXPRESSION)
	{
	myOperator = ((BinaryExpressionNode*)myExpression)->myToken;
	((BinaryExpressionNode*)myExpression)->firstExpression = combineLikeTerms( ((BinaryExpressionNode*)myExpression)->firstExpression, symbolTable);
	((BinaryExpressionNode*)myExpression)->secondExpression = combineLikeTerms( ((BinaryExpressionNode*)myExpression)->secondExpression, symbolTable);
	 getType( ((BinaryExpressionNode*)myExpression)->firstExpression, &firstType, symbolTable);
         getType( ((BinaryExpressionNode*)myExpression)->secondExpression, &secondType, symbolTable);
	 if(isLiteral(firstType) && isLiteral(secondType))
		{
		if(strcmp(firstType->primitiveName, "DOUBLE_LIT") == 0 || strcmp(secondType->primitiveName, "DOUBLE_LIT") == 0)
			{
			firstDouble = extractDouble( ((BinaryExpressionNode*)myExpression)->firstExpression);
			secondDouble = extractDouble( ((BinaryExpressionNode*)myExpression)->secondExpression);
			return new DoubleLiteralNode(doubleBinaryCombine(firstDouble, secondDouble, myOperator));
			}
		else if(strcmp(firstType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "INT_LIT") == 0)
			{
			firstInt = extractInt( ((BinaryExpressionNode*)myExpression)->firstExpression);
			secondInt = extractInt( ((BinaryExpressionNode*)myExpression)->secondExpression);
			return new IntegerLiteralNode(binaryCombine(firstInt, secondInt, myOperator));
			}

		else if(strcmp(firstType->primitiveName, "CHAR_LIT") == 0 || strcmp(secondType->primitiveName, "CHAR_LIT") == 0)
			{
			firstChar = extractChar( ((BinaryExpressionNode*)myExpression)->firstExpression);
			secondChar = extractChar( ((BinaryExpressionNode*)myExpression)->secondExpression);
			return new CharLiteralNode(binaryCombine(firstChar, secondChar, myOperator));
			}

		else if(strcmp(firstType->primitiveName, "BOOL_LIT") == 0 || strcmp(secondType->primitiveName, "BOOL_LIT") == 0)
			{
			firstBool = extractBool( ((BinaryExpressionNode*)myExpression)->firstExpression);
			secondBool = extractBool( ((BinaryExpressionNode*)myExpression)->secondExpression);
			return new BooleanLiteralNode(binaryCombine(firstBool, secondBool, myOperator));
			}

		else if(strcmp(firstType->primitiveName, "NULL") == 0 || strcmp(secondType->primitiveName, "NULL") == 0)
			{
			if(strcmp(firstType->primitiveName, "NULL") != 0 || strcmp(secondType->primitiveName, "NULL") != 0)
				{
				std::cout << "Error: NULL can't be combined with non NULL literal. " << std::endl;
				exit(1);
				}
			if(myOperator == EQUALS_EQUALS)
				return new BooleanLiteralNode(true);
			else
				return new BooleanLiteralNode(false);
			
			}
		
		else
			{
			std::cout << "Error: Binary operation may only contain object names, booleans, chars, ints or doubles. " << std::endl;
			exit(1);
			}
		

		}



	}

return myExpression;
}


//used to check compatability in assignment operations ONLY
bool isCompatable(Type* firstType, Type* secondType)
{
if(parseDebug)
std::cout << "Entered isCompatable() for Types " << *firstType << " and " << *secondType << std::endl;

if((*firstType) == (*secondType))
return true;

if(secondType->isPrimitive == true && 
strcmp(secondType->primitiveName, "NULL") == 0 && 
(firstType->isPrimitive == false || 
	(isLiteral(firstType) == false &&  
		(strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "double") != 0
		&& strcmp(firstType->primitiveName, "char") != 0 && strcmp(firstType->primitiveName, "boolean") != 0)) ))
return true;

if(firstType->isPrimitive == false || secondType->isPrimitive == false)
return false;

//first variable must be an lvalue.
if(strcmp(firstType->primitiveName, "INT_LIT") == 0 || strcmp(firstType->primitiveName, "DOUBLE_LIT") == 0 
|| strcmp(firstType->primitiveName, "CHAR_LIT") == 0 || strcmp(firstType->primitiveName, "BOOL_LIT") == 0
|| strcmp(firstType->primitiveName, "STRING_LIT") == 0 || strcmp(firstType->primitiveName, "NULL") == 0)
return false;

if(strcmp(firstType->primitiveName, "String") == 0 && strcmp(secondType->primitiveName, "STRING_LIT") == 0)
return true;

if(strcmp(firstType->primitiveName, "int") == 0)
{
	if(strcmp(secondType->primitiveName, "double") == 0 || strcmp(secondType->primitiveName, "char") == 0	|| strcmp(secondType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "DOUBLE_LIT") == 0
|| strcmp(secondType->primitiveName, "CHAR_LIT") == 0 || strcmp(secondType->primitiveName, "int") == 0)
	return true;

	return false;

}

if(strcmp(firstType->primitiveName, "double") == 0)
{
	if(strcmp(secondType->primitiveName, "double") == 0 || strcmp(secondType->primitiveName, "int") == 0 || strcmp(secondType->primitiveName, "char") == 0
	|| strcmp(secondType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "DOUBLE_LIT") == 0 || strcmp(secondType->primitiveName, "CHAR_LIT") == 0)
	return true;

	return false;

}

if(strcmp(firstType->primitiveName, "char") == 0)
{
	if(strcmp(secondType->primitiveName, "int") == 0 || strcmp(secondType->primitiveName, "char") == 0 || strcmp(secondType->primitiveName, "double") == 0
	|| strcmp(secondType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "DOUBLE_LIT") == 0 || strcmp(secondType->primitiveName, "CHAR_LIT") == 0)
	return true;

	return false;
}

if(strcmp(firstType->primitiveName, "boolean") == 0)
	{
	if(strcmp(secondType->primitiveName, "BOOL_LIT") == 0 || strcmp(secondType->primitiveName, "boolean") == 0)
	return true;

	return false;
	}


if(strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "double") != 0
&& strcmp(firstType->primitiveName, "char") != 0 && strcmp(firstType->primitiveName, "boolean") != 0)
{
if(strcmp(secondType->primitiveName, "NULL") == 0)
	return true;

	return false;
}

return false;
}

//DEBUGGED THROUGH HERE NOW!
void checkPipeLine(abstractSymbolNode* myAbstraction, SymbolTable& symbolTable)
{
if(parseDebug)
std::cout << "Entered checkPipeLine()" << std::endl;
PipeLineNode* myPipeLine = ((PipeLineNode*)myAbstraction);
Function* myFunction;
Type* lastReturnType = new Type((char*)"void");
if(myPipeLine->getNumFunctions() == 0)
{
std::cout << "Error: pipeline must have at least 1 function. " << std::endl;
exit(1);
}

else if(myPipeLine->getNumFunctions() == 1)
{
if(symbolTable.getNonMemberFunction(myPipeLine->getFunctionNumber(0)) == NULL)
{
std::cout << "Error: Function in PipeLine constructor was not a defined non-member function. Terminating execution now..." << std::endl;
exit(1);
}

if(parseDebug)
std::cout << "PipeLine had only one function in it, which was a non member function. Returning..." << std::endl;
return;
}

for(int i = 0; i < myPipeLine->getNumFunctions(); ++i)
{
	myFunction = symbolTable.getNonMemberFunction(myPipeLine->getFunctionNumber(i));
	if(myFunction == NULL)
	{
	std::cout << "Error: undefined non-member function encountered in pipeline constructor" << std::endl;
	exit(1);
	}
	if(i != 0)
	{
	if(myFunction->parameterList.size() != 1)
		{
		std::cout << "Error: Functions outside of beggining of pipeline must take exactly one argument for input. " << std::endl;
		exit(1);
		}
	if(!isCompatable(lastReturnType, myFunction->parameterList[0]->variableType))
		{
		std::cout << "Error: return type of one function in pipeline cannot be converted to input of next function for function " << myFunction->functionName << std::endl;
		exit(1);
		}
	}
	lastReturnType = myFunction->functionReturnType;
}
return;
}

//DEBUGGED THROUGH HERE
void getType(abstractSymbolNode* myAbstraction, Type** returnedType, SymbolTable& symbolTable)
{
ExpressionNode* myExpression = ((ExpressionNode*) myAbstraction);
char className[1000];
tokenType myToken;
Type* firstType = NULL;
Type* secondType = NULL;
Type* idealType = NULL;
Type* realType = NULL;
Type* argType = NULL;
Function* function = NULL;
FunctionExpression* myFuncExpr = NULL;
if(myExpression == NULL)
{
std::cout << "An exception has occured in getType(): myAbstraction was NULL\n";
}
switch(myExpression->myType)
{
case EMPTY_EXPRESSION_NODE:
case NULL_EXPRESSION:
(*returnedType) = new Type((char*)"NULL");
return;
break;

case INT_LIT:
(*returnedType) = new Type((char*)"INT_LIT");
return;
break;

case DOUBLE_LIT:
(*returnedType) = new Type((char*)"DOUBLE_LIT");
return;
break;

case CHAR_LIT:
(*returnedType) = new Type((char*)"CHAR_LIT");
return;
break;

case BOOL_LIT:
(*returnedType) = new Type((char*)"BOOL_LIT");
return;
break;

case STRING_LIT:
(*returnedType) = new Type((char*)"STRING_LIT");
return;
break;

//GET_INT_EXPR, GET_DOUBLE_EXPR, GET_BOOLEAN_EXPR, GET_CHAR_EXPR, GET_WORD_EXPR, GET_LINE_EXPR, IS_INPUT_FAIL_FUNC_EXPR
case GET_INT_EXPR:
(*returnedType) = new Type( (char*)"int");
return;
break;

case GET_DOUBLE_EXPR:
(*returnedType) = new Type( (char*)"double");
return;
break;

case GET_BOOLEAN_EXPR:
(*returnedType) = new Type( (char*)"boolean");
return;
break;

case GET_CHAR_EXPR:
(*returnedType) = new Type( (char*)"char");
return;
break;

case GET_WORD_EXPR:
(*returnedType) = new Type( (char*)"string");
return;
break;


case GET_LINE_EXPR:
(*returnedType) = new Type( (char*)"string");
return;
break;

case IS_INPUT_FAIL_FUNC_EXPR:
(*returnedType) = new Type( (char*)"boolean");
return;
break;

case PRINT_EXPR:
for(int i = 0; i < ((PrintExpression*)myExpression)->argumentList.size(); ++i)
{
getType( ((PrintExpression*)myExpression)->argumentList[i], &firstType, symbolTable);
}
(*returnedType) = new Type( (char*) "void");
return;
break;

case VARIABLE_EXPRESSION:
//std::cout << "in VARIABLE_EXPRESSION case in getType()\n";
(*returnedType) = symbolTable.getTypeOfVariable(((VariableNode*) myExpression)->varName);
if(*returnedType == NULL)
{
std::cout << "Error: expression contains undefined variable named " << ((VariableNode*)myExpression)->varName << std::endl;
exit(1);
}
return;
break;

case PLUS_PLUS_EXPR:
getType( ((PlusPlusNode*) myExpression)->myExpr, &firstType, symbolTable);
if(firstType->isPrimitive == false)
{
std::cout << "Error: No primitive in ++" << std::endl;
exit(1);
}
if(strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "double") != 0
&& strcmp(firstType->primitiveName, "char") != 0)
{
std::cout << "Error: Type of " << (*firstType) << " encountered in plus-plus expression. Expected int, double, or char. " << std::endl;
exit(1);
}
*returnedType = firstType;
return;
break;

case MINUS_MINUS_EXPR:
getType( ((MinusMinusNode*)myExpression)->myExpr, &firstType, symbolTable);
if(!firstType->isPrimitive)
{
std::cout << "Error: Expected primitive in --" << std::endl;
exit(1);
}
if(strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "double") != 0
&& strcmp(firstType->primitiveName, "char") != 0)
{
std::cout << "Error: Type of " << *firstType << " encountered in minus-minus expression. Expected int, double or char. " << std::endl;
exit(1);
}
*returnedType = firstType;
return;
break;

case MEM_VAR_EXPR:
//std::cout << "in MEM_VAR_EXPR case in getType()\n";
*returnedType = symbolTable.getMemberVarTypeOfObject( ((MemberVarExpression*) myExpression)->objectName, ((MemberVarExpression*)myExpression)->varName);
if((*returnedType) == NULL)
{
std::cout << "Error: Undefined member variable expression of object " << ((MemberVarExpression*)myExpression)->objectName << 
" and field of " << ((MemberVarExpression*)myExpression)->varName << " encountered" << std::endl;
}
return;
break;

case MEM_FUNC_EXPR:
case NON_MEM_FUNC_EXPR:
case NEW_OBJECT_EXPRESSION:
case BUILTIN_CREATION_EXPRESSION:

if(myExpression->myType == MEM_FUNC_EXPR)
{
function = symbolTable.getMemberFunctionOfObject( ((MemberFunctionExpression*) myExpression)->objectName, ((FunctionExpression*)myExpression)->funcName);
*returnedType = function->functionReturnType;
myFuncExpr = ((FunctionExpression*)myExpression);
}

else if(myExpression->myType == NON_MEM_FUNC_EXPR)
{
function = symbolTable.getNonMemberFunction( ((FunctionExpression*)myExpression)->funcName);
*returnedType = function->functionReturnType;
myFuncExpr = ((FunctionExpression*)myExpression);
}

//handling new object expression here.
else if(myExpression->myType == NEW_OBJECT_EXPRESSION)
{
function = symbolTable.getConstructorOfClass( ((NewObjectExpression*)myExpression)->className);
*returnedType = new Type( ((NewObjectExpression*)myExpression)->className);
myFuncExpr = ((NewObjectExpression*)myExpression)->myFuncExpression;
}

else
{
strcpy(className,  ((BuiltinCreationExpression*)myExpression)->className);
if(strcmp(className, "Array") == 0)
{
if( ((BuiltinCreationExpression*)myExpression)->numElements == NULL)
{
std::cout << "Error: Array constructor must have exactly 1 argument." << std::endl;
exit(1);
}
getType( ((BuiltinCreationExpression*)myExpression)->numElements, &firstType, symbolTable);
if(firstType->isPrimitive == false || (strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "INT_LIT") != 0))
{
std::cout << "Error: Type of argument in Array construction must be an int" << std::endl;
exit(1);
}
}
else
{
if( ((BuiltinCreationExpression*)myExpression)->numElements  != NULL)
{
std::cout << "Error: All buliltin types besides arrays and pipelines must have 0 arguments. " << std::endl;
std::cout << ((BuiltinCreationExpression*)myExpression)->numElements << std::endl;
exit(1);
}
}


*returnedType = ((BuiltinCreationExpression*)myExpression)->fullType;
if((*returnedType) == NULL)
{
std::cout << "Error: Type of builtin argument was NULL " << std::endl;
exit(1);
}
return;
}

//std::cout << "Type of: " << ((ExpressionNode*)myExpression)->myType << std::endl;
if( myFuncExpr->argumentList.size() != function->parameterList.size())
{
std::cout << "Error: Mismatch in number of expressions in function call for function " << function->functionName << " . Expected " << function->parameterList.size() << " arguments, but found " << 
myFuncExpr->argumentList.size() << " arguments instead" << std::endl;
exit(1);
}

//since we have a match in number of args, now checking if each argument is type compatable to its corresponding parameter.

for(int i = 0; i < function->parameterList.size(); ++i)
{
getType( myFuncExpr->argumentList[i], &firstType, symbolTable);

secondType = function->parameterList[i]->variableType;

if(! isCompatable(secondType, firstType))
{
std::cout << "Error: conflicting types of " << *firstType << " and " << *secondType << " found while comparing actual function arguments to expected function arguments " << std::endl;
exit(1);
}

}
return;
break;

case BUILTIN_MEM_FUNC:
argType = symbolTable.getTypeOfVariable(  ((BuiltinMemberFunctionExpression*) myExpression)->objName);

if( ((BuiltinMemberFunctionExpression*)myExpression)->functionName == SORT || ((BuiltinMemberFunctionExpression*)myExpression)->functionName == LENGTH || ((BuiltinMemberFunctionExpression*)myExpression)->functionName == POP || ((BuiltinMemberFunctionExpression*)myExpression)->functionName == FRONT)
{
	if( ((BuiltinMemberFunctionExpression*)myExpression)->numArgs() != 0)
	{
	std::cout << "Error: Builtin sort, front, pop and length functions take no arguments\n";
	exit(1);
	}
	
	if( ((BuiltinMemberFunctionExpression*)myExpression)->functionName == SORT)
	*returnedType = new Type((char*)"void");

	else if( ((BuiltinMemberFunctionExpression*)myExpression)->functionName == LENGTH)
	*returnedType = new Type((char*)"int");
		
	else
	*returnedType = stripType(argType);
	return;
}

else if( ((BuiltinMemberFunctionExpression*)myExpression)->functionName == UPDATE)
{
if( ((BuiltinMemberFunctionExpression*)myExpression)->numArgs() != 2)
{
std::cout << "Error: Builtin function UPDATE must take exactly 2 arguments\n";
exit(1);
}
}

else
{
	if( ((BuiltinMemberFunctionExpression*)myExpression)->numArgs() != 1)
	{
	std::cout << "Error: Every builtin function besides sort, length, pop and update takes exactly 1 argument\n";
	exit(1);
	}
}

if( ((BuiltinMemberFunctionExpression*) myExpression)->functionName == GET)
{
getType(   ((BuiltinMemberFunctionExpression*)myExpression)->argumentList[0], &realType, symbolTable);
idealType = stripType(argType);
if(!isCompatable(idealType, realType))
{
std::cout << "Error: Argument to function get was supposed to be " << *idealType << " but was " << *realType << std::endl;
exit(1);
}	
*returnedType = new Type((char*)"int");
return;
}

else if(  ((BuiltinMemberFunctionExpression*)myExpression)->functionName == PUT)
{
getType(   ((BuiltinMemberFunctionExpression*)myExpression)->argumentList[0], &realType, symbolTable);
idealType = stripType(argType);
if(!isCompatable(idealType, realType))
{
std::cout << "Error: Argument to function put was supposed to be " << *idealType << " but was " << *realType << std::endl;
exit(1);
}
*returnedType = new Type((char*)"void");
return;
}

else if(  ((BuiltinMemberFunctionExpression*)myExpression)->functionName == CONTAINS)
{
getType( ((BuiltinMemberFunctionExpression*)myExpression)->argumentList[0], &realType, symbolTable);
idealType = stripType(argType);
if(!isCompatable(idealType, realType))
{
std::cout << "Error: Argument to function contains was supposed to be " << *idealType << " but was " << *realType << std::endl;
exit(1);
}
*returnedType = new Type((char*)"boolean");
return;
}


else if( ((BuiltinMemberFunctionExpression*)myExpression)->functionName == UPDATE)
{
getType( ((BuiltinMemberFunctionExpression*)myExpression)->argumentList[0], &realType, symbolTable);
idealType = new Type((char*)"int");
if(!isCompatable(idealType, realType))
{
std::cout << "Error: first argument to function UPDATE must evaluate to an int.\n";
exit(1);
}

getType( ((BuiltinMemberFunctionExpression*)myExpression)->argumentList[1], &realType, symbolTable);
idealType = stripType(argType);
if(!isCompatable(idealType, realType))
{
std::cout << "Error: Argument to function UPDATE was supposed to be " << *idealType << " but was " << *realType << std::endl;
exit(1);
}

*returnedType = new Type((char*)"void");
return;
}

else if( ((BuiltinMemberFunctionExpression*)myExpression)->functionName == PUSH)
{
getType( ((BuiltinMemberFunctionExpression*)myExpression)->argumentList[0], &realType, symbolTable);
idealType = stripType(argType);
if(!isCompatable(idealType, realType))
{
std::cout << "Error: Argument to function PUSH was supposed to be " << *idealType << " but was " << *realType << std::endl;
exit(1);
}
*returnedType = new Type((char*)"void"); 
}

//in this case, the function must be AT, since there are no other builtin functions left.
else
{
getType( ((BuiltinMemberFunctionExpression*)myExpression)->argumentList[0], &realType, symbolTable);
idealType = new Type((char*)"int");
if(!isCompatable(idealType, realType))
{
std::cout << "Error: Argument to function at must be int. Here, type was " << *realType << std::endl;
exit(1);
}

*returnedType = stripType(argType);
return;
}
return;
break;

case UNARY_EXPRESSION:
myToken = ((UnaryOperationNode*) myExpression)->myToken;
getType( ((UnaryOperationNode*)myExpression)->myExpression, &firstType, symbolTable);
if(!firstType->isPrimitive)
{
std::cout << "Error: " << getTokenType(myToken) << " must be followed by a primitive type " << std::endl;
exit(1);
}
if(myToken == NOT)
{
if(strcmp(firstType->primitiveName, "boolean") != 0 && strcmp(firstType->primitiveName, "BOOL_LIT") != 0)
{
std::cout << "Error: Not operator must be followed by boolean expression" << std::endl;
exit(1);
}
*returnedType = firstType;
return;
break;
}

//DEBUGGED THROUGH HERE!

else if(myToken == MINUS || myToken == PLUS)
{
if(strcmp(firstType->primitiveName, "double") != 0 && strcmp(firstType->primitiveName, "DOUBLE_LIT") != 0 
&& strcmp(firstType->primitiveName, "char") != 0 && strcmp(firstType->primitiveName, "CHAR_LIT") != 0 
&& strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "INT_LIT") != 0)
{
std::cout << "Error: int, double, or char must follow a unary plus or minus " << std::endl;
exit(1);
}
*returnedType = firstType;
return;
break;
}

//this is the case of the bitwise not
else
{
if(strcmp(firstType->primitiveName, "char") != 0 && strcmp(firstType->primitiveName, "CHAR_LIT") != 0
&& strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "INT_LIT") != 0
&& strcmp(firstType->primitiveName, "boolean") != 0 && strcmp(firstType->primitiveName, "BOOL_LIT") != 0)
{
std::cout << "Error: Bitwise not can only be applied to chars, ints, and booleans" << std::endl;
exit(1);
}
*returnedType = firstType;
return;
break;
}
return;
break;

//DEBUGGED THROUGH HERE!

case BINARY_EXPRESSION:
{
myToken = ((BinaryExpressionNode*) myExpression)->myToken;
getType( ((BinaryExpressionNode*) myExpression)->firstExpression, &firstType, symbolTable);
getType( ((BinaryExpressionNode*) myExpression)->secondExpression, &secondType, symbolTable);

if(myToken == PLUS || myToken == MINUS || myToken == EXPONENT 
|| myToken == MULTIPLY || myToken == DIVIDE)
{
if(firstType->isPrimitive == false || secondType->isPrimitive == false)
{
std::cout << "Error: both types to operator must be primitives First type:" << *firstType << ". Second type: " << *secondType << std::endl;
exit(1);
}

if(   (strcmp(firstType->primitiveName, "int") != 0  && strcmp(firstType->primitiveName, "INT_LIT") != 0 && strcmp(firstType->primitiveName, "double") != 0 && strcmp(firstType->primitiveName, "DOUBLE_LIT") != 0 &&
strcmp(firstType->primitiveName, "char") != 0 && strcmp(firstType->primitiveName, "CHAR_LIT") != 0)       || (strcmp(secondType->primitiveName, "int") != 0 && strcmp(secondType->primitiveName, "INT_LIT") != 0
&& strcmp(secondType->primitiveName, "double") != 0 && strcmp(secondType->primitiveName, "DOUBLE_LIT") != 0 
&& strcmp(secondType->primitiveName, "char") != 0 && strcmp(secondType->primitiveName, "CHAR_LIT") != 0))
{
std::cout << "Error: both types of this operation must be of types int, double, or char. Actual types: " << *firstType << " and " << *secondType << std::endl;
exit(1);
}

if(strcmp(firstType->primitiveName, "double") == 0 || strcmp(secondType->primitiveName, "double") == 0)
*returnedType = new Type((char*)"double");

else if(strcmp(firstType->primitiveName, "DOUBLE_LIT") == 0 || strcmp(secondType->primitiveName, "DOUBLE_LIT") == 0)
{
if(strcmp(firstType->primitiveName, "double") == 0 || strcmp(secondType->primitiveName, "double") == 0 || strcmp(firstType->primitiveName, "int") == 0 || strcmp(secondType->primitiveName, "int") == 0
|| strcmp(firstType->primitiveName, "char") == 0 || strcmp(secondType->primitiveName, "char") == 0)
*returnedType = new Type((char*)"Double");

else
*returnedType = new Type((char*)"DOUBLE_LIT");
}

else if(strcmp(firstType->primitiveName, "int") == 0 || strcmp(secondType->primitiveName, "int") == 0)
*returnedType = new Type((char*)"int");

else if(strcmp(firstType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "INT_LIT") == 0)
{
if(strcmp(firstType->primitiveName, "int") == 0 || strcmp(secondType->primitiveName, "int") == 0 || strcmp(firstType->primitiveName, "char") == 0 || strcmp(secondType->primitiveName, "char") == 0)
*returnedType = new Type((char*)"int");

else
*returnedType = new Type((char*)"INT_LIT");
}

else if(strcmp(firstType->primitiveName, "char") == 0 || strcmp(secondType->primitiveName, "char") == 0)
{
*returnedType = new Type((char*)"char");
}

else
*returnedType = new Type((char*)"CHAR_LIT");

return;
break;
}

if(myToken == MOD)
{
if(firstType->isPrimitive == false || secondType->isPrimitive == false)
{
std::cout << "Error: Mod operator requires int or char operands only." << std::endl;
exit(1);
}

if( (strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "INT_LIT") != 0 && strcmp(firstType->primitiveName, "char") != 0 && 
strcmp(firstType->primitiveName, "CHAR_LIT") != 0) || (strcmp(secondType->primitiveName, "int") != 0 && 
strcmp(secondType->primitiveName, "INT_LIT") != 0 && strcmp(secondType->primitiveName, "char") != 0 && strcmp(secondType->primitiveName, "CHAR_LIT") != 0)  )
{
std::cout << "Error: Mod operator only accepts int or char operands." << std::endl;
exit(1);
}

if(strcmp(firstType->primitiveName, "int") == 0 || strcmp(secondType->primitiveName, "int") == 0)
*returnedType = new Type((char*)"int");

else if(strcmp(firstType->primitiveName, "char") == 0 || strcmp(secondType->primitiveName, "char") == 0)
{
if(strcmp(firstType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "INT_LIT") == 0)
*returnedType = new Type((char*)"int");
else
*returnedType = new Type((char*)"char");
} 

else if(strcmp(firstType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "INT_LIT") == 0)
*returnedType = new Type((char*)"INT_LIT");

else
*returnedType = new Type((char*)"CHAR_LIT");

return;
break;
}

else if(myToken == LOGICAL_AND || myToken == LOGICAL_OR)
{
if(firstType->isPrimitive == false || secondType->isPrimitive == false)
{
std::cout << "Error: both operands to logical and and logical or must be booleans" << std::endl;
exit(1);
}
if( (strcmp(firstType->primitiveName, "boolean") != 0 && strcmp(firstType->primitiveName, "BOOL_LIT") != 0) 
|| (strcmp(secondType->primitiveName, "boolean") != 0 && strcmp(secondType->primitiveName, "BOOL_LIT") != 0))
{
std::cout << "Error: Both operands of logical and and logical or must be booleans" << std::endl;
exit(1);
}

if(strcmp(firstType->primitiveName, "boolean") == 0 || strcmp(secondType->primitiveName, "boolean") == 0)
*returnedType = new Type((char*)"boolean");

else
*returnedType = new Type((char*)"BOOL_LIT");

return;
break;
}

//in these cases, the token must be either an int or a char on both sides
else if(myToken == BITWISE_AND || myToken == BITWISE_OR || myToken == BITWISE_XOR || myToken == BIT_SHIFT_LEFT || myToken == BIT_SHIFT_RIGHT)
{
if( (strcmp(firstType->primitiveName, "int") != 0 && strcmp(firstType->primitiveName, "INT_LIT") != 0 && strcmp(firstType->primitiveName, "char") != 0 && strcmp(firstType->primitiveName, "CHAR_LIT") != 0) ||

(strcmp(secondType->primitiveName, "int") != 0 && strcmp(secondType->primitiveName, "INT_LIT") != 0 && strcmp(secondType->primitiveName, "char") != 0 && strcmp(secondType->primitiveName, "CHAR_LIT") != 0)   )
{
std::cout << "Error: Only int and char types are allowed to use bitwise operators. " << std::endl;
exit(1);
}

if(strcmp(firstType->primitiveName, "int") == 0 || strcmp(secondType->primitiveName, "int") == 0)
*returnedType = new Type((char*)"int");


else if(strcmp(firstType->primitiveName, "char") == 0 || strcmp(secondType->primitiveName, "char") == 0)
{
if(strcmp(firstType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "INT_LIT") == 0)
*returnedType = new Type((char*)"int");

else
*returnedType = new Type((char*)"char");
}

else if(strcmp(firstType->primitiveName, "INT_LIT") == 0 || strcmp(secondType->primitiveName, "INT_LIT") == 0)
*returnedType = new Type((char*)"INT_LIT");

else
*returnedType = new Type((char*)"CHAR_LIT");

return;
break;
}

//this must be the case where the binary operator is <, <=, ==, !=, >, or >=
else
{
if(!isCompatable(firstType, secondType))
{
std::cout << "Error: Incompatable types of " << *firstType << " and " << *secondType << " cannot be compared by boolean operators." << std::endl;
exit(1);
}
if(isLiteral(firstType) && isLiteral(secondType))
*returnedType = new Type((char*)"BOOL_LIT");

else
*returnedType = new Type((char*)"boolean");

return;
break;
}
}

default:
std::cout << "Error: Default cause hit in getType() " << std::endl;
exit(1);
}

return;
}

bool isLeftType(ExpressionNode* myExpression, SymbolTable& symbolTable)
{
if(myExpression == NULL)
std::cout << "Entered isLeftType()... myExpression was NULL! Expect a segfault message to occur shortly after." << std::endl;
Type* myType = NULL;
switch(myExpression->myType)
{
case BINARY_EXPRESSION:
case UNARY_EXPRESSION:
case INT_LIT:
case DOUBLE_LIT:
case CHAR_LIT:
case BOOL_LIT:
case STRING_LIT:
case NULL_EXPRESSION:
case EMPTY_EXPRESSION_NODE:
case NEW_OBJECT_EXPRESSION:
case BUILTIN_MEM_FUNC:
case PRINT_EXPR:
case GET_INT_EXPR:
case GET_DOUBLE_EXPR:
case GET_WORD_EXPR:
case GET_LINE_EXPR:
case GET_CHAR_EXPR:
case GET_BOOLEAN_EXPR:
case IS_INPUT_FAIL_FUNC_EXPR:
return false;

case VARIABLE_EXPRESSION:
case MEM_VAR_EXPR:
case PLUS_PLUS_EXPR:
case MINUS_MINUS_EXPR:
return true;

case MEM_FUNC_EXPR:
case NON_MEM_FUNC_EXPR:
getType(myExpression, &myType, symbolTable);
if(!myType->isPrimitive)
return true;

else
{
if(strcmp(myType->primitiveName, "NULL") != 0 && strcmp(myType->primitiveName, "int") != 0 && strcmp(myType->primitiveName, "INT_LIT") != 0 && strcmp(myType->primitiveName, "double") != 0 && strcmp(myType->primitiveName, "DOUBLE_LIT") != 0
&& strcmp(myType->primitiveName, "char") != 0 && strcmp(myType->primitiveName, "CHAR_LIT") != 0 && strcmp(myType->primitiveName, "boolean") != 0 && strcmp(myType->primitiveName, "BOOL_LIT") != 0)
return true;

else
return false;
}

default:
return false;
}

return false;
}

//DEBUGGED THROUGH HERE!

void MatchTypesLastStatement(SymbolTable& symbolTable)
{
if(parseDebug)
std::cout << "Entered MatchTypesLastStatement()" << std::endl;

abstractSymbolNode* lastStatement = NULL;
Type* firstType = NULL;
Type* secondType = NULL;
abstractSymbolNode* travelPointer = currentNode;
abstractType parentType = currentNode->symbolType;
if(currentNode->symbolType == GLOBAL_LIST)
	lastStatement = ((GlobalListNode*)currentNode)->getLastStatement();
else if(currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == MEMBER_FUNCTION_DEFINITION)
	lastStatement = ((FunctionDefinitionNode*)currentNode)->getLastStatement();

else if(currentNode->symbolType == BOOLEAN_BLOCK)
	{
	if(((BooleanBlockNode*) currentNode)->myType == FOR_BLOCK)
		lastStatement = ((ForBlockNode*)currentNode)->getLastFor();
	else
		lastStatement = ((BooleanBlockNode*)currentNode)->getLastStatement();

	}
else
{
std::cout << "Error: Unknown type of current node encountered in MatchTypesLastStatement()" << std::endl;
exit(1);
}

if(lastStatement->symbolType == EXPRESSION)
{
getType(lastStatement, &firstType, symbolTable);
switch(parentType)
{
case GLOBAL_LIST:
	((GlobalListNode*)currentNode)->replaceLast(combineLikeTerms(((ExpressionNode*)lastStatement), symbolTable));
	break;
case NON_MEMBER_FUNCTION_DEFINITION:
case MEMBER_FUNCTION_DEFINITION:
	((FunctionDefinitionNode*) currentNode)->replaceLast(combineLikeTerms(((ExpressionNode*)lastStatement), symbolTable));
	break;

case BOOLEAN_BLOCK:
	if( ((BooleanBlockNode*)currentNode)->myType == FOR_BLOCK)
		((ForBlockNode*)currentNode)->replaceLastFor(combineLikeTerms(((ExpressionNode*)lastStatement), symbolTable));
	else
		((BooleanBlockNode*)currentNode)->replaceLast(combineLikeTerms(((ExpressionNode*)lastStatement), symbolTable));
	break;

default:
std::cout << "Error: Unknown block\n";
exit(1);
}
return;
}
else if(lastStatement->symbolType != STATEMENT)
{
std::cout << "Error: Expected last line processed to be an expression, or statement. However, last line was neither. Terminating execution now..." << std::endl;
exit(1);
}

else
{
if( ((StatementNode*)lastStatement)->myType == ASSIGNMENT_STATEMENT)
{
if(!isLeftType( ((AssignmentStatement*)lastStatement)->leftValue, symbolTable))
{
std::cout << "Error: lefthand side of assignment statement was not an lvalue" << std::endl;
exit(1);
}
getType(((AssignmentStatement*) lastStatement)->leftValue, &firstType, symbolTable);
getType(((AssignmentStatement*) lastStatement)->rightValue, &secondType, symbolTable);

if(!isCompatable(firstType, secondType))
{
std::cout << "Error: Incompatable types of " << *firstType << " and " << *secondType << " in assignment statement" << std::endl;
exit(1);
}
((AssignmentStatement*)lastStatement)->leftValue = combineLikeTerms( ((AssignmentStatement*)lastStatement)->leftValue, symbolTable);
((AssignmentStatement*)lastStatement)->rightValue = combineLikeTerms( ((AssignmentStatement*)lastStatement)->rightValue, symbolTable);
}

else if( ((StatementNode*)lastStatement)->myType == COMPOUND_ASSIGNMENT_STATEMENT)
{
if(! isLeftType( ((CompoundAssignmentStatement*)lastStatement)->leftHalf, symbolTable))
{
std::cout << "Error: Left half of compound assignment was not an lvalue. " << std::endl;
exit(1);
}
getType(((CompoundAssignmentStatement*) lastStatement)->leftHalf, &firstType, symbolTable);
getType(((CompoundAssignmentStatement*) lastStatement)->rightHalf, &secondType, symbolTable);
if(! isCompatable(firstType, secondType))
{
std::cout << "Error: Incompatable types of " << *firstType << " and " << *secondType << " in compound assignment statement" << std::endl;
exit(1);
}
((CompoundAssignmentStatement*)lastStatement)->leftHalf = combineLikeTerms( ((CompoundAssignmentStatement*)lastStatement)->leftHalf, symbolTable);
((CompoundAssignmentStatement*)lastStatement)->rightHalf = combineLikeTerms( ((CompoundAssignmentStatement*)lastStatement)->rightHalf, symbolTable);
}
else if( ((StatementNode*) lastStatement)->myType == DECLARATION_STATEMENT)
{
;
}
else if( ((StatementNode*) lastStatement)->myType == DECLARE_ASSIGN_STATEMENT)
{
getType(((DeclareAssignStatement*) lastStatement)->assignedPortion, &secondType, symbolTable);

if(! isCompatable( ((DeclareAssignStatement*) lastStatement)->declarePortion->myType, secondType))
{
std::cout << "Error: Incompatable types of " << *((DeclareAssignStatement*)lastStatement)->declarePortion->myType << " and " << *secondType << " in declare-assign statement" << std::endl;
exit(1);
}
((DeclareAssignStatement*)lastStatement)->assignedPortion = combineLikeTerms( ((DeclareAssignStatement*)lastStatement)->assignedPortion, symbolTable);
}
else if( ((StatementNode*) lastStatement)->myType == EXPRESSION_STATEMENT)
{
getType( ((ExpressionStatement*) lastStatement)->myExpression, &firstType, symbolTable);
((ExpressionStatement*)lastStatement)->myExpression = combineLikeTerms( ((ExpressionStatement*)lastStatement)->myExpression, symbolTable);
}
else if( ((StatementNode*) lastStatement)->myType == FUNCTION_CALL_STATEMENT)
{
getType( ((FunctionCallStatement*)lastStatement)->myStatement, &firstType, symbolTable);
((FunctionCallStatement*)lastStatement)->myStatement = combineLikeTerms( ((FunctionCallStatement*)lastStatement)->myStatement, symbolTable);
}

//if we're down here, then it must be a ReturnStatement
else
{
if(parseDebug)
std::cout << "in MatchTypesLastStatement() reading a return statement right now." << std::endl;

while(travelPointer->symbolType != NON_MEMBER_FUNCTION_DEFINITION && travelPointer->symbolType != MEMBER_FUNCTION_DEFINITION)
{
travelPointer = ((BooleanBlockNode*)travelPointer)->parentBlock;
}

getType( ((ReturnStatement*) lastStatement)->myStatement, &firstType, symbolTable);
if( ((FunctionDefinitionNode*)travelPointer)->returnType->isPrimitive 
&& strcmp(((FunctionDefinitionNode*)travelPointer)->returnType->primitiveName, "void") == 0)
{
if( ((ReturnStatement*) lastStatement)->myStatement->myType == EMPTY_EXPRESSION_NODE)
return;

else
{
std::cout << "Error: Cannot return anything in a void function." << std::endl;
exit(1);
}
}
if(!isCompatable(((FunctionDefinitionNode*)travelPointer)->returnType, firstType))
{
std::cout << "Error: incompatable return types: " << *firstType << ", " << *((FunctionDefinitionNode*)travelPointer)->returnType << std::endl;
exit(1);
}
((ReturnStatement*)lastStatement)->myStatement = combineLikeTerms( ((ReturnStatement*)lastStatement)->myStatement, symbolTable);
}
}
switch(parentType)
{
case GLOBAL_LIST:
         ((GlobalListNode*)currentNode)->replaceLast(lastStatement);
         break;
 case NON_MEMBER_FUNCTION_DEFINITION:
 case MEMBER_FUNCTION_DEFINITION:
         ((FunctionDefinitionNode*) currentNode)->replaceLast(lastStatement);
         break;
 
 case BOOLEAN_BLOCK:
         if( ((BooleanBlockNode*)currentNode)->myType == FOR_BLOCK)
                 ((ForBlockNode*)currentNode)->replaceLastFor(lastStatement);
         else
                 ((BooleanBlockNode*)currentNode)->replaceLast(lastStatement);
         break;
 
default:
std::cout << "Error: Unknown block\n";
exit(1);
 }
 return;

}

//checks for matching types in the last statement added to the current node, and then simplifies the expressions in the statement as much as possible.
//Also, checks if function calls match function definitions in the passed symbol table
void simplifyLast(SymbolTable& symbolTable)
{
if(parseDebug)
std::cout << "Entering simplifyLast()" << std::endl;
abstractType parentType = currentNode->symbolType;
abstractSymbolNode* lastStatement = NULL;
ExpressionNode* leftHalf = NULL;
ExpressionNode* rightHalf = NULL;
VariableNode* myVar = NULL;
tokenType myCompoundedOperator;
MemberVarExpression* myMemberVar = NULL;
CompoundAssignmentStatement* myCompound = NULL;
ExpressionNode* literalExpr = NULL;
tokenType myOperator;
char leftObjectName[1000];
char rightObjectName[1000];
char leftVariableName[1000];
char rightVariableName[1000];
if(currentNode->symbolType != GLOBAL_LIST && currentNode->symbolType != NON_MEMBER_FUNCTION_DEFINITION && currentNode->symbolType != MEMBER_FUNCTION_DEFINITION && currentNode->symbolType != BOOLEAN_BLOCK)
{
std::cout << "Error: When simplifyLast() was called, expected the current node of the parse tree to be a global list, a function definition, or a boolean block. However, the current node was neither of these. Terminating execution now..." << std::endl;
exit(1);
}

MatchTypesLastStatement(symbolTable);

if(currentNode->symbolType == GLOBAL_LIST)
         lastStatement = ((abstractSymbolNode*)((GlobalListNode*)currentNode)->getLastStatement());
 else if(currentNode->symbolType == NON_MEMBER_FUNCTION_DEFINITION || currentNode->symbolType == MEMBER_FUNCTION_DEFINITION)
         lastStatement = ((abstractSymbolNode*)((FunctionDefinitionNode*)currentNode)->getLastStatement());
 
 else
         {
         if(((BooleanBlockNode*) currentNode)->myType == FOR_BLOCK)
                 {
			lastStatement = ((abstractSymbolNode*)((ForBlockNode*)currentNode)->getLastFor());
			}
		else
                 lastStatement = ((abstractSymbolNode*)((BooleanBlockNode*)currentNode)->getLastStatement());
 
         }

//checking for replaceable statement of the form x = x + 1 (which would be replaced with x += 1)
if(lastStatement->symbolType == STATEMENT && ((StatementNode*)lastStatement)->myType == ASSIGNMENT_STATEMENT)
{
	printTreeHelper(lastStatement, 0);
	leftHalf = ((AssignmentStatement*)lastStatement)->leftValue;
	rightHalf = ((AssignmentStatement*)lastStatement)->rightValue;
	if(leftHalf == NULL || rightHalf == NULL)
	{
	std::cout << "Error: Null half of assignment in simplifyLast()\n";
	exit(1);
	}
//	std::cout << "Type of leftHalf: " << leftHalf->myType << std::endl;
	if(leftHalf->myType == VARIABLE_EXPRESSION && rightHalf->myType == BINARY_EXPRESSION)
		{
		strcpy(leftVariableName, ((VariableNode*) leftHalf)->varName);
		if( ((BinaryExpressionNode*)rightHalf)->firstExpression->myType == VARIABLE_EXPRESSION)
			{
				strcpy(rightVariableName, ((VariableNode*)((BinaryExpressionNode*)rightHalf)->firstExpression)->varName);
			}

		else if( ((BinaryExpressionNode*) rightHalf)->secondExpression->myType == VARIABLE_EXPRESSION)
			{
				strcpy(rightVariableName, ((VariableNode*)((BinaryExpressionNode*)rightHalf)->secondExpression)->varName);
			}
		else
			return;

		if(isLiteralNode( ((BinaryExpressionNode*)rightHalf)->firstExpression))
			literalExpr = ((BinaryExpressionNode*)rightHalf)->firstExpression;

		else if(isLiteralNode( ((BinaryExpressionNode*) rightHalf)->secondExpression))
			literalExpr = ((BinaryExpressionNode*)rightHalf)->secondExpression;

		else
			return;

		if(strcmp(leftVariableName, rightVariableName) != 0)
			return;
		}

	else if(leftHalf->myType == MEM_VAR_EXPR && rightHalf->myType == BINARY_EXPRESSION)
		{
		strcpy(leftVariableName, ((MemberVarExpression*) leftHalf)->varName);
		strcpy(leftObjectName, ((MemberVarExpression*) leftHalf)->objectName);
		if( ((BinaryExpressionNode*)rightHalf)->firstExpression->myType == MEM_VAR_EXPR)
			{
				strcpy(rightVariableName, ((MemberVarExpression*)((BinaryExpressionNode*)rightHalf)->firstExpression)->varName);
				strcpy(rightObjectName, ((MemberVarExpression*)((BinaryExpressionNode*)rightHalf)->firstExpression)->objectName);
			}
		else if( ((BinaryExpressionNode*) rightHalf)->secondExpression->myType == MEM_VAR_EXPR)
			{
				strcpy(rightVariableName, ((MemberVarExpression*)((BinaryExpressionNode*)rightHalf)->secondExpression)->varName);
				strcpy(rightObjectName, ((MemberVarExpression*)((BinaryExpressionNode*)rightHalf)->secondExpression)->objectName);
			}
		else
			return;

		if(isLiteralNode( ((BinaryExpressionNode*)rightHalf)->firstExpression))
			literalExpr = ((BinaryExpressionNode*)rightHalf)->firstExpression;

		else if(isLiteralNode( ((BinaryExpressionNode*)rightHalf)->secondExpression))
			literalExpr = ((BinaryExpressionNode*)rightHalf)->secondExpression;
		else
			return;

		if(strcmp(leftVariableName, rightVariableName) != 0 || strcmp(leftObjectName, rightObjectName) != 0)
			return;

		}
	else
		return;
myOperator = ((BinaryExpressionNode*)rightHalf)->myToken;
myCompoundedOperator = getCompounded(myOperator);
if(myOperator == PLUS || myOperator == MULTIPLY)
{
	if(leftHalf->myType == VARIABLE_EXPRESSION)
	{
		myCompound = new CompoundAssignmentStatement(myCompoundedOperator, new VariableNode(leftVariableName), literalExpr);
	}
	else
	{
		myCompound = new CompoundAssignmentStatement(myCompoundedOperator, new MemberVarExpression(leftObjectName, leftVariableName), literalExpr);
	}
}
else if(myOperator == MINUS || myOperator == DIVIDE || myOperator == MOD)
{
	if(isLiteralNode(((BinaryExpressionNode*)rightHalf)->firstExpression))
		return;
	
	if(leftHalf->myType == VARIABLE_EXPRESSION)
		myCompound = new CompoundAssignmentStatement(myCompoundedOperator, new VariableNode(leftVariableName), literalExpr);
	
	else
		myCompound = new CompoundAssignmentStatement(myCompoundedOperator, new MemberVarExpression(leftObjectName, leftVariableName), literalExpr);

}
else
	return;

switch(parentType)
 {
 case GLOBAL_LIST:
          ((GlobalListNode*)currentNode)->replaceLast(myCompound);
          break;
  case NON_MEMBER_FUNCTION_DEFINITION:
  case MEMBER_FUNCTION_DEFINITION:
          ((FunctionDefinitionNode*) currentNode)->replaceLast(myCompound);
          break;
 
  case BOOLEAN_BLOCK:
         if( ((BooleanBlockNode*)currentNode)->myType == FOR_BLOCK)
                  ((ForBlockNode*)currentNode)->replaceLastFor(myCompound);
          else
                  ((BooleanBlockNode*)currentNode)->replaceLast(myCompound);
          break;
  
default:
std::cout << "Error: Unknown block type\n";
exit(1);

  }
  if(parseDebug)
  std::cout << "Finished adding replaced statement (turned assignment statement into compound assignment statement)." << std::endl;
  return;
}

}

private:
abstractSymbolNode* root;
abstractSymbolNode* currentNode;
};

#endif
