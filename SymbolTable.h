#ifndef SYM_TAB
#define SYM_TAB

#include <vector>
#include <cstring>
#include <string>
#include "Class.h"
#include "Parameter.h"
#include "Member_Function.h"
#include "Non_Member_Function.h"
#include "Variable.h"
#include "PipeLine.h"
#include "Block.h"
#include "stringLit.h"

using namespace std;

class SymbolTable
{
public:
bool symbolDebug;

	SymbolTable()
	{
	symbolDebug = false;
	currentPointer = root = new symbolNode(GLOBAL_SCOPE, 0, NULL);
	indexInCurrentChild = 0;
	nextUniqueNum = 1;
	}

	symbolNode* getCurrent()
	{
	return currentPointer;
	}

	symbolNode* getPointerToNextChild()
	{
	if(indexInCurrentChild >= currentPointer->numChildren())
	return NULL;

	else
	{
	++indexInCurrentChild;
	return currentPointer->childrenBlockList[indexInCurrentChild - 1];
	}
	}



	char* getBaseType(Type* myType)
	{
	Type* otherType = myType;
	char* returnStr;
	if(myType->isPrimitive == true && strcmp(myType->primitiveName, "PipeLine") == 0)
		{
		return myType->primitiveName;
		}

	while(otherType->isPrimitive == false)
		otherType = otherType->nextType;
	return otherType->primitiveName;
	}
	
	void backtrackOne()
	{
	if(currentPointer == root)
	return;
	
	currentPointer = currentPointer->parentBlock;
	indexInCurrentChild = 0;
	}

	void backtrackOutOfFunction()
	{
	while(currentPointer->symbolType == BLOCK)
	{
	currentPointer = currentPointer->parentBlock;
	}


	indexInCurrentChild = 0;
	if(currentPointer == NULL || currentPointer->parentBlock == NULL)
	{
	std::cout << "Already can't backtrack out of function?\n";
	exit(1);
	}
	currentPointer = currentPointer->parentBlock;
	return;
	}


	void backtrackOutOfClass()
	{
	if(currentPointer == NULL || currentPointer->parentBlock == NULL)
	{
	std::cout << "Already can't backtrack out of class?\n";
	if(currentPointer == NULL)
	std::cout << "Current was NULL\n";
	else
	std::cout << "Current's parent was NULL. Current was " << currentPointer->symbolType;
	exit(1);
	}
	while(currentPointer->symbolType != CLASS)
	{
	currentPointer = currentPointer->parentBlock;
	}

	indexInCurrentChild = 0;
	if(currentPointer->parentBlock == NULL)
	{
	return;
	}
	currentPointer = currentPointer->parentBlock;
	}

