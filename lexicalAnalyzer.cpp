#include "lexicalAnalyzer.h"
#include <stdio.h>
#include <ctype.h>
#include <cctype>

void lexicalAnalyzer::prevUpdate(char* newIdentifierString, tokenType newTypeOfString)
{
if(currentTokenType != START)
{
previousToken = currentTokenType;
strcpy(prevTokenName, mostRecentToken);
}

strcpy(mostRecentToken, newIdentifierString);
currentTokenType = newTypeOfString;
return;
}


//this function takes as input a position in the file.
//it returns the location of the next non-white space character
//(which could include the null terminator if the EOF is reached)

int lexicalAnalyzer::skipWhiteSpace(int startingPosition)
{
while(startingPosition <= fileLength && (isspace(fileContents[startingPosition]) || fileContents[startingPosition] == '\0' || !isprint(fileContents[startingPosition])))
{
++startingPosition;
}
return startingPosition;
}

bool lexicalAnalyzer::isValidIdentifierChar(char thisChar)
{
if(isalnum(thisChar) || thisChar == '_')
return true;

return false;
}

//returns the character corresponding to the combination of a \ with the inputChar.
//ex. if inputChar is 'n', then '\n' is returned. If the inputChar is not a valid
//escape sequence, then 'i' is returned (for invalid character)
char lexicalAnalyzer::escapeSequence(char inputChar)
{
if(inputChar == 'a')
return '\a';

if(inputChar == 'b')
return '\b';

if(inputChar == 'f')
return '\f';

if(inputChar == 'n')
return '\n';

if(inputChar == 'r')
return '\r';

if(inputChar == 't')
return '\t';

if(inputChar == 'v')
return '\v';

if(inputChar == '\\')
return '\\';

if(inputChar == '\'')
return '\'';

if(inputChar == '\"')
return '\"';

if(inputChar == '?')
return '\?';

if(inputChar == '0')
return '\0';

return 'i';

}

tokenType lexicalAnalyzer::tokenize(char* tokenValue, int arrayLength)
{

if(usePrevious)
{
	if(currentTokenType == START)
	{
	std::cout << "Error: Attempted to backtrack passed the beggining of the file in the class lexicalAnalyzer. Terminating execution now..." << std::endl;
	exit(1);
	}

	usePrevious = false;
	strcpy(tokenValue, mostRecentToken);
	return currentTokenType;

}

int locationInReturnArray = 0;
bool decimalPointSeen = false;
char charLiteral = 'Z';
//skipping white space
currentPosition = skipWhiteSpace(currentPosition);

//checking to see if the end of the file was reached.
if(currentPosition >= fileLength)
{
currentTokenType = END_OF_FILE;
strcpy(mostRecentToken, "");
strcpy(tokenValue, "");
return END_OF_FILE;
}

//checking for reserved words like int, char literals, string literals, and special symbols like ( and {
switch(fileContents[currentPosition])
{
//case where first letter of token was an i
case 'i':

//checking if the token is if
//there was at least one space after the if or the if was immediately followed by an open parenthesis. In this case, the token is if, and we can return it here.
if(fileContents[currentPosition + 1] == 'f' && !isValidIdentifierChar(fileContents[currentPosition + 2]))
{
currentPosition = currentPosition + 2;
strcpy(tokenValue, "if");
prevUpdate(tokenValue, IF);
return IF;
}

//if we're still here, the token wasn't if. Checking now to see if the token was "int"
//if the string "int" is followed by a character which is not a valid identifier char (which includes a space, then the token is int.
if(fileContents[currentPosition + 1] == 'n' && fileContents[currentPosition + 2] == 't'
 && !isValidIdentifierChar(fileContents[currentPosition + 3]))
{
currentPosition += 3;
strcpy(tokenValue, "int");
prevUpdate(tokenValue, INT);
return INT;
}

//IS_INPUT_FAIL
if(fileContents[currentPosition + 1] == 's' && fileContents[currentPosition + 2] == 'I'
&& fileContents[currentPosition + 3] == 'n' && fileContents[currentPosition + 4] == 'p'
&& fileContents[currentPosition + 5] == 'u' && fileContents[currentPosition + 6] == 't'
&& fileContents[currentPosition + 7] == 'F' && fileContents[currentPosition + 8] == 'a'
&& fileContents[currentPosition + 9] == 'i' && fileContents[currentPosition + 10] == 'l'
&& !isValidIdentifierChar(fileContents[currentPosition + 11]))
{
currentPosition += 11;
strcpy(tokenValue, "isInputFail");
prevUpdate(tokenValue, IS_INPUT_FAIL);
return IS_INPUT_FAIL;
}

//if we made it here, then the token wasn't if, int or isInputFail. No other reserved words start with an i, so we can break out of the switch now.
break;


case '+':

//checking if this is the ++ operator
if(fileContents[currentPosition + 1] == '+')
{
currentPosition += 2;
strcpy(tokenValue, "++");
prevUpdate(tokenValue, PLUS_PLUS);
return PLUS_PLUS;
}
//checking if this is the += operator
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "+=");
prevUpdate(tokenValue, PLUS_EQUALS);
return PLUS_EQUALS;
}

//if we made it here, then the token is just plus.
++currentPosition;
strcpy(tokenValue, "+");
prevUpdate(tokenValue, PLUS);
return PLUS;


case '-':

