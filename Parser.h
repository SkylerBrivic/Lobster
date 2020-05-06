#ifndef PARSER_H_REAL
#define PARSER_H_REAL

#include "ParseTree.h"
#include "SymbolTable.h"
#include "lexicalAnalyzer.h"
#include <vector>
#include <string>
#include "expressionCompare.h"
#include "CodeGenerator.h"

using namespace std;

class Parser
{
public:
bool printBacktracks;
bool printGets;
Parser(string fileName)
: myAnalyzer(fileName)
{
}

bool isAssignOp(tokenType myToken)
{
switch(myToken)
{
case ASSIGNMENT_OPERATOR:
case PLUS_EQUALS:
case MINUS_EQUALS:
case DIVIDE_EQUALS:
case MOD_EQUALS:
case TIMES_EQUALS:
case POW_EQUALS:
case BITWISE_AND_EQUALS:
case BITWISE_OR_EQUALS:
return true;

default:
return false;
}
}

Type* parseType()
{
Type* currentType = NULL;
Type* nextType = NULL;

tokenType myToken;
char typeName[1000];


myToken = myAnalyzer.tokenize(typeName, 1000);
if(printGets)
std::cout << "Just got next token, which was " << typeName << " in parseType()\n\n";
if(myToken == ARRAY || myToken == LIST || myToken == DICT || myToken == QUEUE)
currentType = new Type(myToken, NULL);
else
{

if(!symbolTable.isTypeDefined(typeName))
{
std::cout << "Error: Expected type name. Encountered undefined type of " << typeName << std::endl;
exit(1);
}
return new Type(typeName);
}
myToken = myAnalyzer.tokenize(typeName, 1000);
if(printGets)
std::cout << "Just got next token, which was " << typeName << " in parseType()\n\n";
if(myToken != LESS_THAN)
{
std::cout << "Error: After type of " << getTokenType(currentType->complexType) << " Expected < symbol, instead found " << typeName << " symbol. Format should be Array<int> x = new Array<int>(5);" << std::endl;
exit(1);
}

nextType = parseType();
currentType->setNext(nextType);

myToken = myAnalyzer.tokenize(typeName, 1000);
if(printGets)
std::cout << "Just got next token, which was " << typeName << " in parseType()\n\n";
if(myToken != GREATER_THAN)
{
std::cout << "Error: After parsing type, expected > symbol. Instead encountered " << getTokenType(myToken) << " Example Format: Array<int> x = new Array<int>(5);" << std::endl;
exit(1);
}

return currentType;

}


void printIndented(char* messageString, char* identifierName, int depth)
{
if(! debug)
return;
for(int i = 0; i < depth; ++i)
std::cout << "\t";

std::cout << messageString << "\t" << identifierName << std::endl;
}
bool isLiteral(tokenType myToken)
{
switch(myToken)
{
case INTEGER_LITERAL:
case DOUBLE_LITERAL:
case CHAR_LITERAL:
case STRING_LITERAL:
case TRUE:
case FALSE:
case NULL_WORD:

return true;

default:
return false;
}
}

bool isBinaryOperator(tokenType myToken)
{
switch(myToken)
{
case PLUS:
case MINUS:
case EXPONENT:
case MULTIPLY:
case DIVIDE:
case MOD:
case LOGICAL_AND:
case LOGICAL_OR:
case EQUALS_EQUALS:
case NOT_EQUALS:
case GREATER_THAN:
case LESS_THAN:
case GREATER_THAN_EQUAL:
case LESS_THAN_EQUAL:
case BIT_SHIFT_LEFT:
case BIT_SHIFT_RIGHT:
case BITWISE_AND:
case BITWISE_OR:
case BITWISE_XOR:
return true;

default:
return false;
}
}

bool isUnaryOperator(tokenType myToken)
{
switch(myToken)
{
case PLUS:
case MINUS:
case NOT:
case BITWISE_NOT:
return true;

default:
return false;
}
}

bool isOperator(tokenType myToken)
{
return (isBinaryOperator(myToken) || isUnaryOperator(myToken));
}

//returns an expression node pointer to an ExpressionNode allocated on the heap
//which contains a literal of type myType with the contents of litString
ExpressionNode* createLiteralNode(char* litString, tokenType myType)
{
switch(myType)
{
case INTEGER_LITERAL:
return new IntegerLiteralNode(atoi(litString));

case DOUBLE_LITERAL:
return new DoubleLiteralNode(atof(litString));

case CHAR_LITERAL:
return new CharLiteralNode(litString[0]);

case TRUE:
return new BooleanLiteralNode(true);

case FALSE:
return new BooleanLiteralNode(false);


case STRING_LITERAL:
symbolTable.addStringLit(litString);
return new StringLiteralNode(litString);

case NULL_WORD:
return new NullExpressionNode();

default:
std::cout << "Error: Expected literal value. Instead encountered token " << litString << " with type " << myType << std::endl;
exit(1);
}

return NULL;
}

//starts parsing an if, for, while, else, or elif block right after the closing parenthases has been read.
void parseBlockBody()
{
if(debug)
std::cout << "Entered parseBlockBody()" << std::endl;
char currentToken[1000];
bool manyStatements = false;
StatementNode* myStatement = NULL;
tokenType mostRecent;
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got next token which was " << currentToken << " in parseBlockBody()\n\n";
if(mostRecent == OPENING_BRACE)
	manyStatements = true;
else
	{
	if(printBacktracks)
	std::cout << "IMPORTANT: Backtracked in parseBlockBody() since no opening brace was seen." << std::endl;
	myAnalyzer.backtrack();
	}
while(mostRecent != CLOSING_BRACE)
{
	mostRecent = myAnalyzer.tokenize(currentToken, 1000);
	if(printGets)
	std::cout << "Just got next token which was " << currentToken << " in parseBlockBody()\n\n";
	if(mostRecent == CLOSING_BRACE)
		break;
	else if(mostRecent == SEMI_COLON)
		continue;
	else if(mostRecent == IF)
		parseIf();
	else if(mostRecent == ELSE)
		{
			mostRecent = myAnalyzer.tokenize(currentToken, 1000);
			if(printGets)
			std::cout << "Just got next token which was " << currentToken << " in parseBlockBody()\n\n";
			if(mostRecent == IF)
				parseElif();
			else
				{
				if(printBacktracks)
				std::cout << "IMPORTANT: backtracked in parseBlockBody() since if did not follow else" << std::endl;
				myAnalyzer.backtrack();
				parseElse();
				}
		}
	else if(mostRecent == WHILE)
		parseWhile();

	else if(mostRecent == FOR)
		parseFor();

	else
	{
		if(printBacktracks)
		std::cout << "IMPORTANT: backtracked in parseBlockBody() since the next line is a regular statement, and not an if, while, or for." << std::endl;
		myAnalyzer.backtrack();
		myStatement = ((StatementNode*)parseLine(SEMI_COLON));
		myParseTree.addStatement(myStatement);
		myParseTree.simplifyLast(symbolTable);
	}
	if(!manyStatements)
		break;

}
}

void parseWhile()
{
char currentToken[1000];
tokenType mostRecent;
symbolTable.addBlock();
ExpressionNode* boolExpression = NULL;
WhileBlockNode* myBlock = NULL;
Type* myType = NULL;

mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseWhile()\n\n";
if(mostRecent != OPENING_PARENTHASES)
{
std::cout << "Error: opening parenthases must follow while " << std::endl;
exit(1);
}
boolExpression = parseExpression(true, 0);

mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseWhile()\n\n";
if(mostRecent != CLOSING_PARENTHASES)
{
std::cout << "Error: Expected closing parenthases following boolean condition." << std::endl;
exit(1);
}

myParseTree.getType(boolExpression, &myType, symbolTable);

if(myType->isPrimitive == false || (strcmp(myType->primitiveName, "boolean") != 0 && strcmp(myType->primitiveName, "BOOL_LIT") != 0))
{
std::cout << "Error: Condition in while loop must evaluate to a boolean. " << std::endl;
exit(1);
}

myBlock = new WhileBlockNode(myParseTree.getCurrent());
myBlock->addBoolCondition(boolExpression);
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseWhile()\n\n";
if(mostRecent == OPENING_BRACE)
	myBlock->setOnlyOneStatement(false);
else
	myBlock->setOnlyOneStatement(true);

if(printBacktracks)
std::cout << "IMPORTANT: backtracked in parseWhile() after checking if the while block had an opening brace after it." << std::endl;
myAnalyzer.backtrack();
myParseTree.addBooleanBlock(myBlock);
myParseTree.simplifyLast(symbolTable);
parseBlockBody();
symbolTable.backtrackOutOfBlock();
myParseTree.endBlock();

}

void parseFor()
{
char currentToken[1000];
Type* myType = NULL;
StatementNode* initialStatement = NULL;
ExpressionNode* boolCondition = NULL;
StatementNode* actionStatement = NULL;
tokenType mostRecent;
ForBlockNode* myForBlock = new ForBlockNode(myParseTree.getCurrent());
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseFor()\n\n";
if(mostRecent != OPENING_PARENTHASES)
{
std::cout << "Error: Opening parenthases must follow for " << std::endl;
exit(1);
}

symbolTable.addBlock();
myParseTree.addBooleanBlock(myForBlock);
initialStatement = ((StatementNode*)parseLine(SEMI_COLON));
myForBlock->setInitialStatement(initialStatement);
myParseTree.simplifyLast(symbolTable);
boolCondition = parseExpression(false, 0);
myParseTree.getType(boolCondition, &myType, symbolTable);
if(!myType->isPrimitive || (strcmp(myType->primitiveName, "boolean") != 0 && strcmp(myType->primitiveName, "BOOL_LIT") != 0 ))
{
std::cout << "Error: Boolean condition expected in second statement of for loop declaration." << std::endl;
exit(1);
}

mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseFor()\n\n";
if(mostRecent != SEMI_COLON)
{
std::cout << "Error: Semi-colon must follow boolean condition in for loop." << std::endl;
exit(1);
}
myForBlock->addBoolCondition(boolCondition);
myParseTree.printTreeHelper(boolCondition, 0);
myParseTree.simplifyLast(symbolTable);
actionStatement = ((StatementNode*)parseLine(CLOSING_PARENTHASES));
myForBlock->setActionStatement(actionStatement);
myParseTree.simplifyLast(symbolTable);
if(actionStatement->symbolType == STATEMENT && (((StatementNode*)actionStatement)->myType == DECLARATION_STATEMENT || ((StatementNode*)actionStatement)->myType == DECLARE_ASSIGN_STATEMENT || ((StatementNode*)actionStatement)->myType == RETURN_STATEMENT)     )
{
std::cout << "Error: Declarations of variables and return statements are not allowed in the last statement of a for loop declaration." << std::endl;
exit(1);
}
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got most recent token, which was " << currentToken << " in parseFor()\n\n";
if(mostRecent == OPENING_BRACE)
	myForBlock->setOnlyOneStatement(false);
else
	myForBlock->setOnlyOneStatement(true);
if(printBacktracks)
std::cout << "IMPORTANT: backtracked in parseFor() after checking for opening brace." << std::endl;
myAnalyzer.backtrack();

parseBlockBody();
symbolTable.backtrackOutOfBlock();
myParseTree.endBlock();
}


//starts parsing right after the token if, else if, or else has been read, before adjusting symbol table or parse tree.
void parseIfType(ifType myType)
{
if(debug)
std::cout << "In parseIfType()" << std::endl;
char currentToken[1000];
bool manyStatements = false;
StatementNode* myStatement = NULL;
tokenType mostRecent;
IfTypeBlock* myNode = NULL;
Type* statType = NULL;
symbolTable.addBlock();
if(myType == IF_TYPE)
myNode = new IfNode(myParseTree.getCurrent());
else if(myType == ELIF_TYPE)
myNode = new ElifBlock(myParseTree.getCurrent());
else
myNode = new ElseBlock(myParseTree.getCurrent());

ExpressionNode* boolCondition = NULL;
if(myType != ELSE_TYPE)
{
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseIfType()\n\n";
if(mostRecent != OPENING_PARENTHASES)
	{
	std::cout << "Error: Opening parenthases expected after if. " << std::endl;
	exit(1);
	}
boolCondition = parseExpression(true, 0);
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseIfType()\n\n";
if(mostRecent != CLOSING_PARENTHASES)
	{
	std::cout << "Error: Expected closing parenthases after boolean expression." << std::endl;
	exit(1);
	}

myNode->addBoolCondition(boolCondition);

 myParseTree.getType(boolCondition, &statType, symbolTable);
  
  if(statType->isPrimitive == false || (strcmp(statType->primitiveName, "boolean") != 0 && strcmp(statType->primitiveName, "BOOL_LIT") != 0))
          {
          std::cout << "Error: Boolean expression not found in if statement." << std::endl;
          exit(1);
          }
}
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseIfType()\n\n";
if(mostRecent == OPENING_BRACE)
	myNode->setOnlyOneStatement(false);
else
	myNode->setOnlyOneStatement(true);
if(printBacktracks)
std::cout << "IMPORTANT: backtracked in parseIfType() after checking for opening brace." << std::endl;
myAnalyzer.backtrack();
myParseTree.addBooleanBlock(myNode);
myParseTree.simplifyLast(symbolTable);

parseBlockBody();
symbolTable.backtrackOutOfBlock();
myParseTree.endBlock();
}

void parseIf()
{
parseIfType(IF_TYPE);
}

void parseElif()
{
parseIfType(ELIF_TYPE);
}

void parseElse()
{
parseIfType(ELSE_TYPE);
}

ExpressionNode* parseNewObject(int depth)
{
Type* myType;
tokenType mostRecent;
char currentToken[1000];
myType = parseType();
BuiltinCreationExpression* myBuiltin = NULL;
NewObjectExpression* myNewObject = NULL;
MemberFunctionExpression* myExpression = NULL;

if(myType->isPrimitive == false)
{
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseNewObject()\n\n";
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseNewObject()\n\n";
if(mostRecent == CLOSING_PARENTHASES)
{
myBuiltin = new BuiltinCreationExpression(myType, NULL);
}
else
{
if(printBacktracks)
std::cout << "IMPORTANT: backtracked in parseNewObject() after seeing that new constructor had 1 or more arguments. Calling parseExpression now..." << std::endl;
myAnalyzer.backtrack();
myBuiltin = new BuiltinCreationExpression(myType, parseExpression(true, depth));
myAnalyzer.tokenize(currentToken, 1000);
}
return myBuiltin;
}

else
{
myExpression = new MemberFunctionExpression(myType->primitiveName, myType->primitiveName);
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(mostRecent != CLOSING_PARENTHASES)
{
myAnalyzer.backtrack();
parseParameterExpression(((ExpressionNode**)&myExpression), depth + 1);
}
myNewObject = new NewObjectExpression(myType->primitiveName, myExpression);
return myNewObject;
}

}

//a helper function which takes as input a string representing the next token in the parameter, which is the start of the expression. Stores the entire expression in the pointer whose address is referenced in expressionNodeAddress
//returns the type of the next token after the end of the expression was encountered, which should be either a closing parenthases
tokenType parseParameterExpression(ExpressionNode** expressionNodeAddress, int depth)
{
if(debug)
std::cout << "Entered parseParameterExpression()" << std::endl;
printIndented((char*)"Entered parseParameterExpression for function ", ((FunctionExpression*)(*expressionNodeAddress))->funcName, depth);

char temp[1000];
tokenType returnType;

returnType = myAnalyzer.tokenize(temp, 1000);
if(printGets)
std::cout << "Just got last token, which was " << temp << " in parseParameterExpression()\n\n";
if(returnType == CLOSING_PARENTHASES)
return returnType;

if(printBacktracks)
std::cout << "IMPORTANT: backtracked in parseParameterExpression() after seeing that closing parenthases did not immediately follow opening parenthases." << std::endl;
myAnalyzer.backtrack();

while(returnType != CLOSING_PARENTHASES)
{
((FunctionExpression*)(*expressionNodeAddress))->addArgument(parseExpression(true, depth + 1));
returnType = myAnalyzer.tokenize(temp, 1000);
if(printGets)
std::cout << "Just got last token, which was " << temp << " in parseParameterExpression()\n\n";
if(returnType != COMMA && returnType != CLOSING_PARENTHASES)
{
std::cout << "ABOUT TO EXIT LINE 561 PARSER.h\n";
std::cout << "Last token was " << temp << std::endl;
printIndented((char*)"Error: Expected comma or closing parenthases. Instead, encountered token ", temp, depth);
exit(1);
}
}

return returnType;
}

//TODO: DEBUGGED THROUGH HERE!

//a helper function of parseExpression. Takes as input a char* representing an identifier name
//also, takes as input the address of a pointer to an ExpressionNode
//this function sets the pointer to point to the expressionNode that corresponds to having
//evaluated this expression
//tokenType returned by this function is the next token in the expression. This should be either an operator, a ), or a ;
//this function is used on variable names, which could be either non member fucntion calls, a variable identifier, a member function call, or a member variable access
tokenType parseExpressionIdentifier(char* identifierName, ExpressionNode** expressionNodeAddress, int depth)
{
printIndented((char*) "Entered parseExpressionIdentifier()", (char*)"", depth);
tokenType dotOrParen, memberType, parenOrOperator;
char memberName[1000];
char temp[1000];
tokenType returnType;

dotOrParen = myAnalyzer.tokenize(temp, 1000);
if(printGets)
std::cout << "Just got last token, which was " << temp << " in parseExpressionIdentifier()\n\n";
//if the next token was ;, ), or an operator, then the first identifier must have been a variable access
//thus, expressionNodeAddress is set to refer to an identifierNode (a non-member variable access),
//and the last token type encountered is returned.
if(dotOrParen == SEMI_COLON || dotOrParen == CLOSING_PARENTHASES || isOperator(dotOrParen) || dotOrParen == COMMA || dotOrParen == ASSIGNMENT_OPERATOR
|| dotOrParen == PLUS_EQUALS || dotOrParen == MINUS_EQUALS || dotOrParen == PLUS_PLUS || dotOrParen == MINUS_MINUS ||
  dotOrParen == TIMES_EQUALS || dotOrParen == DIVIDE_EQUALS || dotOrParen == MOD_EQUALS
|| dotOrParen == POW_EQUALS || dotOrParen == BITWISE_AND_EQUALS || dotOrParen == BITWISE_OR_EQUALS)
{
printIndented((char*)"Semi colon or closing parenthases or operator encountered. Returning new variable node for ", identifierName, depth);
returnType = dotOrParen;
*expressionNodeAddress = new VariableNode(identifierName);
myAnalyzer.backtrack();
return returnType;
}

//if the next token was (, then this is a non-member function call.
else if(dotOrParen == OPENING_PARENTHASES)
{
if(symbolTable.getFunction(identifierName) != NULL && symbolTable.getFunction(identifierName)->symbolType == MEMBER_FUNCTION)
*expressionNodeAddress = new MemberFunctionExpression((char*)"this", identifierName);
else
*expressionNodeAddress = new NonMemberFunctionExpression(identifierName);
//down below, both member functions and non member functions will have parsing of their parameters handled together
}

//in this case, we have either a member variable access or a member function call
else if(dotOrParen == DOT_OPERATOR)
{
printIndented((char*)"Getting next token...", (char*)"", depth);
memberType = myAnalyzer.tokenize(memberName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << memberName << " in parseExpressionIdentifier()\n\n";
if(memberType != IDENTIFIER)
{
std::cout << "ABOUT TO EXIT PARSER.H LINE 623\n";
printIndented((char*)"Error: Identifier required after . operator. Instead, encountered token ", memberName, depth);
exit(1);
}

printIndented((char*)"Getting next token...", (char*)"", depth);
parenOrOperator = myAnalyzer.tokenize(temp, 1000);
if(printGets)
std::cout << "Just got last token, which was " << temp << " in parseExpressionIdentifier()\n\n";
//in this case, we have a member function call
if(parenOrOperator == OPENING_PARENTHASES)
{
printIndented((char*)"Opening parenthases encountered... Parsing member function call.", (char*)"", depth);
Type* myVarType = symbolTable.getTypeOfVariable(identifierName);

if(myVarType->isPrimitive == false)
*expressionNodeAddress = new BuiltinMemberFunctionExpression(identifierName, memberName);

else
*expressionNodeAddress = new MemberFunctionExpression(identifierName, memberName);
//the code for parsing parameters is handled down below along with non member function parameter parsing
}

//otherwise, we have a member variable call, and can create the appropriate node and return the next token
else
{
printIndented((char*)"Encountered member variable access. Returning MemberVarExpression now...", (char*)"", depth);
*expressionNodeAddress = new MemberVarExpression(identifierName, memberName);
returnType = parenOrOperator;
myAnalyzer.backtrack();
return returnType;
}
}

tokenType lookType;
char lastLookahead[1000];

//finally, we are now ready to handle parsing of function arguments. The last token processed at this
//point in the code was (
if(debug)
std::cout << "About to enter parseParameterExpression from parseExpressionIdentifier" << std::endl;
lookType = parseParameterExpression(expressionNodeAddress, depth + 1);
if(debug)
std::cout << "Exited parseParameterExpression. Returning from parseExpressionIdentifier now." << std::endl;

return lookType;
}

//takes as input an expression representing the first part of a binary expression
//parses the next term, combines it appropriately, and then recursively continues in this fashion down the expression, eventually hitting the end of the expression and returning it.
ExpressionNode* singleTermParse(ExpressionNode* firstExpression, tokenType firstOperator, int depth, bool hasParenthases)
{
ExpressionNode* secondExpression = unaryParse(depth, hasParenthases);
ExpressionNode* thirdExpression = NULL;
ExpressionNode* returnedHalf = NULL;
expressionCompare myComparator;
char identifierName[1000];
tokenType thirdOperator;
tokenType secondOperator;
tokenType myToken = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in singleTermParse()\n\n";
if(myToken == SEMI_COLON || myToken == COMMA || (hasParenthases && myToken == CLOSING_PARENTHASES))
{
myAnalyzer.backtrack();
return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
}
if(!isOperator(myToken))
{
std::cout << "ABOUT TO EXIT PARSER.H line 692\n";
printIndented((char*)"Error: Expected operator, encountered token ", identifierName, depth);
exit(1);
}

secondOperator = myToken;
thirdExpression = unaryParse(depth, hasParenthases);
myToken = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in singleTermParse()\n\n";
if(myToken == SEMI_COLON || myToken == COMMA || (hasParenthases && myToken == CLOSING_PARENTHASES))
{
myAnalyzer.backtrack();
if(myComparator.compareTo(firstOperator, secondOperator) <= 0)
return new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression);

else
return new BinaryExpressionNode(firstOperator, firstExpression, new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression));
}

thirdOperator = myToken;

//now handling the case where there is a 4th (or more) terms left.
// X + 3 * 6 ^ 2
//note: at this point would have just parsed the symbol ^, not 2.

//this is divided up into 11 cases. To start, with, we check for the case where all 3 operators have the same prescedence.
//ex. X + Y + Z + ...

//CASE 1:
if(myComparator.compareTo(firstOperator, secondOperator) == 0 && myComparator.compareTo(secondOperator, thirdOperator) == 0)
{
firstExpression = new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}

//Case where first and last operator have same prescedence, and first operator is higher prescedence than the second operator.
//ex. X ^ Y * Z ^ ...
//(Adding these two cases into here because I forgot to put them in earlier)

//ALT_CASE_A:
if(myComparator.compareTo(firstOperator, thirdOperator) == 0 
&& myComparator.compareTo(firstOperator, secondOperator) < 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
secondExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(secondOperator, firstExpression, secondExpression);
}

//case where the first and last operator have the same prescedence, and the first operator is lower prescedence than the second operator.
//ex. X * Y ^ Z * ...

//ALT_CASE_B:
if(myComparator.compareTo(firstOperator, thirdOperator) == 0
&& myComparator.compareTo(firstOperator, secondOperator) > 0)
{
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}


if(myComparator.compareTo(firstOperator, secondOperator) == 0)
{
//Now handling the case where the first 2 operators have the same prescedence, and the third operator has lower prescdence than the 
//first two.
 //ex. X ^ Y ^ Z * ...
 //CASE 2:
if(myComparator.compareTo(secondOperator, thirdOperator) < 0)
{
firstExpression = new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}

//now handling the case where the first 2 operators have the same prescedence, and the third operator has higher prescedence than the first two.
//ex. X * Y * Z ^ ...
//CASE 3:
else
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
secondExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(secondOperator, firstExpression, secondExpression);
}
}