	void backtrackOutOfBlock()
	{
	backtrackOne();
	}


void addPipeLine(char* myPipeName)
{
root->insertChild(new PipeLine(nextUniqueNum, root, myPipeName));
++nextUniqueNum;
}

void addPipeLineArgument(char* newFunc)
{
PipeLine* myPipe = ((PipeLine*)(root->childrenBlockList[ (root->childrenBlockList.size()) - 1]));
myPipe->addFunction(newFunc);
}

private:

//helper function to return a symbolNode at a given index in the children list from a specified symbol node
symbolNode* getChildAt(symbolNode* parentNode, int childNum)
{

if(parentNode == NULL || childNum >= parentNode->numChildren())
return NULL;

return parentNode->childrenBlockList[childNum];
}

//helper function that returns true if a symbol node passed to it is a local variable, global variable or function parameter, and false otherwise
bool isVariable(symbolNode* myProspectiveVar)
{
if(myProspectiveVar->symbolType == FUNCTION_LOCAL_VARIABLE || myProspectiveVar->symbolType == GLOBAL_VARIABLE
 || myProspectiveVar->symbolType == PARAMETER || myProspectiveVar->symbolType == MEMBER_VARIABLE)
return true;

return false;
}

//helper function that returns true if a symbol node passed to it is a non-member function, and false otherwise
bool isNonMemberFunction(symbolNode* myPropsectiveFunction)
{
if(myPropsectiveFunction->symbolType == NON_MEMBER_FUNCTION)
return true;

return false;
}

//helper function that returns true if the symbol node passed to is a class name, and false otherwise
bool isClass(symbolNode* myProspectiveClass)
{
if(myProspectiveClass->symbolType == CLASS)
return true;

return false;
}

//helper function that returns true if the symbol node passed to it is a member variable, and false otherwise
bool isMemberVariable(symbolNode* myProspectiveVar)
{
if(myProspectiveVar->symbolType == MEMBER_VARIABLE)
return true;

return false;
}

bool isParameter(symbolNode* myProspectiveVar)
{
if(myProspectiveVar->symbolType == PARAMETER)
return true;

return false;
}

//helper function that returns true if the symbol node passed to is a member function, and false otherwise
bool isMemberFunction(symbolNode* myProspectiveFunction)
{
if(myProspectiveFunction->symbolType == MEMBER_FUNCTION)
return true;

return false;
}


bool isFunction(symbolNode* myFunc)
{
return (isMemberFunction(myFunc) || isNonMemberFunction(myFunc));
}

//from a start index, looks at a parent node's children one by one. If a variable with the same name as identifierName is found, then true is returned (there is a duplicate)
//otherwise, false is returned
bool isDuplicateVar(symbolNode* parentNode, int startIndex, char* identifierName)
{
while(startIndex < parentNode->numChildren())
{
if(isVariable(getChildAt(parentNode, startIndex)) && strcmp(identifierName,  ((Variable*) getChildAt(parentNode, startIndex))->variableName) == 0)
return true;

++startIndex;
}
return false;
}

//same as above, but checks for duplicate non-member function
bool isDuplicateNonMember(symbolNode* parentNode, int startIndex, char* identifierName)
{
while(startIndex < parentNode->numChildren())
{
if(isNonMemberFunction(getChildAt(parentNode, startIndex)) && strcmp(identifierName, ((Function*) getChildAt(parentNode, startIndex))->functionName) == 0)
return true;

++startIndex;
}
return false;
}

//continuing in this pattern...
bool isDuplicateMember(symbolNode* parentNode, int startIndex, char* identifierName)
{
while(startIndex < parentNode->numChildren())
{
if(isMemberFunction(getChildAt(parentNode, startIndex)) && strcmp(identifierName, ((Function*) getChildAt(parentNode, startIndex))->functionName) == 0)
return true;

++startIndex;
}
return false;
}

//another continuation
bool isDuplicateClass(symbolNode* parentNode, int startIndex, char* identifierName)
{
while(startIndex < parentNode->numChildren())
{
if(isClass(getChildAt(parentNode, startIndex)) && strcmp(identifierName, ((Class*) getChildAt(parentNode, startIndex))->className) == 0)
return true;

++startIndex;
}
return false;
}


//this function does all the work for the following functions with respect to finding if things are defined, their type, and the location of their code
//myType is 0 for a local or global variable, 1 for a Non-Member function, and 2 for a class name
//identifierName is the name of a function, or local or global variable, or a class name
//identifierType is the name of the type of the variable or the function's return type
//if identifierType is NULL, then type checking is not performed
//Note: This function handles all work except finding member functions, which is handled by the function below
	symbolNode* generalFind(int myType, char* identifierName, Type* identifierType)
	{
	symbolNode* travelPointer = currentPointer;
	symbolNode* returnPointer = NULL;
	Variable* VariableCastObject;
	Function* FunctionCastObject;
	int tempIndex = 0;
	int innerIndex = 0;
	if(travelPointer == NULL)
	return NULL;


//searching for a local or global variable.
//if identifierType is NULL, that means ignore the type. Otherwise, type matching occurs.
	if(myType == 0)
	{
	while(travelPointer != NULL)
		{
		while(tempIndex < travelPointer->numChildren())
		{
			if(isVariable(getChildAt(travelPointer, tempIndex)))
			{
				VariableCastObject = (Variable*) travelPointer->childrenBlockList[tempIndex];
			
				//if this is true, this means we have a match for variableName. Next, we  will need to see if the type matches (or if it needs to match)
				if(strcmp(VariableCastObject->variableName, identifierName) == 0)
				{
					//if this is true, then the variable is the wrong type, so an error has occurred.
					if(identifierType != NULL && (*(VariableCastObject->variableType)) != *identifierType)
					return NULL;

					return getChildAt(travelPointer, tempIndex);
				}
			}
	

			++tempIndex;
		}

		travelPointer = travelPointer->parentBlock;
		tempIndex = 0;
		}
	return NULL;

	}
	else if(myType == 1)
	{
	travelPointer = root;
	while(travelPointer != NULL)
	{
		while(tempIndex < travelPointer->numChildren())
		{
		if(isNonMemberFunction(getChildAt(travelPointer, tempIndex)))
		{
			FunctionCastObject = ((Function*) getChildAt(travelPointer, tempIndex));
			if(strcmp(FunctionCastObject->functionName, identifierName) == 0)
			{
				if(identifierType != NULL && (*(FunctionCastObject->functionReturnType)) != *identifierType)
				return NULL;

				return getChildAt(travelPointer, tempIndex);
			
			}


		}

		++tempIndex;
		}
		tempIndex = 0;
		travelPointer = travelPointer->parentBlock;

	}
	return NULL;
	}	

//if we're here, then we're searching for a class
	else
{
travelPointer = root;
	while(tempIndex < travelPointer->numChildren())
	{
	if(isClass(getChildAt(travelPointer, tempIndex)) && strcmp(identifierName, ((Class*) getChildAt(travelPointer, tempIndex))->className) == 0)
	{
	return getChildAt(travelPointer, tempIndex);

	}
	++tempIndex;

	}


return NULL;

}


return NULL;

	}
	

//className is name of desired class, memberFunctionName is name of desired member function, and memberFunctionReturnType is the name of the function's return type
//if memberFunctionReturnType is NULL, then the type of the function will be ignored.
//this function looks for a specific member function's symbol node, and returns it if it finds it
//if memberFunctionReturnType isn't NULL, then this must also match the function's return type.
//otherwise, NULL is returned.
symbolNode* memberFunctionFind(char* className, char* memberFunctionName, Type* memberFunctionReturnType)
{
symbolNode* travelPointer;
Function* FunctionCastObject;
int tempIndex = 0;

//after this, travelPointer equals the symbolNode* containing the class definition, or NULL if an error happened.
travelPointer = generalFind(2, className, NULL);

if(travelPointer == NULL)
return NULL;


//now looking for the member function inside of this class
while(tempIndex < travelPointer->numChildren())
{
if(isMemberFunction(getChildAt(travelPointer, tempIndex)))
{
FunctionCastObject = (Function*) getChildAt(travelPointer, tempIndex);
if(strcmp(FunctionCastObject->functionName, memberFunctionName) == 0)
{
	if(memberFunctionReturnType != NULL && (*(FunctionCastObject->functionReturnType)) !=  *memberFunctionReturnType)
	return NULL;

	return getChildAt(travelPointer, tempIndex);

}


}



++tempIndex;
}
return NULL;
}


public:

bool isMemberFunc()
{
symbolNode* travelPointer = currentPointer;
while(travelPointer->symbolType != MEMBER_FUNCTION && travelPointer->symbolType != NON_MEMBER_FUNCTION)
travelPointer = travelPointer->parentBlock;

if(travelPointer->symbolType == MEMBER_FUNCTION)
return true;

return false;
}
	symbolNode* getVar(char* identifierName, Type* varType)
	{
	return generalFind(0, identifierName, varType);
	}

