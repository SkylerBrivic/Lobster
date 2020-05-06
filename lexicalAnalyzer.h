#ifndef LEX_AN
#define LEX_AN

#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

enum tokenType {
START, INTEGER_LITERAL, DOUBLE_LITERAL, TRUE, FALSE, CHAR_LITERAL,
STRING_LITERAL, ASSIGNMENT_OPERATOR, PLUS_PLUS, MINUS_MINUS,
PLUS_EQUALS, MINUS_EQUALS, TIMES_EQUALS, MOD, MOD_EQUALS,
DIVIDE_EQUALS, POW_EQUALS, LOGICAL_AND, LOGICAL_OR, OPENING_PARENTHASES, CLOSING_PARENTHASES, NULL_WORD, THIS,
IDENTIFIER, CLASS_KEY_WORD, OPENING_BRACE, COLON, COMMA, PIPE_LINE, QUEUE, LIST, DICT, ARRAY, STRING,
CLOSING_BRACE, SEMI_COLON, PUBLIC, PRIVATE, READ, WRITE, RW, MULTI_THREAD, SINGLE_THREAD, IF, WHILE, DO, 
FOR, ELSE, SWITCH_TOKEN, CASE, PLUS, MINUS, MULTIPLY, DIVIDE, EQUALS_EQUALS, NOT_EQUALS,
GREATER_THAN, LESS_THAN, GREATER_THAN_EQUAL, LESS_THAN_EQUAL, NOT,
BIT_SHIFT_LEFT, BIT_SHIFT_RIGHT, EXPONENT, RETURN, DOT_OPERATOR, 
OPENING_BRACKET, CLOSING_BRACKET, INT, VOID, BOOLEAN, CHAR, DOUBLE, CONTINUE, BREAK, 
BITWISE_AND, BITWISE_OR, BITWISE_XOR, BITWISE_NOT, BITWISE_AND_EQUALS, BITWISE_OR_EQUALS, NEW, 
PRINT, GET_INT, GET_CHAR, GET_DOUBLE, GET_BOOLEAN, GET_WORD, GET_LINE, IS_INPUT_FAIL, END_OF_FILE, OTHER
};

class lexicalAnalyzer
{
public: 
tokenType previousToken;
tokenType currentTokenType;

bool usePrevious;
char prevTokenName[1000];
char mostRecentToken[1000];
lexicalAnalyzer(std::string newString)
{
previousToken = currentTokenType = START;
usePrevious = false;
fileName = newString;
std::ifstream myStream(fileName.c_str(), std::ifstream::in);
myStream.seekg(0, myStream.end);
fileLength = myStream.tellg();
myStream.seekg(0, myStream.beg);
fileContents = new char[fileLength + 1];
myStream.read(fileContents, fileLength);
fileContents[fileLength] = '\0';
currentPosition = 0;
}

void printInput()
{
std::cout << fileContents << std::endl;
}

void backtrack()
{
//std::cout << "Just backtracked with current token of " << mostRecentToken << std::endl;
if(usePrevious)
{
std::cout << "Exception in class lexicalAnalyzer: backtrack was called twice in a row, indicating an invalid statement. Terminating execution now..." << std::endl;
exit(1);
}

usePrevious = true;

}
//returns the type of the next token in the file.
//Additionally, the value of the token (as a string)
//is stored in tokenValue, which must be big enough for this.
//arrayLength stores the size of the char array.
tokenType tokenize(char* tokenValue, int arrayLength);

private:
std::string fileName;
char* fileContents;
int fileLength;
int currentPosition;

int skipWhiteSpace(int startingPosition);
bool isValidIdentifierChar(char thisChar);
char escapeSequence(char inputChar);
friend std::ostream& operator<<(std::ostream& out, const tokenType myToken);
void prevUpdate(char*, tokenType);
};
std::string getTokenType(tokenType myToken);
#endif