//checking for the case where the token is --
if(fileContents[currentPosition + 1] == '-')
{
currentPosition += 2;
strcpy(tokenValue, "--");
prevUpdate(tokenValue, MINUS_MINUS);
return MINUS_MINUS;
}
//checking for the case where the token is -=
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "-=");
prevUpdate(tokenValue, MINUS_EQUALS);
return MINUS_EQUALS;
}

//if we made it here, then the token is just -
++currentPosition;
strcpy(tokenValue, "-");
prevUpdate(tokenValue, MINUS);
return MINUS;


case '*':

//checking for the case where the token was **= (pow equals)
if(fileContents[currentPosition + 1] == '*' && fileContents[currentPosition + 2] == '=')
{
currentPosition += 3;
strcpy(tokenValue, "**=");
prevUpdate(tokenValue, POW_EQUALS);
return POW_EQUALS;
}

//checking for the case where the token was ** (exponent operator)
if(fileContents[currentPosition + 1] == '*')
{
currentPosition += 2;
strcpy(tokenValue, "**");
prevUpdate(tokenValue, EXPONENT);
return EXPONENT;
}

//checking for the case where the token was *=
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "*=");
prevUpdate(tokenValue, TIMES_EQUALS);
return TIMES_EQUALS;
}

//if we made it here, then the token was just *
++currentPosition;
strcpy(tokenValue, "*");
prevUpdate(tokenValue, MULTIPLY);
return MULTIPLY;


case '/':

//checking for the case where the token was /=
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "/=");
prevUpdate(tokenValue, DIVIDE_EQUALS);
return DIVIDE_EQUALS;
}

//if we made it here, then the token was just /
++currentPosition;
strcpy(tokenValue, "/");
prevUpdate(tokenValue, DIVIDE);
return DIVIDE;

case '=':

//checking for the case where the token was ==
if(fileContents[currentPosition + 1] == '=')
{
currentPosition +=2;
strcpy(tokenValue, "==");
prevUpdate(tokenValue, EQUALS_EQUALS);
return EQUALS_EQUALS;
}

//if we made it here, the token was just =
++currentPosition;
strcpy(tokenValue, "=");
prevUpdate(tokenValue, ASSIGNMENT_OPERATOR);
return ASSIGNMENT_OPERATOR;

case '!':

//checking for the case where the token was !=
if(fileContents[currentPosition + 1] == '=')
{
currentPosition +=2;
strcpy(tokenValue, "!=");
prevUpdate(tokenValue, NOT_EQUALS);
return NOT_EQUALS;
}

//if we made it here, then the token was just !
++currentPosition;
strcpy(tokenValue, "!");
prevUpdate(tokenValue, NOT);
return NOT;

//if the token was {, then we can return that token immediately.
case '{':
++currentPosition;
strcpy(tokenValue, "{");
prevUpdate(tokenValue, OPENING_BRACE);
return OPENING_BRACE;

//if the token was }, then we can return that token immediately.
case '}':
++currentPosition;
strcpy(tokenValue, "}");
prevUpdate(tokenValue, CLOSING_BRACE);
return CLOSING_BRACE;

//if the token was (, then we can return that token immediately.
case '(':
++currentPosition;
strcpy(tokenValue, "(");
prevUpdate(tokenValue, OPENING_PARENTHASES);
return OPENING_PARENTHASES;

//if the token was ), then we can return that token immediately.
case ')':
++currentPosition;
strcpy(tokenValue, ")");
prevUpdate(tokenValue, CLOSING_PARENTHASES);
return CLOSING_PARENTHASES;

//if the token was ;, then we can return that token immediately
case ';':
++currentPosition;
strcpy(tokenValue, ";");
prevUpdate(tokenValue, SEMI_COLON);
return SEMI_COLON;

//checking for the case where the token was "true"
case 't':
if(fileContents[currentPosition + 1] == 'r' && fileContents[currentPosition + 2] == 'u' && fileContents[currentPosition + 3] == 'e' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "true");
prevUpdate(tokenValue, TRUE);
return TRUE;
}

if(fileContents[currentPosition + 1] == 'h' && fileContents[currentPosition + 2] == 'i'
&& fileContents[currentPosition + 3] == 's' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "this");
prevUpdate(tokenValue, THIS);
return THIS;
}

break;

//checking for the opening bracket
case '[':
++currentPosition;
strcpy(tokenValue, "[");
prevUpdate(tokenValue, OPENING_BRACKET);
return OPENING_BRACKET;

//checking for the closing bracket.
case ']':
++currentPosition;
strcpy(tokenValue, "]");
prevUpdate(tokenValue, CLOSING_BRACKET);
return CLOSING_BRACKET;


case 'f':
//checking to see if the token was "false"
if(fileContents[currentPosition + 1] == 'a' && fileContents[currentPosition + 2] == 'l'
&& fileContents[currentPosition + 3] == 's' && fileContents[currentPosition + 4] == 'e'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "false");
prevUpdate(tokenValue, FALSE);
return FALSE;
}

//checking to see if the token was "for"
if(fileContents[currentPosition + 1] == 'o' && fileContents[currentPosition + 2] == 'r' && !isValidIdentifierChar(fileContents[currentPosition + 3]))
{
currentPosition += 3;
strcpy(tokenValue, "for");
prevUpdate(tokenValue, FOR);
return FOR;
}
//no other reserved words start with f, so break.
break;