if(myComparator.compareTo(secondOperator, thirdOperator) == 0)
{

//handling the case where the last 2 operators have the same prescedence, and the first operator has lower prescedence than the other two.
//ex. X * Y ^ Z ^ ...
//CASE 4:
if(myComparator.compareTo(firstOperator, secondOperator) > 0)
{
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
secondExpression = singleTermParse(secondExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
}

//now handling the case where the last 2 operators have the same prescedence and the first operator has higher prescedence than the other two.
//ex. X ^ Y * Z *
//CASE 5:
else
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
firstExpression = new BinaryExpressionNode(secondOperator, firstExpression, thirdExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}
}

//now handling the case where the first operator has the highest prescedence, the second operator has a middle-level of prescedence, and the third operator has the lowest level of prescedence.
//ex. X ^ Y * Z + ...
//CASE 6:
if(myComparator.compareTo(firstOperator, secondOperator) < 0 && myComparator.compareTo(secondOperator, thirdOperator) < 0 && myComparator.compareTo(firstOperator, thirdOperator) < 0)
{
return singleTermParse(new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression), thirdOperator, depth + 1, hasParenthases);
}

//now handling the case where the first operator has the highest prescedence, the second operator has the lowest prescedence, and the third operator has a middle-level of prescedence.
//ex. X ^ Y + Z * ...
//CASE 7:
if(myComparator.compareTo(firstOperator, secondOperator) < 0 && myComparator.compareTo(secondOperator, thirdOperator) > 0 && myComparator.compareTo(firstOperator, thirdOperator) < 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
return new BinaryExpressionNode(secondOperator, firstExpression, singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases));
}

