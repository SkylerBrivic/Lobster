//this is the main compiler program (stored in an executable called LobsterC

#include "CodeGenerator.h"
#include <iostream>
#include "Parser.h"
#include "preProcessor.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

void convertToUpper(char* inputArg, char* outputLocation)
{
int length = strlen(inputArg);

for(int i = 0; i <= length; ++i)
{
outputLocation[i] = toupper(inputArg[i]);
}

}

int main(int argc, char** argv)
{
bool debugPrint = false;
char* tempArg = NULL;
char* outputLocation = NULL;
if(argc >= 3 && strlen(argv[2]) == 4)
{
tempArg = argv[2];
outputLocation = new char[5];
convertToUpper(tempArg, outputLocation);
if(outputLocation[0] == 'T' && outputLocation[1] == 'R'
&& outputLocation[2] == 'U' && outputLocation[3] == 'E')
debugPrint = true;
delete outputLocation;
}

if(argc < 2)
{
std::cerr << "Usage: " << argv[0] << " inputFile\n";
exit(1);
}
srand(time(NULL));

int randInt = (rand() * 191) + 3;
preProcessor myProcessor = preProcessor();
string myTempString = argv[1];
myTempString = myTempString + ".tempLob" + std::to_string(randInt);
myProcessor.processFile(argv[1], myTempString.c_str());
Parser p = Parser(myTempString);
p.startProcessing(false);


CodeGenerator myGenerator = CodeGenerator(p.myParseTree, p.symbolTable, false);
myGenerator.generateCode();
remove(myTempString.c_str());

if(debugPrint)
myGenerator.printAll();

string outputDestination = argv[1];
outputDestination = outputDestination + ".Lobster";
std::ofstream outputStream;
outputStream.open(outputDestination.c_str(), ios::trunc | ios::out);
int fileSize = myGenerator.myByteCode.size();
unsigned char* bytePointer = &myGenerator.myByteCode[0];
const char* tempPoint = (const char*) bytePointer;
outputStream.write(tempPoint, fileSize);
outputStream.flush();
outputStream.close();
return 0;
}