case '&':

//checking if the token was "&="
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "&=");
prevUpdate(tokenValue, BITWISE_AND_EQUALS);
return BITWISE_AND_EQUALS;
}

//checking for the case where the token was "&&"
else if(fileContents[currentPosition + 1] == '&')
{
currentPosition += 2;
strcpy(tokenValue, "&&");
prevUpdate(tokenValue, LOGICAL_AND);
return LOGICAL_AND;
}

//if we made it here, then the token was "&"
++currentPosition;
strcpy(tokenValue, "&");
prevUpdate(tokenValue, BITWISE_AND);
return BITWISE_AND;


case '|':

//checking for the case where the token was "|="
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "|=");
prevUpdate(tokenValue, BITWISE_OR_EQUALS);
return BITWISE_OR_EQUALS;
}

//checking for the case where the token was "||"
if(fileContents[currentPosition + 1] == '|')
{
currentPosition += 2;
strcpy(tokenValue, "||");
prevUpdate(tokenValue, LOGICAL_OR);
return LOGICAL_OR;
}

//if we made it here, then the token was "|"
++currentPosition;
strcpy(tokenValue, "|");
prevUpdate(tokenValue, BITWISE_OR);
return BITWISE_OR;

//Note: Checked everything above this point in the code.


case 'c':

//checking for the case where the string was "class"
if(fileContents[currentPosition + 1] == 'l' && fileContents[currentPosition + 2] == 'a' 
&& fileContents[currentPosition + 3] == 's' && fileContents[currentPosition + 4] == 's'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "class");
prevUpdate(tokenValue, CLASS_KEY_WORD);
return CLASS_KEY_WORD;
}

//checking for the case where the token was "case"
if(fileContents[currentPosition + 1] == 'a' && fileContents[currentPosition + 2] == 's'
&& fileContents[currentPosition + 3] == 'e' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "case");
prevUpdate(tokenValue, CASE);
return CASE;
}

//checking for the case where the token was "continue"
if(fileContents[currentPosition + 1] == 'o' && fileContents[currentPosition + 2] == 'n' && fileContents[currentPosition + 3] == 't'
&& fileContents[currentPosition + 4] == 'i' && fileContents[currentPosition + 5] == 'n' && fileContents[currentPosition + 6] == 'u'
&& fileContents[currentPosition + 7] == 'e' && !isValidIdentifierChar(fileContents[currentPosition + 8]))
{
currentPosition += 8;
strcpy(tokenValue, "continue");
prevUpdate(tokenValue, CONTINUE);
return CONTINUE;
}

//checking for the case where the token was "char"
if(fileContents[currentPosition + 1] == 'h' && fileContents[currentPosition + 2] == 'a'
&& fileContents[currentPosition + 3] == 'r' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "char");
prevUpdate(tokenValue, CHAR);
return CHAR;
}

//there are no more key words that start with c
break;

case 'p':

//checking for the case where the token was "public"

if(fileContents[currentPosition + 1] == 'u' && fileContents[currentPosition + 2] == 'b'
&& fileContents[currentPosition + 3] == 'l' && fileContents[currentPosition + 4] == 'i'
&& fileContents[currentPosition + 5] == 'c' && !isValidIdentifierChar(fileContents[currentPosition + 6]))
{
currentPosition += 6;
strcpy(tokenValue, "public");
prevUpdate(tokenValue, PUBLIC);
return PUBLIC;
}

//checking for the case where the token was "private"
if(fileContents[currentPosition + 1] == 'r' && fileContents[currentPosition + 2] == 'i'
&& fileContents[currentPosition + 3] == 'v' && fileContents[currentPosition + 4] == 'a'
&& fileContents[currentPosition + 5] == 't' && fileContents[currentPosition + 6] == 'e'
&& !isValidIdentifierChar(fileContents[currentPosition + 7]))
{
currentPosition += 7;
strcpy(tokenValue, "private");
prevUpdate(tokenValue, PRIVATE);
return PRIVATE;
}

//checking for the print token
if(fileContents[currentPosition + 1] == 'r' && fileContents[currentPosition + 2] == 'i'
&& fileContents[currentPosition + 3] == 'n' && fileContents[currentPosition + 4] == 't'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "print");
prevUpdate(tokenValue, PRINT);
return PRINT;
}
/*
//checking for the push token
if(fileContents[currentPosition + 1] == 'u' && fileContents[currentPosition + 2] == 's' && fileContents[currentPosition + 3] == 'h' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "push");
prevUpdate(tokenValue, PUSH_TOKEN);
return PUSH_TOKEN;
}

//checking for the pop token
if(fileContents[currentPosition + 1] == 'o' && fileContents[currentPosition + 2] == 'p' && !isValidIdentifierChar(fileContents[currentPosition + 3]))
{
currentPosition += 3;
strcpy(tokenValue, "pop");
prevUpdate(tokenValue, POP_TOKEN);
return POP_TOKEN;
}

*/
//no other reserved words start with a p
break;

case 'M':