//now handling the case where the first operator has a middle level of prescedence, the second operator has the highest prescedence, and the thid operator has the lowest prescedence.
//ex. X * Y ^ Z + ...
//CASE 8:
if(myComparator.compareTo(firstOperator, secondOperator) > 0 && myComparator.compareTo(secondOperator, thirdOperator) < 0 && myComparator.compareTo(firstOperator, thirdOperator) < 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression));
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}


//now handling the case where the first operator has a middle level of prescedence, the second operator has the lowest prescedence, and the third operator has the highest prescedence.
//ex. X * Y + Z ^ ...
//CASE 9:
if(myComparator.compareTo(firstOperator, secondOperator) < 0 && myComparator.compareTo(secondOperator, thirdOperator) > 0 && myComparator.compareTo(firstOperator, thirdOperator) > 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
secondExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(secondOperator, firstExpression, secondExpression);
}

//now handling the case where the first operator has the lowest level of prescedence, the second operator has the highest level of prescedence, and the third operator has a middle level of prescedence.
//ex. X + Y ^ Z * ...
//CASE 10:
if(myComparator.compareTo(firstOperator, secondOperator) > 0 && myComparator.compareTo(secondOperator, thirdOperator) < 0 && myComparator.compareTo(firstOperator, thirdOperator) > 0)
{
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
secondExpression = singleTermParse(secondExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
}

//if we made it down here, then this must be case 11 (the last case). In this case, the first operator has the lowest level of prescedence, the second operator has a middle level of prescedence, and the third operator has the highest level of prescedence.
//ex. X + Y * Z ^ ...
//CASE 11:
thirdExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
}


//starts processing after the token "print" has been read. Returns the full print function expression
ExpressionNode* parsePrint()
{
tokenType tokType;
char tempToken[1000];
tokType = myAnalyzer.tokenize(tempToken, 1000);

if(tokType != OPENING_PARENTHASES)
{
std::cout << "Error: print token must be followed by opening parenthases\n";
exit(1);
}

ExpressionNode* printExpr = new PrintExpression();
parseParameterExpression(&printExpr, 0);
return printExpr;
}

//starts processing after the token "getInt" has been read. Returns the full getInt() expression
ExpressionNode* parseGetInt()
{
return new getIntExpression(); 
}

//starts processing after the token "getDouble" has been read. Returns the full getDouble() expression.
ExpressionNode* parseGetDouble()
{
return new getDoubleExpression();
}

//starts processing after the token "getBoolean" has been read. Returns the full "getBoolean()" expression.
ExpressionNode* parseGetBoolean()
{
return new getBooleanExpression();
}

//starts processing after the token "getChar" has been read. Returns the full "getChar()" expression
ExpressionNode* parseGetChar()
{
return new getCharExpression();
}

//starts processing after the token "getWord" has been read. Returns the full "getWord()" expression
ExpressionNode* parseGetWord()
{
return new getWordExpression();
}

//starts processing after the token "getLine" has been read. Returns the full "getLine()" expression
ExpressionNode* parseGetLine()
{
return new getLineExpression();
}

//starts processing after the token "isInputFail" has been read. Returns the full "isInputFail()" expression
ExpressionNode* parseGetInputFail()
{
return new isInputFailFunctionExpression();
}

