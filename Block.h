#ifndef BLOCK_H
#define BLOCK_H

#include "symbolNode.h"

class Block : public symbolNode
{
public:

Block(int newNum, symbolNode* newParent)
: symbolNode(BLOCK, newNum, newParent)
{
lastInstructionInBlock = -1;
}

int lastInstructionInBlock;
};
#endif