//checking for the case where the string was "MULTI_THREAD"
if(fileContents[currentPosition + 1] == 'U' && fileContents[currentPosition + 2] == 'L'
&& fileContents[currentPosition + 3] == 'T' && fileContents[currentPosition + 4] == 'I'
&& fileContents[currentPosition + 5] == '_' && fileContents[currentPosition + 6] == 'T'
&& fileContents[currentPosition + 7] == 'H' && fileContents[currentPosition + 8] == 'R'
&& fileContents[currentPosition + 9] == 'E' && fileContents[currentPosition + 10] == 'A'
&& fileContents[currentPosition + 11] == 'D' && !isValidIdentifierChar(fileContents[currentPosition + 12]))
{
currentPosition += 12;
strcpy(tokenValue, "MULTI_THREAD");
prevUpdate(tokenValue, MULTI_THREAD);
return MULTI_THREAD;
}
//no other case starts with an M
break;

case 's':
//looking to see if the string was "switch"
if(fileContents[currentPosition + 1] == 'w' && fileContents[currentPosition + 2] == 'i'
&& fileContents[currentPosition + 3] == 't' && fileContents[currentPosition + 4] == 'c'
&& fileContents[currentPosition + 5] == 'h' && !isValidIdentifierChar(fileContents[currentPosition + 6]))
{
currentPosition += 6;
strcpy(tokenValue, "switch");
prevUpdate(tokenValue, SWITCH_TOKEN);
return SWITCH_TOKEN;
}

//no other case starts with an s
break;

case 'S':

//checking for the case where the string was "SINGLE_THREAD"
if(fileContents[currentPosition + 1] == 'I' && fileContents[currentPosition + 2] == 'N'
&& fileContents[currentPosition + 3] == 'G' && fileContents[currentPosition + 4] == 'L'
&& fileContents[currentPosition + 5] == 'E' && fileContents[currentPosition + 6] == '_'
&& fileContents[currentPosition + 7] == 'T' && fileContents[currentPosition + 8] == 'H'
&& fileContents[currentPosition + 9] == 'R' && fileContents[currentPosition + 10] == 'E'
&& fileContents[currentPosition + 11] == 'A' && fileContents[currentPosition + 12] == 'D' && 
!isValidIdentifierChar(fileContents[currentPosition + 13]))
{
currentPosition += 13;
strcpy(tokenValue, "SINGLE_THREAD");
prevUpdate(tokenValue, SINGLE_THREAD);
return SINGLE_THREAD;
}
//checking for the case where the string was "String"
if(fileContents[currentPosition + 1] == 't' && fileContents[currentPosition + 2] == 'r'
&& fileContents[currentPosition + 3] == 'i' && fileContents[currentPosition + 4] == 'n'
&& fileContents[currentPosition + 5] == 'g' && !isValidIdentifierChar(fileContents[currentPosition + 6]))
{
currentPosition += 6;
strcpy(tokenValue, "String");
prevUpdate(tokenValue, STRING);
return STRING;
}

//no other cases start with an S
break;

//checking for the case where the token was ":"
case ':':
++currentPosition;
strcpy(tokenValue, ":");
prevUpdate(tokenValue, COLON);
return COLON;

case '%':

//checking for the case where the token was "%="
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "%=");
prevUpdate(tokenValue, MOD_EQUALS);
return MOD_EQUALS;
}

//if we made it here, then the token was "%"
++currentPosition;
strcpy(tokenValue, "%");
prevUpdate(tokenValue, MOD);
return MOD;

case 'w':

//checking to see if the token was "while"
if(fileContents[currentPosition + 1] == 'h' && fileContents[currentPosition + 2] == 'i'
&& fileContents[currentPosition + 3] == 'l' && fileContents[currentPosition + 4] == 'e'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "while");
prevUpdate(tokenValue, WHILE);
return WHILE;
}

//there are no other keywords that start with a w.
break;

case 'L':

//checking to see if the token is "List"
if(fileContents[currentPosition + 1] == 'i' && fileContents[currentPosition + 2] == 's' 
&& fileContents[currentPosition + 3] == 't' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "List");
prevUpdate(tokenValue, LIST);
return LIST;
}

//no other tokens start with an L
break;

case 'A':
//checking to see if the token is "Array"
if(fileContents[currentPosition + 1] == 'r' && fileContents[currentPosition + 2] == 'r'
&& fileContents[currentPosition + 3] == 'a' && fileContents[currentPosition + 4] == 'y'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "Array");
prevUpdate(tokenValue, ARRAY);
return ARRAY;
}

//no other tokens start with an A
break;

case 'D':

//checking to see if the token is "Dict"
if(fileContents[currentPosition + 1] == 'i' && fileContents[currentPosition + 2] == 'c'
&& fileContents[currentPosition + 3] == 't' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "Dict");
prevUpdate(tokenValue, DICT);
return DICT;
}


case 'd':

//checking for the case where the token was "double"
if(fileContents[currentPosition + 1] == 'o' && fileContents[currentPosition + 2] == 'u'
&& fileContents[currentPosition + 3] == 'b' && fileContents[currentPosition + 4] == 'l'
&& fileContents[currentPosition + 5] == 'e' && !isValidIdentifierChar(fileContents[currentPosition + 6]))
{
currentPosition += 6;
strcpy(tokenValue, "double");
prevUpdate(tokenValue, DOUBLE);
return DOUBLE;
}

//checking to see if the token was 'do'
if(fileContents[currentPosition + 1] == 'o' && !isValidIdentifierChar(fileContents[currentPosition + 2]))
{
currentPosition += 2;
strcpy(tokenValue, "do");
prevUpdate(tokenValue, DO);
return DO;
}


//no other keywords start with a d
break;


case 'e':