//GET_INT_EXPR, GET_DOUBLE_EXPR, GET_BOOLEAN_EXPR, GET_CHAR_EXPR, GET_WORD_EXPR, GET_LINE_EXPR, IS_INPUT_FAIL_FUNC_EXPR
ExpressionNode* inputParseFunctions(tokenType myMainToken)
{
char tempToken[1000];
tokenType tokType;

tokType = myAnalyzer.tokenize(tempToken, 1000);
if(tokType != OPENING_PARENTHASES)
{
std::cout << "Error: " << getTokenType(myMainToken) << " must be followed by an opening parenthases\n";
exit(1);
}
tokType = myAnalyzer.tokenize(tempToken, 1000);
if(tokType != CLOSING_PARENTHASES)
{
std::cout << "Error: " << getTokenType(myMainToken) << "( must be followed by a closing parenthases\n";
exit(1);
}


switch(myMainToken)
{
case GET_INT:
return parseGetInt();
break;

case GET_DOUBLE:
return parseGetDouble();
break;

case GET_BOOLEAN:
return parseGetBoolean();
break;

case GET_CHAR:
return parseGetChar();
break;

case GET_WORD:
return parseGetWord();
break;

case GET_LINE:
return parseGetLine();
break;

case IS_INPUT_FAIL:
return parseGetInputFail();
break;

default:
std::cout << "Error: token of " << getTokenType(myMainToken) << " in inputParseFunctions() encountered\n";
exit(1);
break;



}

return NULL;
}

bool isInputParseToken(tokenType myToken)
{

switch(myToken)
{
case GET_INT:
case GET_DOUBLE:
case GET_BOOLEAN:
case GET_CHAR:
case GET_WORD:
case GET_LINE:
case IS_INPUT_FAIL:
return true;

default:
return false;
}


}

//handles parsing of unary expressions (will try to bind the unary operator as close to a term as possible.
// ex. 4 + -3 + 8 would incorrectly return 4 + -(3 + 8) w/o this function being called.
//with this function, the correct expression of 4 + -(3) + 8 will be returned.
ExpressionNode* unaryParse(int depth, bool hasParenthases)
{
if(debug)
std::cout << "Now in unaryParse()" << std::endl;
char identifierName[1000];
tokenType tempToken;
ExpressionNode* myExpression = NULL;

tempToken = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in unaryParse()\n\n";
if(tempToken == IDENTIFIER || tempToken == THIS)
{
if(debug)
std::cout << "read the token " << identifierName << " in unaryParse(), and now entering parseExpressionIdentifier()..." << std::endl;
tempToken = parseExpressionIdentifier(identifierName, &myExpression, depth + 1);
return myExpression;
}

else if(isUnaryOperator(tempToken))
return new UnaryOperationNode(tempToken, unaryParse(depth, hasParenthases));

else if(isLiteral(tempToken))
return createLiteralNode(identifierName, tempToken);

else if(tempToken == OPENING_PARENTHASES)
{
myExpression = parseExpression(true, depth + 1);
myAnalyzer.tokenize(identifierName, 1000);
return myExpression;
}

else if(isInputParseToken(tempToken))
{
myExpression = inputParseFunctions(tempToken);
return myExpression;
}

else if(tempToken == PRINT)
{
myExpression = parsePrint();
return myExpression;
}

else
return NULL;
}

//the main entry point for processing expressions
ExpressionNode* parseExpression(bool hasParenthases, int depth)
{
if(hasParenthases)
printIndented((char*)"Entered parseExpression() with hasParenthases of true", (char*)"", depth);
else
printIndented((char*)"Entered parseExpression() with hasParenthases of false", (char*)"", depth);
ExpressionNode* firstExpression = NULL;
ExpressionNode* secondExpression = NULL;
ExpressionNode* thirdExpression = NULL;
ExpressionNode* returnedHalf = NULL;
expressionCompare myComparator;
firstExpression = secondExpression = thirdExpression = returnedHalf = NULL;
tokenType tempToken;
char identifierName[1000];
tokenType firstOperator, secondOperator, thirdOperator, endSymbol;

tempToken = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
if(tempToken == OPENING_PARENTHASES)
	{
	printIndented((char*)"Hit an open parenthases...", (char*)"", depth);
	firstExpression = parseExpression(true, depth + 1);
	printIndented((char*)"Getting next token...", (char*)"", depth);
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
	}

else if(tempToken == CLOSING_PARENTHASES && hasParenthases)
	{
	myAnalyzer.backtrack();
	printIndented((char*)"Hit a closing parenthases... returning empty expression node",(char*) "", depth);
	return new EmptyExpressionNode();
	}
else if(tempToken == SEMI_COLON)
	{
	myAnalyzer.backtrack();
	printIndented((char*)"Hit a semicolon... returning empty expression node", (char*)"", depth);
	return new EmptyExpressionNode();
	}
else if(tempToken == IDENTIFIER || tempToken == THIS)
	{
	printIndented((char*)"identifier encountered.", (char*)"", depth);
	parseExpressionIdentifier(identifierName, &firstExpression, depth + 1);
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	}
else if(isLiteral(tempToken))
	{
	printIndented((char*)"Parsed the literal", identifierName, depth);
	firstExpression = createLiteralNode(identifierName, tempToken);
	printIndented((char*)"Getting next token...", (char*)"", depth);
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
	}

else if(tempToken == NEW)
{
	printIndented((char*)"Parsing new object ", (char*)"", depth);
	return parseNewObject(depth + 1);

}

else if(tempToken == PLUS_PLUS)
	{
	printIndented((char*)"Hit ++. Returning expression now.", (char*)"", depth);
	firstExpression = new PlusPlusNode(unaryParse(depth + 1, hasParenthases));
	return firstExpression;
	}

else if(tempToken == MINUS_MINUS)
	{
	printIndented((char*)"Hit --. Returning expression now.", (char*)"", depth);
	firstExpression = new MinusMinusNode(unaryParse(depth + 1, hasParenthases));
	return firstExpression;
	}

else if(isUnaryOperator(tempToken))
	{
	printIndented((char*)"Unary operator encountered. Calculating rest of expression...", (char*)"", depth);
	firstExpression = new UnaryOperationNode(tempToken, unaryParse(depth + 1, hasParenthases));
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
	}

else if(isInputParseToken(tempToken))
{
firstExpression = inputParseFunctions(tempToken);
tempToken = myAnalyzer.tokenize(identifierName, 1000);
}

else if(tempToken == PRINT)
{
firstExpression = parsePrint();
tempToken = myAnalyzer.tokenize(identifierName, 1000);
}

 if(tempToken == SEMI_COLON || (tempToken == CLOSING_PARENTHASES && hasParenthases) || tempToken == COMMA
	|| isAssignOp(tempToken))
	{
	myAnalyzer.backtrack();
	printIndented((char*)"Hit assignment op or semi colon or closing parenthases or comma. Returning first expression now.", (char*)"", depth);
	return firstExpression;
	}

 if(tempToken == PLUS_PLUS)
	{
	printIndented((char*)"Hit ++. Returning expression now.", (char*)"", depth);
	return new PlusPlusNode(firstExpression);
	}

 if(tempToken == MINUS_MINUS)
	{
	printIndented((char*)"Hit --. Returning expression now.", (char*)"", depth);
	return new MinusMinusNode(firstExpression);
	}

 if(!isOperator(tempToken))
{
std::cout << "ABOUT TO EXIT PARSER.H LINE 1006\n";
std::cout << "Error: Operator token expected. Instead, encountered token: " << identifierName << std::endl;
exit(1);
}

firstOperator = tempToken;

tempToken = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
if(tempToken == OPENING_PARENTHASES)
	{
	printIndented((char*)"Hit an open parenthases...", (char*)"", depth);
	secondExpression = parseExpression(true, depth + 1);
	printIndented((char*)"Getting next token...", (char*)"", depth);
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
	}

else if(tempToken == IDENTIFIER || tempToken == THIS)
	{
	printIndented((char*)"Identifier encountered...", (char*)"", depth);
	parseExpressionIdentifier(identifierName, &secondExpression, depth + 1);
	//std::cout << "Just returned from parseExpressionIdentifier() into parseExpression(), with returned token of " << getTokenType(tempToken) << std::endl;
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	}

else if(isLiteral(tempToken))
	{
	printIndented((char*)"Encountered Literal", identifierName, depth);
	secondExpression = createLiteralNode(identifierName, tempToken);
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
	}

else if(isUnaryOperator(tempToken))
	{
	printIndented((char*)"Encountered unary operator", (char*)"", depth);
	secondExpression = new UnaryOperationNode(tempToken, unaryParse(depth + 1, hasParenthases));
	tempToken = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseExpression()\n\n";
	}

else if(isInputParseToken(tempToken))
{
secondExpression = inputParseFunctions(tempToken);
tempToken = myAnalyzer.tokenize(identifierName, 1000);
}

else if(tempToken == PRINT)
{
secondExpression = parsePrint();
tempToken = myAnalyzer.tokenize(identifierName, 1000);
}

if(tempToken == SEMI_COLON || tempToken == COMMA || (tempToken == CLOSING_PARENTHASES && hasParenthases)
|| isAssignOp(tempToken))
	{
	myAnalyzer.backtrack();
	printIndented((char*)"Encountered semi colon or comma or closing parenthases. Returning binary expresison now.", (char*)"", depth);
	return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
	}

if(!isOperator(tempToken))
{
std::cout << "ABOUT TO EXIT PARSER.H LINE 1065 and hasParenthases = " << hasParenthases << "\n";
int* x = NULL;
std::cout << *x;
printIndented((char*)"Error: Operator expected. Instead, encountered token ", identifierName, depth);
exit(1);
}

secondOperator = tempToken;
thirdExpression = unaryParse(depth + 1, hasParenthases);
thirdOperator = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got next token, which was " << identifierName << " in parseExpression()\n\n";
if(thirdOperator == SEMI_COLON || thirdOperator == COMMA || (thirdOperator == CLOSING_PARENTHASES && hasParenthases))
{
myAnalyzer.backtrack();
if(myComparator.compareTo(firstOperator, secondOperator) <= 0)
return new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression);
else
return new BinaryExpressionNode(firstOperator, firstExpression, new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression));
}


