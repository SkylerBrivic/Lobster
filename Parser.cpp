#include <iostream>
#include "Parser.h"
#include <stdlib.h>
using namespace std;

int main()
{
Parser p = Parser("Sample.txt");
p.startProcessing(true);
}