//checking to see if the token was "else"

if(fileContents[currentPosition + 1] == 'l' && fileContents[currentPosition + 2] == 's' 
&& fileContents[currentPosition + 3] == 'e' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "else");
prevUpdate(tokenValue, ELSE);
return ELSE;
}

//no other keywords start with an e
break;


case '>':

//checking for the case where the token was ">>"
if(fileContents[currentPosition + 1] == '>')
{
currentPosition += 2;
strcpy(tokenValue, ">>");
prevUpdate(tokenValue, BIT_SHIFT_RIGHT);
return BIT_SHIFT_RIGHT;
}

//checking for the case where the token was ">="
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, ">=");
prevUpdate(tokenValue, GREATER_THAN_EQUAL);
return GREATER_THAN_EQUAL;
}

//if we made it here, then the token was ">"
++currentPosition;
strcpy(tokenValue, ">");
prevUpdate(tokenValue, GREATER_THAN);
return GREATER_THAN;

case '<':

//checking for the case where the token was "<<"
if(fileContents[currentPosition + 1] == '<')
{
currentPosition += 2;
strcpy(tokenValue, "<<");
prevUpdate(tokenValue, BIT_SHIFT_LEFT);
return BIT_SHIFT_LEFT;
}

//checkig for the case where the token was "<="
if(fileContents[currentPosition + 1] == '=')
{
currentPosition += 2;
strcpy(tokenValue, "<=");
prevUpdate(tokenValue, LESS_THAN_EQUAL);
return LESS_THAN_EQUAL;
}

//if we made it here, then the token was "<"
++currentPosition;
strcpy(tokenValue, "<");
prevUpdate(tokenValue, LESS_THAN);
return LESS_THAN;

//checking for the case where the token was "PipeLine"
case 'P':
if(fileContents[currentPosition + 1] == 'i' && fileContents[currentPosition + 2] == 'p'
&& fileContents[currentPosition + 3] == 'e' && fileContents[currentPosition + 4] == 'L'
&& fileContents[currentPosition + 5] == 'i' && fileContents[currentPosition + 6] == 'n'
&& fileContents[currentPosition + 7] == 'e' && !isValidIdentifierChar(fileContents[currentPosition + 8]))
{
currentPosition += 8;
strcpy(tokenValue, "PipeLine");
prevUpdate(tokenValue, PIPE_LINE);
return PIPE_LINE;
}
//no other keywords start with a P
break;

case 'Q':

//checking for the case where the token was "Queue"
if(fileContents[currentPosition + 1] == 'u' && fileContents[currentPosition + 2] == 'e'
&& fileContents[currentPosition + 3] == 'u' && fileContents[currentPosition + 4] == 'e'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "Queue");
prevUpdate(tokenValue, QUEUE);
return QUEUE;

}

//no other tokens start with a Q
break;

case 'r':

//checking for the case where the token was "return"
if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 't' 
&& fileContents[currentPosition + 3] == 'u' && fileContents[currentPosition + 4] == 'r'
&& fileContents[currentPosition + 5] == 'n' && !isValidIdentifierChar(fileContents[currentPosition + 6]))
{
currentPosition += 6;
strcpy(tokenValue, "return");
prevUpdate(tokenValue, RETURN);
return RETURN;
}

//no other reserved words start with an r.
break;



case 'R':
//checking for the case where the token was "READ" token
if(fileContents[currentPosition + 1] == 'E' && fileContents[currentPosition + 2] == 'A'
&& fileContents[currentPosition + 3] == 'D' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "READ");
prevUpdate(tokenValue, READ);
return READ;
}

//checking to see if the token was RW
if(fileContents[currentPosition + 1] == 'W' && !isValidIdentifierChar(fileContents[currentPosition + 2]))
{
currentPosition += 2;
strcpy(tokenValue, "RW");
prevUpdate(tokenValue, RW);
return RW;
}
//no other reserved words start with an R.
break;

case 'W':

//checking to see if the token was WRITE
if(fileContents[currentPosition + 1] == 'R' && fileContents[currentPosition + 2] == 'I'
&& fileContents[currentPosition + 3] == 'T' && fileContents[currentPosition + 4] == 'E'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "WRITE");
prevUpdate(tokenValue, WRITE);
return WRITE;
}

//no other reserved words start with a W
break;

case '.':

//checking for the case where this token was the dot operator
//(if the next letter is a number, than this is a double literal, 
//and so the dot operator token will not be returned. Otherwise, this is the dot operator.
if(!isdigit(fileContents[currentPosition + 1]))
{
++currentPosition;
strcpy(tokenValue, ".");
prevUpdate(tokenValue, DOT_OPERATOR);
return DOT_OPERATOR;
}
else
break;

case 'v':

//checking for the case where the token was void
if(fileContents[currentPosition + 1] == 'o' && fileContents[currentPosition + 2] == 'i'
&& fileContents[currentPosition + 3] == 'd' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "void");
prevUpdate(tokenValue, VOID);
return VOID;
}
//no other reserved words start with a v
break;

case 'b':

//checking for the case where the token was "break"
if(fileContents[currentPosition + 1] == 'r' && fileContents[currentPosition + 2] == 'e' 
&& fileContents[currentPosition + 3] == 'a' && fileContents[currentPosition + 4] == 'k'
&& !isValidIdentifierChar(fileContents[currentPosition + 5]))
{
currentPosition += 5;
strcpy(tokenValue, "break");
prevUpdate(tokenValue, BREAK);
return BREAK;
} 