//now handling the case where there is a 4th (or more) terms left.
// X + 3 * 6 ^ 2
//note: at this point would have just parsed the symbol ^, not 2.

//this is divided up into 11 cases. To start, with, we check for the case where all 3 operators have the same prescedence.
//ex. X + Y + Z + ...

//CASE 1:
if(myComparator.compareTo(firstOperator, secondOperator) == 0 && myComparator.compareTo(secondOperator, thirdOperator) == 0)
{
firstExpression = new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}

//Case where first and last operator have same prescedence, and first operator is higher prescedence than the second operator.
//ex. X ^ Y * Z ^ ...
//(Adding these two cases into here because I forgot to put them in earlier)

//ALT_CASE_A:
if(myComparator.compareTo(firstOperator, thirdOperator) == 0
&& myComparator.compareTo(firstOperator, secondOperator) < 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
secondExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(secondOperator, firstExpression, secondExpression);
}

//case where the first and last operator have the same prescedence, and the first operator is lower prescedence than the second operator.
//ex. X * Y ^ Z * ...

//ALT_CASE_B:
if(myComparator.compareTo(firstOperator, thirdOperator) == 0
&& myComparator.compareTo(firstOperator, secondOperator) > 0)
{
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}



if(myComparator.compareTo(firstOperator, secondOperator) == 0)
{
//Now handling the case where the first 2 operators have the same prescedence, and the third operator has lower prescdence than the 
//first two.
 //ex. X ^ Y ^ Z * ...
 //CASE 2:
if(myComparator.compareTo(secondOperator, thirdOperator) < 0)
{
firstExpression = new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}

//now handling the case where the first 2 operators have the same prescedence, and the third operator has higher prescedence than the first two.
//ex. X * Y * Z ^ ...
//CASE 3:
else
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
secondExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(secondOperator, firstExpression, secondExpression);
}
}

if(myComparator.compareTo(secondOperator, thirdOperator) == 0)
{

//handling the case where the last 2 operators have the same prescedence, and the first operator has lower prescedence than the other two.
//ex. X * Y ^ Z ^ ...
//CASE 4:
if(myComparator.compareTo(firstOperator, secondOperator) > 0)
{
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
secondExpression = singleTermParse(secondExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
}

//now handling the case where the last 2 operators have the same prescedence and the first operator has higher prescedence than the other two.
//ex. X ^ Y * Z *
//CASE 5:
else
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
firstExpression = new BinaryExpressionNode(secondOperator, firstExpression, thirdExpression);
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}
}

//now handling the case where the first operator has the highest prescedence, the second operator has a middle-level of prescedence, and the third operator has the lowest level of prescedence.
//ex. X ^ Y * Z + ...
//CASE 6:
if(myComparator.compareTo(firstOperator, secondOperator) < 0 && myComparator.compareTo(secondOperator, thirdOperator) < 0 && myComparator.compareTo(firstOperator, thirdOperator) < 0)
{
return singleTermParse(new BinaryExpressionNode(secondOperator, new BinaryExpressionNode(firstOperator, firstExpression, secondExpression), thirdExpression), thirdOperator, depth + 1, hasParenthases);
}

//now handling the case where the first operator has the highest prescedence, the second operator has the lowest prescedence, and the third operator has a middle-level of prescedence.
//ex. X ^ Y + Z * ...
//CASE 7:
if(myComparator.compareTo(firstOperator, secondOperator) < 0 && myComparator.compareTo(secondOperator, thirdOperator) > 0 && myComparator.compareTo(firstOperator, thirdOperator) < 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
return new BinaryExpressionNode(secondOperator, firstExpression, singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases));
}

//now handling the case where the first operator has a middle level of prescedence, the second operator has the highest prescedence, and the thid operator has the lowest prescedence.
//ex. X * Y ^ Z + ...
//CASE 8:
if(myComparator.compareTo(firstOperator, secondOperator) > 0 && myComparator.compareTo(secondOperator, thirdOperator) < 0 && myComparator.compareTo(firstOperator, thirdOperator) < 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression));
return singleTermParse(firstExpression, thirdOperator, depth + 1, hasParenthases);
}


//now handling the case where the first operator has a middle level of prescedence, the second operator has the lowest prescedence, and the third operator has the highest prescedence.
//ex. X * Y + Z ^ ...
//CASE 9:
if(myComparator.compareTo(firstOperator, secondOperator) < 0 && myComparator.compareTo(secondOperator, thirdOperator) > 0 && myComparator.compareTo(firstOperator, thirdOperator) > 0)
{
firstExpression = new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
secondExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(secondOperator, firstExpression, secondExpression);
}

//now handling the case where the first operator has the lowest level of prescedence, the second operator has the highest level of prescedence, and the third operator has a middle level of prescedence.
//ex. X + Y ^ Z * ...
//CASE 10:
if(myComparator.compareTo(firstOperator, secondOperator) > 0 && myComparator.compareTo(secondOperator, thirdOperator) < 0 && myComparator.compareTo(firstOperator, thirdOperator) > 0)
{
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
secondExpression = singleTermParse(secondExpression, thirdOperator, depth + 1, hasParenthases);
return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
}

//if we made it down here, then this must be case 11 (the last case). In this case, the first operator has the lowest level of prescedence, the second operator has a middle level of prescedence, and the third operator has the highest level of prescedence.
//ex. X + Y * Z ^ ...
//CASE 11:
thirdExpression = singleTermParse(thirdExpression, thirdOperator, depth + 1, hasParenthases);
secondExpression = new BinaryExpressionNode(secondOperator, secondExpression, thirdExpression);
return new BinaryExpressionNode(firstOperator, firstExpression, secondExpression);
}


bool debug;
void parseGlobalDeclaration(Type* VarType, char* IdentifierName)
{
if(symbolTable.isVariable(IdentifierName))
{
std::cout << "Error: Conflicting global redefinition of variable " << IdentifierName << " of type " << VarType << std::endl;
exit(1);
}

myParseTree.addDeclaration(VarType, IdentifierName);
symbolTable.addVariable(VarType, IdentifierName);
return;

}

void parseGlobalDeclareAssign(Type* nameOfVarType, char* IdentifierName)
{
if(symbolTable.isVariable(IdentifierName))
{
std::cout << "Error: Conflicting global redefinition of variable " << IdentifierName << " of type " << nameOfVarType << std::endl;
exit(1);
}

ExpressionNode* secondHalf = parseExpression(false, 0);
symbolTable.addVariable(nameOfVarType, IdentifierName);
myParseTree.addDeclareAssign(nameOfVarType, IdentifierName, secondHalf);
}

void parseGlobalAssignment(ExpressionNode* IdentifierExpression)
{
ExpressionNode* secondHalf = parseExpression(false, 0);
myParseTree.addAssignment(IdentifierExpression, secondHalf);
}

