#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <exception>

class preProcessor
{

private: 

class cycleException : public std::exception
{
virtual const char* what() const throw() {return "An exception has occured. Input file contained a cycle of files that included each other, which causes an infinite loop. Terminating compilation now...\n";}
};

class selfIncludeException : public std::exception
{
virtual const char* what() const throw() { return "An exception has occured. File included itself, creating an infinite loop of includes. Terminating compilation now...\n";}
};

class emptyIncludeException : public std::exception
{
virtual const char* what() const throw() { return "An exception has occured. A file used the #include tag, followed by an empty file name. Terminating compilation now...\n";}

};


class graphNodeObject
{
public:
graphNodeObject(int newKey, graphNodeObject* newObject)
{
keyVal = newKey;
nextObject = newObject;
}

int keyVal;
graphNodeObject* nextObject;
};


public: class directedGraph
{

public: directedGraph()
{
myAdjacencyList = std::vector<graphNodeObject*>(100, NULL);
symbolTable = std::map<std::string, int>();
numElements = 0;
colorArray = NULL;
}

private: void insertFirstNode(std::string newFileName)
{
symbolTable[newFileName] = numElements;
++numElements;
myAdjacencyList[0] = new graphNodeObject(0, myAdjacencyList[0]);
previousElement = 0;
return;
}


void addOtherNode(std::string newFileName)
{

if(myAdjacencyList.size() <=  (2 * numElements))
myAdjacencyList.resize(myAdjacencyList.size() * 3, NULL);

std::map<std::string, int>::iterator myIterator = symbolTable.find(newFileName);

int nextElement;
if(myIterator == symbolTable.end())
{
symbolTable[newFileName] = numElements;
++numElements;
nextElement = numElements - 1;
}
else
{
nextElement = symbolTable[newFileName];
}



myAdjacencyList[previousElement] = new graphNodeObject(nextElement, myAdjacencyList[previousElement]);

previousElement = nextElement;

}
public:
void addNext(std::string newFileName)
{
if(numElements == 0)
insertFirstNode(newFileName);

else
addOtherNode(newFileName);
}


enum colorType {WHITE, GREY, BLACK};

bool hasCycleHelper(int currentVertex)
{
colorArray[currentVertex] = GREY;

for(graphNodeObject* currentElement = myAdjacencyList[currentVertex]; currentElement != NULL; currentElement = currentElement->nextObject)
{
if(colorArray[currentElement->keyVal] == WHITE && hasCycleHelper(currentElement->keyVal))
	return true;

else if(colorArray[currentElement->keyVal] == BLACK)
continue;

else // if grey was seen, then true must be returned, as there was a cycle
return true;
}

colorArray[currentVertex] = BLACK;
return false;


}

bool hasCycle()
{
if(myAdjacencyList[0] == NULL)
return false;

colorArray = new colorType[numElements];

for(int i = 0; i < numElements; ++i)
{
colorArray[i] = WHITE;
}

for(int i = 0; i < numElements; ++i)
{
if(colorArray[i] == WHITE && hasCycleHelper(i) == true)
return true;
}
delete[] colorArray;
colorArray = NULL;
return false;
}


private:
std::vector<graphNodeObject*> myAdjacencyList;
std::map<std::string, int> symbolTable;
int numElements;
int previousElement;
colorType* colorArray;
};




public:

preProcessor(){includeCount = 0; myGraph = directedGraph();}
void processFile(std::string inputFileName, std::string outputFileName)
{
std::fstream myInputStream;
std::fstream outputStream;
std::string includeName;
char* fileInputBuffer;
char* fileOutputBuffer;
int fileInputLength;
int currentInputPosition = 0;
int currentOutputPosition = 0;

if(includeCount == 0)
{
outputStream.open(outputFileName.c_str(), std::fstream::out);
outputStream.close();
}

myInputStream.open(inputFileName.c_str(), std::fstream::in);

myInputStream.seekg(0, myInputStream.end);
fileInputLength = myInputStream.tellg();
myInputStream.seekg(0, myInputStream.beg);
fileInputBuffer = new char[fileInputLength + 1];
fileOutputBuffer = new char[fileInputLength + 1];
myInputStream.read(fileInputBuffer, fileInputLength);
myInputStream.close();
fileInputBuffer[fileInputLength] = '\0';

while(currentInputPosition < fileInputLength)
{
if(fileInputBuffer[currentInputPosition] == '/' && fileInputBuffer[currentInputPosition + 1] == '/')
{
currentInputPosition += 2;
while(fileInputBuffer[currentInputPosition] != '\n' && currentInputPosition < fileInputLength)
++currentInputPosition;
++currentInputPosition;
continue;
}

else if(fileInputBuffer[currentInputPosition] == '/' && fileInputBuffer[currentInputPosition + 1] == '*')
{
currentInputPosition += 2;
while(!(fileInputBuffer[currentInputPosition] == '*'  && fileInputBuffer[currentInputPosition + 1] == '/') && currentInputPosition < fileInputLength)
++currentInputPosition;

currentInputPosition += 2;
continue;
}

else if(fileInputBuffer[currentInputPosition] == '\'')
{
fileOutputBuffer[currentOutputPosition++] = '\'';
++currentInputPosition;

if(fileInputBuffer[currentInputPosition] == '\\')
{
fileOutputBuffer[currentOutputPosition] = '\\';
fileOutputBuffer[currentOutputPosition + 1] = fileInputBuffer[currentInputPosition + 1];
currentOutputPosition += 2;
currentInputPosition += 2;
continue;
}
else
{
fileOutputBuffer[currentOutputPosition] = fileInputBuffer[currentInputPosition];
fileOutputBuffer[currentOutputPosition + 1] = fileInputBuffer[currentInputPosition + 1];
currentOutputPosition += 2;
currentInputPosition += 2;
continue;
}

}

else if(fileInputBuffer[currentInputPosition] == '\"')
{
fileOutputBuffer[currentOutputPosition++] = '\"';
++currentInputPosition;

if(fileInputBuffer[currentInputPosition] == '\"')
{
fileOutputBuffer[currentOutputPosition++] = '\"';
++currentInputPosition;
continue;
}

while(!((fileInputBuffer[currentInputPosition] == '\\' && fileInputBuffer[currentInputPosition + 1] == '\"') || (fileInputBuffer[currentInputPosition] == '\"')) && (! (currentInputPosition >= fileInputLength)))
{
fileOutputBuffer[currentOutputPosition++] = fileInputBuffer[currentInputPosition++];
}

if(fileInputBuffer[currentInputPosition] == '\\')
{
fileOutputBuffer[currentOutputPosition++] = '\\';
fileOutputBuffer[currentOutputPosition++] = '\"';

currentInputPosition += 2;
continue;
}
else
{
fileOutputBuffer[currentOutputPosition++] = '\"';
++currentInputPosition;
continue;
}
}

else if(fileInputBuffer[currentInputPosition] == '#' && fileInputBuffer[currentInputPosition + 1] == 'i' 
&& fileInputBuffer[currentInputPosition + 2] == 'n' && fileInputBuffer[currentInputPosition + 3] == 'c'
&& fileInputBuffer[currentInputPosition + 4] == 'l' && fileInputBuffer[currentInputPosition + 5] == 'u'
&& fileInputBuffer[currentInputPosition + 6] == 'd' && fileInputBuffer[currentInputPosition + 7] == 'e' )
{
currentInputPosition += 8;
while(isspace(fileInputBuffer[currentInputPosition]))
++currentInputPosition;

++currentInputPosition;

if(fileInputBuffer[currentInputPosition] == '\"' || fileInputBuffer[currentInputPosition] == '>')
throw emptyIncludeException();

while(! (fileInputBuffer[currentInputPosition] == '\"' || fileInputBuffer[currentInputPosition] == '>'))
{
includeName += fileInputBuffer[currentInputPosition++];
}

++currentInputPosition;
if(includeName == inputFileName)
throw selfIncludeException();

if(currentOutputPosition > 0)
{
outputStream.open(outputFileName.c_str(), std::fstream::out | std::fstream::app);
outputStream.write(fileOutputBuffer, currentOutputPosition + 1);
outputStream.write(" ", 1);
outputStream.close();
currentOutputPosition = 0;
}
++includeCount;
myGraph.addNext(includeName);
if(includeCount % 100 == 0 && includeCount != 0)
{
if(myGraph.hasCycle())
throw cycleException();
}

processFile(includeName, outputFileName);
continue;


}
else
{
fileOutputBuffer[currentOutputPosition++] = fileInputBuffer[currentInputPosition++];
continue;
}




}

if(currentOutputPosition > 0)
{
outputStream.open(outputFileName.c_str(), std::fstream::out | std::fstream::app);
outputStream.write(fileOutputBuffer, currentOutputPosition + 1);
outputStream.write(" ", 1);
outputStream.close();

}
}





private: 

int includeCount;
directedGraph myGraph;

};