//checking for the case where the token was "boolean"
if(fileContents[currentPosition + 1] == 'o' && fileContents[currentPosition + 2] == 'o'
&& fileContents[currentPosition + 3] == 'l' && fileContents[currentPosition + 4] == 'e'
&& fileContents[currentPosition + 5] == 'a' && fileContents[currentPosition + 6] == 'n'
&& !isValidIdentifierChar(fileContents[currentPosition + 7]))
{
currentPosition += 7;
strcpy(tokenValue, "boolean");
prevUpdate(tokenValue, BOOLEAN);
return BOOLEAN;
}
//no more reserved words start with a b
break;

//handing the case where the next token is a character literal
case '\'':
if(fileContents[currentPosition + 1] == '\\')
{
charLiteral = escapeSequence(fileContents[currentPosition + 2]);

if(charLiteral == 'i' || currentPosition + 3 >= fileLength || fileContents[currentPosition + 3] != '\'')
{
std::cerr << "IN ERROR BRANCH\n";
currentPosition += 2;
strcpy(tokenValue, "");
prevUpdate(tokenValue, OTHER);
return OTHER;
}
else
{
currentPosition += 4;
strcpy(tokenValue, &charLiteral);
prevUpdate(tokenValue, CHAR_LITERAL);
return CHAR_LITERAL;
}
}
else if( fileContents[currentPosition + 1] == '\'' || currentPosition + 2 >= fileLength || fileContents[currentPosition + 2] != '\'')
{
std::cerr << "in second error branch\n";
++currentPosition;
strcpy(tokenValue, "");
prevUpdate(tokenValue, OTHER);
return OTHER;
}
else
{
charLiteral = fileContents[currentPosition + 1];
currentPosition += 3;
strcpy(tokenValue, &charLiteral);
prevUpdate(tokenValue, CHAR_LITERAL);
return CHAR_LITERAL;
}

//handling the case where the token is a string literal.
case '\"':
++currentPosition;

while(currentPosition < fileLength && fileContents[currentPosition] != '\"')
{
if(fileContents[currentPosition] == '\\')
{
charLiteral = escapeSequence(fileContents[currentPosition + 1]);
if(charLiteral == 'i')
{
strcpy(tokenValue, "");
prevUpdate(tokenValue, OTHER);
return OTHER;
}
tokenValue[locationInReturnArray] = charLiteral;
++locationInReturnArray;
currentPosition += 2;
}
else
{
tokenValue[locationInReturnArray] = fileContents[currentPosition];
++locationInReturnArray;
++currentPosition;
}
}

if(currentPosition >= fileLength)
{
strcpy(tokenValue, "");
prevUpdate(tokenValue, OTHER);
return OTHER;
}
else
{
tokenValue[locationInReturnArray] = '\0';
++currentPosition;
prevUpdate(tokenValue, STRING_LITERAL);
return STRING_LITERAL;
}

//checking for the case where the token is the bitwise xor operator
case '^':
strcpy(tokenValue, "^");
++currentPosition;
prevUpdate(tokenValue, BITWISE_XOR);
return BITWISE_XOR;

//checking for the case where the token is the bitwise not operator
case '~':
strcpy(tokenValue, "~");
++currentPosition;
prevUpdate(tokenValue, BITWISE_NOT);
return BITWISE_NOT;

case ',':
strcpy(tokenValue, ",");
++currentPosition;
prevUpdate(tokenValue, COMMA);
return COMMA;

case 'N':
if(fileContents[currentPosition + 1] == 'U' && fileContents[currentPosition + 2] == 'L'
&& fileContents[currentPosition + 3] == 'L' && !isValidIdentifierChar(fileContents[currentPosition + 4]))
{
currentPosition += 4;
strcpy(tokenValue, "NULL");
prevUpdate(tokenValue, NULL_WORD);
return NULL_WORD;
}

break;

case 'n':

//checking to see if this is the "new" token
if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 'w' 
&& !isValidIdentifierChar(fileContents[currentPosition + 3]))
{
currentPosition += 3;
strcpy(tokenValue, "new");
prevUpdate(tokenValue, NEW);
return NEW;
}
break;


//checking for the input functions (getInt(), getChar(), getBoolean(), getDouble(), getWord(), and getLine())
case 'g':

if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 't'
&& fileContents[currentPosition + 3] == 'I' && fileContents[currentPosition + 4] == 'n'
&& fileContents[currentPosition + 5] == 't' && !isValidIdentifierChar(fileContents[currentPosition + 6]))
{
currentPosition += 6;
strcpy(tokenValue, "getInt");
prevUpdate(tokenValue, GET_INT);
return GET_INT;
}

if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 't'
&& fileContents[currentPosition + 3] == 'C' && fileContents[currentPosition + 4] == 'h'
&& fileContents[currentPosition + 5] == 'a' && fileContents[currentPosition + 6] == 'r'
&& !isValidIdentifierChar(fileContents[currentPosition + 7]))
{
currentPosition += 7;
strcpy(tokenValue, "getChar");
prevUpdate(tokenValue, GET_CHAR);
return GET_CHAR;
}

