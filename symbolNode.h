#ifndef SYMBOL_NODE
#define SYMBOL_NODE 

#include <vector>
using namespace std;

enum SymbolType
{
GLOBAL_SCOPE, STRING_LIT_SYMBOL_TYPE, GLOBAL_VARIABLE, NON_MEMBER_FUNCTION, FUNCTION_LOCAL_VARIABLE, PARAMETER, BLOCK, CLASS, MEMBER_FUNCTION, MEMBER_VARIABLE, PIPE_LINE_SYM
};

//note: firstInstructionInCode stores the offset in the code to where the first line of a function definition or block of code starts. When the symbolNode is first created, this will be set to -1, since no code has been generated yet. However, the actual value will be filled in later.
	class symbolNode
	{
	public:
		int firstInstructionInCode;
		int uniqueIdentifierNum;
		symbolNode* parentBlock;
		vector<symbolNode*> childrenBlockList;
		SymbolType symbolType;


	symbolNode(SymbolType newSymbolType, int newNum, symbolNode* newParent)
	{
	firstInstructionInCode = -1;
	parentBlock = newParent;
	childrenBlockList = vector<symbolNode*>();
	symbolType = newSymbolType;
	uniqueIdentifierNum = newNum;
	}



	void insertChild(symbolNode* childBlock)
	{
	childrenBlockList.push_back(childBlock);
	}

	int numChildren()
	{
	return childrenBlockList.size();
	}


	};
#endif
