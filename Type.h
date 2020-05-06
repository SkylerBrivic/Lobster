#ifndef TYPE_H
#define TYPE_H
#include <string.h>
#include "lexicalAnalyzer.h"
//this class is used to implement arrays, which are considered to have a "complex" type, since they can't be summarized in only one string
//ex. to declare an array of ints in this language, one would write the following:
// Array<Int> myArr = new Array<Int>(6); In this case, BaseType would be Int, and DerivedType would be NULL

class Type 
{
public:

Type(bool newPrimitive)
{
isPrimitive = newPrimitive;
nextType = NULL;
}


//used for types that are just a primitive type.
Type(char* newPrimitiveName)
{
isPrimitive = true;
nextType = NULL;
strcpy(primitiveName, newPrimitiveName);
}

//used for types that are a compelx type.
Type(tokenType newComplexName, Type* newType)
{
isPrimitive = false;
complexType = newComplexName;
nextType = newType;
}

void setPrimitive(char* newPrimitive)
{
strcpy(primitiveName, newPrimitive);
}

void setComplex(tokenType newComplex)
{
complexType = newComplex;
}

void setNext(Type* newType)
{
nextType = newType;
}

bool isPrimitive;
char primitiveName[1000];
tokenType complexType;
Type* nextType;

bool operator==(const Type& otherType) const
{
if(&otherType == NULL)
std::cout << "Error: otherType was NULL!\n";
const Type* firstType;
const Type* secondType;
firstType = this;
secondType = &otherType;


if(isPrimitive != otherType.isPrimitive)
return false;

while(firstType->isPrimitive == false)
{
if(firstType->isPrimitive != secondType->isPrimitive)
return false;

if(firstType->complexType != secondType->complexType)
return false;

firstType = firstType->nextType;
secondType = secondType->nextType;
}

if((!firstType->isPrimitive) || (!secondType->isPrimitive))
return false;

if(strcmp(firstType->primitiveName, secondType->primitiveName) == 0)
return true;

return false;
}

bool operator!=(const Type& secondType)
{
return !((*this) == secondType);
}
};

void helperFunc(ostream& out, Type myType)
{
if(myType.isPrimitive == false)
{
out << getTokenType(myType.complexType) << "<";
helperFunc(out, *(myType.nextType));
out << ">";
}

else
out << myType.primitiveName;
}

ostream& operator<<(ostream& out, Type myType)
{
if(myType.isPrimitive == false)
{
out << getTokenType(myType.complexType) << "<"; 
helperFunc(out, *(myType.nextType));
out << ">"; 
}
else
out << myType.primitiveName;

return out; 

}

#endif
