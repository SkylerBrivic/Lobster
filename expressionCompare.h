#ifndef EXPR_COMPR
#define EXPR_COMPR

#include "lexicalAnalyzer.h"

class expressionCompare
{
public:
expressionCompare()
{}

//returns a negative number if first token has higher precedence than second token
//returns 0 if the two tokens have the same prescedence
//returns a positive number if first token has lower precedence than second token
int compareTo(tokenType firstToken, tokenType secondToken)
{
return getValue(firstToken) - getValue(secondToken);
}

//returns the prescedence value of a token. higher precedence (more important/higher priority operations) have a lower number.
//
int getValue(tokenType myToken)
{
if(myToken == PLUS_PLUS || myToken == MINUS_MINUS || myToken == DOT_OPERATOR)
return 1;

if(myToken == NOT || myToken == NEW)
return 2;

if(myToken == EXPONENT)
return 3;

if(myToken == MULTIPLY || myToken == DIVIDE || myToken == MOD)
return 4;

if(myToken == PLUS || myToken == MINUS)
return 5;

if(myToken == BIT_SHIFT_LEFT || myToken == BIT_SHIFT_RIGHT)
return 6;

if(myToken == LESS_THAN || myToken == LESS_THAN_EQUAL || myToken == GREATER_THAN || myToken == GREATER_THAN_EQUAL)
return 7;

if(myToken == EQUALS_EQUALS || myToken == NOT_EQUALS)
return 8;

if(myToken == BITWISE_AND)
return 9;

if(myToken == BITWISE_XOR)
return 10;

if(myToken == BITWISE_OR)
return 11;

if(myToken == LOGICAL_AND)
return 12;

if(myToken == LOGICAL_OR)
return 13;

if(myToken == ASSIGNMENT_OPERATOR || myToken == PLUS_EQUALS || myToken == MINUS_EQUALS
|| myToken == TIMES_EQUALS || myToken == DIVIDE_EQUALS || myToken == MOD_EQUALS 
|| myToken == BITWISE_AND_EQUALS || myToken == BITWISE_OR_EQUALS)
return 14;

if(myToken == COMMA)
return 15;

//if we're down here, then the token wasn't any of the above operators, so an error occured.
std::cout << "Error: Undefined operator of tokenType " << getTokenType(myToken) << " encountered " << std::endl;
exit(1);

return -1;
}
}; 
#endif