	symbolNode* getNonMemberFunction(char* identifierName, Type* returnType)
	{
	return generalFind(1, identifierName, returnType);
	}

	symbolNode* getClass(char* identifierName)
	{
	return generalFind(2, identifierName, NULL);
	}

	symbolNode* getMemberFunction(char* className, char* functionName, Type* returnType)
	{
	
	bool debugMode = false;
	if(debugMode)
	std::cout << "in symbolTable.getMemberFunction() with class name of " << className << ", functionName of " << functionName <<  " currentPointer type of " << currentPointer->symbolType << "  and NULL returnType" << std::endl;
	return memberFunctionFind(className, functionName, returnType);
	}


	int getNonMemberFunctionLocation(char* identifierName, Type* returnType)
	{
	Function* myFunction = (Function*) generalFind(1, identifierName, returnType);
	if(myFunction == NULL)
	return -1;

	return myFunction->firstInstructionInCode;
	}

	int getClassLocation(char* identifierName)
	{
	Class* myClass = (Class*) generalFind(2, identifierName, NULL);
	if(myClass == NULL)
	return -1;

	return myClass->firstInstructionInCode;
	}

	int getMemberFunctionLocation(char* className, char* functionName, Type* returnType)
	{
	Function* myFunction = (Function*) memberFunctionFind(className, functionName, returnType);
	if(myFunction == NULL)
	return -1;

	return myFunction->firstInstructionInCode;
	}





//here are where the important functions are for adding symbols to the symbol table.
//this is also where any checks for duplicates and redefinitions occurs


//returns a parameter node with the same variable name as varName in myFunction, or NULL if this variable is not in the parameter list
symbolNode* variableFindParameter(Function* myFunction, char* varName)
{
for(int i = 0; i < myFunction->parameterList.size(); ++i)
if(strcmp(myFunction->parameterList[i]->variableName, varName) == 0)
return myFunction->parameterList[i];

return NULL;
}


//checks only the children of parentNode (and then parameters of parentNode, if it's a functionNode) for a variable with the same name
//returns NULL if this node is not found, and returns the first matching node if a matching node is found
symbolNode* variableFindLocal(symbolNode* parentNode, char* varName)
{
int myIndex = 0;
symbolNode* travelPointer = parentNode;
symbolNode* returnPointer = NULL;

//std::cout << "in variableFindLocal(), type of start node is " << parentNode->symbolType << ", varName is " << varName << std::endl;
while(myIndex < travelPointer->numChildren())
{
if(isVariable(getChildAt(travelPointer, myIndex)) && strcmp(varName, ((Variable*)getChildAt(travelPointer, myIndex))->variableName) == 0)
	return getChildAt(travelPointer, myIndex);

++myIndex;
}

if(parentNode->symbolType == CLASS)
{
myIndex = 0;
while(myIndex < ((Class*)travelPointer)->MemberVariableList.size())
{
	if(strcmp( ((Class*)travelPointer)->MemberVariableList[myIndex]->variableName, varName) == 0)
		return ((Class*)travelPointer)->MemberVariableList[myIndex];
++myIndex;
	
}
}

if(isFunction(parentNode))
{
returnPointer = variableFindParameter((Function*) parentNode, varName);
if(returnPointer != NULL)
return returnPointer;
}

return NULL;
}


//used to find the correct variable definition within a block.
//starting from parentNode, searches outward to higher and higher scopes for a matching variable.
//this function doesn't search at the member function and global function level (it stops when it goes out of the scope of a function).
//the innermost defined variableNode with a matching name to varName is returned, or NULL if this is not found from the search
//Note: This function also checks the parameters of the function of the block. It checks them last, more specifically.
symbolNode* variableFindBlock(symbolNode* parentNode, char* varName)
{
symbolNode* travelPointer = parentNode;
symbolNode* returnPointer = NULL;

while(travelPointer->symbolType == BLOCK || travelPointer->symbolType == NON_MEMBER_FUNCTION || travelPointer->symbolType == MEMBER_FUNCTION)
{
returnPointer = variableFindLocal(parentNode, varName);
if(returnPointer != NULL)
return returnPointer;

travelPointer = travelPointer->parentBlock;
}

return NULL;
}


//checks the parent node's children, and all of the parent node's ancestor's immediate children for a Variable with the same name.
//if this is not found, NULL is returned. Otherwise, the first matching node found is returned
symbolNode* variableFindTotal(symbolNode* parentNode, char* varName)
{
//std::cout << "in variableFindTotal(), varName was " << varName << std::endl;
int myIndex = 0;
symbolNode* travelNode = parentNode;
symbolNode* returnResult = NULL;
while(travelNode != NULL)
{
returnResult = variableFindLocal(travelNode, varName);
if(returnResult != NULL)
return returnResult;

travelNode = travelNode->parentBlock;

}

return NULL;
}

public:

//this function returns true if symbolType is a builtin type, or is a class name which is defined and visible from parent
//node's immediate children (at the same scope), or the children of the direct ancestors of parent node.
//if the type was not a builtin or already defined, then false is returned. Otherwise, true is returned.
//Note: This function can check if an Array's type is valid
bool isTypeDefined(symbolNode* parentNode, char* symbolType)
{
//first, we must check if symbolType is one of the builtin types:

if(strcmp("PipeLine", symbolType) == 0 || strcmp("Queue", symbolType) == 0 || strcmp("List", symbolType) == 0 || strcmp("Dict", symbolType) == 0
|| strcmp("Array", symbolType) == 0 || strcmp("String", symbolType) == 0 || strcmp("int", symbolType) == 0 || strcmp("void", symbolType) == 0 || 
strcmp("boolean", symbolType) == 0 || strcmp("char", symbolType) == 0 || strcmp("double", symbolType) == 0)
return true;

//if we made it down here, then symbolType is either a class name or is undefined.
symbolNode* travelPointer = parentNode;
int travelIndex = 0;

while(travelPointer != NULL)
{
	while(travelIndex < travelPointer->numChildren())
	{
	if(isClass(getChildAt(travelPointer, travelIndex)) && strcmp(((Class*) getChildAt(travelPointer, travelIndex))->className, symbolType) == 0)
	return true;
	
	++travelIndex;
	}

travelIndex = 0;
travelPointer = travelPointer->parentBlock;
}



return false;
}

//this function handles all adding of variables to the symbol table EXCEPT adding parameters to functions
//to add a parameter to a function, use the addParameter() function instead
//if variable was succesfully added to symbolTable at currentNode, then 0 is returned.
//if this variable was already defined elsewhere, then 1 is returned (an error code)
//if the type name was undefined, then 2 is returned (another error code)
int addVariable(Type* varType, char* varName)
{

SymbolType TypeOfVar = GLOBAL_VARIABLE;
//first, we will check if we are at the global scope or in a class, or at the top level of a function (which could be member or nonmember). If so, then the variable name can't be the name of a class or a reserved word.
//Additionally, the varType must be either a builtin type (like int) or an already defined class name.
if(currentPointer->symbolType == GLOBAL_SCOPE || currentPointer->symbolType == CLASS || currentPointer->symbolType == NON_MEMBER_FUNCTION || currentPointer->symbolType == MEMBER_FUNCTION)
{
if(currentPointer->symbolType == GLOBAL_SCOPE)
TypeOfVar = GLOBAL_VARIABLE;
else if(currentPointer->symbolType == CLASS)
TypeOfVar = MEMBER_VARIABLE;
else
TypeOfVar = FUNCTION_LOCAL_VARIABLE;
//if this is true, then this means that the variable was already declared elsewhere at the same scope. Error code 1 is thus returned, indicating a duplicate
if(variableFindLocal(currentPointer, varName) != NULL)
{
std::cout << "Error 1 in addVariable()\n";
return 1;
}
//if this is true, then the type is undefined, so an error code of 2 is returned.
if(isTypeDefined(currentPointer, getBaseType(varType)) == false)
{
std::cout << "Error 2 in addVariable()\n";
return 2;
}
}

//now, we need to do a check for duplicates declared in the same function, which uses a different helper function.
else if(currentPointer->symbolType == BLOCK)
{
TypeOfVar = FUNCTION_LOCAL_VARIABLE;

//indicates redefinition of variable
if(variableFindBlock(currentPointer, varName) != NULL)
return 1;

//indicates undefined type name.
if(isTypeDefined(currentPointer, getBaseType(varType)) == false)
return 2;
}

if(TypeOfVar == GLOBAL_VARIABLE)
{
/*std::cout << nextUniqueNum << std::endl;
std::cout << *varType << std::endl;
std::cout << varName << std::endl;
*/
currentPointer->insertChild(new Variable(GLOBAL_VARIABLE, nextUniqueNum, varType, varName, currentPointer));
}

else if(TypeOfVar == MEMBER_VARIABLE)
{
((Class*) currentPointer)->addMemberVariable(new Member_Variable(nextUniqueNum, varType, varName, currentPointer));
}
else
currentPointer->insertChild(new Variable(FUNCTION_LOCAL_VARIABLE, nextUniqueNum, varType, varName, currentPointer));

++nextUniqueNum;
return 0;
}

//DEBUGGED THROUGH HERE!

//this function handles adding parameters to functions.
//it assumes that the currentNode is either memberFunction or NonMemberFunction. Otherwise, an error occurs.
//the function returns 0 after succesfully adding a variable. It returns 1 if varName was another parameter defined in the same parameter list 
//the function returns 2 if varType if undefined
int addParameter(Type* varType, char* varName)
{
if(currentPointer->symbolType != MEMBER_FUNCTION && currentPointer->symbolType != NON_MEMBER_FUNCTION)
{
return 3;
}

for(int i = 0; i < ((Function*) currentPointer)->parameterList.size(); ++i)
{
if(strcmp(varName, ((Function*) currentPointer)->parameterList[i]->variableName) == 0)
return 1;
}

if(isTypeDefined(currentPointer, getBaseType(varType)) == false)
return 2;

((Function*) currentPointer)->addParameter(new Parameter(nextUniqueNum, varType, varName, currentPointer));
++nextUniqueNum;
return 0;


}

int addBlock()
{
currentPointer->insertChild((symbolNode*) new Block(nextUniqueNum, currentPointer));
++nextUniqueNum;
currentPointer = currentPointer->childrenBlockList[currentPointer->numChildren() - 1];
return 0;
}

//for now, classes can only be declared at the global scope.
//if className was already defined or is a builtin type, then error code 1 is returned
//otherwise, className is added to the symbol table at the global scope, and currentPointer
//is set to point to the newly created class node (currentNode is set one level deeper), and 0 is returned
int addClass(char* className)
{
if(symbolDebug)
std::cout << "Adding class " << className << " to symbol table now. " << std::endl;
if(currentPointer->symbolType != GLOBAL_SCOPE)
return 2;

if(isTypeDefined(currentPointer, className) == true)
return 1;

currentPointer->insertChild(new Class(nextUniqueNum, className, currentPointer));
++nextUniqueNum;
currentPointer = currentPointer->childrenBlockList[currentPointer->childrenBlockList.size() - 1];
if(symbolDebug)
std::cout << "Finished adding class " << className << " to symbol table." << std::endl;
return 0;

}

//if currentNode is not the global scope or a class, then an error code of 4 is returned
//if a function with the same name was defined at the same scope, then an error code of 3 is returned
//if funcName is a reserved word or a type, then an error code of 2 is returned
//if returnType is undefined, then an error code of 1 is returned.
//otherwise, this function is added to the symbol table, and currentPointer is set to
//point to the node containing this function (after this function finishes, calling addParameter will work)
int addFunction(Type* returnType, char* funcName)
{

if(currentPointer->symbolType != GLOBAL_SCOPE && currentPointer->symbolType != CLASS)
return 4;


if(isTypeDefined(currentPointer, funcName) && !(generalFind(2, funcName, NULL) != NULL && currentPointer->symbolType == CLASS ))
return 2;

if(!isTypeDefined(currentPointer, getBaseType(returnType)))
return 1;

//searching for non-member functions at the same level
if(currentPointer->symbolType == GLOBAL_SCOPE)
{
	for(int i = 0; i < currentPointer->numChildren(); ++i)
	{
		if(getChildAt(currentPointer, i)->symbolType == NON_MEMBER_FUNCTION && strcmp(((Function*) getChildAt(currentPointer, i))->functionName, funcName) == 0)
return 3;
	}

currentPointer->insertChild(new Non_Member_Function(nextUniqueNum, returnType, funcName, currentPointer));
currentPointer = currentPointer->childrenBlockList[currentPointer->numChildren() - 1];
++nextUniqueNum;
return 0;
}

//now checking for a member function at the same level.
else
{
	for(int i = 0; i < currentPointer->numChildren(); ++i)
	{
	if(getChildAt(currentPointer, i)->symbolType == MEMBER_FUNCTION && strcmp(((Function*) getChildAt(currentPointer, i))->functionName, funcName) == 0)
	return 3;
	}

((Class*) currentPointer)->addMemberFunction(new Member_Function(nextUniqueNum, returnType, funcName, currentPointer));
currentPointer = ((Class*)currentPointer)->MemberFunctionList[((Class*)currentPointer)->MemberFunctionList.size() - 1];
++nextUniqueNum;
return 0;
}




}

public:

Function* getNonMemberFunction(char* funcName)
{
symbolNode* varInfo = generalFind(1, funcName, NULL);


return ((Function*) varInfo);
}

//returns type of variable
Type* getTypeOfVariable(char* myVariableName)
{
if(strcmp(myVariableName, "this") == 0)
{
symbolNode* travelPointer = NULL;
for(travelPointer = currentPointer; travelPointer->symbolType != CLASS; travelPointer = travelPointer->parentBlock)
{}
return new Type(   ((Class*)travelPointer)->className);

}
symbolNode* varInfo = variableFindTotal(currentPointer, myVariableName);
if(varInfo == NULL)
{
std::cout << "NOTE: in getTypeOfVariable() for " << myVariableName << ", NULL was returned for variable type (variable definition not found)\n";
return NULL;
}
return ((Variable*) varInfo)->variableType;
}

//DEBUGGED THROUGH HERE! 

Function* getMemberFunctionOfObject(char* objectName, char* memberFuncName)
{
if(generalFind(2, objectName, NULL) != NULL)
{
Function* tempFunc = getConstructorOfClass(objectName);
return tempFunc;
}

bool parseDebug = false;
if(parseDebug)
std::cout << "in getMemberFunctionOfObject() objectName was " << objectName << " and memberFuncName was " << memberFuncName << std::endl;
Type* varInfo = getTypeOfVariable(objectName);
if(!varInfo->isPrimitive)
{
std::cout << "Error: Attempted to locate member function of a builtin type such as an array, list or dict. Terminating execution now... " << std::endl;
exit(1);
}
char className[1000];
strcpy(className, varInfo->primitiveName);

return (Function*) getMemberFunctionOfClass(className, memberFuncName);
}

Function* getMemberFunctionOfClass(char* className, char* memberFunctionName)
{
bool parseDebug = false;
if(parseDebug)
std::cout << "in getMemberFunctionOfClass(), className was " << className << " and memberFunctionName was " << memberFunctionName << std::endl;
symbolNode* varInfo = generalFind(2, className, NULL);
for(int i = 0; i < ((Class*) varInfo)->MemberFunctionList.size(); ++i)
{
	if(strcmp(memberFunctionName, ( ((Class*) varInfo)->MemberFunctionList[i])->functionName) == 0)
		return ((Class*) varInfo)->MemberFunctionList[i];
}
return NULL;
}

Function* getFuncNode()
{
symbolNode* travelPointer = currentPointer;

while(travelPointer != NULL && travelPointer->symbolType != NON_MEMBER_FUNCTION
&& travelPointer->symbolType != MEMBER_FUNCTION)
travelPointer = travelPointer->parentBlock;

return (Function*) travelPointer;
}

Type* getFunctionReturnType(symbolNode* funcNode)
{
return ((Function*)funcNode)->functionReturnType;
}


symbolNode* getFunction(char* funcName)
{
symbolNode* travelPointer = currentPointer;

while(travelPointer != NULL)
{
	if(travelPointer->symbolType == GLOBAL_SCOPE)
	{
	for(int i = 0; i < travelPointer->numChildren(); ++i)
	{
	if( travelPointer->childrenBlockList[i]->symbolType == MEMBER_FUNCTION || travelPointer->childrenBlockList[i]->symbolType == NON_MEMBER_FUNCTION)
	{
		if( strcmp(((Function*)travelPointer->childrenBlockList[i])->functionName, funcName) == 0)
			return travelPointer->childrenBlockList[i];
	}
	}
	}
	else if(travelPointer->symbolType == CLASS)
	{
		for(int i = 0; i < ((Class*)travelPointer)->MemberFunctionList.size(); ++i)
		{
		if( strcmp(((Class*)travelPointer)->MemberFunctionList[i]->functionName, funcName) == 0)
			return ((Class*)travelPointer)->MemberFunctionList[i];
		}
	}
	travelPointer = travelPointer->parentBlock;
}
}

Function* getConstructorOfObject(char* objectName)
{
Type* myClassType = getTypeOfVariable(objectName);
Function* myFunction = getMemberFunctionOfClass(myClassType->primitiveName, myClassType->primitiveName);
return myFunction;
}

bool isConstructor(symbolNode* funcNode)
{
char* myString = ((Function*)funcNode)->functionName;
symbolNode* travelPointer = funcNode;

while(travelPointer != NULL)
{
if(travelPointer->symbolType == CLASS)
{
if(strcmp(myString, ((Class*)travelPointer)->className) == 0)
return true;

else
return false;
}
travelPointer = travelPointer->parentBlock;
}

return false;
}

Function* getConstructorOfClass(char* className)
{
Class* myClass = ((Class*)generalFind(2, className, NULL));
for(int i = 0; i < myClass->MemberFunctionList.size(); ++i)
{
if(strcmp(myClass->MemberFunctionList[i]->functionName, className) == 0)
return myClass->MemberFunctionList[i];
} 

return NULL;
}

Type* getExpectedTypeOfBuiltinParameter(char* objName, Builtin_Function myFunc, int myIndex)
{
Type* varType = getTypeOfVariable(objName);
if(myFunc == GET)
return varType->nextType;

else if(myFunc == PUT)
return varType->nextType;

else if(myFunc == CONTAINS)
return varType->nextType;

else if(myFunc == AT)
return new Type((char*)"int");

else if(myFunc == UPDATE)
{
if(myIndex == 0)
return new Type((char*)"int");

else
return varType->nextType;
}

else if(myFunc == FIND)
return varType->nextType;

else if(myFunc == PUSH)
return varType->nextType;

else
return new Type((char*)"void");

}
symbolNode* getMemberVarOfObject(char* objectName, char* memberField)
{
Type* classType = getTypeOfVariable(objectName);
if(classType == NULL || !classType->isPrimitive)
{
std::cout << "Error: Class of variable not found\n";
exit(1);
}
symbolNode* classLocation = generalFind(2, classType->primitiveName, NULL);
for(int i = 0; i < ((Class*)classLocation)->MemberVariableList.size(); ++i)
{
if( strcmp(((Class*)classLocation)->MemberVariableList[i]->variableName, memberField) == 0)
return ((Class*)classLocation)->MemberVariableList[i];
}
return NULL;
}

int findMemberVarIndex(char* objectName, char* memberField)
{
Type* classType = getTypeOfVariable(objectName);
if(classType == NULL || !classType->isPrimitive)
{
std::cout << "Error: Class of variable not found]n";
exit(1);
}
symbolNode* classLocation = generalFind(2, classType->primitiveName, NULL);
for(int i = 0; i < ((Class*)classLocation)->MemberVariableList.size(); ++i)
{
if( strcmp( ((Class*)classLocation)->MemberVariableList[i]->variableName, memberField) == 0)
return i;
}

return -1;
}


Type* getMemberVarTypeOfObject(char* objectName, char* memberField)
{
Type* classType = getTypeOfVariable(objectName);
if(classType == NULL || !classType->isPrimitive)
{
std::cout << "Error in function getMemberVarTypeOfObject()" << std::endl;
exit(1);
}
symbolNode* classLocation = generalFind(2, classType->primitiveName, NULL);
if(classLocation == NULL)
{
std::cout << "Error: " << classType->primitiveName << " was not a defined class name" << std::endl;
exit(1);
}

for(int i = 0; i < ((Class*)classLocation)->MemberVariableList.size(); ++i)
{
if(strcmp(memberField, ((Class*)classLocation)->MemberVariableList[i]->variableName) == 0)
return ((Class*)classLocation)->MemberVariableList[i]->variableType;
}

std::cout << "Error: No such member field as " << memberField << " in class " << classType->primitiveName << ", which " << objectName << " is an instance of." << std::endl;
exit(1);

return NULL;
}

public:

bool isVariable(char* myString)
{
return generalFind(0, myString, NULL) != NULL;
}

bool isTypeDefined(char* myString)
{
return isTypeDefined(root, myString);
}

bool isDuplicateMemberVar(char* myString)
{
return variableFindLocal(currentPointer, myString) != NULL;
}

bool isDuplicateParam(char* myIdentifier)
{
return variableFindParameter(((Function*) currentPointer), myIdentifier) != NULL;
}

symbolNode* variableFindTotal(char* varName)
{
return variableFindTotal(currentPointer, varName);
}


int getIndexOfMemberVar(Member_Variable* member, char* myStr)
{
Class* myClass = ((Class*)member->parentBlock);
for(int i = 0; i < myClass->MemberVariableList.size(); ++i)
{
if(strcmp(myClass->MemberVariableList[i]->variableName, myStr) == 0)
return i;
}

return -1;
}

string symbolOutput(SymbolType myType)
{
if(myType == GLOBAL_SCOPE)
return string("Global_Scope");

if(myType == GLOBAL_VARIABLE)
return string("Global_Variable");

if(myType == NON_MEMBER_FUNCTION)
return string("Non-Member_Function");

if(myType == FUNCTION_LOCAL_VARIABLE)
return string("Local_Variable");

if(myType == PARAMETER)
return string("Parameter");

if(myType == BLOCK)
return string("Block");

if(myType == CLASS)
return string("Class");

if(myType == MEMBER_FUNCTION)
return string("Member_Function");

return string("Member_Variable");
}
void symbolTraceHelper(symbolNode* myPointer, int indentationLevel)
{
if(myPointer->symbolType == GLOBAL_SCOPE)
{
std::cout << "Global Scope " << std::endl;
//std::cout << "Error: encountered global scope as child of root" << std::endl;
//return;
}
for(int i = 0; i < indentationLevel; ++i)
std::cout << "\t";

if(myPointer->symbolType == GLOBAL_VARIABLE)
{
std::cout << "Global Variable: ";
}

if(myPointer->symbolType == NON_MEMBER_FUNCTION)
{
std::cout << "Non-Member Function: ";
}

if(myPointer->symbolType == FUNCTION_LOCAL_VARIABLE)
std::cout << "Local Variable: ";

if(myPointer->symbolType == PARAMETER)
std::cout << "Parameter: ";

if(myPointer->symbolType == BLOCK)
std::cout << "Block:\n";

if(myPointer->symbolType == MEMBER_FUNCTION)
std::cout << "Member Function: ";

if(myPointer->symbolType == MEMBER_VARIABLE)
std::cout << "Member Variable: ";

if(myPointer->symbolType == GLOBAL_VARIABLE || myPointer->symbolType == FUNCTION_LOCAL_VARIABLE || myPointer->symbolType == PARAMETER || myPointer->symbolType == MEMBER_VARIABLE)
{
std::cout << "Variable Type: " << *(((Variable*) myPointer)->variableType) << "\tVariable Name: " << ((Variable*) myPointer)->variableName << std::endl; 
}

if(myPointer->symbolType == CLASS)
{
std::cout << "Class Name: " << ((Class*) myPointer)->className << std::endl;

for(int i = 0; i < ((Class*) myPointer)->MemberVariableList.size(); ++i)
{
symbolTraceHelper(((Class*) myPointer)->MemberVariableList[i], indentationLevel + 1);
}

for(int i = 0; i < ((Class*) myPointer)->MemberFunctionList.size(); ++i)
{
symbolTraceHelper(((Class*) myPointer)->MemberFunctionList[i], indentationLevel + 1);
}
}

if(myPointer->symbolType == MEMBER_FUNCTION || myPointer->symbolType == NON_MEMBER_FUNCTION)
{
std::cout << "\tFunction Return Type: " << (*((Function*)myPointer)->functionReturnType) << "\tFunction Name: " << ((Function*) myPointer)->functionName << std::endl;

for(int i = 0; i < ((Function*)myPointer)->parameterList.size(); ++i)
{
symbolTraceHelper(((Function*) myPointer)->parameterList[i], indentationLevel + 1);
}
}


for(int i = 0; i < myPointer->numChildren(); ++i)
{
symbolTraceHelper(getChildAt(myPointer, i), indentationLevel + 1);
}



}


//used for debugging to print the contents of the symbol table. Uses a depth-first traversal
void symbolTrace()
{
symbolNode* myNode = root;
std::cout << "Type of root: " << symbolOutput(root->symbolType) << std::endl;
std::cout << "Now listing the children of the root: " << std::endl;

for(int i = 0; i < root->numChildren(); ++i)
{
symbolTraceHelper(getChildAt(root, i), 1);
}

std::cout << "End of symbol table" << std::endl;
}

void moveToRoot()
{
currentPointer = root;
}

void addStringLit(char* newLit)
{
for(int i = 0; i < myStringLits.size(); ++i)
{
if(strcmp(newLit, myStringLits[i]->myStr) == 0)
return;
}
myStringLits.push_back(new stringLit(nextUniqueNum, root, newLit));
++nextUniqueNum;
}

symbolNode* getStringLitNode(int myNum)
{
for(int i = 0; i < myStringLits.size(); ++i)
{
if(myStringLits[i]->uniqueIdentifierNum == myNum)
return (symbolNode*)myStringLits[i];
}
std::cout << "Error: stringLit not found in symbolTable\n";
exit(1);
return NULL;
}

symbolNode* getStringLitNode(char* desiredString)
{
for(int i = 0; i < myStringLits.size(); ++i)
{
if(strcmp(desiredString, myStringLits[i]->myStr) == 0)
return ((symbolNode*)myStringLits[i]);
}
std::cout << "Error: stringLit not found in symbolTable.\n";
exit(1);
return NULL;
}

int getStringNum(char* myStr)
{
symbolNode* myPointer = ((symbolNode*)getStringLitNode(myStr));

return myPointer->uniqueIdentifierNum;
}

void enterClass(char* myClassName)
{
currentPointer = root;

for(int i = 0; i < currentPointer->childrenBlockList.size(); ++i)
{
if(currentPointer->childrenBlockList[i]->symbolType == CLASS
&& strcmp(((Class*) (currentPointer->childrenBlockList[i]))->className, myClassName) == 0)
{
currentPointer = currentPointer->childrenBlockList[i];
return;
}
}

std::cout << "Error: Class definition of class " << myClassName << " not found\n";
exit(1);
}

void enterNonMemberFunction(char* funcName)
{
currentPointer = root;

for(int i = 0; i < currentPointer->childrenBlockList.size(); ++i)
{
if(currentPointer->childrenBlockList[i]->symbolType == NON_MEMBER_FUNCTION
&& strcmp( ((Function*)currentPointer->childrenBlockList[i])->functionName, funcName) == 0)
{
currentPointer = currentPointer->childrenBlockList[i];
return;
}
}

std::cout << "Error: Non-Member Function definition of name " << funcName << " was not found.\n";
exit(1);
}

symbolNode* getBreakInstructionNode(int numBlocksBack)
{
symbolNode* travelPointer = currentPointer;

for(int i = 0; i < numBlocksBack; ++i)
travelPointer = travelPointer->parentBlock;

return travelPointer;

}

symbolNode* getContinueInstructionNode(int numBlocksBack)
{
symbolNode* travelPointer = currentPointer;
for(int i = 0; i < numBlocksBack; ++i)
travelPointer = travelPointer->parentBlock;
return travelPointer;
}

void enterMemberFunction(char* memFuncName)
{
if(currentPointer->symbolType != CLASS)
{
std::cout << "Error: In SymbolTable.h, enterMemberFunction is called while current node was not a class.\n";
exit(1);
}

for(int i = 0; i < ((Class*)currentPointer)->MemberFunctionList.size(); ++i)
{
if(strcmp( ((Function*) ((Class*)currentPointer)->MemberFunctionList[i])->functionName, memFuncName) == 0)
{
currentPointer = ((Class*)currentPointer)->MemberFunctionList[i];
return;
}
}
std::cout << "Error: In symbolTable.h, member function with the name " << memFuncName << " was not found\n";
exit(1);
}

//Note: Numbered starting from 0
void enterBlock(int blockNum)
{
int blockCounter = -1;
if(currentPointer->symbolType != NON_MEMBER_FUNCTION && currentPointer->symbolType != MEMBER_FUNCTION && currentPointer->symbolType != BLOCK)
{
std::cout << "Error: Enter block was called when current pointer was not a function or a block\n";
exit(1);
}

for(int i = 0; i < currentPointer->childrenBlockList.size(); ++i)
{
if(currentPointer->childrenBlockList[i]->symbolType == BLOCK)
{
++blockCounter;
if(blockCounter >= blockNum)
{
currentPointer = currentPointer->childrenBlockList[i];
return;
}
}
}
std::cout << "Error: Block with number " << blockNum  << " was not found. Highest reached (starting from -1) was " << blockCounter << std::endl;
exit(1);
}


//Note: Numbered starting from 0
symbolNode* getBlockNum(int blockNum)
{
int blockCounter = -1;
if(currentPointer->symbolType != NON_MEMBER_FUNCTION && currentPointer->symbolType != MEMBER_FUNCTION && currentPointer->symbolType != BLOCK)
{
std::cout << "Error: Enter block was called when current pointer was not a function or a block\n";
exit(1);
}

for(int i = 0; i < currentPointer->childrenBlockList.size(); ++i)
{
if(currentPointer->childrenBlockList[i]->symbolType == BLOCK)
{
++blockCounter;
if(blockCounter >= blockNum)
{
return currentPointer->childrenBlockList[i];
}
}
}
std::cout << "Error: Block with number " << blockNum  << " was not found. Highest reached (starting from -1) was " << blockCounter << std::endl;
exit(1);
return NULL;
}


Type* getTypeOfMemberFunctionParameter(char* objName, char* funcName, int varPos)
{
if(generalFind(2, objName, NULL) != NULL)
{
//std::cout << "in getTypeOfMemberFunctionparameter()\n";
Function* tempFunc = (Function*) getConstructorOfClass(objName);
return tempFunc->parameterList[varPos]->variableType;
}

Type* classType = getTypeOfVariable(objName);
Class* myClass = (Class*) generalFind(2, classType->primitiveName, NULL);

Member_Function* myMemberFunc = NULL;

for(int i = 0; i < myClass->MemberFunctionList.size(); ++i)
{
if(strcmp(funcName, myClass->MemberFunctionList[i]->functionName) == 0)
{
myMemberFunc = myClass->MemberFunctionList[i];
break;
}

}

Parameter* myParam = myMemberFunc->parameterList[varPos];
return myParam->variableType;
}

Type* getTypeOfNonMemberFunctionParameter(char* funcName, int varPos)
{
Function* myFunc = NULL;

for(int i = 0; i < root->numChildren(); ++i)
{
if( root->childrenBlockList[i]->symbolType == NON_MEMBER_FUNCTION
&& strcmp(funcName, ((Function*)(root->childrenBlockList[i]))->functionName) == 0)
{
myFunc = (Function*) root->childrenBlockList[i];
break;
}
}
Parameter* myParam = myFunc->parameterList[varPos];
return myParam->variableType;

}

symbolNode* getNonMemberFunction(int idenNum)
{
for(int i = 0; i < root->numChildren(); ++i)
{
if(root->childrenBlockList[i]->uniqueIdentifierNum == idenNum)
return root->childrenBlockList[i];
}
return NULL;
}

	private:
	symbolNode* root;
	symbolNode* currentPointer;
	int indexInCurrentChild;
	int nextUniqueNum;

public:
vector<stringLit*> myStringLits;


};

#endif