void parseClass()
{
tokenType currentToken;
char className[1000];
char tokenContent[1000];
char returnType[1000];
char identifierName[1000];
Type* typeVar = NULL;
currentToken = myAnalyzer.tokenize(className, 1000);
if(printGets)
std::cout << "Just got next token, which was " << className << " in parseClass()\n\n";
if(currentToken != IDENTIFIER)
{
std::cout << "Error: Identifier expected after keyword class. Instead, got token " << className << std::endl;
exit(1);
}

if(debug)
std::cout << "Adding class definition to symbol table and parse tree." << std::endl;
myParseTree.addClassDefinition(className);

if(symbolTable.isTypeDefined(className))
{
std::cout << "Error: Duplicate definition of class " << className << std::endl;
exit(1);
}

symbolTable.addClass(className);

//after this point, the class has been added to the parse tree and symbol table.
//now, removing the { curly brace, and then entering a while loop to finish parsing the class

currentToken = myAnalyzer.tokenize(tokenContent, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tokenContent << " in parseClass()\n\n";
if(debug)
std::cout << "Finished adding class definition to symbol table and parse tree." << std::endl;
if(currentToken != OPENING_BRACE)
{
std::cout << "Error: Opening brace expected after start of class definition. Instead, encountered " << tokenContent << std::endl;
exit(1);
}

//now that the brace is gone, we are ready to enter a while loop.
//we will make repeated calls to the functions parseMemberVariable()
//and parseFunction() until the closing brace at the end of the class is encountered

while(currentToken != CLOSING_BRACE)
{
currentToken = myAnalyzer.tokenize(returnType, 1000);
if(printGets)
std::cout << "Just got last token, which was " << returnType << " in parseClass()\n\n";
if(currentToken == CLOSING_BRACE)
break;

if(!symbolTable.isTypeDefined(returnType))
{
std::cout << "Error: Primitive type name or class name expected. Instead, encountered undefined token: " << returnType << std::endl;
exit(1);

}
if(printBacktracks)
std::cout << "IMPORTANT: backtracking in parseClass() after seeing that type of next symbol was defined." << std::endl;
myAnalyzer.backtrack();
typeVar = parseType();

if(debug)
std::cout << "Encountered the type known as " << returnType << std::endl;

currentToken = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in parseClass()\n\n";
if(currentToken != IDENTIFIER)
{
std::cout << "Error: Expected identifier. Instead, encountered token " << identifierName << std::endl;
exit(1);
}

if(debug)
std::cout << "Encountered the identifier " << identifierName << std::endl;
currentToken = myAnalyzer.tokenize(tokenContent, 1000);
if(printGets)
std::cout << "Just got the last token, which was " << tokenContent << " in parseClass()\n\n";
//handling the case where we have a member variable definition
if(currentToken == SEMI_COLON)
{
if(debug)
std::cout << "Parsing member variable declaration now." << std::endl;
if(symbolTable.isDuplicateMemberVar(identifierName))
{
std::cout << "Error: Redefinition of member variable " << identifierName << std::endl;
exit(1);
}
if(typeVar->isPrimitive && strcmp(typeVar->primitiveName, "void") == 0)
	{
	std::cout << "Error: Member variable cannot have type void. Terminating execution now." << std::endl;
	exit(1);
	}
symbolTable.addVariable(typeVar, identifierName);
myParseTree.addMemberVariable(typeVar, identifierName);
if(debug)
std::cout << "Added member variable " << identifierName << " of type " << returnType << " to the above mentioned class " << std::endl;

}

//handling the case of a member function definition
else if(currentToken == OPENING_PARENTHASES)
{
if(debug)
std::cout << "Adding member function with name " << identifierName << " and return type " << returnType << " to the symbol table. " << std::endl;
symbolTable.addFunction(typeVar, identifierName);
myParseTree.addMemberFunction(typeVar, identifierName);
if(debug)
std::cout << "Added member function to symbol table and parse tree. Parsing function body now..." << std::endl;
parseFunction(typeVar, identifierName, true);
if(debug)
std::cout << "Finished parsing member function body" << std::endl;
symbolTable.backtrackOutOfFunction();
myParseTree.endMemberFunctionDefinition();
}

else if(currentToken == CLOSING_BRACE)
break;

else
{
std::cout << "Error: Expected semicolon or open parentheses. Encountered symbol " << tokenContent << std::endl;
exit(1);
}



}

myParseTree.endClassDefinition();
symbolTable.backtrackOutOfClass();

}

/*
void parseMemberFunction(Type* returnType, char* identifierName)
{
parseFunction(returnType, identifierName, true);
myParseTree.endMemberFunctionDefinition();
symbolTable.backtrackOne();
}


void parseNonMemberFunction(Type* returnType, char* identifierName)
{
parseFunction(returnType, identifierName, false);
myParseTree.endNonMemberFunctionDefinition();
symbolTable.backtrackOne();
}
*/
void parseParameter(Type* myType)
{

char identifierName[1000];
tokenType currentToken;
if(symbolTable.isTypeDefined(symbolTable.getBaseType(myType)) == false)
{
std::cout << "Error: Undefined type " << myType << " was encountered" << std::endl;
exit(1);
}
if(myType->isPrimitive && strcmp(myType->primitiveName, "void") == 0)
	{
	std::cout << "Error: Type of function parameter cannot be void. Terminating execution now..." << std::endl;
	exit(1);
	}
currentToken = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in parseParameter()\n\n";
if(symbolTable.isTypeDefined(identifierName))
{
std::cout << "Error: expected identifier in parameter list, Found type name " << identifierName << " instead" << std::endl;
exit(1);
}

if(currentToken != IDENTIFIER)
{
std::cout << "Error: expected identifier in parameter list. Found " << identifierName << " instead" << std::endl;
exit(1);
}

if(symbolTable.isDuplicateParam(identifierName))
{
std::cout << "Error: redefinition of parameter " << identifierName << std::endl;
exit(1);
}

symbolTable.addParameter(myType, identifierName);
myParseTree.addParameter(myType, identifierName);


}

void parseOptions()
{}

//parses a single line/statement inside of a function or a boolean block. However, this function must not be called when the next token is } or an error will occur. The calling function thus must check to make sure its block is not ending.
//lineEnding represents what token should terminate this line.
//returns the current statement (which could be a statement or a boolean block node
abstractSymbolNode* parseLine(tokenType lineEnding)
{
if(debug)
std::cout << "Entered parseLine() " << std::endl;
bool shouldBacktrack = true;
tokenType mostRecent;
char currentToken[1000];
char identifierType[1000];
char identifierName[1000];
char temp[1000];
int returnCode = 0;
Type* myType = NULL;
StatementNode* myStatement = NULL;
ExpressionNode* leftHalf = NULL;
ExpressionNode* rightHalf = NULL;
bool shouldHaveParenthases = false;

if(lineEnding == CLOSING_PARENTHASES)
shouldHaveParenthases = true;

else
shouldHaveParenthases = false;

mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseLine()\n\n";
 if(mostRecent == IF)
	{
	parseIf();
	return NULL;
	}

else if(mostRecent == FOR)
	{
	parseFor();
	return NULL;
	}

else if(mostRecent == WHILE)
	{
	parseWhile();
 	return NULL;
	}

 else if(mostRecent == SEMI_COLON)
         {
	if(debug)
	std::cout << "hit semi-colon in parseLine(). Returning now..." << std::endl;
	return new EmptyExpressionNode();
	}
  else if(mostRecent == BREAK)
	{
	myStatement = new ExpressionStatement(new BreakExpressionNode());
	}  
else if(mostRecent == CONTINUE)
	{
	myStatement = new ExpressionStatement(new ContinueExpressionNode());
	}

 else if(mostRecent == PLUS || mostRecent == MINUS || mostRecent == NOT || mostRecent == BITWISE_NOT)
{
	rightHalf = parseExpression(shouldHaveParenthases, 0);
	myStatement = new ExpressionStatement(new UnaryOperationNode(mostRecent, rightHalf));
}
 else if(mostRecent == PLUS_PLUS)
 {
 myStatement = new ExpressionStatement(new PlusPlusNode(unaryParse(0, shouldHaveParenthases)));
 
}

 else if(mostRecent == MINUS_MINUS)
{
myStatement = new ExpressionStatement(new MinusMinusNode(unaryParse(0, shouldHaveParenthases)));
}

else if(mostRecent == INTEGER_LITERAL || mostRecent == DOUBLE_LITERAL || mostRecent == TRUE || mostRecent == FALSE || mostRecent == CHAR_LITERAL || mostRecent == STRING_LITERAL)
{
if(printBacktracks)
std::cout << "IMPORTANT: backtracking in parseLine() after encountering a literal, so that unaryParse() can be called." << std::endl;
myAnalyzer.backtrack();
myStatement = new ExpressionStatement(unaryParse(0, shouldHaveParenthases));
} 

else if(mostRecent == ASSIGNMENT_OPERATOR || mostRecent == PLUS_EQUALS || mostRecent == MINUS_EQUALS || mostRecent == TIMES_EQUALS || mostRecent == MOD || mostRecent == MOD_EQUALS
|| mostRecent == DIVIDE_EQUALS || mostRecent == POW_EQUALS || mostRecent == LOGICAL_AND || mostRecent == LOGICAL_OR
|| mostRecent == CLOSING_PARENTHASES || mostRecent == CLASS_KEY_WORD
|| mostRecent == OPENING_BRACE || mostRecent == COLON || mostRecent == COMMA || mostRecent == CLOSING_BRACE || mostRecent == PUBLIC || mostRecent == PRIVATE || mostRecent == READ
|| mostRecent == WRITE || mostRecent == RW || mostRecent == MULTI_THREAD || mostRecent == SINGLE_THREAD
|| mostRecent == MULTIPLY || mostRecent == DIVIDE || mostRecent == EQUALS_EQUALS
|| mostRecent == NOT_EQUALS || 
mostRecent == GREATER_THAN || mostRecent == LESS_THAN || mostRecent == GREATER_THAN_EQUAL || mostRecent == LESS_THAN_EQUAL 
|| mostRecent == BIT_SHIFT_LEFT || mostRecent == BIT_SHIFT_RIGHT || mostRecent == EXPONENT || mostRecent == DOT_OPERATOR
|| mostRecent == OPENING_BRACKET || mostRecent == CLOSING_BRACKET || mostRecent == VOID || mostRecent == BITWISE_AND || mostRecent == BITWISE_OR || mostRecent == BITWISE_XOR || mostRecent == BITWISE_AND_EQUALS || mostRecent == BITWISE_OR_EQUALS || mostRecent == END_OF_FILE || mostRecent == OTHER
|| mostRecent == START || mostRecent == CASE || mostRecent == SWITCH_TOKEN || mostRecent == DO)
{
std::cout << "Error: Unexpected token " << getTokenType(mostRecent) << " encountered while parsing statement. Terminating execution now..." << std::endl;
exit(1);
}

else if(mostRecent == OPENING_PARENTHASES)
{
leftHalf = parseExpression(true, 0);
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseLine()\n\n";
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(mostRecent == SEMI_COLON)
	{
	myStatement = new ExpressionStatement(leftHalf);
	myAnalyzer.backtrack();
	}
else if(mostRecent == ASSIGNMENT_OPERATOR)
	{
	rightHalf = parseExpression(shouldHaveParenthases, 0);
	myStatement = new AssignmentStatement(leftHalf, rightHalf);
	}
else if(mostRecent == PLUS_EQUALS || mostRecent == MINUS_EQUALS || mostRecent == TIMES_EQUALS || mostRecent == MOD_EQUALS || mostRecent == DIVIDE_EQUALS)
	{
	rightHalf = parseExpression(shouldHaveParenthases, 0);
	myStatement = new CompoundAssignmentStatement(mostRecent, leftHalf, rightHalf);
	}
else if(mostRecent == PLUS_PLUS)
	myStatement = new ExpressionStatement(new PlusPlusNode(leftHalf));

else if(mostRecent == MINUS_MINUS)
	myStatement = new ExpressionStatement(new MinusMinusNode(leftHalf));
else
	{
	std::cout << "Error: Unexpected token of " << getTokenType(mostRecent) << " encountered. Terminating execution now..." << std::endl;
	exit(1);
	}
}

else if(mostRecent == IDENTIFIER || mostRecent == THIS)
{
//std::cout << "Here in identifier in parseLine()" << std::endl;
	//if this is a defined type, then this is either a declare-assign or a declaration. 
	if(symbolTable.isTypeDefined(currentToken))
	{
	if(printBacktracks)
	std::cout << "IMPORTANT: backtracked in parseLine() after reading identifier which was a type name, in order to call parseType()" << std::endl;
	myAnalyzer.backtrack();
	myType = parseType();
        mostRecent = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseLine()\n\n";
	if(mostRecent != IDENTIFIER || symbolTable.isTypeDefined(identifierName))
	{
	std::cout << "Error: Expected identifier (and not a class type) after type" << std::endl;
	exit(1);
	}
	mostRecent = myAnalyzer.tokenize(currentToken, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << currentToken << " in parseLine()\n\n";
	if(mostRecent == SEMI_COLON)
		{
		myAnalyzer.backtrack();
		returnCode = symbolTable.addVariable(myType, identifierName);
		if(returnCode != 0)
			{
			std::cout << "Error adding var to symbol table." << std::endl;
			exit(1);
			}
		myStatement = new DeclarationStatement(myType, identifierName);
		}
	else if(mostRecent == ASSIGNMENT_OPERATOR)
		{
		rightHalf = parseExpression(shouldHaveParenthases, 0);
		returnCode = symbolTable.addVariable(myType, identifierName);
		if(returnCode != 0)
			{
			std::cout << "Error adding var to symbol table." << std::endl;
			exit(1);
			}
		myStatement = new DeclareAssignStatement(myType, identifierName, rightHalf);
		}
	else
		{
		std::cout << "Error: declaration must be followed by semi colon or assignment operator. " << std::endl;
		exit(1);
		}

	}
	
	//in the following branch, there is either a function call or a modificaition of a variable
	else
	{
	parseExpressionIdentifier(currentToken, &leftHalf, 0);
	mostRecent = myAnalyzer.tokenize(currentToken, 1000);
	if(mostRecent == SEMI_COLON)
	{
	myAnalyzer.backtrack();
	myStatement = new ExpressionStatement(leftHalf);
	}
	else if(mostRecent == ASSIGNMENT_OPERATOR)
	{
	rightHalf = parseExpression(shouldHaveParenthases, 0);
	myStatement = new AssignmentStatement(leftHalf, rightHalf);
	}
	else if(mostRecent == PLUS_PLUS)
		{
		myStatement = new ExpressionStatement(new PlusPlusNode(leftHalf));
		}
	else if(mostRecent == MINUS_MINUS)
		{
		myStatement = new ExpressionStatement(new MinusMinusNode(leftHalf));
		}

	else if(mostRecent == PLUS_EQUALS || mostRecent == MINUS_EQUALS || mostRecent == TIMES_EQUALS || mostRecent == MOD_EQUALS || mostRecent == DIVIDE_EQUALS)
         {
         rightHalf = parseExpression(shouldHaveParenthases, 0);
         myStatement = new CompoundAssignmentStatement(mostRecent, leftHalf, rightHalf);
	}
	else if(mostRecent == CLOSING_PARENTHASES && lineEnding == CLOSING_PARENTHASES)
	{
	myStatement = new ExpressionStatement(leftHalf);
	myAnalyzer.backtrack();
	}
	else
	{
	std::cout << "Error: Undefined token of type " << getTokenType(mostRecent) << " encountered." << std::endl;
	exit(1);
	}
}
}

else if(isInputParseToken(mostRecent))
{
myAnalyzer.backtrack();
myStatement = new ExpressionStatement(parseExpression(shouldHaveParenthases, 0));
}

else if(mostRecent == PRINT)
{
myStatement = new ExpressionStatement(parsePrint());
}

else if(mostRecent == INT || mostRecent == DOUBLE || mostRecent == CHAR || mostRecent == BOOLEAN || mostRecent == ARRAY || mostRecent == LIST || mostRecent == QUEUE 
|| mostRecent == DICT || mostRecent == STRING)
{
	strcpy(identifierType, currentToken);
	if(printBacktracks)
	std::cout << "IMPORTANT: backtracked in parseLine() to call parseType() after reading a primitive or builtin type." << std::endl;
	myAnalyzer.backtrack();
	myType = parseType();
	mostRecent = myAnalyzer.tokenize(identifierName, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << identifierName << " in parseLine()\n\n";
	if((mostRecent != IDENTIFIER) || symbolTable.isTypeDefined(identifierName))
	{
	std::cout << "Error: Unexpected type after declaration started." << std::endl;
	exit(1);
	}
	returnCode = symbolTable.addVariable(myType, identifierName);
	if(returnCode != 0)
	{
	std::cout << "Error adding var to symbol table." << std::endl;
	exit(1);
	}
	mostRecent = myAnalyzer.tokenize(currentToken, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << currentToken << " in parseLine()\n\n";
	if(mostRecent == SEMI_COLON)
		{
		myAnalyzer.backtrack();
		myStatement = new DeclarationStatement(myType, identifierName);
		}
	else if(mostRecent == ASSIGNMENT_OPERATOR)
		{
		rightHalf = parseExpression(shouldHaveParenthases, 0);
		myStatement = new DeclareAssignStatement(myType, identifierName, rightHalf);
		}
	else
		{
		std::cout << "Error: Declaration must be followed by semi-colon or assignment operator." << std::endl;
		exit(1);
		}
	

}
else if(mostRecent == RETURN)
	{
	mostRecent = myAnalyzer.tokenize(currentToken, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << currentToken << " in parseLine()\n\n";
	if(mostRecent == SEMI_COLON)
		{
		myStatement = new ReturnStatement(new EmptyExpressionNode());
		myAnalyzer.backtrack();
		}
	else
		{
		if(printBacktracks)
		std::cout << "IMPORTANT: backtracked in parseLine() after seeing that return statement was not followed by semi-colon. Calling parseExpression()..." << std::endl;
		myAnalyzer.backtrack();
		myStatement = new ReturnStatement(parseExpression(shouldHaveParenthases, 0));
		}
	}



else
	{
	std::cout << "Error: Unexpected token encountered while parsing." << std::endl;
	exit(1);
	}
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseLine()\n\n";
if(mostRecent != lineEnding)
	{
	std::cout << "Error: last token of line did not match expected ending" << std::endl;
	exit(1);
	}

if(debug)
std::cout << "Exiting parseLine() now" << std::endl;
return myStatement;

}

//DEBUGGED THROUGH HERE!

void parseFunction(Type* returnType, char* funcName, bool isMemberFunction)
{
char currentToken[1000];
StatementNode* myStatement = NULL;
Type* newType = NULL;
StatementNode* lastStatement = NULL;
tokenType mostRecent;
myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseFunction()\n\n";
if(strcmp(currentToken, ")") != 0)
{
if(printBacktracks)
std::cout << "IMPORTANT: backtracked in parseFunction() after seeing that function definition opening parenthases was not followed immediatley by a closing parenthases." << std::endl;
myAnalyzer.backtrack();
newType = parseType();
parseParameter(newType);
myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseFunction()\n\n";
while(strcmp(currentToken, ",") == 0)
{
newType = parseType();
parseParameter(newType);
myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseFunction()\n\n";
}


if(strcmp(currentToken, ")") != 0)
{
std::cout << "Error: Expected closing parentheses, encountered " << currentToken << std::endl;
exit(1);
}
}

parseOptions();

myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseFunction()\n\n";
//std::cout << "In parseFunction with token of " << currentToken << std::endl;
while(strcmp(currentToken, "}") != 0)
{
mostRecent = myAnalyzer.tokenize(currentToken, 1000);
if(printGets)
std::cout << "Just got last token, which was " << currentToken << " in parseFunction()\n\n";
if(mostRecent == CLOSING_BRACE)
break;

else if(mostRecent == IF)
	parseIf();

else if(mostRecent == ELSE)
	{
	mostRecent = myAnalyzer.tokenize(currentToken, 1000);
	if(printGets)
	std::cout << "Just got last token, which was " << currentToken << " in parseFunction()\n\n";
	if(mostRecent == IF)
		parseElif();
	else
		{
		if(printBacktracks)
		std::cout << "IMPORTANT: backtracked in parseFunction() after seeing that if does not follow else." << std::endl;
		myAnalyzer.backtrack();
		parseElse();
		}
	}
else if(mostRecent == WHILE)
	parseWhile();

else if(mostRecent == FOR)
	parseFor();
else
{
if(printBacktracks)
std::cout << "IMPORTANT: backtracked in parseFunction() after seeing that next line of function body was not a block. Calling parseLine() now..." << std::endl;
myAnalyzer.backtrack();
myStatement = ((StatementNode*)parseLine(SEMI_COLON));
myParseTree.addStatement(myStatement);
myParseTree.simplifyLast(symbolTable);
}
}

}

void pipeError()
{
std::cout << "Error: Incorrectly formatted pipeline expression. Should be of the form Pipe_line p = new Pipe_line(firstFunc->secondFunc->thirdFunc);" << std::endl;
exit(1);
}

//called at global level. Starts after token "Pipe_Line" was read in.
//line should have the form:	Pipe_Line p = new Pipe_Line(firstFunc->secondFunc->thirdFunc);
//otherwise, an error occurs.
void parsePipeline()
{
//std::cout << "in parsePipeline()\n";
char identifierName[1000];
char funcName[1000];
char tempString[1000];
tokenType mostRecent;
PipeLineNode* myNewNode = NULL;
PipeLine* mySymPipe = NULL;
Type* previousType = NULL;
mostRecent = myAnalyzer.tokenize(identifierName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << identifierName << " in parsePipeline()\n\n";
if(mostRecent != IDENTIFIER || symbolTable.isTypeDefined(identifierName))
{
//std::cout << "in PipeError()\n";
pipeError();
}

Type* tempType = new Type((char*) "PipeLine");
symbolTable.addVariable(tempType, identifierName);
mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tempString << " in parsePipeline()\n\n";
if(mostRecent != ASSIGNMENT_OPERATOR)
{
pipeError();
}

mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tempString << " in parsePipeline()\n\n";
if(mostRecent != NEW)
pipeError();

mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tempString << " in parsePipeline()\n\n";
if(mostRecent != PIPE_LINE)
pipeError();

mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tempString << " in parsePipeline()\n\n";
if(mostRecent != OPENING_PARENTHASES)
pipeError();

symbolTable.addPipeLine(identifierName);
myNewNode = new PipeLineNode(identifierName);

mostRecent = myAnalyzer.tokenize(funcName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << funcName << " in parsePipeline()\n\n";
if(mostRecent != IDENTIFIER)
pipeError();

if(symbolTable.getNonMemberFunction(funcName, NULL) == NULL)
{
std::cout << "Error: all functions in a pipeline must be non member functions. " << std::endl;
exit(1);
}
myNewNode->addFunction(funcName);
symbolTable.addPipeLineArgument(funcName);
mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tempString << " in parsePipeline()\n\n";
while(mostRecent != CLOSING_PARENTHASES)
{
if(mostRecent != MINUS)
{
std::cout << "Error: Each function in pipeline list must be seperated with -> symbol" << std::endl;
exit(1);
}
mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tempString << " in parsePipeline()\n\n";
if(mostRecent != GREATER_THAN)
{
std::cout << "Error: Each function in pipeline list must be seperated with -> symbol" << std::endl;
exit(1);
}
mostRecent = myAnalyzer.tokenize(funcName, 1000);
if(printGets)
std::cout << "Just got last token, which was " << funcName << " in parsePipeline()\n\n";
if(mostRecent != IDENTIFIER || symbolTable.getNonMemberFunction(funcName, NULL) == NULL)
{
std::cout << "Error: All arguments to pipeline must be non member functions. " << std::endl;
}
	if(previousType == NULL)
	{
	previousType = ((Function*)symbolTable.getNonMemberFunction(funcName, NULL))->functionReturnType;
	}
	else
	{
	if(((Function*)symbolTable.getNonMemberFunction(funcName, NULL))->parameterList.size() != 1)
	{
	std::cout << "Error: All functions in a pipeline besides the first function must take exactly one argument as input.\n";
	exit(1);
	}
	if(!myParseTree.isCompatable(previousType, ((Function*)(symbolTable.getNonMemberFunction(funcName, NULL)))->parameterList[0]->variableType))
	{
	std::cout << "Error: output of function in pipeline with type " << (*previousType) << " cannot be converted as input to the function that takes as input an argument of type " << *(((Function*)symbolTable.getNonMemberFunction(funcName, NULL))->parameterList[0]->variableType) << std::endl;
	exit(1);
	}
	previousType = ((Function*)symbolTable.getNonMemberFunction(funcName, NULL))->functionReturnType;
	}
myNewNode->addFunction(funcName);
symbolTable.addPipeLineArgument(funcName);
mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(printGets)
std::cout << "Just got last token, which was " << tempString << " in parsePipeline()\n\n";
}

myParseTree.addPipeLine(myNewNode);
mostRecent = myAnalyzer.tokenize(tempString, 1000);
if(mostRecent != SEMI_COLON)
{
std::cout << "Error: Semi-colon most follow PipeLine initialziation." << std::endl;
exit(1);
}

//std::cout << "Exiting parsePipeLine()\n";
return;
}


//This is where execution starts in Parser.h
void startProcessing(bool myDebug)
{
//std::cout << "Did you see this new statement?" << std::endl << std::endl;
debug = false;
printBacktracks = false;
printGets = false;
symbolTable = SymbolTable();
tokenType currentToken = START;
char thirdToken[1000];
tokenType thirdType = OTHER;
tokenType functionOrVarReturnType = START;
char functionOrVarTypeName[1000];
char IdentifierName[1000];
ExpressionNode* secondHalf = NULL;
ExpressionNode* firstHalf = NULL;
Type* typeVar = NULL;
abstractSymbolNode* lastParsedStatement = NULL;
myParseTree = parseTree();

while(functionOrVarReturnType != END_OF_FILE && functionOrVarReturnType != OTHER)
{
functionOrVarReturnType = myAnalyzer.tokenize(functionOrVarTypeName, 1000);
if(functionOrVarReturnType == END_OF_FILE || functionOrVarReturnType == OTHER)
break;
if(printGets)
std::cout << "Just got last token, which was " << functionOrVarTypeName << " in startProcessing()\n\n";
if(functionOrVarReturnType == CLASS_KEY_WORD)
	{
	if(debug)
	std::cout << "Parsing class now" << std::endl;
	parseClass();
	if(debug)
	std::cout << "Finished parsing class." << std::endl;
	}
else if(functionOrVarReturnType == SEMI_COLON)
continue;

else if(functionOrVarReturnType == PIPE_LINE)
	{
	if(debug)
	std::cout << "Parsing pipeline now." << std::endl;
	parsePipeline();
	if(debug)
	std::cout << "Finished parsing pipeline." << std::endl;
	}
else if(symbolTable.isTypeDefined(functionOrVarTypeName))
	{
	if(printBacktracks)
	std::cout << "IMPORTANT: backtracking in startProcessing() after finding a defined type to start the next line, in order to call parseType()" << std::endl;
	myAnalyzer.backtrack();
	typeVar = parseType();
	currentToken = myAnalyzer.tokenize(IdentifierName, 1000);
	if(printGets)
	std::cout << "Just got next token, which was " << IdentifierName << " in startProcessing()\n\n";
		if(currentToken != IDENTIFIER)
		{
		std::cout << "Error: Expected identifier after type name. Instead, encountered token of type " << getTokenType(currentToken) << " named " << IdentifierName << std::endl;
		exit(1);
		}
	thirdType = myAnalyzer.tokenize(thirdToken, 1000);
	if(printGets)
	std::cout << "Just got next token, which was " << thirdToken << " in startProcessing()\n\n";
	if(thirdType == ASSIGNMENT_OPERATOR)
	{
	 if(debug)
         {
         std::cout << "Parsing declare assign now." << std::endl;
         }
	if(symbolTable.isVariable(IdentifierName))
	{
	std::cout << "Error: Redefinition of variable " << IdentifierName << " at the global level " << std::endl;
	exit(1);
	}
	if(symbolTable.isTypeDefined(IdentifierName))
	{
	std::cout << "Error: " << IdentifierName << " is a defined type/class. Was expecting an identifier which was not a class name. " << std::endl;
	exit(1);
	}
	secondHalf = parseExpression(false, 0);
	myParseTree.addDeclareAssign(typeVar, IdentifierName, secondHalf);
	symbolTable.addVariable(typeVar, IdentifierName);
	if(typeVar->isPrimitive && strcmp(typeVar->primitiveName, "void") == 0)
		{
		std::cout << "Error: Type of variable cannot be void. Terminating execution now." << std::endl;
		exit(1);
		}
	myParseTree.simplifyLast(symbolTable);
	
	if(!myParseTree.isLastBasic(symbolTable))
	{
	std::cout << "Error: only literals and NULL can be assigned to variables at the global level. " << std::endl;
	exit(1);
	}
	if(debug)
	std::cout << "Finished parsing declare assign." << std::endl;
	}
	else if(thirdType == SEMI_COLON)
	{
	if(debug)
	std::cout << "Parsing a declaration now." << std::endl;
	if(symbolTable.isVariable(IdentifierName))
	{
	std::cout << "Error: Redefinition of variable " << IdentifierName << " at the global level " << std::endl;
	exit(1);
	}
	if(symbolTable.isTypeDefined(IdentifierName))
	{
	std::cout << "Error: " << IdentifierName << " is a defined type/class. Was expecting an identifier which was not a class name. " << std::endl;
	exit(1);
	}
	if(typeVar->isPrimitive && strcmp(typeVar->primitiveName, "void") == 0)
		{
		std::cout << "Error: Type of variable cannot be void. Terminating execution now. " << std::endl;
		exit(1);
		}	
	symbolTable.addVariable(typeVar, IdentifierName);
	myParseTree.addDeclaration(typeVar, IdentifierName);
	if(debug)
	std::cout << "Finished adding declaration" << std::endl;
	}

	else if(thirdType == OPENING_PARENTHASES)
	{
	if(debug)
	std::cout << "Parsing Non-Member function definition now." << std::endl;
	if(symbolTable.isTypeDefined(IdentifierName))
	{
	std::cout << "Error: " << IdentifierName << " is defined as a class, and cannot also be used as a non-member function name. Terminating execution now..." << std::endl;
	}
	if(symbolTable.getNonMemberFunction(IdentifierName, NULL) != NULL)
	{
	std::cout << "Error: Duplicate definition of function " << IdentifierName << std::endl;
	exit(1);
	}
	symbolTable.addFunction(typeVar, IdentifierName);
	myParseTree.addNonMemberFunction(typeVar, IdentifierName);
	parseFunction(typeVar, IdentifierName, false);
	symbolTable.backtrackOutOfFunction();
	myParseTree.endNonMemberFunctionDefinition();
	if(debug)
	std::cout << "Finished parsing non-member function definition." << std::endl;
	}
	
	}

else if(!symbolTable.isVariable(functionOrVarTypeName))
	{
	std::cout << "Error: Encountered undefined identifier known as " << functionOrVarTypeName << " Terminating execution now..." << std::endl;
	symbolTable.symbolTrace();
	myParseTree.printTree();
	exit(1);
	}
else
{
if(debug)
std::cout << "Parsing assignment statement at global level now." << std::endl;
thirdType = myAnalyzer.tokenize(thirdToken, 1000);
if(printGets)
std::cout << "Just got next token, which was " << thirdToken << " in startProcessing()\n\n";
if(thirdType != ASSIGNMENT_OPERATOR)
	{
	std::cout << "Error: Expected assignment operator. Instead, encountered token " << thirdToken << std::endl;
	exit(1);
	}
firstHalf = new VariableNode(functionOrVarTypeName);
secondHalf = parseExpression(false, 0);
myParseTree.addAssignment(firstHalf, secondHalf);
myParseTree.simplifyLast(symbolTable);

if(! myParseTree.isLastBasic(symbolTable))
	{
	std::cout << "Error: Only literals and NULL may be assigned at the global level " << std::endl;
	exit(1);
	}
if(debug)
std::cout << "Finished parsing assignment statement at global level." << std::endl;
}

}

}
public:
SymbolTable symbolTable;
parseTree myParseTree;
lexicalAnalyzer myAnalyzer;
};
#endif
