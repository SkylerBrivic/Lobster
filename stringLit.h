#ifndef STRING_LIT_DEFINE_H_HEAD
#define STRING_LIT_DEFINE_H_HEAD
#include "symbolNode.h"
#include "abstractSymbolNode.h"
#include <cstring>

class stringLit : public symbolNode
{
public:
stringLit(int nextNum, symbolNode* newParen, char* newString)
: symbolNode(STRING_LIT_SYMBOL_TYPE, nextNum, newParen)
{
myStr = strdup(newString);
}

char* myStr;
};
#endif