if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 't'
&& fileContents[currentPosition + 3] == 'B' && fileContents[currentPosition + 4] == 'o'
&& fileContents[currentPosition + 5] == 'o' && fileContents[currentPosition + 6] == 'l'
&& fileContents[currentPosition + 7] == 'e' && fileContents[currentPosition + 8] == 'a'
&& fileContents[currentPosition + 9] == 'n' && !isValidIdentifierChar(fileContents[currentPosition + 10]))
{
currentPosition += 10;
strcpy(tokenValue, "getBoolean");
prevUpdate(tokenValue, GET_BOOLEAN);
return GET_BOOLEAN;
}

if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 't'
&& fileContents[currentPosition + 3] == 'D' && fileContents[currentPosition + 4] == 'o'
&& fileContents[currentPosition + 5] == 'u' && fileContents[currentPosition + 6] == 'b'
&& fileContents[currentPosition + 7] == 'l' && fileContents[currentPosition + 8] == 'e'
&& !isValidIdentifierChar(fileContents[currentPosition + 9]))
{
currentPosition += 9;
strcpy(tokenValue, "getDouble");
prevUpdate(tokenValue, GET_DOUBLE);
return GET_DOUBLE;
}

if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 't'
&& fileContents[currentPosition + 3] == 'W' && fileContents[currentPosition + 4] == 'o'
&& fileContents[currentPosition + 5] == 'r' && fileContents[currentPosition + 6] == 'd'
&& !isValidIdentifierChar(fileContents[currentPosition + 7]))
{
currentPosition += 7;
strcpy(tokenValue, "getWord");
prevUpdate(tokenValue, GET_WORD);
return GET_WORD;
}

if(fileContents[currentPosition + 1] == 'e' && fileContents[currentPosition + 2] == 't'
&& fileContents[currentPosition + 3] == 'L' && fileContents[currentPosition + 4] == 'i'
&& fileContents[currentPosition + 5] == 'n' && fileContents[currentPosition + 6] == 'e'
&& !isValidIdentifierChar(fileContents[currentPosition + 7]))
{
currentPosition += 7;
strcpy(tokenValue, "getLine");
prevUpdate(tokenValue, GET_LINE);
return GET_LINE;
}

break;

default: break;
}

//since we made it out of the switch block, if the token is a valid token, it must be either an integer literal,
//a double literal, or an identifier.

//if the token starts with a decimal point and wasn't the dot operator (it would have returned in the switch statement if it was the dot operator),
//then it must be a double literal.
if(fileContents[currentPosition] == '.')
{
	if(!isdigit(fileContents[currentPosition + 1]))
	{
		strcpy(tokenValue, "");
		++currentPosition;
		prevUpdate(tokenValue, OTHER);
		return OTHER;
	}
	++currentPosition;
	tokenValue[locationInReturnArray] = '.';
	++locationInReturnArray;
	while(isdigit(fileContents[currentPosition]))
	{
		tokenValue[locationInReturnArray] = fileContents[currentPosition];
		++locationInReturnArray;
		++currentPosition;
	}
	if(fileContents[currentPosition] == '.')
	{
		strcpy(tokenValue, "");
		++currentPosition;
		prevUpdate(tokenValue, OTHER);
		return OTHER;
	}
	else
	{
		tokenValue[locationInReturnArray] = '\0';
		prevUpdate(tokenValue, DOUBLE_LITERAL);
		return DOUBLE_LITERAL;
	}
}


//if the token starts with a digit, then it must be either an integer literal or a double literal
if(isdigit(fileContents[currentPosition]))
{
	while(isdigit(fileContents[currentPosition]))
	{
		tokenValue[locationInReturnArray] = fileContents[currentPosition];
		++locationInReturnArray;
		++currentPosition;
	}
	//if a decimal point is in the number, then this must be a double literal
	if(fileContents[currentPosition] == '.')
	{	//if there isn't at least one digit after the decimal point, then this is an invalid token. (ex. 154. is not a valid number)
		if(!isdigit(fileContents[currentPosition + 1]))
		{
			strcpy(tokenValue, "");
			prevUpdate(tokenValue, OTHER);
			return OTHER;
		}
		decimalPointSeen = true;
		++currentPosition;
		tokenValue[locationInReturnArray] = '.';
		++locationInReturnArray;
		while(isdigit(fileContents[currentPosition]))
		{
			tokenValue[locationInReturnArray] = fileContents[currentPosition];
			++locationInReturnArray;
			++currentPosition;
		}
		if(fileContents[currentPosition] == '.')
		{
			strcpy(tokenValue, "");
			prevUpdate(tokenValue, OTHER);
			return OTHER;
		}
		else
		{
			tokenValue[locationInReturnArray] = '\0';
			prevUpdate(tokenValue, DOUBLE_LITERAL);
			return DOUBLE_LITERAL;
		}
	}
	//if we made it here, then the string is a series of numbers with no decimal point, which means it is an integer literal.
	else
	{
		tokenValue[locationInReturnArray] = '\0';
		prevUpdate(tokenValue, INTEGER_LITERAL);
		return INTEGER_LITERAL;
	}
}

//if we made it down here, then for this to be a valid token, the string must be an identifier.
if(!isalpha(fileContents[currentPosition]) && fileContents[currentPosition] != '_')
{
strcpy(tokenValue, "");
prevUpdate(tokenValue, OTHER);
return OTHER;
}

tokenValue[locationInReturnArray] = fileContents[currentPosition];
++currentPosition;
++locationInReturnArray;

