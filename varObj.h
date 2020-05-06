#include <iostream>
//varObj: public: long long myVal; 
class varObj
{
public:
varObj()
{
myVal = 0;
return;
}

varObj(const varObj& secondVar)
{
//std::cout << "IN COPY CONSTRUCTOR OF varObj\n";
myVal = secondVar.myVal;
}

void operator=(const varObj& secondVar)
{
//std::cout << "IN ASSIGNMENT OPERATOR OF varObj\n";
myVal = secondVar.myVal;
}

long long myVal;
};
