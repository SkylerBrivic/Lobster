#include "UserQueue.h"
#include <iostream>

int main()
{
UserQueue* myQueue = new UserQueue();


for(int i = 0; i < 5; ++i)
	myQueue->push((void*) i + 1);

for(int i = 0; i < 6; ++i)
	std::cout << i + 1 << ":\t" << (long long) myQueue->pop() << std::endl;
return 0;
}