while(isValidIdentifierChar(fileContents[currentPosition]))
{
tokenValue[locationInReturnArray] = fileContents[currentPosition];
++currentPosition;
++locationInReturnArray;
}
tokenValue[locationInReturnArray] = '\0';
prevUpdate(tokenValue, IDENTIFIER);
return IDENTIFIER;

}


std::string getTokenType(tokenType myToken)
{
switch(myToken)
{
case INTEGER_LITERAL: return "INTEGER_LITERAL";
case DOUBLE_LITERAL: return "DOUBLE_LITERAL";
case TRUE: return "TRUE";
case FALSE: return "FALSE";
case CHAR_LITERAL: return "CHAR_LITERAL";
case STRING_LITERAL: return "STRING_LITERAL";
case ASSIGNMENT_OPERATOR: return "ASSIGNMENT_OPERATOR";
case PLUS_PLUS: return "PLUS_PLUS";
case MINUS_MINUS: return "MINUS_MINUS";
case PLUS_EQUALS: return "PLUS_EQUALS";
case MINUS_EQUALS: return "MINUS_EQUALS";
case TIMES_EQUALS: return "TIMES_EQUALS";
case MOD: return "MOD";
case MOD_EQUALS: return "MOD_EQUALS";
case DIVIDE_EQUALS: return "DIVIDE_EQUALS";
case POW_EQUALS: return "POW_EQUALS";
case LOGICAL_AND: return "LOGICAL_AND";
case LOGICAL_OR: return "LOGICAL_OR";
case OPENING_PARENTHASES: return "OPENING_PARENTHASES";
case CLOSING_PARENTHASES: return "CLOSING_PARENTHASES";
case IDENTIFIER: return "IDENTIFIER";
case CLASS_KEY_WORD: return "CLASS_KEY_WORD";
case OPENING_BRACE: return "OPENING_BRACE";
case COLON: return "COLON";
case CLOSING_BRACE: return "CLOSING_BRACE";
case SEMI_COLON: return "SEMI_COLON";
case IF: return "IF";
case WHILE: return "WHILE";
case DO: return "DO";
case FOR: return "FOR";
case ELSE: return "ELSE";
case SWITCH_TOKEN: return "SWITCH_TOKEN";
case CASE: return "CASE";
case PLUS: return "PLUS";
case MINUS: return "MINUS";
case MULTIPLY: return "MULTIPLY";
case DIVIDE: return "DIVIDE";
case EQUALS_EQUALS: return "EQUALS_EQUALS";
case NOT_EQUALS: return "NOT_EQUALS";
case GREATER_THAN: return "GREATER_THAN";
case LESS_THAN: return "LESS_THAN";
case GREATER_THAN_EQUAL: return "GREATER_THAN_EQUAL";
case LESS_THAN_EQUAL: return "LESS_THAN_EQUAL";
case NOT: return "NOT";
case BIT_SHIFT_LEFT: return "BIT_SHIFT_LEFT";
case BIT_SHIFT_RIGHT: return "BIT_SHIFT_RIGHT";
case EXPONENT: return "EXPONENT";
case RETURN: return "RETURN";
case DOT_OPERATOR: return "DOT_OPERATOR";
case OPENING_BRACKET: return "OPENING_BRACKET";
case CLOSING_BRACKET: return "CLOSING_BRACKET";
case INT: return "INT";
case VOID: return "VOID";
case ARRAY: return "ARRAY";
case BOOLEAN: return "BOOLEAN";
case CHAR: return "CHAR";
case DOUBLE: return "DOUBLE";
case CONTINUE: return "CONTINUE";
case BREAK: return "BREAK";
case BITWISE_AND: return "BITWISE_AND";
case BITWISE_OR: return "BITWISE_OR";
case BITWISE_XOR: return "BITWISE_XOR";
case BITWISE_NOT: return "BITWISE_NOT";
case BITWISE_AND_EQUALS: return "BITWISE_AND_EQUALS";
case BITWISE_OR_EQUALS: return "BITWISE_OR_EQUALS";
case STRING: return "STRING";
case END_OF_FILE: return "END_OF_FILE";
case COMMA: return "COMMA";
case NEW: return "NEW";
case NULL_WORD: return "NULL_WORD";
case PUBLIC: return "PUBLIC";
case PRIVATE: return "PRIVATE";
case READ: return "READ";
case WRITE: return "WRITE";
case RW: return "RW";
case MULTI_THREAD: return "MULTI_THREAD";
case SINGLE_THREAD: return "SINGLE_THREAD";
case PIPE_LINE: return "PIPE_LINE";
case QUEUE: return "QUEUE";
case DICT: return "DICT";
case LIST: return "LIST";
default: return "OTHER";

}


}

std::ostream& operator<<(std::ostream& out, const tokenType myToken)
{
out << getTokenType(myToken);
return out;
}

/*
int main(int argc, char** argv)
{
if(argc <= 1)
return -1;

std::string fileName = argv[1];
char bufferArray[3000];
lexicalAnalyzer myAnalyzer = lexicalAnalyzer(fileName);
tokenType returnToken = VOID;
while(returnToken != END_OF_FILE && returnToken != OTHER)
{
returnToken = myAnalyzer.tokenize(bufferArray, 3000);
std::cout << getTokenType(returnToken) << ": \"" << bufferArray << "\"" << std::endl;
}

return 0;
}
*/
