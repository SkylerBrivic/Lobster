#include "stackTableStructure.h"
#include <stack>
#include <mutex>
#include <list>
#include <cmath>
#include "UserQueue.h"
#include "UserArray.h"
#include "UserList.h"
#include "Instructions.h"
#include "UserDefinedObject.h"
#include "PipeLineStruct.h"

class runTimeObject
{
public:

runTimeObject(bool isMainNew, stackTableStructure* newGlob, std::list<long long*>* newList, std::mutex* newMut, std::unordered_map<int, PipeLineStruct>* newMap)
{
isMainThread = isMainNew;
otherThreadsActive = false;
pc = 0;
byteCodeStart = NULL;
memoryAllocationList = newList;
memoryLock = newMut;
pipeLineMap = newMap;
inputQueue = NULL;
outputQueue = NULL;

for(int i = 0; i < 256; ++i)
{
registerArray[i] = 0;
restorationStacks[i] = std::stack<long long>();
}
functionReturnLocations = std::stack<int>();
globalFrame = newGlob;
callStack = stackTableStructure();

isPipeLine = false;
isInputQueueUsed = false;
isOutputQueueUsed = false;
startPosition = 0;

}

template<class ArgType>
int parseArg(Source_Location myLocation, Op_Type myType, int nextSpot, ArgType& returnVal)
{
bool debugger = false;

unsigned char regVal;
switch(myLocation)
{
case REGISTER_VAL:
if(debugger) std::cout << "in parseArg REGISTER_VAL\n";
regVal = byteCodeStart[nextSpot];
returnVal = *((ArgType*)&registerArray[regVal]);
return nextSpot + 1;

case REGISTER_MEM_ADDR:
if(debugger) std::cout << "in parseArg REGISTER_MEM_ADDR\n";
regVal = byteCodeStart[nextSpot];
returnVal = *((ArgType*)registerArray[regVal]);
return nextSpot + 1; 


case CODE_LOCATION:
if(debugger) std::cout << "in parseArg CODE_LOCATION\n";
std::cout << "Error: Code Location not allowed in operator expression\n";
exit(1);
break;

case LITERAL_L:
if(debugger) std::cout << "in parseArg LITERAL_L\n";
	switch(myType)
	{
	case INTEGER_T:
	if(debugger) std::cout << "in parseArg INTERGER_T LITERAL\n";
	returnVal = *((int*)&byteCodeStart[nextSpot]);
	return nextSpot + 4;
	break;

	case DOUBLE_T:
	if(debugger) std::cout << "in parseArg DOUBLE_T LITERAL\n";
	returnVal = *((float*)&byteCodeStart[nextSpot]);
	return nextSpot + 4;
	break;

	case CHAR_T:
	if(debugger) std::cout << "in parseArg CHAR_T LITERAL\n";
	returnVal = *((unsigned char*)&byteCodeStart[nextSpot]);
	return nextSpot + 1;
	break;

	case BOOLEAN_T:
	if(debugger) std::cout << "in parseArg BOOLEAN_T LITERAL\n";
	if(byteCodeStart[nextSpot] != 0)
		returnVal = true;
	else
		returnVal = false;
	return nextSpot + 1;
	break;

	case NULL_T:
	if(debugger) std::cout << "in parseArg NULL_T LITERAL\n";
	returnVal = 0;
	return nextSpot + 1;
	break;

	default:
	if(debugger) std::cout << "in parseArg DEFAULT LITERAL\n";
	*((long long**)&returnVal) = *((long long**)&byteCodeStart[nextSpot]);
	return nextSpot + sizeof(long long **);
	break;

	}

default:
return -1;
}


return -1;
}


void runProgram();


long long registerArray[256];
bool isMainThread;
bool otherThreadsActive;
int pc;
unsigned char* byteCodeStart;
stackTableStructure callStack;
stackTableStructure* globalFrame;
std::stack<long long> restorationStacks[256];
std::stack<int> functionReturnLocations;
std::list<long long *>* memoryAllocationList;
std::mutex* memoryLock;
std::unordered_map<int, PipeLineStruct>* pipeLineMap;

//the following 6 variables are only used in pipeLine threads:
bool isPipeLine;
bool isInputQueueUsed;
bool isOutputQueueUsed;
long long startPosition;
UserQueue* inputQueue;
UserQueue* outputQueue; 


int readInt(unsigned char* byteArray, int offSet)
{
return *((int*)(byteArray + offSet));
}

float readDouble(unsigned char* byteArray, int offSet)
{
return *((float*)(byteArray + offSet));
}

bool readBoolean(unsigned char* byteArray, int offSet)
{
return *((bool*)(byteArray + offSet));
}

template< class param>
void arrayExecute(int regWObjAddr, Builtin_Function funcName, param decidingParameter)
{
UserArray<param>* myArray = *(UserArray<param>**)registerArray[regWObjAddr];
        switch(funcName)
        {
                case SORT:
                        myArray->sort();
                        return;

                case AT:
                        *((param*)&registerArray[RETURN_REGISTER_NUM]) = myArray->at( (long long) *((long long*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]));
                        return;

		case GET:
			registerArray[RETURN_REGISTER_NUM] = (long long) myArray->get( *((param*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]));
			return;

		case CONTAINS:
			*((bool*)&registerArray[RETURN_REGISTER_NUM]) = myArray->contains( *((param*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]));
			return;

                case LENGTH:
                        registerArray[RETURN_REGISTER_NUM] = myArray->length();
                        break;

		case UPDATE:
	myArray->update( *((int*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]), (*(param*)&registerArray[NUMBER_OF_GENERAL_REGISTERS + 1]));
			return;

                default:
                        std::cout << "ERROR: Invalid array function " << funcName << " called\n";
                        exit(1);

        }
}
void builtinFunctionExecute(int regWObjAddr, Op_Type objType, Op_Type innerType, Builtin_Function funcName)
{
//TODO

long long myLongLong = 0;
double myDouble = 0;
unsigned char myChar = 0;
bool myBool = false;
long long* myPointer = NULL;

UserList* myList = NULL;
UserQueue* myQueue = NULL;
//GET, PUT, FIND, CONTAINS, AT, SORT, LENGTH, UPDATE
switch(objType)
{
case ARRAY_T:
	switch(innerType)
	{
	case INTEGER_T:
	arrayExecute(regWObjAddr, funcName, myLongLong);
	return;

	case DOUBLE_T:
	arrayExecute(regWObjAddr, funcName, myDouble);
	return;

	case CHAR_T:
	arrayExecute(regWObjAddr, funcName, myChar);
	return;
	
	case BOOLEAN_T:
	arrayExecute(regWObjAddr, funcName, myBool);
	return;

	default:
	arrayExecute(regWObjAddr, funcName, myPointer);
	return;
	}
break;


case LIST_T:
myList = *(UserList**)registerArray[regWObjAddr];
switch(funcName)
{
case CONTAINS:
*((bool*)&registerArray[RETURN_REGISTER_NUM]) = myList->contains( (long long*)  registerArray[NUMBER_OF_GENERAL_REGISTERS]);
return;

case LENGTH:
registerArray[RETURN_REGISTER_NUM] = myList->length();
return;

case UPDATE:
myList->update( *((int*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]), *((long long**)&registerArray[NUMBER_OF_GENERAL_REGISTERS + 1]));
return;

case AT:
registerArray[RETURN_REGISTER_NUM] = (long long) myList->at( registerArray[NUMBER_OF_GENERAL_REGISTERS]);
return;

case GET:
registerArray[RETURN_REGISTER_NUM] = (long long) myList->get( (long long*) registerArray[NUMBER_OF_GENERAL_REGISTERS]);
return;

case PUSH:
myList->push( (long long*)registerArray[NUMBER_OF_GENERAL_REGISTERS]);
return;

case POP:
*((long long**) &registerArray[RETURN_REGISTER_NUM]) = (long long*) myList->pop();
return;

case FRONT:
*((long long**) &registerArray[RETURN_REGISTER_NUM]) = (long long*) myList->front();
return;

default:
std::cout << "Error Unimplemented List function of " << funcName << " was called\n";
exit(1);
}
break;

case QUEUE_T:
myQueue = *(UserQueue**)registerArray[regWObjAddr];

	switch(funcName)
	{
	case LENGTH:
	registerArray[RETURN_REGISTER_NUM] = myQueue->length();
	return;

	case PUSH:
	myQueue->push( (long long*) registerArray[NUMBER_OF_GENERAL_REGISTERS]);
	return;

	case POP:
	*((long long**)&registerArray[RETURN_REGISTER_NUM]) =  (long long*) myQueue->pop();
	return;

	case FRONT:
	*((long long**)&registerArray[RETURN_REGISTER_NUM]) = (long long*) myQueue->front();
	return;

	default:
	std::cout << "Error: Unimplemented Queue function of " << funcName << " was called\n";
	exit(1);
	}

default:
std::cout << "Error: Unimplemented builtin function execute call\n";
exit(1);


}
return;
}

void* builtinCreateFunc(Op_Type objType, Op_Type optionalType)
{
//TODO

UserArray<long long>* myIntArray = NULL;
UserArray<double>* myDoubleArray = NULL;
UserArray<unsigned char>* myCharArray = NULL;
UserArray<bool>* myBoolArray = NULL;
UserArray<long long*>* myPointerArray = NULL;

UserList* myList = NULL;
UserQueue* myQueue = NULL;
varObj* myVarObj = NULL;

switch(objType)
{
case ARRAY_T:


	switch(optionalType)
	{
	case INTEGER_T:
	myIntArray = new UserArray<long long>( (*((int*)&registerArray[NUMBER_OF_GENERAL_REGISTERS])));
	myVarObj = new varObj();
	myVarObj->myVal = (long long) myIntArray;
	memoryLock->lock();
	memoryAllocationList->push_front((long long*) myVarObj);
	memoryAllocationList->push_front((long long*)myIntArray);
	memoryLock->unlock();
	return &myVarObj->myVal;
	break;

	case DOUBLE_T:
	myDoubleArray = new UserArray<double>(*((int*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]));
	myVarObj = new varObj();
	myVarObj->myVal = (long long) myDoubleArray;
	memoryLock->lock();
	memoryAllocationList->push_front((long long*) myVarObj);
	memoryAllocationList->push_front((long long*)myDoubleArray);
	memoryLock->unlock();
	return &myVarObj->myVal;
	break;

	case CHAR_T:
	myCharArray = new UserArray<unsigned char>(*((int*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]));
	myVarObj = new varObj();
	myVarObj->myVal = (long long) myCharArray;
	memoryLock->lock();
	memoryAllocationList->push_front((long long*)myVarObj);
	memoryAllocationList->push_front((long long*)myCharArray);
	memoryLock->unlock();
	return &myVarObj->myVal;
	break;

	case BOOLEAN_T:
	myBoolArray = new UserArray<bool>(*(int*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]);
	myVarObj = new varObj();
	myVarObj->myVal = (long long) myBoolArray;
	memoryLock->lock();
	memoryAllocationList->push_front((long long*)myVarObj);
	memoryAllocationList->push_front((long long*)myBoolArray);
	memoryLock->unlock();
	return &myVarObj->myVal;
	break;

	default:
	myPointerArray = new UserArray<long long*>(*(int*)&registerArray[NUMBER_OF_GENERAL_REGISTERS]);
	myVarObj = new varObj();
	myVarObj->myVal = (long long) myPointerArray;
	memoryLock->lock();
	memoryAllocationList->push_front((long long*)myVarObj);
	memoryAllocationList->push_front((long long*)myPointerArray);
	memoryLock->unlock();
	return &myVarObj->myVal;
	break;
	}
break;

case LIST_T:
myList = new UserList();
myVarObj = new varObj();
myVarObj->myVal = (long long) myList;
memoryLock->lock();
memoryAllocationList->push_front((long long*)myVarObj);
memoryAllocationList->push_front((long long*)myList);
memoryLock->unlock();
return &myVarObj->myVal;


case QUEUE_T:
myQueue = new UserQueue();
myVarObj = new varObj();
myVarObj->myVal = (long long) myQueue;
memoryLock->lock();
memoryAllocationList->push_front((long long*)myVarObj);
memoryAllocationList->push_front((long long*)myQueue);
memoryLock->unlock();
return &myVarObj->myVal;

default:
std::cout << "Error: Un-implemented builtin create function for Op_Type: " << objType << std::endl;
exit(1);
}
return NULL;
}

};

/*newUserObj = new UserDefinedObject(intLit);
myNewVarObj = new varObj();
myNewVarObj->myVal = (long long)(newUserObj->internalArray);
if(debugger) std::cout << "Address of myNewVarObj->myVal - " << &(myNewVarObj->myVal) << " Treating myVal as a pointer, it equals - " << ((long long**)myNewVarObj->myVal) << " and the address of the internalArray is " <<  newUserObj->internalArray << " value of myNewVarObj->myVal = " << myNewVarObj->myVal << std::endl;
registerArray[THIS_REGISTER_NUM] = (long long) &(myNewVarObj->myVal);
if(debugger) std::cout << "Value of THIS register is now " << *((long long**)&registerArray[THIS_REGISTER_NUM]) << std::endl;
memoryLock->lock();
memoryAllocationList->push_front( (long long*)newUserObj);
memoryAllocationList->push_front( (long long*) myNewVarObj);
*/
